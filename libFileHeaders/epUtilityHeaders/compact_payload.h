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
 * @file    compact_payload.h
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

#ifndef COMPACT_PAYLOAD_H
#define COMPACT_PAYLOAD_H

#include "ep_compact_payload.h"
#include "compact_payload_types.h"

/*
EPCP payload format per subpacket:
+-----------+--------------+---------+
| Sensor ID | Sensor Error |  Data   |
+-----------+--------------+---------+
| 7 bits    | 1 bit        | N Bytes |
+-----------+--------------+---------+
*/

/**
 * @brief Used to initialize the epcp_builder_beacon_gw struct as well as all subpacket structs.
 */
void compact_payload_init(void);

/**
 * @brief Used to add sensor data to the packet
 *
 * @param sensor_id         Sensor we are adding the data for
 * @param sensor_slot       Sensor slot that we are adding the data into
 * @param sensor_data       Pointer to the actual data we are adding. Must be in uint* format. Use the included epcp_convert_type helper union for conversion
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum
 */
EPCP_ERROR_CODE compact_payload_add_data(EPCP_SENSOR_ID sensor_id , EPCP_SENSOR_SLOT_TYPE sensor_slot, void * sensor_data);

/**
 * @brief Sets an alarm value as active or inactive for a sensor data slot. For an error bit that
 * applies to the whole sensor subpacket, see set_error()
 *
 * Alarms are typically set when a sensor reading violates a set parameter. For example, an environmental sensor
 * might set an alarm bit for temperature when about 25C, and an alarm bit for humidity when above 80%. In contrast,
 * an error bit applies to the whole sensor and typically would indicate an issue with the sensor. For example if
 * the system was unable to communicate with the sensor.
 *
 * @param sensor_id         Sensor that includes the data that we are adding the alarm to
 * @param sensor_slot       The sensor data slot that the alarm applies to
 * @param alarm_status      true if alarm is active, else false
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum
 */
EPCP_ERROR_CODE compact_payload_set_alarm(EPCP_SENSOR_ID sensor_id, EPCP_SENSOR_SLOT_TYPE sensor_slot, bool alarm_status);

/**
 * @brief Sets the error value for a sensor as active or inactive. This applies to the whole sensor
 * packet. For setting alarm bits for a specific sensor data slot, see set_alarm()
 *
 * Alarms are typically set when a sensor reading violates a set parameter. For example, an environmental sensor
 * might set an alarm bit for temperature when about 25C, and an alarm bit for humidity when above 80%. In contrast,
 * an error bit applies to the whole sensor and typically would indicate an issue with the sensor. For example if
 * the system was unable to communicate with the sensor.
 *
 * @param sensor_id         Sensor that includes the data that we are adding the alarm to
 * @param error_status      true if error is active, else false
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum
 */
EPCP_ERROR_CODE compact_payload_set_error(EPCP_SENSOR_ID sensor_id, bool error_status);

/**
 * @brief Retrieves the packet size. The packet size can also be retrieved during packet retrieval with beacon_gw_get_packet()
 *
 * @return uint16_t         Size of the completed packet in bytes
 */
uint16_t compact_payload_get_packet_size(void);

/**
 * @brief Generates the final packet consisting of beacon_gw sensor subpackets, and returns
 * a pointer to that dynamically created packet.
 *
 * @param uint8_t*          Pointer to the completed packet
 * @return uint16_t         Size of the completed packet
 */
uint16_t compact_payload_get_packet(char *ret_packet);


#endif