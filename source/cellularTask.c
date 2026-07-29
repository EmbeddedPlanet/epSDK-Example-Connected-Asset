/**
 * Created on: Sept 1, 2022
 * Created by: golobmichael
 * 
 * Copyright (c) Embedded Planet, Inc - All rights reserved
 *
 * This source file is private and confidential.
 * Unauthorized copying of this file is strictly prohibited.
 * 
 * Version 1.0 - 01SEPT22  Initial, golobmichael
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
#include "main.h"
#include "cell_helper.h"
#include "led_helper.h"
#include "time_helper.h"
#include "uart_helper.h"

cellParam cellParams;
CellRegStatus_t regStatusFlag;
bool newDataAdded;

/*-----------------------------------------------------------*/

bool initCell( void )
{
    // Set cell parameters, must be called prior to scheduler starting if using SSL/TLS due to NRF SDK incompatibilities
    strncpy(cellParams.appVersion, system_info.updateVerStr, strlen(system_info.updateVerStr));
    strncpy(cellParams.cert, CERTIFICATE, sizeof(CERTIFICATE));
    strncpy(cellParams.pvtKey, PVT_KEY, sizeof(PVT_KEY));
    strncpy(cellParams.symmetricKey, SYMMETRIC_KEY, sizeof(SYMMETRIC_KEY));
    strncpy(cellParams.rootCA1, ROOT_CA1, sizeof(ROOT_CA1));
    cellParams.technology = CELLULAR_TECH;
    cellParams.checksumMethod = otapal_CHECKSUM_METHOD;
    cellParams.fotaFlashStart = otapal_FLASH_START;
    cellParams.fotaDescrTblStart = otapal_DESCRIPTOR_START;
    cellParams.fotaBankSize = otapal_BANK_SIZE;
    cellParams.cellOnOffPin = CELL_ON_OFF;
    cellParams.cellPwrEnPin = CELL_PWR_EN;
    cellParams.cellRTSPin = CELL_RTS_PIN_NUMBER;
    cellParams.payloadFormat = CELL_PAYLOAD_FORMAT;

    // Initialize cell with new parameters and restart modem
    bool retCellular = cellInit( &cellParams, pdnConfig.apnName, MODEM_SLEEP_STATE);    
    
    if( retCellular != true )
    {
        DBGE("Cellular failed to initialize.\r\n");
    }
    else
    {
        DBGI( ( "Cellular successfully initialized.\r\n" ) );  
    }

    return retCellular;
}

/*-----------------------------------------------------------*/
void CellularTask( void * pvParameters )
{
    #define REG_TIMEOUT      900    // 900s
    bool retCellular = false;
    static bool first_run = true, skipInit = true;
    uint32_t lastGnssTime = 0, lastGtpTime = 0, sasTokenTimer = 0, currentTime = 0;
    bool fotaAvailable = false;
    int32_t httpsRespSize = 0;
    uint8_t *httpsRespBody;

    /* Init cell UART */
    init_uart(TASK_2);

    /* Immediately call initCell to get IMEI, keep modem powered and wait for more data */
    if( initCell() != true ){
        DBGE("Failed to initialize cell, delay and retry\r\n");
        /* Reset flag so function will try again */
        skipInit = false;
    }
    else{
        skipInit = true;
    }

    /* Uninit cell UART */
    uninit_uart(TASK_2);

    while(1){
        uint8_t satellites = 0;
        static uint8_t gnssFix = 0;
        cellStatus = CellGeneralFail;
        
        /* Wait for item in queue */
        if(newDataAdded == true && xQueuePeek(xCellQueue, NULL, 0) == pdTRUE){
            /* Init cell UART */
            init_uart(TASK_2);

            /* Clear flag */
            newDataAdded = false;
            if(!first_run){
                // Reenable LED task
                led_resume();
                vTaskResume( ledTaskHandle );
            }

            /* Initialize cell library and startup modem hardware, don't need to call on first run */
            if( !skipInit && initCell() != true ){
                DBGE("Failed to initialize cell, delay and retry\r\n");
                closeCell(MODEM_SLEEP_STATE);
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }

            /* Clear flag */
            skipInit = false;

            /* Get current time */
            currentTime = get_time_s();

            /* Is it time to attain GNSS fix, will run on second pass */
            if(TELIT_GNSS_STATUS == true && (currentTime - lastGnssTime) > GNSS_ATTEMPT_INTERVAL && (strstr( IOT_BROKER_ADDRESS_POST, "mqtt" ) == NULL || !PERSISTENT_CONNECT_FLAG)){
                if( setGNSS(true) != true ){
                    DBGE("Failed to enable GNSS, delay and retry\r\n");
                    closeCell(MODEM_SLEEP_STATE);
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    continue;
                }

                /* Check registration status, advance when registered or timeout */
                for(uint16_t i = 0; i < GNSS_TIMEOUT; i++){
                    gnssFix = statusGNSS(&satellites);
                    if(gnssFix >= 2){
                        /* App callback to add GNSS fix if desired */
                        addGnssQueueMsg(true);
                        break;
                    }
                    /* Not enough satellites aquired after a specified amount of time, break loop */
                    if(LOW_SAT_STATUS == true && satellites < LOW_SAT_NUMBER && i == LOW_SAT_TIMEOUT){
                        DBGW( ">>>  GNSS aquire satellite timeout  <<<\r\n" );
                        break;
                    }
                    vTaskDelay(pdMS_TO_TICKS(10000));
                }

                /* If fix failed, then add to queue*/
                if(gnssFix < 2){
                    /* App callback to add GNSS fix if desired */
                    addGnssQueueMsg(false);
                }

                /* Return back to cell comms */
                setGNSS(false);

                /* Update last gnss time */
                lastGnssTime = get_time_s();
            }

            /* Clear flag */
            first_run = false;

            /* Enable eDRX and register cell */
            if( registerCellular(CELLULAR_CARRIER, REG_TIMEOUT) != CellSuccess ){
                DBGE("Failed to start cell registration process, delay and retry\r\n");
                closeCell(MODEM_SLEEP_STATE);
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }

            /* Check registration status */
            regStatusFlag = regStatus();
            if(regStatusFlag != REG_STATUS_REGISTERED_HOME && regStatusFlag != REG_STATUS_ROAMING_REGISTERED){
                /* Wait 10s and check again */
                vTaskDelay(pdMS_TO_TICKS(10000));
                regStatusFlag = regStatus();
                if(regStatusFlag != REG_STATUS_REGISTERED_HOME && regStatusFlag != REG_STATUS_ROAMING_REGISTERED){
                    DBGE("Failed to register to cell network, delay and retry\r\n");
                    cellStatus = CellRegisterFail;
                    closeCell(MODEM_SLEEP_STATE);
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    continue;
                }
            }

            /* Is it time to attain GTP fix, will run on second pass */
            if(GTP_STATUS == true && (currentTime - lastGtpTime) > GNSS_ATTEMPT_INTERVAL){
                if(connectCell(IOT_BROKER_ADDRESS_POST, GTP_STATUS) != cellStatus){
                    DBGE("Failed to connect to cell socket, delay and retry\r\n");
                    closeCell(MODEM_SLEEP_STATE);
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    continue;
                }
                /* App callback to add GTP fix if desired */
                addGnssQueueMsg(true);
                
                /* Update last gnss time */
                lastGtpTime = get_time_s();
            }
            else{
                if(connectCell(IOT_BROKER_ADDRESS_POST, false) != cellStatus){
                    DBGE("Failed to connect to cell socket, delay and retry\r\n");
                    closeCell(MODEM_SLEEP_STATE);
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    continue;
                }
            }

            /* Check if Azure SaS token needs to be regenerated */
            if(strstr( IOT_BROKER_ADDRESS_POST, "azure" ) != NULL){
                static bool initAzurePass = true;
                if(SECURITY_TYPE == TLS_SAS_TOKENS && ((currentTime-sasTokenTimer) > SAS_EXPIRE_TIME_S == 0) || initAzurePass == true){
                    DBGI("Regenerating Azure SaS token");
                    azureRegenerateSaSToken(IOT_BROKER_ADDRESS_POST, SAS_EXPIRE_TIME_S);
                    sasTokenTimer = get_time_s();
                }
                /* Clear flag */
                initAzurePass = false;
            }
            
            if(strstr( IOT_BROKER_ADDRESS_GET, "http://" ) != NULL){
                /* Send HTTP Post (without TLS) */
                sendHttpMsg(METHOD_GET, IOT_BROKER_ADDRESS_GET, SERVER_ADDR_PREFIX_GET, SERVER_ADDR_SUFFIX_ATTR, SERVER_PATH_ACCESS_TOKEN_GET_ENABLED, &fotaAvailable);
            }
            else if(strstr( IOT_BROKER_ADDRESS_GET, "coap://" ) != NULL){
                /* Send CoAP Post (without DTLS) */
                sendCoAPMsg(METHOD_GET, IOT_BROKER_ADDRESS_GET, SERVER_ADDR_PREFIX_GET, SERVER_ADDR_SUFFIX_ATTR, SERVER_PATH_ACCESS_TOKEN_GET_ENABLED, &fotaAvailable);
            }
            else if(strstr( IOT_BROKER_ADDRESS_GET, "https://" ) != NULL){
                /* Send HTTP Post (with TLS)*/
                sendHttpTLSMsg(METHOD_GET, IOT_BROKER_ADDRESS_GET, SERVER_ADDR_PREFIX_GET, SERVER_ADDR_SUFFIX_ATTR, SERVER_PATH_ACCESS_TOKEN_GET_ENABLED, SECURITY_TYPE, &httpsRespSize, &httpsRespBody);
                DBGI( "Response Body:\n%.*s\n",  ( int32_t ) httpsRespSize, httpsRespBody);
            }

            /* Check registration status */
            regStatusFlag = regStatus();
            if(regStatusFlag != REG_STATUS_REGISTERED_HOME && regStatusFlag != REG_STATUS_ROAMING_REGISTERED){
                /* Wait 10s and check again */
                vTaskDelay(pdMS_TO_TICKS(10000));
                regStatusFlag = regStatus();
                if(regStatusFlag != REG_STATUS_REGISTERED_HOME && regStatusFlag != REG_STATUS_ROAMING_REGISTERED){
                    DBGE("Failed to register to cell network, delay and retry\r\n");
                    cellStatus = CellRegisterFail;
                    closeCell(MODEM_SLEEP_STATE);
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    continue;
                }
            }

            if(strstr( IOT_BROKER_ADDRESS_POST, "http://" ) != NULL){
                /* Send HTTP Post (without TLS) */
                sendHttpMsg(METHOD_POST, IOT_BROKER_ADDRESS_POST, SERVER_ADDR_PREFIX_POST, SERVER_ADDR_SUFFIX_TELE, SERVER_PATH_ACCESS_TOKEN_POST_ENABLED, NULL);
            }
            else if(strstr( IOT_BROKER_ADDRESS_POST, "coap://" ) != NULL){
                /* Send CoAP Post (without DTLS) */
                sendCoAPMsg(METHOD_POST, IOT_BROKER_ADDRESS_POST, SERVER_ADDR_PREFIX_POST, SERVER_ADDR_SUFFIX_TELE, SERVER_PATH_ACCESS_TOKEN_POST_ENABLED, NULL);
            }
            else if(strstr( IOT_BROKER_ADDRESS_POST, "https://" ) != NULL){
                /* Send HTTP Post (with TLS)*/
                sendHttpTLSMsg(METHOD_POST, IOT_BROKER_ADDRESS_POST, SERVER_ADDR_PREFIX_POST, SERVER_ADDR_SUFFIX_TELE, SERVER_PATH_ACCESS_TOKEN_POST_ENABLED, SECURITY_TYPE, &httpsRespSize, &httpsRespBody);
            }
            
            /* Run Thingsboard FOTA if detected, currently only supported on http */
            if(fotaAvailable == true && ( IOT_BROKER_ADDRESS_GET, "thingsboard" ) != NULL && ( IOT_BROKER_ADDRESS_GET, "http://" ) != NULL){
                runThingsboardFOTA(IOT_BROKER_ADDRESS_GET, SERVER_ADDR_PREFIX_GET, SERVER_ADDR_SUFFIX_FOTA);
            }
            else if(strstr( IOT_BROKER_ADDRESS_GET, "amazonaws" ) != NULL && strstr( IOT_BROKER_ADDRESS_POST, "mqtts://" ) != NULL){
                /* Start AWS task. Persistent will only return if there is an error. Function handles uplink, downlink, FOTA */
                runAwsMqttTlsTask(IOT_BROKER_ADDRESS_POST, SERVER_ADDR_PREFIX_POST, SERVER_ADDR_SUFFIX_TELE, SERVER_PATH_ACCESS_TOKEN_POST_ENABLED, PERSISTENT_CONNECT_FLAG, GNSS_ATTEMPT_INTERVAL, GNSS_TIMEOUT, TELIT_GNSS_STATUS);
            }
            else if(strstr( IOT_BROKER_ADDRESS_POST, "azure" ) != NULL && strstr( IOT_BROKER_ADDRESS_POST, "mqtts://" ) != NULL){
                /* Start Azure MQTT task. Persistent will only return if there is an error. Function handles uplink, downlink */
                runAzureMqttTlsTask(IOT_BROKER_ADDRESS_POST, SERVER_ADDR_PREFIX_POST, SERVER_ADDR_SUFFIX_TELE, SERVER_PATH_ACCESS_TOKEN_POST_ENABLED, SECURITY_TYPE, PERSISTENT_CONNECT_FLAG, GNSS_ATTEMPT_INTERVAL, GNSS_TIMEOUT, TELIT_GNSS_STATUS);
            }

            closeCell(MODEM_SLEEP_STATE);

            /* Uninit cell UART */
            uninit_uart(TASK_2);

            // Suspend LED task
            vTaskSuspend( ledTaskHandle );
            // Enable debug out for LED
            led_pause();
        }
        /* Delay between checking queue*/
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/
void mqttTask( void * pvParameters )
{
    for(;;)
    {
        /* Periodically check status to see if mqtt connection can be closed */
        CellStatus_t ret = statusMQTTCellular(IOT_BROKER_ADDRESS_POST);

        /* If Success then close, otherwise wait and check again */
        if( ret == CellSuccess ){
            DBGI("MQTT ready to be closed");
            if( closeMQTTCellular() != CellSuccess){
                DBGE("Fail to close MQTT connection");
            }
            /* Suspend task */
            vTaskSuspend( NULL );
        }

        vTaskDelay(pdMS_TO_TICKS(10000));    
    }
    vTaskDelete( NULL );
}