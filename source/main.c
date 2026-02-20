/****************************************************************************                                                                     *
 * Copyright (c) 2026 Embedded Planet, Inc.                                 *
 * SPDX-License-Identifier: Apache-2.0                                      *
 *                                                                          *
 * Licensed under the Apache License, Version 2.0 (the "License");          *
 * you may not use this file except in compliance with the License.         *
 * You may obtain a copy of the License at                                  *
 *                                                                          *
 *     http://www.apache.org/licenses/LICENSE-2.0                           *
 *                                                                          *
 * Unless required by applicable law or agreed to in writing, software      *
 * distributed under the License is distributed on an "AS IS" BASIS,        *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and      *
 * limitations under the License.                                           *
 ****************************************************************************/
/**
 * Created on: Sept 1, 2022
 * Created by: golobmichael
 * 
 * Copyright (c) Embedded Planet, Inc - All rights reserved
 *
 * This source file is private and confidential.
 * Unauthorized copying of this file is strictly prohibited.
 * 
 * Version 1.0 - 01SEPT22  Initial, ported from: https://github.com/EmbeddedPlanet/nrf_sdk_17_1/tree/master/examples/ble_peripheral/ble_app_hrs_freertos, golobmichael
 */
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "queue.h"

#include "ep_bsp.h"

#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "nrf_crypto.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_freertos.h"
#include "nrf_delay.h"
#include "fds.h"
#include "nrf_drv_clock.h"
#include "uart_helper.h"
#include "cell_helper.h"
#include "htu21d.h"
#include "icm20602.h"
#include "qspi_helper.h"
#include "epcp_builder_asset.h"
#include "led_helper.h"
#include "time_helper.h"

SemaphoreHandle_t cellularSemaphore;
SemaphoreHandle_t dataReadySemaphore;
QueueHandle_t xCellQueue;
/* Semaphore to lock changing of sensor power enable */
SemaphoreHandle_t xSensPwrEnSemaphore;

#define mainLED_TASK_STACK_SIZE             128
#define mainCell_TASK_STACK_SIZE            4500
#define DEAD_BEEF                           0xDEADBEEF                              /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */
#define OSTIMER_WAIT_FOR_QUEUE              2                                       /**< Number of ticks to wait for the timer queue to be ready */
#define CELL_QUEUE_TIMEOUT                  pdMS_TO_TICKS(3000)                     /** Time to wait on queue before moving on to the next sample */

#if ICM20602_ACTIVE
/* Empty ICM struct */
ICM20602 icm;
#endif

/* Cellular Transmission Interval */
static uint32_t cellTransInt = MIN_TRANS_INTERVAL;

/* Sensor power enable is required */
#define SENSOR_PWR_ENABLE NRF_GPIO_PIN_MAP(0, 31)

/* TWI instance ID. */
#define TWI_INSTANCE_ID 0

/* TWI instance. */
static const nrfx_twi_t twi = NRFX_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Initial PDP config */
CellularBLEConfig_t pdnConfig = { ATT_PDN, CELLULAR_PDN_CONTEXT_IPV4, "" };

system_info_struct system_info;

/* TWI initialization */
void twi_init (void)
{
    nrfx_err_t err_code;

    const nrfx_twi_config_t twi_lm75b_config = {
       .scl                = PIN_NAME_SCL,
       .sda                = PIN_NAME_SDA,
       .frequency          = NRF_TWI_FREQ_100K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .hold_bus_uninit     = false
    };

    err_code = nrfx_twi_init(&twi, &twi_lm75b_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrfx_twi_enable(&twi);
}

/* Task for reading local sensors and adding to cell data queue */
void sensorSampleTask(void *pvParameters);

/* The task function to setup cellular with thread ready environment. */
static void CellularTask( void * pvParameters );

/* Miscellaneous initialization including preparing the logging and cell. */
static void prvMiscInitialization( void );

/* Helper function for making received Asset version numbers more readable */
static void parse_version(char* version, char* conv_fw_ver);

TaskHandle_t ledTaskHandle;
TaskHandle_t cellularTaskHandle;
TaskHandle_t sensorSampleTaskHandle;

CellStatus_t cell_status;


/*-----------------------------------------------------------*/
// Taken from FreeRTOS demo: https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS-Plus/Demo/FreeRTOS_Cellular_Interface_Windows_Simulator/Common/main.c
static void CellularTask( void * pvParameters )
{
    bool first_run = true;
    bool retCellular = false;
    cellParam cellParams = {0};
    uint32_t regTimeout = 900; //15 minutes, 900s

    // Set cell parameters, must be called prior to scheduler starting if using SSL/TLS due to NRF SDK incompatibilities
    strncpy(cellParams.appVersion, system_info.updateVerStr, strlen(system_info.updateVerStr));
    strncpy(cellParams.brokerAddrPost, IOT_BROKER_ADDRESS_POST, strlen(IOT_BROKER_ADDRESS_POST));
    strncpy(cellParams.brokerAddrGet, IOT_BROKER_ADDRESS_GET, strlen(IOT_BROKER_ADDRESS_GET));
    cellParams.carrier = CELLULAR_CARRIER;
    strncpy(cellParams.cert, CERTIFICATE, sizeof(CERTIFICATE));
    cellParams.gnssStatus = TELIT_GNSS_STATUS;
    cellParams.gnssInterval = GNSS_ATTEMPT_INTERVAL;
    cellParams.gnssTimeout = GNSS_TIMEOUT;
    strncpy(cellParams.serverAddrPrefixPost, SERVER_ADDR_PREFIX_POST, strlen(SERVER_ADDR_PREFIX_POST));
    strncpy(cellParams.serverAddrPrefixGet, SERVER_ADDR_PREFIX_GET, strlen(SERVER_ADDR_PREFIX_GET));
    strncpy(cellParams.serverAttribSuffix, SERVER_ADDR_SUFFIX_ATTR, strlen(SERVER_ADDR_SUFFIX_ATTR));
    strncpy(cellParams.serverDownloadSuffix, SERVER_ADDR_SUFFIX_FOTA, strlen(SERVER_ADDR_SUFFIX_FOTA));
    strncpy(cellParams.serverTelemSuffix, SERVER_ADDR_SUFFIX_TELE, strlen(SERVER_ADDR_SUFFIX_TELE));
    cellParams.accessTokenInPathPost = SERVER_PATH_ACCESS_TOKEN_POST_ENABLED;
    cellParams.accessTokenInPathGet = SERVER_PATH_ACCESS_TOKEN_GET_ENABLED;
    strncpy(cellParams.pvtKey, PVT_KEY, sizeof(PVT_KEY));
    strncpy(cellParams.rootCA1, ROOT_CA1, sizeof(ROOT_CA1));
    strncpy(cellParams.mqttTopicString, MQTT_TOPIC, sizeof(MQTT_TOPIC));
    cellParams.technology = CELLULAR_TECH;
    cellParams.checksumMethod = otapal_CHECKSUM_METHOD;
    cellParams.fotaFlashStart = otapal_FLASH_START;
    cellParams.fotaDescrTblStart = otapal_DESCRIPTOR_START;
    cellParams.fotaBankSize = otapal_BANK_SIZE;
    cellParams.cellOnOffPin = CELL_ON_OFF;
    cellParams.cellPwrEnPin = CELL_PWR_EN;
    cellParams.cellRTSPin = CELL_RTS_PIN_NUMBER;
    cellParams.payloadFormat = CELL_PAYLOAD_FORMAT;

    // Initialize cell
    retCellular = cellInit( &cellParams, &pdnConfig );    
    
    if( retCellular != true )
    {
        DBGE("Cellular failed to initialize.\r\n");
    }
    else
    {
        DBGI( ( "Cellular successfully initialized.\r\n" ) );  
    }

    cellStatus = runCellular(cellTransInt, regTimeout);
    cellStatus = runCellular(cellTransInt, regTimeout);
    for(;;)
    {        
        vTaskDelay(pdMS_TO_TICKS(30000));
        while(xQueuePeek(xCellQueue, NULL, 0) == pdTRUE){
            if(!first_run){
                // Reenable LED task
                led_resume();
                vTaskResume( ledTaskHandle );
            }else{
                first_run = false;
            }

            //This call of function to get unit registered to network and connected to desired endpoint address
            cell_status = runCellular(cellTransInt, regTimeout);

            // Suspend LED task
            vTaskSuspend( ledTaskHandle );
            // Enable debug out for LED
            led_pause();
            // Shutoff any LEDs, turn to input
            nrf_gpio_cfg_default(NRF_GPIO_PIN_MAP(0, 8));
        
            // If the cell fails for any reason, we timeout for 30 seconds on 3 more attempts. If it is still failing after 3 attempts, we exit and retry next transmission.
            if (cell_status != CellSuccess){
                static uint8_t fail_count = 1;
                if(fail_count++ > 3){
                    fail_count = 1;
                    break;
                }else{
                    vTaskDelay(pdMS_TO_TICKS(30000));                    
                }
            }
        }        
    }
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/
static void LEDTask( void * pvParameters )
{
    led_init();

    for(;;)
    {
        if( cellStatus == CellSuccess )
        {
            led_mode(LED_CELL_OK, true, 1);
        }
        else if( cellStatus == CellFailToSend || cellStatus == CellFailToRec )
        {
            led_mode(LED_FAIL_TO_SEND, true, 1);
        }
        else
        {
            led_mode(LED_FAIL_TO_REG, true, 1);
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/
void sensorPwrEnConfig( bool status )
{
    static uint8_t counter = 0;
    static bool init = true;

    //If first pass then configure to output and create semaphore
    if(init)
    {
        xSensPwrEnSemaphore = xSemaphoreCreateBinary();
        nrf_gpio_cfg_output(SENSOR_PWR_ENABLE);
        //Clear flag
        init = false;
    }
    else
    {
        //Take semaphore
        xSemaphoreTake(xSensPwrEnSemaphore, pdMS_TO_TICKS(500));
    }

    //If status is false, and all tasks want this pin disabled, then clear pin
    if(status == false)
    {
        //Decrement counter if above zero
        if(counter > 0)
        {
            counter--;
        }
        //If counter is now 0, then disable
        if(counter == 0)
        {
            nrf_gpio_pin_clear(SENSOR_PWR_ENABLE);
        }
    }

    //If status is true then set pin and increase counter
    if(status == true)
    {
        nrf_gpio_pin_set(SENSOR_PWR_ENABLE);
        if(counter < 0xFF)
        {
            counter++;
        }
    }

    //Give semaphore
    xSemaphoreGive(xSensPwrEnSemaphore);
}

/*-----------------------------------------------------------*/

/**@brief Function for application main entry.
 */
int main(void)
{
    /* Miscellaneous initialization including preparing the logging and cell */
    prvMiscInitialization();

    // Start FreeRTOS scheduler.
    vTaskStartScheduler();

    for (;;)
    {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
}

/*-----------------------------------------------------------*/
// Taken from FreeRTOS demo: https://github.com/FreeRTOS/FreeRTOS/blob/main/FreeRTOS-Plus/Demo/FreeRTOS_Cellular_Interface_Windows_Simulator/Common/main.c
static void prvMiscInitialization( void )
{
    // Initialize bsp
    ep_bsp_init( WATCHDOG_RELOAD, WATCHDOG_RELOAD_RATE );

    init_uart(MAIN_LOOP);
    uart_helper.dbgi = true;

    // Initialize LEDs, default to alive blink normal operation
    led_init();
    led_mode(LED_ALIVE_BLINK,1,true);

    //Enable sensor power
    sensorPwrEnConfig(true);

    nrf_delay_ms(1500);

    /* Check for version number in qpsi, otherwise use default */
    ImageDescriptor_t xDescriptor;
    ProductionTable_t xProdTable;

    /* Initialize qspi driver.*/
    nrfx_err_t err_code = qspi_init();

    if( err_code != NRFX_SUCCESS )
    {
        DBGE( "Unable to initialize QSPI driver" );
    }

    nrf_delay_ms(pdMS_TO_TICKS(2500));

    err_code = qspi_read( (uint8_t * ) &xDescriptor, sizeof(xDescriptor), 0 );
    if( err_code != NRFX_SUCCESS )
    {
        DBGE( "Read failed with with error code %d", err_code );
    }

    if( xDescriptor.usImageFlags == otapalIMAGE_FLAG_VALID )
    {
        DBGI("Image Flag: %d",xDescriptor.usImageFlags);
        DBGI("Image Checksum: %ld", xDescriptor.checksum);

        system_info.updateVerMaj = ( ( xDescriptor.updateVersion >> 24 ) & 0xFF );
        system_info.updateVerMin = ( ( xDescriptor.updateVersion >> 16 ) & 0xFF );
        system_info.updateVerBui = ( xDescriptor.updateVersion & 0xFFFF );
        sprintf( system_info.updateVerStr,"%02d.%02d.%02ld", system_info.updateVerMaj, system_info.updateVerMin, system_info.updateVerBui );
    }
    else
    {
        /* Set integer version */
        system_info.updateVerMaj = VERSION_MAJOR;
        system_info.updateVerMin = VERSION_MINOR;
        system_info.updateVerBui = VERSION_BUILD;
        /* Set string version */
        sprintf( system_info.updateVerStr,"%02d.%02d.%02ld", system_info.updateVerMaj, system_info.updateVerMin, system_info.updateVerBui );
    }

    /* Read production table in external flash */
    err_code = qspi_read( (uint8_t * ) &xProdTable, sizeof(xProdTable), otapal_PROD_TBL_START );
    if( err_code != NRFX_SUCCESS )
    {
        DBGE( "Prod Table read failed with with error code %d", err_code );
    }
    else
    {
        /* Use production table values in external flash */
        if( strncmp( xProdTable.serialNumber, "\xFF\xFF\xFF\xFF\xFF\xFF", sizeof( xProdTable.serialNumber ) ) != 0 )
        {
            memcpy( system_info.ep_serial, xProdTable.serialNumber, sizeof( xProdTable.serialNumber ) );
            DBGI("Device SN: %s", system_info.ep_serial);
        }
        /* Use default production table values, if needed */
        else
        {
            DBGE("Invalid Production Table, Programming Defaults");
        }
    }

    DBGI("******************************************************");
    DBGI("*       Embedded Planet: CONNECTED ASSET v%s    *", system_info.updateVerStr);
    DBGI("******************************************************");

    /* Task to read local sensor data and forward it to the cell queue */
    xTaskCreate(sensorSampleTask, 
                "SensorTask", 
                1024, 
                NULL, 
                tskIDLE_PRIORITY+2,
                &sensorSampleTaskHandle);
#if ENABLE_LED_OPERATION
    /* Create the task to run tests. */
    xTaskCreate( LEDTask,
                "LEDTask",
                mainLED_TASK_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY+1,
                &ledTaskHandle );
#endif

    dataReadySemaphore = xSemaphoreCreateBinary();
    if(!dataReadySemaphore){
        DBGE("dataReadySemaphore creation failed!");
    }else{
        xSemaphoreTake(dataReadySemaphore, 0);
    }

#if CELLULAR_ACTIVE
    /* Cell needs a couple of seconds before starting */
    nrf_delay_ms(2000);

    DBGI( ( "Cell Task Initializing...\r\n" ) );

    cellularSemaphore = xSemaphoreCreateBinary();
    if(!cellularSemaphore){
        DBGE("cellularSemaphore creation failed!");
    }

    //Setup cellular data queue
    xCellQueue = xQueueCreate(CELL_QUEUE_SIZE, sizeof(cell_queue_msg));
    if(xCellQueue == NULL){
        DBGE("xCellQueue creation failed!");
    }

    #if defined(AZURE_MQTT_X509) || defined(AWS_MQTT_X509)
    //If protocol is MQTT, then initialize crypto here
    if( nrf_crypto_init() != NRF_SUCCESS)
    {
        DBGE(("Failed to initialize nrf crypto"));
    }
    #endif

    /* Create the task to run tests. */
    xTaskCreate( CellularTask,
                "CellularTask",
                mainCell_TASK_STACK_SIZE,
                NULL,
                tskIDLE_PRIORITY+2,
                &cellularTaskHandle );
#endif      

    uninit_uart(MAIN_LOOP); //comment out to prevent sleep
}

/* Waits for the semaphore that is given by sensor_sample_callback. Retrieves latest local sensor data and adds to cell queue */
void sensorSampleTask(void *pvParameters)
{
     // Frame counter for payload
    static uint8_t frameCounter = 0;

    //Init nRF TWI interface
    twi_init();

    //Small delay needed after twi/spi init before communication can begin
    vTaskDelay(pdMS_TO_TICKS(20));

    // Sensor Initialize
    #if ICM20602_ACTIVE
    icm20602_init(&icm, ICM20602_ADDR_LOW, twi);
    #endif

    while(1){
        init_uart(TASK_1);

        nrfx_err_t err;

        cellDiag parameters = {'\0'};

        //Get latest cell diagnostic values
        if( queryCellularDiag(&parameters) != CELLULAR_SUCCESS )
        {
            /* Do not send if unable to attain values */
            continue;
        }

        //Loop and give time for IMEI during power up
        uint8_t loopCnt = 0;
        DBGI("Waiting for IMEI to be received");
        while( parameters.imei[0] == 0 && parameters.imei[15] == 0 )
        {
            vTaskDelay(pdMS_TO_TICKS(500));
            queryCellularDiag(&parameters);
            loopCnt++;
            if(loopCnt > 240)
            {
                break;
            }
        }
        //Cannot send data on this loop if IMEI is unavailable
        if(loopCnt > 240)
        {
            DBGE("No IMEI to receive");
            continue;
        }
        DBGI("IMEI received");

        uint64_t ts = get_time_s();

        //Increment frame counter or roll over if needed
        frameCounter++;

        #if SI7021_ACTIVE
        htu21_init(twi);
        err = htu21_is_connected();
        if(err != NRFX_SUCCESS){
            DBGW("SI7021 error!");
        }
        float si_temp, si_hum;
        htu21_read_temperature_and_relative_humidity(&si_temp, &si_hum);
        #endif

        #if ICM20602_ACTIVE
        icm20602_sensor_data icm_data;

        //Get new ICM20602 data if available
        bool icm_data_ready = icm20602_data_ready(&icm, &err);
        if(icm_data_ready){
            icm_data = icm20602_get_data(&icm, &err); 
        }else{
            DBGE("icm20602 err: %x\r\n", err);
        }
        #endif

        //Set up compact payload
        epcp_builder_asset asset_compact_payload;
        epcp_builder_asset_init(&asset_compact_payload);

        //Add System data to subpacket
        //Convert battery from float to int per spec
        uint32_t batVolt = ep_bsp_read_battery_voltage() * 100;
        uint8_t epcpVer = EPCP_VERSION;
        if(frameCounter == 1){
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_VER, &epcpVer);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_FW_MAJOR, &system_info.updateVerMaj);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_FW_MINOR, &system_info.updateVerMin);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_FW_PATCH, &system_info.updateVerBui);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_MSG_CNT, &frameCounter);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_SN, system_info.ep_serial);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_TIME, &ts);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM, EPCP_BATT, &batVolt);
        }else{
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM_V2, EPCP_VER, &epcpVer);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM_V2, EPCP_MSG_CNT, &frameCounter);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM_V2, EPCP_TIME, &ts);
            asset_add_data(&asset_compact_payload, EPCP_SYSTEM_V2, EPCP_BATT, &batVolt);
        }

        //Add Cell data to subpacket
        char* endptr;
        uint64_t hexIMEI = strtoull(parameters.imei,&endptr,10); //Convert IMEI from string to number
        asset_add_data(&asset_compact_payload, EPCP_CELL, EPCP_IMEI, &(hexIMEI));
        asset_add_data(&asset_compact_payload, EPCP_CELL, EPCP_RSSI, &(parameters.rssi));
        asset_add_data(&asset_compact_payload, EPCP_CELL, EPCP_RSRQ, &(parameters.rsrq));

        #if SI7021_ACTIVE
        //Add HTU21D / SI7021 data to subpacket
        //Convert TEMP to int per spec
        int32_t tempSi = si_temp * 100;
        asset_add_data(&asset_compact_payload, EPCP_SI7021, EPCP_TEMP, &tempSi);
        //Convert HUM to int per spec
        int32_t humiditySi = si_hum * 100;
        asset_add_data(&asset_compact_payload, EPCP_SI7021, EPCP_HUM, &humiditySi);
        #endif

        #if ICM20602_ACTIVE
        //Convert data from float to int per spec and add to ICM subpacket
        int32_t accelX = icm_data.accel_x * 100;
        asset_add_data(&asset_compact_payload, EPCP_ICM20602, EPCP_ACCEL_X, &accelX);
        int32_t accelY = icm_data.accel_y * 100;
        asset_add_data(&asset_compact_payload, EPCP_ICM20602, EPCP_ACCEL_Y, &accelY);
        int32_t accelZ = icm_data.accel_z * 100;
        asset_add_data(&asset_compact_payload, EPCP_ICM20602, EPCP_ACCEL_Z, &accelZ);
        int32_t gyroX = icm_data.gyro_x * 100;
        asset_add_data(&asset_compact_payload, EPCP_ICM20602, EPCP_GYRO_X, &gyroX);
        int32_t gyroY = icm_data.gyro_y * 100;
        asset_add_data(&asset_compact_payload, EPCP_ICM20602, EPCP_GYRO_Y, &gyroY);
        int32_t gyroZ = icm_data.gyro_z * 100;
        asset_add_data(&asset_compact_payload, EPCP_ICM20602, EPCP_GYRO_Z, &gyroZ);
        #endif
                    
        // Generate completed packet
        cell_queue_msg local_sensor_data_msg;
        local_sensor_data_msg.size = asset_get_packet(&asset_compact_payload, local_sensor_data_msg.data);

        #if CELLULAR_ACTIVE
        //Forward data to cell queue if there is space
        if(xQueueSend(xCellQueue, &local_sensor_data_msg, CELL_QUEUE_TIMEOUT) != pdTRUE){
            DBGE("Queue Timeout!");
        }
        #endif

        char sharedAppAttr[MAX_ATTR_VALUE_LENGTH] = {'\0'};
        for( uint8_t attribute = 0; attribute < num_HTTP_ATTRIBUTES; attribute++ )
        {
            getHTTPAttributes(attribute,sharedAppAttr);
            // Check if attribute value is valid
            if(sharedAppAttr[0] != '\0')
            {
                switch(attribute){
                    case 0:
                    {
                        //Tranmission interval update
                        cellTransInt = ( uint32_t ) strtoul( sharedAppAttr, NULL, MAX_ATTR_VALUE_LENGTH );
                        DBGI("New Cell Transmission Interval: %ds",cellTransInt);
                    }break;
                }
            }
        }

        epcp_builder_asset_deinit(&asset_compact_payload);

        // Monitor FreeRTOS stack usage
        DBGI("FreeRTOS Heap Remaining: %d Bytes", xPortGetFreeHeapSize());

        uninit_uart(TASK_1);
        vTaskDelay(pdMS_TO_TICKS(cellTransInt * 1000));
    }
    vTaskDelete( NULL );
}

void appendMsgWithGNSS( void )
{
    //Create message to send to queue
    cell_queue_msg cell_msg;
    static uint32_t prevFixTime = 0;

    if( xQueuePeek( xCellQueue,
                        &( cell_msg ),
                        ( TickType_t ) 10 ) == pdTRUE )
    {
        //Read message to append
        if( xQueueReceive( xCellQueue, &( cell_msg ), ( TickType_t ) 100 ) == pdTRUE )
        {
            cellDiag parameters = {'\0'};
            //Get latest cell values
            if( queryCellularDiag(&parameters) != CELLULAR_SUCCESS )
            {
                /* retry once */
                if( queryCellularDiag(&parameters) != CELLULAR_SUCCESS )
                {
                    DBGE("Failed to query cell diagnostic data");
                    return;
                }
            }

            //Set up compact payload
            epcp_builder_asset asset_compact_payload;
            epcp_builder_asset_init(&asset_compact_payload);

            //Print GNSS data to debug log
            DBGI("Lat:%f,Long:%f",parameters.lat_float,parameters.long_float);

            //Add GNSS data to subpacket
            if(parameters.fixTime != prevFixTime){           
                prevFixTime = parameters.fixTime;
                //Convert LAT from float to int per spec
                int32_t latFloat = parameters.lat_float * 10000;
                asset_add_data(&asset_compact_payload, EPCP_GNSS, EPCP_LAT, &latFloat);

                //Convert LON from float to int per spec
                int32_t longFloat = parameters.long_float * 10000;
                asset_add_data(&asset_compact_payload, EPCP_GNSS, EPCP_LON, &longFloat);
            }

            //Generate completed packet
            cell_queue_msg gnss_sensor_data_msg;
            gnss_sensor_data_msg.size = asset_get_packet(&asset_compact_payload, gnss_sensor_data_msg.data);

            // Generate combined packet */
            cell_queue_msg combined_sensor_data_msg;

            memcpy(&combined_sensor_data_msg.data[0],&cell_msg.data[0],cell_msg.size);
            memcpy(&combined_sensor_data_msg.data[cell_msg.size],&gnss_sensor_data_msg.data[0],gnss_sensor_data_msg.size);

            combined_sensor_data_msg.size = ((cell_msg.size)+(gnss_sensor_data_msg.size));

            //Its possible to miss gnss fix if cell service is unable for an extended period and will retry on next gnss interval
            if(xQueueSendToFront(xCellQueue, &combined_sensor_data_msg, CELL_QUEUE_TIMEOUT) != pdTRUE){
                DBGE("Queue Timeout!");
            }

            //Free message
            epcp_builder_asset_deinit(&asset_compact_payload);            
        }
    }
}
