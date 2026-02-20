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
 * @file    agora.h
 * @author  Embedded Planet, Inc.
 * @author  M. Timieski  & Dan Maher
 * @date    21 Oct 2022
 * 
 * @brief Contains definitions for Agora (DES0263_11) support
 * 
 * Built for use with the nRF SDK 17.1
 * 
 */


#ifndef AGORA_H
#define AGORA_H

//For conversions to Agora in MBED See:
//https://github.com/EmbeddedPlanet/mbed-os/blob/master/targets/TARGET_NORDIC/TARGET_NRF5x/TARGET_NRF52/TARGET_MCU_NRF52840/TARGET_EP_AGORA/PinNames.h

//Port 0 pins 0 to 31 are accessed using: "0 to 31"
//Port 1 pins 0 to 31 are accessed using: "32 to 64" (pin number + 32)

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_gpio.h"

//GPIO voltage set in UICR register
//Agora GPIO voltage is driven externally and is not set.

// SDK LED definitions for Agora
#define LEDS_NUMBER    1

#define LED_1          NRF_GPIO_PIN_MAP(0,5)

#define LED_START      LED_1
#define LED_STOP       LED_1

#define LEDS_ACTIVE_STATE 0

#define LEDS_LIST { LED_1 }

#define LEDS_INV_MASK  LEDS_MASK

#define BSP_LED_0      LED_1

#define BUTTONS_NUMBER 1

#define BUTTON_1       NRF_GPIO_PIN_MAP(0,29)
#define BUTTON_PULL    NRF_GPIO_PIN_PULLUP

#define BUTTONS_ACTIVE_STATE 0

#define BUTTONS_LIST { BUTTON_1 }

#define BSP_BUTTON_0   BUTTON_1

//Agora Debug Output
#define PIN_NAME_DEBUG_RX NRF_GPIO_PIN_MAP(0,16)
#define PIN_NAME_DEBUG_TX NRF_GPIO_PIN_MAP(0,13)

/** Pin Numbering used in SDK examples */ 
#define RX_PIN_NUMBER     PIN_NAME_DEBUG_RX
#define TX_PIN_NUMBER     PIN_NAME_DEBUG_TX
#define CTS_PIN_NUMBER    UART_PIN_DISCONNECTED
#define RTS_PIN_NUMBER    UART_PIN_DISCONNECTED
#define HWFC              false  //Hardware Flow control

//Agora Cell connections
#define CELL_PWR_EN                 NRF_GPIO_PIN_MAP(0, 28)
#define CELL_ON_OFF                 NRF_GPIO_PIN_MAP(0, 30)
#define CELL_PWRMON                 NRF_GPIO_PIN_MAP(1, 15) //Differs from Galaxis
#define CELL_HW_SHUTDOWN            NRF_GPIO_PIN_MAP(0, 24)
#define CELL_RX_PIN_NUMBER          NRF_GPIO_PIN_MAP(1, 1)
#define CELL_TX_PIN_NUMBER          NRF_GPIO_PIN_MAP(1, 2)
#define CELL_RTS_PIN_NUMBER         NRF_GPIO_PIN_MAP(0, 14)
#define CELL_CTS_PIN_NUMBER         NRF_GPIO_PIN_MAP(1, 8)
#define CELL_DTR_PIN_NUMBER         NRF_GPIO_PIN_MAP(1,4)
#define CELL_DSR_PIN_NUMBER         NRF_GPIO_PIN_MAP(1,3)
#define CELL_DCD_PIN_NUMBER         NRF_GPIO_PIN_MAP(0,15)

// Sensors (I2C)
#define PIN_NAME_SENSOR_POWER_ENABLE    NRF_GPIO_PIN_MAP(0,31)
#define PIN_NAME_SDA                    NRF_GPIO_PIN_MAP(0,26)
#define PIN_NAME_SCL                    NRF_GPIO_PIN_MAP(0,27)

// Miscellaneous I/O
#define PIN_NAME_PUSH_BUTTON        BUTTON_1
#define PIN_NAME_LED_RED            LED_1  //Red
#define PIN_NAME_BT840_RESETN       NRF_GPIO_PIN_MAP(0,18)
#define PIN_NAME_BT840_SWO          NRF_GPIO_PIN_MAP(1,0)
#define BAT_MON_EN_PIN              NRF_GPIO_PIN_MAP(1,11)
#define PIN_NAME_I2S_SCK            NRF_GPIO_PIN_MAP(0,6)

//QSPI I/O
#define BSP_QSPI_SCK_PIN    NRF_GPIO_PIN_MAP(0,19)
#define BSP_QSPI_CSN_PIN    NRF_GPIO_PIN_MAP(0,17)
#define BSP_QSPI_IO0_PIN    NRF_GPIO_PIN_MAP(0,20)
#define BSP_QSPI_IO1_PIN    NRF_GPIO_PIN_MAP(0,21)
#define BSP_QSPI_IO2_PIN    NRF_GPIO_PIN_MAP(0,22)
#define BSP_QSPI_IO3_PIN    NRF_GPIO_PIN_MAP(0,23)

//GPIO
#define PIN_NAME_TE_GPIO1   CELL_ON_OFF
#define PIN_NAME_TE_GPIO2   PIN_NAME_SENSOR_POWER_ENABLE
#define PIN_NAME_TE_GPIO3   NRF_GPIO_PIN_MAP(0,25)
#define PIN_NAME_TE_GPIO4   NRF_GPIO_PIN_MAP(0,8)
#define PIN_NAME_TE_GPIO5   PIN_NAME_I2S_SCK

//Sensor Interrupts
#define PIN_NAME_INT_TOF    NRF_GPIO_PIN_MAP(0,4)
#define PIN_NAME_INT_ACCEL  NRF_GPIO_PIN_MAP(1,5)

//Set low to enable DB_ID analog signal
#define PIN_NAME_BD_ID_NEN  NRF_GPIO_PIN_MAP(1,6)

//LoRa SPI Signals
#define PIN_NAME_LORA_SCLK  NRF_GPIO_PIN_MAP(0,7)
#define PIN_NAME_LORA_MOSI  NRF_GPIO_PIN_MAP(0,11)
#define PIN_NAME_LORA_MISO  NRF_GPIO_PIN_MAP(0,12)
#define PIN_NAME_LORA_NSS   NRF_GPIO_PIN_MAP(1,9)
#define PIN_NAME_LORA_NRST  NRF_GPIO_PIN_MAP(1,10)
#define PIN_NAME_LORA_DIO0  NRF_GPIO_PIN_MAP(1,7)
#define PIN_NAME_LORA_DIO1  NRF_GPIO_PIN_MAP(1,12)
#define PIN_NAME_LORA_DIO2  NRF_GPIO_PIN_MAP(1,13)
#define PIN_NAME_LORA_DIO3  NRF_GPIO_PIN_MAP(1,14)

//General purpose SPI port mapping
#define SER_CON_SPIS_SCK_PIN        PIN_NAME_LORA_SCLK        // SPI SCK signal.
#define SER_CON_SPIS_MOSI_PIN       PIN_NAME_LORA_MOSI        // SPI MOSI signal.
#define SER_CON_SPIS_MISO_PIN       PIN_NAME_LORA_MISO        // SPI MISO signal.
#define SER_CON_SPIS_CSN_PIN        PIN_NAME_LORA_NSS         // SPI CSN signal.
#define SER_CON_SPIS_REQ_PIN        PIN_NAME_LORA_DIO3        // SPI REQUEST GPIO pin number.

// Arduino board mappings
#define ARDUINO_SCL_PIN             PIN_NAME_SCL     // SCL signal pin
#define ARDUINO_SDA_PIN             PIN_NAME_SDA    // SDA signal pin
#define ARDUINO_AREF_PIN            2     // Aref pin

#define ARDUINO_13_PIN              CELL_PWRMON  // Digital pin 13
#define ARDUINO_12_PIN              PIN_NAME_LORA_DIO3  // Digital pin 12
#define ARDUINO_11_PIN              PIN_NAME_LORA_DIO2  // Digital pin 11
#define ARDUINO_10_PIN              PIN_NAME_LORA_DIO1  // Digital pin 10
#define ARDUINO_9_PIN               PIN_NAME_LORA_NRST  // Digital pin 9
#define ARDUINO_8_PIN               PIN_NAME_LORA_NRST  // Digital pin 8

#define ARDUINO_7_PIN               CELL_CTS_PIN_NUMBER // Digital pin 7
#define ARDUINO_6_PIN               PIN_NAME_LORA_DIO0 // Digital pin 6
#define ARDUINO_5_PIN               PIN_NAME_BD_ID_NEN // Digital pin 5
#define ARDUINO_4_PIN               PIN_NAME_INT_ACCEL // Digital pin 4
#define ARDUINO_3_PIN               CELL_DTR_PIN_NUMBER // Digital pin 3
#define ARDUINO_2_PIN               CELL_DSR_PIN_NUMBER // Digital pin 2
#define ARDUINO_1_PIN               CELL_TX_PIN_NUMBER // Digital pin 1
#define ARDUINO_0_PIN               CELL_RX_PIN_NUMBER // Digital pin 0

#define ARDUINO_A0_PIN              3     // Analog channel 0
#define ARDUINO_A1_PIN              4     // Analog channel 1
#define ARDUINO_A2_PIN              28    // Analog channel 2
#define ARDUINO_A3_PIN              29    // Analog channel 3
#define ARDUINO_A4_PIN              30    // Analog channel 4
#define ARDUINO_A5_PIN              31    // Analog channel 5


#ifdef __cplusplus
}
#endif

#endif // AGORA_H
