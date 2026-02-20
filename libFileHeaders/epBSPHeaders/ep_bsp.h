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
 * @file    ep_bsp.h
 * @version 1.0.0
 * @author  Embedded Planet, Inc.
 * @author  golobmichael
 * @date    01 APR 2023
 * 
 * @brief Contains board support functions for Embedded Planet nRF52840 boards.
 * versions:
 *  1.0.0 - Initial
 * 
 * Built for use with the nRF SDK 17.1 and FreeRTOS.
 */

#ifndef EP_BSP_H
#define EP_BSP_H

#include <stdint.h>
#include <stdbool.h>
#include "nrf_drv_wdt.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEFINES

// Watchdog global channel
extern nrf_drv_wdt_channel_id m_channel_id;
// Watchdog reload value
extern uint32_t watchdogReload;

#define NRF_LF_CLK_MODE CLOCK_LFRCMODE_MODE_ULP

#define NRF52_ONRAM1_OFFRAM1    POWER_RAM_POWER_S0POWER_On      << POWER_RAM_POWER_S0POWER_Pos      \
                              | POWER_RAM_POWER_S1POWER_On      << POWER_RAM_POWER_S1POWER_Pos      \
                              | POWER_RAM_POWER_S0RETENTION_On  << POWER_RAM_POWER_S0RETENTION_Pos  \
                              | POWER_RAM_POWER_S1RETENTION_On  << POWER_RAM_POWER_S1RETENTION_Pos;

#define NRF52_ONRAM1_OFFRAM0    POWER_RAM_POWER_S0POWER_On      << POWER_RAM_POWER_S0POWER_Pos      \
                              | POWER_RAM_POWER_S1POWER_On      << POWER_RAM_POWER_S1POWER_Pos      \
                              | POWER_RAM_POWER_S0RETENTION_Off << POWER_RAM_POWER_S0RETENTION_Pos  \
                              | POWER_RAM_POWER_S1RETENTION_Off << POWER_RAM_POWER_S1RETENTION_Pos;

#define NRF52_ONRAM0_OFFRAM0    POWER_RAM_POWER_S0POWER_Off     << POWER_RAM_POWER_S0POWER_Pos      \
                              | POWER_RAM_POWER_S1POWER_Off     << POWER_RAM_POWER_S1POWER_Pos;

//ADC voltage monitor scale
#if defined (BOARD_GALAXIS)
// System voltage should be 3.0V, with the voltage divider cutting the voltage in half
// This voltage is set for the Galaxis with the bsp_init function
static const float SYSTEM_VOLTAGE_SCALAR = 3.0 * 2;
#else
// System voltage should be 3.3V, with the voltage divider cutting the voltage in half
static const float SYSTEM_VOLTAGE_SCALAR = 3.3 * 2;
#endif


/**
 * @brief   Initializes Agora Power System.  This command makes
 *          use of UART messages and should be added AFTER UART
 *          initialization.  
 * 
 * @param[in] reloadValue Watchdog reload value in seconds.
 * @param[in] reloadRate Watchdog reload rate in seconds.
 */
void ep_bsp_init( uint32_t reloadValue, uint32_t reloadRate );

/**
 * @brief   Restore watch dog timer after sleep
 * 
 * @return  Return battery voltage as floating point voltage
 */
float ep_bsp_read_battery_voltage();


#ifdef __cplusplus
}
#endif

#endif // EP_BSP_H