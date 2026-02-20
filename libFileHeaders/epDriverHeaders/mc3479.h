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
 * \file mc3479.h
 *
 * \brief mc3479 Accelerometer driver header file
 * 
 * @see Error codes:    https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v17.1.0%2Fgroup__nrfx__error__codes.html
 *
 * Copyright (c) 2016 Measurement Specialties. All rights reserved.
 *
 */


#ifndef MC3479_H_INCLUDED
#define MC3479_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include "nrfx_twi.h"

/** @brief TWI/I2C device address options of the mc3479
*/
typedef enum{
    MC3479_ADDR_HIGH = 0x6C,  /** < Device address when A6 is logic high */
    MC3479_ADDR_LOW = 0x4C    /** < Device address when A6 is logic low */
} mc3479_dev_addr;

/** @brief Used to hold twi configuration and register data of a single mc3479. Required prior to call to mc3479_init.
*/
typedef struct{
    nrfx_twi_t twi;             /** < TWI interface that hosts the mc3479 */
    mc3479_dev_addr address;    /** < Address of the mc3479. */

    /** @brief The following are mirrors of the configuration registers on the device
     * that are used within this driver. This list is not all-inclusive, as some
     * devices features are not implemented in this driver. The purpose of these
     * mirror registers is to allow the ability to configure the device only once,
     * so if the sensor is powered down we can resync the configuration via mc3479_send_config. 
    */
    uint8_t reg_smplrt_div;
    uint8_t reg_mode;
    uint8_t reg_int_pin_cfg;
    uint8_t reg_accel_config;
} MC3479;

/** @brief Used to hold output data from the accel sensor
*/
typedef struct{
    float accel_x;  /** < Acceleration on x-axis in gs. */
    float accel_y;  /** < Acceleration on y-axis in gs. */
    float accel_z;  /** < Acceleration on z-axis in gs. */
    float tilt;     /** < Tilt of sensor in degrees */
} mc3479_sensor_data;

/** @brief Accel full scale selection enum.
 */
typedef enum{
    MC_ACCEL_2G,
    MC_ACCEL_4G,
    MC_ACCEL_8G,
    MC_ACCEL_16G,
    MC_ACCEL_12G
} mc3479_accel_scale;

/** @brief Filter settings for accel. Some of these settings are incompatible with Wake on Motion, see datasheet for details.
 */ 
typedef enum{
    MC_ACCEL_218HZ_1KHZ    = 0x00, /** < 3dB BW = 218.1Hz, Noise BW = 235.0Hz, Rate = 1kHz */
    MC_ACCEL_99HZ_1KHZ     = 0x02, /** < 3dB BW = 99.0Hz, Noise BW = 121.3Hz, Rate = 1kHz */
    MC_ACCEL_45HZ_1KHZ     = 0x03, /** < 3dB BW = 44.8Hz, Noise BW = 61.5Hz, Rate = 1kHz */
    MC_ACCEL_21HZ_1KHZ     = 0x04, /** < 3dB BW = 21.2Hz, Noise BW = 31.0Hz, Rate = 1kHz */
    MC_ACCEL_10HZ_1KHZ     = 0x05, /** < 3dB BW = 10.2Hz, Noise BW = 15.5Hz, Rate = 1kHz */
    MC_ACCEL_5HZ_8KHZ      = 0x06, /** < 3dB BW = 5.1Hz, Noise BW = 7.8Hz, Rate = 1kHz */
    MC_ACCEL_420HZ_1KHZ    = 0x07, /** < 3dB BW = 420.0Hz, Noise BW = 441.6Hz, Rate = 1kHz */
    MC_ACCEL_1046HZ_4KHZ   = 0x08  /** < 3dB BW = 1046.0Hz, Noise BW = 1100.0Hz, Rate = 4kHz */
} mc3479_accel_lpf;

/***************************************
 * Public Functions
 ***************************************/

/**
 * @brief   Inits an mc3479 struct with a device address & the twi used.
 * 
 * @param mc        Pointer to an MC3479 struct
 * @param address   Address of the mc3479
 * @param twi       The twi port of the nRF used
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t mc3479_init(MC3479 *mc, mc3479_dev_addr address, nrfx_twi_t twi);

/**
 * @brief   Sets the scale for the accel. Default is 250dps
 * 
 * @param mc            Pointer to an MC3479 struct
 * @param accel_scale   Accel scale to be used
 * 
 * @return  0 if successful. 
 *          1 if accel_scale out of range
 *          For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t mc3479_set_scale(MC3479 *mc, mc3479_accel_scale accel_scale);

/**
 * @brief   Checks reg 0x3A bit 0 to see if data is ready for retrieval
 * 
 * @param mc        Pointer to an MC3479 struct
 * @param ret_code  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 * 
 * @return  false if data is not ready
 *          true if data is ready
 */
bool mc3479_data_ready(MC3479 *mc, nrfx_err_t *ret_code);

/**
 * @brief   Retrieves most recent measurement from the system, and populates a results struct.
 * 
 * @param mc        Pointer to an MC3479 struct
 * @param ret_code  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 * 
 * @return  mc3479_sensor_data struct which holds the return data
 */
mc3479_sensor_data mc3479_get_data(MC3479 *mc, nrfx_err_t *ret_code);

/**
 * @brief   Puts the sensor to sleep or wakes it back up.
 * 
 * @param mc            Pointer to an MC3479 struct
 * @param sleep_active  true to put MC to sleep, false to wake it up
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t mc3479_sleep(MC3479 *mc, bool sleep_active);

/**
 * @brief Sends all locally stored mirror registers back down to the device. Useful for after a device restart
 * or power-down to avoid a complete reconfigure.
 * 
 * @param mc   Pointer to an MC3479 struct
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t mc3479_config_send(MC3479 *mc);

#endif /* MC3479_H_INCLUDED */