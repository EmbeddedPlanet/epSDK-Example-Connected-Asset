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
 * @file    compact_payload.h
 * @version 1.0.0
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    26 FEB 2024
 * 
 * @brief Compact payload configuration for Connected Equipment.
 * versions:
 * 
 * Built for use with the nRF SDK 17.1 and FreeRTOS.
 */

/**
 * @file compact_payload_config.h
 */

#ifndef __COMPACT_PAYLOAD_CONFIG_H__
#define __COMPACT_PAYLOAD_CONFIG_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#define EPCP_VERSION 0x04   /** < EP Compact Payload V3 */

/**
 * @brief Sensor ID Numbers
 * 
 * Conform to the EPCP format version listed. DO NOT CHANGE at
 * risk of breaking any decoder that uses these ID numbers. 
 * 
 * Any addition or change requires a new version of the specification P399x024.
 * 
 */
typedef enum epcp_sensor_id{
    /* Agora Sensors */
    EPCP_SYSTEM    = 0x01,
    EPCP_SYSTEM_V2     = 0x02,
    //0x03 - 0x04 reserved for future use
    EPCP_CELL       = 0x05,
    EPCP_GNSS       = 0x06,
    EPCP_SI7021     = 0x07,
    EPCP_BME680     = 0x08,
    EPCP_ICM20602   = 0x09,
    EPCP_LSM9DS1    = 0x0A,
    EPCP_VL53L0X    = 0x0B,
    //0x0E - 0x1F reserved for future use
    /*Customer Sensors (starting at 0x20 through 0x7E) */
    //0x7F reserved for future use
}EPCP_SENSOR_ID;

/**
 * @brief Sensor subpacket sizes
 * 
 * These are used during packet creation to set the size of a particular subpacket.
 * These are found by the addition of all data included in the packet, as specified in
 * P399x024.
 */
typedef enum epcp_sensor_packet_size{
    EPCP_SYSTEM_SIZE   = 19,
    EPCP_SYSTEM_V2_SIZE    = 9,
    EPCP_CELL_SIZE          = 10,
    EPCP_GNSS_SIZE          = 7,
    EPCP_SI7021_SIZE        = 6,
    EPCP_BME680_SIZE        = 25,
    EPCP_ICM20602_SIZE      = 14,
    EPCP_LSM9DS1_SIZE       = 21,
    EPCP_VL53L0X_SIZE       = 6,
} EPCP_SENSOR_PACKET_SIZE;

/** 
 * @brief  Sensor Data Slots
 * 
 * Used for data position tracking within subpackets. These are generic keys, and
 * can be used multiple times per packet, but only once per subpacket.
 * 
 * For example, both the ICM20602 and the LSM9DS1 can have the EPCP_ACCEL_X sensor data slot types.
 * 
 */
typedef enum epcp_sensor_slot_type{
    EPCP_VER,           //EPCP formatting version
    EPCP_SN,            //Device Serial Number
    EPCP_FW_MAJOR,      //Firmware version major value (X.0.0)
    EPCP_FW_MINOR,      //Firmware version minor value (0.X.0)
    EPCP_FW_PATCH,      //Firmware version patch value (0.0.X)
    EPCP_MSG_CNT,       //Message counter
    EPCP_ALARMS,        //Active sensor alarms
    EPCP_IMEI,          //Cell IMEI
    EPCP_ICCID,         //Cell ICCID
    EPCP_RSSI,          //Cell RSSI
    EPCP_RSRQ,          //Cell RSRQ
    EPCP_LAT,           //GNSS Latitude
    EPCP_LON,           //GNSS Longitude
    EPCP_TIME,          //System time or run time
    EPCP_BATT,          //Battery voltage
    EPCP_TEMP,          //Temperature reading
    EPCP_PRES,          //Pressure reading
    EPCP_HUM,           //Humidity reading
    EPCP_GAS,           //Gas reading
    EPCP_CO2,           //CO2 reading
    EPCP_BREATH,        //Breath equivelent reading
    EPCP_IAQ_SCORE,     //Indoor air quality score
    EPCP_IAQ_ACCURACY,  //Indoor air quality score accuracy
    EPCP_ACCEL_X,       //Accelerometer x-axis reading
    EPCP_ACCEL_Y,       //Accelerometer y-axis reading
    EPCP_ACCEL_Z,       //Accelerometer z-axis reading
    EPCP_GYRO_X,        //Gyroscope x-axis reading
    EPCP_GYRO_Y,        //Gyroscope y-axis reading
    EPCP_GYRO_Z,        //Gyroscope z-axis reading
    EPCP_MAG_X,         //Magnetometer x-axis reading
    EPCP_MAG_Y,         //Magnetometer y-axis reading
    EPCP_MAG_Z,         //Magnetometer z-axis reading
    EPCP_DIST,          //Distance reading
    EPCP_ACTIVE_CHANNELS,   //Used to indicate the number of channels present in a CE subpacket.
    EPCP_4_20_CH1,      //4-20mA Ch1 reading
    EPCP_4_20_CH2,      //4-20mA Ch2 reading
    EPCP_4_20_CH3,      //4-20mA Ch3 reading
    EPCP_4_20_CH4,      //4-20mA Ch4 reading
    EPCP_0_5_CH1,      //0-10V Ch1 reading
    EPCP_0_5_CH2,      //0-10V Ch1 reading
    EPCP_0_5_CH3,      //0-10V Ch1 reading
    EPCP_0_5_CH4,      //0-10V Ch1 reading
} EPCP_SENSOR_SLOT_TYPE;

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* __COMPACT_PAYLOAD_CONFIG_H__ */