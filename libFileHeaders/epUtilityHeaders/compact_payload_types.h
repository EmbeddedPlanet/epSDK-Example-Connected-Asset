/****************************************************************************
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
 * @file    compact_payload_types.h
 * @version 0.0.1
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    20 OCT 2026
 *
 * @brief The EP Compact Payload Builder is used to construct packets that conform to the
 * Embedded Planet Compact Payload specification P399x024.
 *
 * Built for use with the nRF5 SDK 17.1 and FreeRTOS.
 *
 * Versions:
 * 0.0.1 - Initial release
 */

#ifndef COMPACT_PAYLOAD_TYPES_H
#define COMPACT_PAYLOAD_TYPES_H

#include "ep_compact_payload.h"

#define EPCP_VERSION 0x08

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
    EPCP_SYSTEM_FULL        = 0x01,
    EPCP_SYSTEM_CONDENSED   = 0x02,
    EPCP_CELL               = 0x05,
    EPCP_GNSS               = 0x06,
    EPCP_GTP                = 0x07,
    EPCP_SI7021             = 0x08,
    EPCP_ICM20602           = 0x0A,
    EPCP_LSM9DS1            = 0x0B,
}EPCP_SENSOR_ID;

/**
 * @brief Sensor subpacket sizes
 *
 * These are used during packet creation to set the size of a particular subpacket.
 * These are found by the addition of all data included in the packet, as specified in
 * P399x024.
 */
typedef enum epcp_sensor_packet_size{
    EPCP_SYSTEM_FULL_SIZE        = 19,
    EPCP_SYSTEM_CONDENSED_SIZE   = 9,
    EPCP_CELL_SIZE          = 10,
    EPCP_GNSS_SIZE          = 10,
    EPCP_GTP_SIZE           = 13,
    EPCP_SI7021_SIZE        = 6,
    EPCP_ICM20602_SIZE      = 14
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
    EPCP_LAT,           //GNSS/GTP Latitude
    EPCP_LON,           //GNSS/GTP Longitude
    EPCP_SAT,           //GNSS satellite count
    EPCP_ACC,           //GTP accuracy
    EPCP_TIME,          //System time, run time, or another format of timestamp
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
    EPCP_DIST,          //Distance reading
    EPCP_BLE_BEACON,    //BLE beacon payload(s)
    EPCP_BLE_ADDR       //BLE beacon/peripheral address
} EPCP_SENSOR_SLOT_TYPE;

/**
 * @brief Struct that contains the sensor subpackets
 *
 */
typedef struct{
    //Sensor subpackets
    epcp_sensor_t system_full;
    epcp_sensor_t system_condensed;
    epcp_sensor_t cell;
    epcp_sensor_t gnss;
    epcp_sensor_t gtp;
    epcp_sensor_t htu21d;
    epcp_sensor_t icm20602;
} compact_payload_t;

static const epcp_slot_info_t slots_system_full[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_VER,         1,      1,      EPCP_ALARM_BIT_0),
    EPCP_SLOT(EPCP_FW_MAJOR,    1,      2,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_FW_MINOR,    1,      3,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_FW_PATCH,    1,      4,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_ALARMS,      1,      5,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_MSG_CNT,     1,      6,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_SN,          6,      7,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_TIME,        4,      13,     EPCP_ALARM_BIT_1),
    EPCP_SLOT(EPCP_BATT,        2,      17,     EPCP_ALARM_BIT_2),
};

static const epcp_slot_info_t slots_system_condensed[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_VER,         1,      1,      EPCP_ALARM_BIT_0),
    EPCP_SLOT(EPCP_MSG_CNT,     1,      2,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_TIME,        4,      3,      EPCP_ALARM_BIT_1),
    EPCP_SLOT(EPCP_BATT,        2,      7,      EPCP_ALARM_BIT_2),
};

static const epcp_slot_info_t slots_cell[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_IMEI,        7,      1,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_RSSI,        1,      8,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_RSRQ,        1,      9,      EPCP_NO_ALARM_BIT),
};

static const epcp_slot_info_t slots_gnss[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_SAT,         1,      1,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_LAT,         4,      2,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_LON,         4,      6,      EPCP_NO_ALARM_BIT),
};

static const epcp_slot_info_t slots_gtp[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_LAT,         4,      1,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_LON,         4,      5,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_ACC,         4,      9,      EPCP_NO_ALARM_BIT),
};

static const epcp_slot_info_t slots_htu21d[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_ALARMS,      1,      1,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_TEMP,        2,      2,      EPCP_ALARM_BIT_0),
    EPCP_SLOT(EPCP_HUM,         2,      4,      EPCP_ALARM_BIT_1),
};

static const epcp_slot_info_t slots_icm20602[] = {
    /*        slot              size    offset  alarm */
    EPCP_SLOT(EPCP_ALARMS,      1,      1,      EPCP_NO_ALARM_BIT),
    EPCP_SLOT(EPCP_ACCEL_X,     2,      2,      EPCP_ALARM_BIT_0),
    EPCP_SLOT(EPCP_ACCEL_Y,     2,      4,      EPCP_ALARM_BIT_1),
    EPCP_SLOT(EPCP_ACCEL_Z,     2,      6,      EPCP_ALARM_BIT_2),
    EPCP_SLOT(EPCP_GYRO_X,      2,      8,      EPCP_ALARM_BIT_3),
    EPCP_SLOT(EPCP_GYRO_Y,      2,      10,     EPCP_ALARM_BIT_4),
    EPCP_SLOT(EPCP_GYRO_Z,      2,      12,     EPCP_ALARM_BIT_5),
};

#endif