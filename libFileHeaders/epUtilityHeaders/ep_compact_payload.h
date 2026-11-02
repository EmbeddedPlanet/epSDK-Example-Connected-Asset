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
 * @file    ep_compact_payload.h
 * @version 0.0.3
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    21 DEC 2023
 *
 * @brief The EP Compact Payload Builder is used to construct packets that conform to the
 * Embedded Planet Compact Payload specification P399x024. This lower-level portion of the utility
 * is used as a foundation for additional layers that build complete device payloads.
 *
 * Built for use with the nRF5 SDK 17.1 and FreeRTOS.
 *
 * Versions:
 * 0.0.1 - Initial release in Mbed - conforms to spec version 1
 * 0.0.2 - Ported to FreeRTOS. Conforms to spec version 2
 */

/*
EPCP payload format per subpacket:
+-----------+--------------+---------+
| Sensor ID | Sensor Error |  Data   |
+-----------+--------------+---------+
| 7 bits    | 1 bit        | N Bytes |
+-----------+--------------+---------+
*/

#ifndef EP_COMPACT_PAYLOAD_H
#define EP_COMPACT_PAYLOAD_H

#include <stdint.h>
#include <stdbool.h>

#define EPCP_SLOT(slot, sz, off, alarm) \
    { .type = (slot), .size = (sz), .offset = (off), .alarm_bit = (alarm) }

/**
 * @brief Sensor alarm bits
 *
 * Used during subpacket creation to assign an alarm bit position to a sensor
 * data slot type. These are assigned per the spec P399x024.
 *
 */
typedef enum epcp_alarm_bit{
    EPCP_NO_ALARM_BIT,
    EPCP_ALARM_BIT_0,
    EPCP_ALARM_BIT_1,
    EPCP_ALARM_BIT_2,
    EPCP_ALARM_BIT_3,
    EPCP_ALARM_BIT_4,
    EPCP_ALARM_BIT_5,
    EPCP_ALARM_BIT_6,
    EPCP_ALARM_BIT_7,
    EPCP_ALARM_BIT_8,
    EPCP_ALARM_BIT_9,
    EPCP_ALARM_BIT_10,
    EPCP_ALARM_BIT_11,
    EPCP_ALARM_BIT_12,
    EPCP_ALARM_BIT_13,
    EPCP_ALARM_BIT_14,
    EPCP_ALARM_BIT_15,
} EPCP_ALARM_BIT;

/**
 * @brief EPCP Error Codes
 *
 * These values are returned by the functions in this library.
 *
 */
typedef enum epcp_error_code{
    EPCP_SUCCESS,
    EPCP_INVALID_SENSOR,
    EPCP_INVALID_DATA_SLOT,
    EPCP_INVALID_DATA,
    EPCP_NO_ALARM_MAP,
    EPCP_ALARM_MAP_SIZE_MISMATCH,
    EPCP_PACKET_SIZE_MISMATCH
} EPCP_ERROR_CODE;

/**
 * @brief EPCP Convert Type Union
 *
 * This can be used in the main application to help convert sensor data to uint32_t prior
 * to adding to the packet. The packet requires all data to be in uint32_t format, but cannot
 * be cast as this will change the structure of the data.
 *
 * Example:
 * epcp_convert_type ct;
 * ct.fl = temp_sensor_get_temp();
 * add_data(EPCP_SI7021, EPCP_TEMP, ct.ui32);
 *
 */
union epcp_convert_type{
    uint32_t    ui32;
    int32_t     i32;
    float       fl;
    bool        bl;
    char*       ch;
};

/**
 * @brief EPCP slot info struct
 *
 * Holds information regarding a specific sensor slot
 */
typedef struct {
    uint32_t type;       /* EPCP_SENSOR_SLOT_TYPE */
    uint32_t size;       /* number of bytes in this slot */
    uint32_t offset;     /* byte offset within the subpacket */
    uint32_t alarm_bit;  /* alarm bit number, 0 = none */
} epcp_slot_info_t;

/**
 * @brief EPCP Sensor Structure
 *
 * Holds the packet pointer and information that is required to modify and build a packet
 */
typedef struct {
    bool active;                /** < Track whether or not there is valid data in this packet/subpacket */
    uint8_t *packet;            /** < Data Packet */
    uint32_t packet_size; /** < Size of Data Packet */
    const epcp_slot_info_t *slot_info;  /** < Array of sensor slot info structs */
    uint32_t slot_qty;              /** < Number of entries in the slot info array */
} epcp_sensor_t;

extern uint8_t epcp_ver;

/**
 * @brief Used to initialize the sensor packet struct.
 *
 * @param sensor                Pointer to an epcp_sensor_t struct
 * @param sensor_id             Predefined type of sensor we are using
 * @param sensor_err_en         Enable the error bit in byte 0
 */
void epcp_sensor_init(epcp_sensor_t *sensor, uint8_t sensor_id, bool sensor_err_en);

/**
 * @brief Used to add the actual sensor data to the packet.
 *
 * @param sensor            Pointer to an epcp_sensor_t struct
 * @param sensor_slot       Sensor slot to add the data to
 * @param sensor_data       Pointer to the actual data we are adding. Must be in uint* format. Use the included epcp_convert_type helper union for conversion
 * @param is_str            True if sensor_data is a string, else False. True causes the data to be ordered MSBF in the payload. Strings are already MSBF.
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum.
 */
EPCP_ERROR_CODE add_data(epcp_sensor_t *sensor, uint8_t sensor_slot, void * sensor_data, bool is_str);

/**
 * @brief Sets an alarm value as active or inactive for a sensor data type. For an error bit that
 * applies to the whole sensor packet, see set_error()
 *
 * Alarms are typically set when a sensor reading violates a set parameter. For example, an environmental sensor
 * might set an alarm bit for temperature when about 25C, and an alarm bit for humidity when above 80%. In contrast,
 * an error bit applies to the whole sensor and typically would indicate an issue with the sensor. For example if
 * the system was unable to communicate with the sensor.
 *
 * @param sensor            Pointer to an epcp_sensor_t struct
 * @param sensor_slot       Sensor slot that corresponds with the alarm being adjusted
 * @param alarm_status      true if alarm is active, else false
 * @return EPCP_ERROR_CODE  EPCP_SUCCESS if successful. Otherwise see the EPCP_ERROR_CODE enum.
 */
EPCP_ERROR_CODE set_alarm(epcp_sensor_t *sensor, uint8_t alarm_slot, uint8_t sensor_slot, bool alarm_status);

/**
 * @brief Sets the error value for a sensor as active or inactive. This applies to the whole sensor
 * packet. For setting alarm bits for specific sensor data types, see set_alarm()
 *
 * Alarms are typically set when a sensor reading violates a set parameter. For example, an environmental sensor
 * might set an alarm bit for temperature when about 25C, and an alarm bit for humidity when above 80%. In contrast,
 * an error bit applies to the whole sensor and typically would indicate an issue with the sensor. For example if
 * the system was unable to communicate with the sensor.
 *
 * @param sensor        Pointer to an epcp_sensor_struct
 * @param error_status  true if error is active, else false
 */
void set_error(epcp_sensor_t *sensor, bool error_status);

/**
 * @brief Get the total size of the sensor packet
 *
 * @param sensor    Pointer to an epcp_sensor_t struct
 * @return uint8_t  Size of the packet in bytes
 */
uint32_t get_packet_size(epcp_sensor_t *sensor);

/**
 * @brief Used to retrieve a copy of the current packet. A call to get_packet_size() should precede a call to this function
 * in order to initialize an array that can fit the returned packet.
 *
 * @param sensor        Pointer to an epcp_sensor_t struct
 * @param ret_packet    Pointer to a uint8_t array that is large enough to hold the packet.
 */
void get_packet(epcp_sensor_t *sensor, uint8_t* ret_packet);

#endif