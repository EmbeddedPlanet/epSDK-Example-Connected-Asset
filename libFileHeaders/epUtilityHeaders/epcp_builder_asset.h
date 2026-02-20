/****************************************************************************
 * Copyright (c) 2023 Embedded Planet, Inc.                                 *
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
 * @file    epcp_builder_asset.c
 * @version 0.0.3
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    21 DEC 2023
 *
 * @brief This layer, which sits on top of the EP Compact Payload Builder, is accessed by the
 * main application to add sensor data to the final asset compact payload packet.
 *
 * Built for use with the nRF5 SDK 17.1 and FreeRTOS.
 *
 * Versions:
 * 0.0.1 - Initial release in Mbed - conforms to spec version 1
 * 0.0.2 - Ported to FreeRTOS. Conforms to spec version 2
 */

#ifndef EPCP_BUILDER_asset_H
#define EPCP_BUILDER_asset_H

#include "ep_compact_payload.h"
#include "compact_payload_config.h"

/*
EPCP payload format per subpacket:
+-----------+--------------+---------+
| Sensor ID | Sensor Error |  Data   |
+-----------+--------------+---------+
| 7 bits    | 1 bit        | N Bytes |
+-----------+--------------+---------+
*/

/**
 * @brief Struct that contains the sensor subpackets
 *
 */
typedef struct{
    //Sensor subpackets
    epcp_sensor system;
    epcp_sensor systemV2;
    epcp_sensor cell;
    epcp_sensor gnss;
    epcp_sensor si7021;
    epcp_sensor bme680;
    epcp_sensor icm20602;
    epcp_sensor lsm9ds1;
    epcp_sensor vl53l0x;

    //Sensor subpacket enables.
    bool system_active;
    bool systemV2_active;
    bool coach_active;
    bool cell_active;
    bool gnss_active;
    bool si7021_active;
    bool bme680_active;
    bool icm20602_active;
    bool lsm9ds1_active;
    bool vl53l0x_active;
} epcp_builder_asset;

/**
 * @brief Used to initialize the epcp_builder_asset struct as well as all subpacket structs.
 *
 * @param asset_builder     Pointer to the epcp_builder_asset struct
 */
void epcp_builder_asset_init(epcp_builder_asset *asset_builder);

/**
 * @brief Used to deinitialize the epcp_builder_asset struct as all subpacket structs if going out of scope.
 * If this is not called, the dynamic arrays of the packet and subpackets will not be deallocated.
 *
 * @param asset_builder     Pointer to the epcp_builder_asset struct
 */
void epcp_builder_asset_deinit(epcp_builder_asset *asset_builder);

/**
 * @brief Used to add sensor data to the packet
 *
 * @param asset_builder     Pointer to an epcp_builder_asset struct
 * @param sensor_id         Sensor we are adding the data for
 * @param sensor_slot       Sensor slot that we are adding the data into
 * @param sensor_data       Pointer to the actual data we are adding. Must be in uint* format. Use the included epcp_convert_type helper union for conversion
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum
 */
EPCP_ERROR_CODE asset_add_data(epcp_builder_asset *asset_builder, EPCP_SENSOR_ID sensor_id , EPCP_SENSOR_SLOT_TYPE sensor_slot, void * sensor_data);

/**
 * @brief Sets an alarm value as active or inactive for a sensor data slot. For an error bit that
 * applies to the whole sensor subpacket, see set_error()
 *
 * Alarms are typically set when a sensor reading violates a set parameter. For example, an environmental sensor
 * might set an alarm bit for temperature when about 25C, and an alarm bit for humidity when above 80%. In contrast,
 * an error bit applies to the whole sensor and typically would indicate an issue with the sensor. For example if
 * the system was unable to communicate with the sensor.
 *
 * @param asset_builder     Pointer to an epcp_builder_asset struct
 * @param sensor_id         Sensor that includes the data that we are adding the alarm to
 * @param sensor_slot       The sensor data slot that the alarm applies to
 * @param alarm_status      true if alarm is active, else false
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum
 */
EPCP_ERROR_CODE asset_set_alarm(epcp_builder_asset *asset_builder, EPCP_SENSOR_ID sensor_id, EPCP_SENSOR_SLOT_TYPE sensor_slot, bool alarm_status);

/**
 * @brief Sets the error value for a sensor as active or inactive. This applies to the whole sensor
 * packet. For setting alarm bits for a specific sensor data slot, see set_alarm()
 *
 * Alarms are typically set when a sensor reading violates a set parameter. For example, an environmental sensor
 * might set an alarm bit for temperature when about 25C, and an alarm bit for humidity when above 80%. In contrast,
 * an error bit applies to the whole sensor and typically would indicate an issue with the sensor. For example if
 * the system was unable to communicate with the sensor.
 *
 * @param asset_builder     Pointer to an epcp_builder_asset struct
 * @param sensor_id         Sensor that includes the data that we are adding the alarm to
 * @param error_status      true if error is active, else false
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum
 */
EPCP_ERROR_CODE asset_set_error(epcp_builder_asset *asset_builder, EPCP_SENSOR_ID sensor_id, bool error_status);

/**
 * @brief Retrieves the packet size. The packet size can also be retrieved during packet retrieval with asset_get_packet()
 *
 * @param asset_builder     Pointer to an epcp_builder_asset struct
 * @return uint16_t         Size of the completed packet in bytes
 */
uint16_t asset_get_packet_size(epcp_builder_asset *asset_builder);

/**
 * @brief Generates the final packet consisting of asset sensor subpackets, and returns
 * a pointer to that dynamically created packet.
 *
 * @param asset_builder     Pointer to an epcp_builder_asset struct
 * @param uint8_t*          Pointer to the completed packet
 * @return uint16_t         Size of the completed packet
 */
uint16_t asset_get_packet(epcp_builder_asset *asset_builder, char *ret_packet);


#endif