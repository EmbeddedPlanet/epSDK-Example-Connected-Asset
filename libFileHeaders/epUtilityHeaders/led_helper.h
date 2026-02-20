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
 * @file    led_helper.h
 * @version 0.0.2
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    02 APR 2023
 * 
 * @brief Utility for the configuration and control of the LEDs on EP products.
 * 
 * Built for use with the nRF5 SDK 17.1 and FreeRTOS.
 * 
 * Versions:
 * 0.0.1 - Initial
 * 0.0.2 - Added led_mask to led_mode to control specific LEDs
 */

#ifndef LED_HELPER_H
#define LED_HELPER_H

#include <stdbool.h>
#include "main.h"

#ifndef LED_SLOW_BLINK_LENGTH
    #define LED_SLOW_BLINK_LENGTH               pdMS_TO_TICKS(1000)     /** < Amount of time, in milliseconds, that the LED remains on during slow blinks*/
#endif

#ifndef LED_SLOW_BLINK_INTERVAL
    #define LED_SLOW_BLINK_INTERVAL             pdMS_TO_TICKS(1000)     /** < Amount of time, in milliseconds, between slow blinks */
#endif

#ifndef LED_FAST_BLINK_LENGTH
    #define LED_FAST_BLINK_LENGTH               pdMS_TO_TICKS(200)      /** < Amount of time, in milliseconds, that the LED remains on during fast blinks */
#endif

#ifndef LED_FAST_BLINK_INTERVAL
    #define LED_FAST_BLINK_INTERVAL             pdMS_TO_TICKS(200)      /** < Amount of time, in milliseconds, between fast blinks */
#endif

#ifndef LED_EXTRA_FAST_BLINK_LENGTH
    #define LED_EXTRA_FAST_BLINK_LENGTH         pdMS_TO_TICKS(75)       /** < Amount of time, in milliseconds, that the LED remains on during extra fast blinks */
#endif

#ifndef LED_EXTRA_FAST_BLINK_INTERVAL
    #define LED_EXTRA_FAST_BLINK_INTERVAL       pdMS_TO_TICKS(75)       /** < Amount of time, in milliseconds, between extra fast blinks */
#endif

#ifndef LED_ALIVE_BLINK_LENGTH
    #define LED_ALIVE_BLINK_LENGTH              pdMS_TO_TICKS(100)      /** < Amount of time, in milliseconds, that the LED remains on during alive blinks */
#endif

#ifndef LED_ALIVE_BLINK_INTERVAL
    #define LED_ALIVE_BLINK_INTERVAL            pdMS_TO_TICKS(15000)    /** < Amount of time, in milliseconds, between alive blinks */
#endif

#ifndef LED_MULTI_BLINK_LENGTH
    #define LED_MULTI_BLINK_LENGTH              pdMS_TO_TICKS(135)      /** < Amount of time, in milliseconds, that the LED remains on during multi-blink patterns */
#endif

#ifndef LED_MULTI_BLINK_INTERVAL
    #define LED_MULTI_BLINK_INTERVAL            pdMS_TO_TICKS(1000)     /** < Amount of time, in milliseconds, between slow multi-blinks */
#endif

/**
 * @brief Passed into the led_mode() function to set the active blink mode of the LED
 * 
 */
typedef enum{
    LED_OFF,                /** < LED off */
    LED_ON,                 /** < LED on */
    LED_ALIVE_BLINK,        /** < LED turns on for LED_ALIVE_BLINK_LENGTH ms every LED_ALIVE_BLINK_INTERVAL ms */
    LED_SLOW_BLINK,         /** < LED turns on for LED_SLOW_BLINK_LENGTH ms every LED_SLOW_BLINK_INTERVAL ms */
    LED_FAST_BLINK,         /** < LED turns on for LED_FAST_BLINK_LENGTH ms every LED_FAST_BLINK_INTERVAL ms */
    LED_EXTRA_FAST_BLINK,   /** < LED turns on for LED_EXTRA_FAST_BLINK_LENGTH ms every LED_EXTRA_FAST_BLINK_INTERVAL ms */
    LED_SINGLE_BLINK,       /** < LED flashes on for LED_MULTI_BLINK_LENGTH ms once every LED_MULTI_BLINK_INTERVAL ms */
    LED_DOUBLE_BLINK,       /** < LED flashes on for LED_MULTI_BLINK_LENGTH ms twice every LED_MULTI_BLINK_INTERVAL ms */
    LED_TRIPLE_BLINK,       /** < LED flashes on for LED_MULTI_BLINK_LENGTH ms three times every LED_MULTI_BLINK_INTERVAL ms */
    LED_QUADRUPLE_BLINK,    /** < LED flashes on for LED_MULTI_BLINK_LENGTH ms four times every LED_MULTI_BLINK_INTERVAL ms */
    LED_QUINTUPLE_BLINK     /** < LED flashes on for LED_MULTI_BLINK_LENGTH ms five times every LED_MULTI_BLINK_INTERVAL ms */
} led_mode_enum;

/**
 * @brief Configures the LED helper utility and creates the LED FreeRTOS task that runs in the background.
 * Must be called before interacting with the LED utility via the led_mode function.
 * 
 * @return bool true for success, 0 for failure
 */
bool led_init();

/**
 * @brief Pauses the LED helper utility and the LED FreeRTOS task that runs in the background.
 * 
 * @return bool true for success, 0 for failure
 */
bool led_pause();

/**
 * @brief Resume the LED helper utility and the LED FreeRTOS task that runs in the background.
 * 
 * @return bool true for success, 0 for failure
 */
bool led_resume();

/**
 * @brief Sets the active mode of the LED
 * 
 * @param mode      led_mode_enum will set the active mode.  Both the LED_OFF and LED_ON modes ignore the latch argument.
 * @param latch     true to latch the mode, causing the pattern to repeat until the next call of led_mode. For example,
 * if led_mode(LED_FAST_BLINK, false,) is called, the LED will only perform a single fast blink and turn off. If 
 * led_mode(LED_FAST_BLINK, true,) is called, the LED will continue performing fast blinks until the next call of led_mode.
 * @param led_mask  Binary mask to set active LEDs, using 2^(n-1), where 'n' is the LED or LEDs of interest.  For example: 
 * If the device supports a single LED the mask is set to 1.  For three LEDs: 1,2, or 4 set the indivual LEDs
 * with 3 setting LED 1 and 2, and 5,6 and 7 setting additional combinations of the LEDs.
 * 
 */
void led_mode(led_mode_enum mode, bool latch, uint8_t led_mask);

#endif