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
 * @file    time_helper.h
 * @version 0.0.1
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    31 JUL 2023
 * 
 * @brief Utility to assist in timekeeping using RTC2 and the external crystal on LFCLK
 * 
 * Built for use with the nRF5 SDK 17.1 and FreeRTOS.
 */

#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#include <inttypes.h>

/**
 * @brief Sets the system time
 * 
 * @param seconds Unix epoch. Time in seconds elapsed since January 1, 1970
 */
void set_time(uint32_t seconds);

/**
 * @brief Gets the system time in seconds
 * 
 * @return uint32_t System time in seconds. Unix epoch. Time in seconds elapsed since January 1, 1970
 */
uint32_t get_time_s();

/**
 * @brief Gets the system time in milliseconds
 * 
 * @return uint32_t System time in seconds. Unix epoch. Time in milliseconds elapsed since January 1, 1970
 */
uint64_t get_time_ms();

#endif