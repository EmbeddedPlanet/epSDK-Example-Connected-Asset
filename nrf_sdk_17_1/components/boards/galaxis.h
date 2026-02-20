/****************************************************************************                                                                     *
 * Copyright (c) 2022 Embedded Planet, Inc.                                 *
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
 * @file    galaxis.h
 * @author  Embedded Planet, Inc.
 * @author  M. Timieski
 * @date    21 Oct 2022
 * 
 * @brief Contains definitions for Galaxis (DES0281_11) support
 * 
 * Built for use with the nRF SDK 17.1
 * 
 */


#ifndef GALAXIS_H
#define GALAXIS_H

//For conversions to Galaxis in MBED See:
//https://github.com/EmbeddedPlanet/alarispro-flight-recorder/blob/master/TARGET_EP_GALAXIS/PinNames.h

//The I/O pins are mapped as follows:
//NRF_GPIO_PIN_MAP(<port>,<pin>)
//The Galaxis schematic lists I/O P<port>.<pin>
// for example P0.8 on the schematic maps to NRF_GPIO_PIN_MAP(0,8)

//For directly mapped I/O:
//Port 0 pins 0 to 31 are accessed using: "0 to 31"
//Port 1 pins 0 to 31 are accessed using: "32 to 64" (pin number + 32)

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

//GPIO voltage set in UICR register: 3V0
#define REQUIRED_UICR_REGOUT0_VOUT   UICR_REGOUT0_VOUT_3V0

// SDK LED definitions for Galaxis
#define LEDS_NUMBER    3

#define LED_1          NRF_GPIO_PIN_MAP(0,8)  //Red
#define LED_2          NRF_GPIO_PIN_MAP(0,12)
#define LED_3          NRF_GPIO_PIN_MAP(1,9)  //Blue

#define LED_START      LED_1
#define LED_STOP       LED_1

//Galaxis LEDs are turned on by setting the output high
#define LEDS_ACTIVE_STATE 1

#define LEDS_LIST { LED_1, LED_2, LED_3 }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      5
#define BSP_LED_1      12
#define BSP_LED_2      41

//No button defined on the Galaxis
#define BUTTONS_NUMBER 0
#define BUTTONS_ACTIVE_STATE 0

//Galaxis Debug Output
#define PIN_NAME_DEBUG_RX NRF_GPIO_PIN_MAP(0,16)
#define PIN_NAME_DEBUG_TX NRF_GPIO_PIN_MAP(0,13)

/** Pin Numbering used in SDK examples */ 
#define RX_PIN_NUMBER     PIN_NAME_DEBUG_RX
#define TX_PIN_NUMBER     PIN_NAME_DEBUG_TX
#define CTS_PIN_NUMBER    UART_PIN_DISCONNECTED
#define RTS_PIN_NUMBER    UART_PIN_DISCONNECTED
#define HWFC           false  //hardware flow control

//Galaxis Cell connections
#define CELL_PWR_EN       NRF_GPIO_PIN_MAP(0, 28)
#define CELL_ON_OFF       NRF_GPIO_PIN_MAP(0, 30)
#define CELL_PWRMON       NRF_GPIO_PIN_MAP(0, 15)  //Differs from Agora
//#define CELL_HW_SHUTDOWN  NRF_GPIO_PIN_MAP(0, 24) //Not available on Galaxis
#define CELL_LED_PIN_NUMBER          NRF_GPIO_PIN_MAP(0,24)
#define CELL_RX_PIN_NUMBER      NRF_GPIO_PIN_MAP(1, 1)
#define CELL_TX_PIN_NUMBER      NRF_GPIO_PIN_MAP(1, 2)
#define CELL_RTS_PIN_NUMBER     NRF_GPIO_PIN_MAP(0, 14)
#define CELL_CTS_PIN_NUMBER     NRF_GPIO_PIN_MAP(1, 8)
#define CELL_DTR_PIN_NUMBER          NRF_GPIO_PIN_MAP(1,4)
//#define CELL_DSR _PIN_NUMBER         NRF_GPIO_PIN_MAP(1,3) //Not available on Galaxis
#define CELL_RING _PIN_NUMBER        NRF_GPIO_PIN_MAP(1,3)
//#define CELL_DCD_PIN_NUMBER          NRF_GPIO_PIN_MAP(0,15) //Not available on Galaxis

//The cell aux tx and rx ports are muxed with the debug UART.
//To enable the aux port populate R2 and R4 on Galaxis.
//Data is sent and read from the aux port through the debug port.

// Sensors (I2C)
#define PIN_NAME_SENSOR_POWER_ENABLE    NRF_GPIO_PIN_MAP(0,31)
#define PIN_NAME_SDA                    NRF_GPIO_PIN_MAP(0,26)
#define PIN_NAME_SCL                    NRF_GPIO_PIN_MAP(0,27)

// Sensors (SPI)
#define SER_CON_SPIS_SCK_PIN        NRF_GPIO_PIN_MAP(0,29)     // SPI SCK signal.
#define SER_CON_SPIS_MOSI_PIN       NRF_GPIO_PIN_MAP(0,3)    // SPI MOSI signal.
#define SER_CON_SPIS_MISO_PIN       NRF_GPIO_PIN_MAP(1,13)    // SPI MISO signal.
#define SER_CON_SPIS_CSN_PIN        NRF_GPIO_PIN_MAP(1,12)     // SPI CSN signal.

// Miscellaneous I/O
#define PIN_NAME_BT840_RESETN      NRF_GPIO_PIN_MAP(0,18)
#define PIN_NAME_BT840_SWO         NRF_GPIO_PIN_MAP(1,0)
#define BAT_MON_EN_PIN    NRF_GPIO_PIN_MAP(1,11)

//QSPI
#define BSP_QSPI_SCK_PIN   19
#define BSP_QSPI_CSN_PIN   17
#define BSP_QSPI_IO0_PIN   20
#define BSP_QSPI_IO1_PIN   21
#define BSP_QSPI_IO2_PIN   22
#define BSP_QSPI_IO3_PIN   23

//Misc
#define PIN_NAME_LED_RED          LED_1  //Red
#define PIN_NAME_LED_GRN          LED_2  //Green
#define PIN_NAME_LED_BLU          LED_3  //Blue
#define PIN_NAME_INT_ACCEL1       NRF_GPIO_PIN_MAP(1,5)
#define PIN_NAME_INT_ACCEL2       NRF_GPIO_PIN_MAP(0,4)
#define PIN_NAME_INT_ALT1         NRF_GPIO_PIN_MAP(0,7)
#define PIN_NAME_INT_ALT2         NRF_GPIO_PIN_MAP(0,11)

// Battery monitoring
#define PIN_NAME_BATTERY          NRF_GPIO_PIN_MAP(0,2)
#define PIN_NAME_BATTERY_MON_EN   NRF_GPIO_PIN_MAP(1,11)

//Galaxis J5 connections
#define PIN_NAME_J5_PIN3		  NRF_GPIO_PIN_MAP(1,14)
#define PIN_NAME_J5_PIN5		  NRF_GPIO_PIN_MAP(1,12)
#define PIN_NAME_J5_PIN7		  NRF_GPIO_PIN_MAP(1,10)
#define PIN_NAME_J5_PIN4		  NRF_GPIO_PIN_MAP(1,13)
#define PIN_NAME_J5_PIN6		  NRF_GPIO_PIN_MAP(0,3)
#define PIN_NAME_J5_PIN8		  NRF_GPIO_PIN_MAP(0,29)

#ifdef __cplusplus
}
#endif

#endif // GALAXIS_H