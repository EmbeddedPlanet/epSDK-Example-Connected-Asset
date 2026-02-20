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
 * @file    icm20602.h
 * @version 1.0.0
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    21 Sept 2022
 * 
 * @brief Contains functions for the configuration and control of the ICM-20602 6-axis IMU.
 * 
 * Built for use with the nRF SDK 17.1
 * 
 * @see Data sheet:     https://invensense.tdk.com/wp-content/uploads/2016/10/DS-000176-ICM-20602-v1.0.pdf
 * @see Error codes:    https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v17.1.0%2Fgroup__nrfx__error__codes.html
 */

#ifndef ICM20602_H
#define ICM20602_H

#include "nrfx_twi.h"

/** @brief TWI/I2C device address options of the icm20602
*/
typedef enum{
    ICM20602_ADDR_HIGH = 0x69,  /** < Device address when SA0 is logic high */
    ICM20602_ADDR_LOW = 0x68    /** < Device address when SA0 is logic low */
} icm20602_dev_addr;

/** @brief Used to hold twi configuration and register data of a single icm20602. Required prior to call to icm20602_init.
*/
typedef struct{
    nrfx_twi_t twi;             /** < TWI interface that hosts the icm20602 */
    icm20602_dev_addr address;  /** < Address of the icm20602. */
    float accel_divisor;        /** < Accel scale. Defaults to ICM_ACCEL_DIV_0 */
    float gyro_divisor;         /** < Gyro scale. Defaults to ICM_GYRO_DIV_0 */

    /** @brief The following are mirrors of the configuration registers on the device
     * that are used within this driver. This list is not all-inclusive, as some
     * devices features are not implemented in this driver. The purpose of these
     * mirror registers is to allow the ability to configure the device only once,
     * so if the sensor is powered down we can resync the configuration via icm20602_send_config. 
    */
    uint8_t reg_smplrt_div;
    uint8_t reg_config;
    uint8_t reg_accel_wom_x;
    uint8_t reg_accel_wom_y;
    uint8_t reg_accel_wom_z;
    uint8_t reg_int_pin_cfg;
    uint8_t reg_int_en;
    uint8_t reg_gyro_config;
    uint8_t reg_accel_config;
    uint8_t reg_accel_config2;
    uint8_t reg_accel_intel_ctrl;
    uint8_t reg_pwr_mgmt_1;
    uint8_t reg_pwr_mgmt_2;
} ICM20602;


/** @brief Used to hold output data from the accel/gyro/temp sensor
*/
typedef struct{
    float accel_x;  /** < Acceleration on x-axis in gs. */
    float accel_y;  /** < Acceleration on y-axis in gs. */
    float accel_z;  /** < Acceleration on z-axis in gs. */
    float gyro_x;   /** < Angular velocity on x-axis in dps. */
    float gyro_y;   /** < Angular velocity on y-axis in dps. */
    float gyro_z;   /** < Angular velocity on z-axis in dps. */
    float temp;     /** < Temperature in degrees C. */
} icm20602_sensor_data;

/** @brief Accel full scale selection enum.
 */
typedef enum{
    ICM_ACCEL_2G,
    ICM_ACCEL_4G,
    ICM_ACCEL_8G,
    ICM_ACCEL_16G
} icm20602_accel_scale;

/** @brief Gyro full scale selection enum.
 */
typedef enum{
    ICM_GYRO_250DPS,
    ICM_GYRO_500DPS,
    ICM_GYRO_1000DPS,
    ICM_GYRO_2000DPS
} icm20602_gyro_scale;

/** @brief Wake on Motion mode enum.
 */
typedef enum{
    ICM_WOM_OR,     /** < OR mode: interrupt will trigger if any WoM activated axis breach the threshold */
    ICM_WOM_AND     /** < AND mode: interrupt will not trigger unless all WoM activated axis breach the threshold */
} icm20602_wom_mode;

/** @brief Wake on Motion compare mode enum.
 */
typedef enum{
    ICM_WOM_COMPARE_STARTUP,    /** < Records accel sample at startup. All WoM comparisons are performed based on this sample. */
    ICM_WOM_COMPARE_LAST        /** < Compares new sample with previous sample to determine WoM. */
} icm20602_wom_compare;

/** @brief Filter settings for gyro. Be aware that these affect the temperature sensor as well.
 */ 
typedef enum{
    ICM_GYRO_250HZ_1KHZ     = 0x00, /** < 3dB BW = 250Hz, Noise BW = 306.6Hz, Rate = 8kHz, (Temp 3dB BW = 4000Hz) */
    ICM_GYRO_176HZ_1KHZ     = 0x01, /** < 3dB BW = 176Hz, Noise BW = 177.0Hz, Rate = 1kHz, (Temp 3dB BW = 188Hz) */
    ICM_GYRO_92HZ_1KHZ      = 0x02, /** < 3dB BW = 92Hz, Noise BW = 108.6Hz, Rate = 1kHz, (Temp 3dB BW = 98Hz) */
    ICM_GYRO_41HZ_1KHZ      = 0x03, /** < 3dB BW = 41Hz, Noise BW = 59.0Hz, Rate = 1kHz, (Temp 3dB BW = 42Hz) */
    ICM_GYRO_25HZ_1KHZ      = 0x04, /** < 3dB BW = 20Hz, Noise BW = 30.5Hz, Rate = 1kHz, (Temp 3dB BW = 20Hz) */
    ICM_GYRO_10HZ_1KHZ      = 0x05, /** < 3dB BW = 10Hz, Noise BW = 15.6Hz, Rate = 1kHz, (Temp 3dB BW = 10Hz) */
    ICM_GYRO_5HZ_1KHZ       = 0x06, /** < 3dB BW = 5Hz, Noise BW = 8.0Hz, Rate = 1kHz, (Temp 3dB BW = 5Hz) */    
    ICM_GYRO_3281HZ_8KHZ    = 0x07, /** < 3dB BW = 3281Hz, Noise BW = 3451.0Hz, Rate = 8kHz, (Temp 3dB BW = 4000Hz) */
    ICM_GYRO_8173HZ_32KHZ   = 0x08, /** < 3dB BW = 8173Hz, Noise BW = 8595.1Hz, Rate = 32kHz, (Temp 3dB BW = 4000Hz) */
    ICM_GYRO_3281HZ_32KHZ   = 0x10  /** < 3dB BW = 3281Hz, Noise BW = 3451.0Hz, Rate = 32kHz, (Temp 3dB BW = 4000Hz) */
} icm20602_gyro_lpf;

/** @brief Filter settings for accel. Some of these settings are incompatible with Wake on Motion, see datasheet for details.
 */ 
typedef enum{
    ICM_ACCEL_218HZ_1KHZ    = 0x00, /** < 3dB BW = 218.1Hz, Noise BW = 235.0Hz, Rate = 1kHz */
    ICM_ACCEL_99HZ_1KHZ     = 0x02, /** < 3dB BW = 99.0Hz, Noise BW = 121.3Hz, Rate = 1kHz */
    ICM_ACCEL_45HZ_1KHZ     = 0x03, /** < 3dB BW = 44.8Hz, Noise BW = 61.5Hz, Rate = 1kHz */
    ICM_ACCEL_21HZ_1KHZ     = 0x04, /** < 3dB BW = 21.2Hz, Noise BW = 31.0Hz, Rate = 1kHz */
    ICM_ACCEL_10HZ_1KHZ     = 0x05, /** < 3dB BW = 10.2Hz, Noise BW = 15.5Hz, Rate = 1kHz */
    ICM_ACCEL_5HZ_8KHZ      = 0x06, /** < 3dB BW = 5.1Hz, Noise BW = 7.8Hz, Rate = 1kHz */
    ICM_ACCEL_420HZ_1KHZ    = 0x07, /** < 3dB BW = 420.0Hz, Noise BW = 441.6Hz, Rate = 1kHz */
    ICM_ACCEL_1046HZ_4KHZ   = 0x08  /** < 3dB BW = 1046.0Hz, Noise BW = 1100.0Hz, Rate = 4kHz */
} icm20602_accel_lpf;


/***************************************
 * Public Functions
 ***************************************/

/**
 * @brief   Inits an icm20602 struct with a device address & the twi used.
 * 
 * @param icm       Pointer to an ICM20602 struct
 * @param address   Address of the icm20602
 * @param twi       The twi port of the nRF used
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_init(ICM20602 *icm, icm20602_dev_addr address, nrfx_twi_t twi);

/**
 * @brief   Sets of the Wake on Motion parameters.
 * Note: This implementation follows the steps in the datasheet section 5.1.
 * Note: This function needs work, it does not trigger the int pin as intended during a WoM event.
 * 
 * @param icm           Pointer to an ICM20602 struct
 * @param x_threshold   The WoM interrupt threshold in milli-gs.
 *                      0 will deactivate the interrupt for this axis
 *                      1-1020 will set the the interrupt threshold for this axis
 * @param y_threshold   The WoM interrupt threshold in milli-gs.
 *                      0 will deactivate the interrupt for this axis
 *                      1-1020 will set the the interrupt threshold for this axis
 * @param z_threshold   The WoM interrupt threshold in milli-gs.
 *                      0 will deactivate the interrupt for this axis
 *                      1-1020 will set the the interrupt threshold for this axis
 * @param wom_mode      ICM_WOM_OR  Interrupt will trigger if any WoM activated axis breach the threshold
 *                      ICM_WOM_AND Interrupt will not trigger unless all WoM activated axis breach the threshold
 * @param wom_compare   ICM_WOM_COMPARE_STARTUP Records accel sample at startup. All WoM comparisons are performed based on this sample.
 *                      ICM_WOM_COMPARE_LAST    Compares new sample with previous sample to determine WoM.
 * @param rate_divisor  Determines the sample rate. Input range is 1-255 which the internal sample rate (1kHz) will be divided by. 
 *                      Sample rate = rate_divisor / 1kHz
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_wom(ICM20602 *icm, uint16_t x_threshold, uint16_t y_threshold, uint16_t z_threshold,  icm20602_wom_mode wom_mode, 
   icm20602_wom_compare wom_compare, uint8_t rate_divisor);

/**
 * @brief Sets the low-pass filter of the ICM20602 gyroscope.
 * 
 * @param icm       Pointer to an ICM20602 struct
 * @param filter    Filter to be used
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_set_gyro_lpf(ICM20602 *icm, icm20602_gyro_lpf filter);

/**
 * @brief Sets the low-pass filter of the ICM20602 accelerometer. Note that not all filters are compatible with the Wake on Motion feature
 * See the datasheet for details.
 * 
 * @param icm       Pointer to an ICM20602 struct
 * @param filter    Filter to be used
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_set_accel_lpf(ICM20602 *icm, icm20602_accel_lpf filter);

/**
 * @brief   Sets the scale for the gyro and accel. Default is 2g and 250dps
 * 
 * @param icm           Pointer to an ICM20602 struct
 * @param accel_scale   Accel scale to be used
 * @param gyro_scale    Gyro scale to be used
 * 
 * @return  0 if successful. 
 *          1 if accel_scale out of range
 *          2 is gyro_scale out of range
 *          For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_set_scale(ICM20602 *icm, icm20602_accel_scale accel_scale, icm20602_gyro_scale gyro_scale);

/**
 * @brief   Checks reg 0x3A bit 0 to see if data is ready for retrieval
 * 
 * @param icm       Pointer to an ICM20602 struct
 * @param ret_code  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 * 
 * @return  false if data is not ready
 *          true if data is ready
 */
bool icm20602_data_ready(ICM20602 *icm, nrfx_err_t *ret_code);

/**
 * @brief   Retrieves most recent measurement from the system, and populates a results struct.
 * 
 * @param icm       Pointer to an ICM20602 struct
 * @param ret_code  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 * 
 * @return  icm20602_sensor_data struct which holds the return data
 */
icm20602_sensor_data icm20602_get_data(ICM20602 *icm, nrfx_err_t *ret_code);

/**
 * @brief   Resets the internal registers and restores the default settings. 
 * 
 * @param icm       Pointer to an ICM20602 struct
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_reset(ICM20602 *icm);

/**
 * @brief   Puts the sensor to sleep or wakes it back up.
 * 
 * @param icm           Pointer to an ICM20602 struct
 * @param sleep_active  true to put ICM to sleep, false to wake it up
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_sleep(ICM20602 *icm, bool sleep_active);

/**
 * @brief Sends all locally stored mirror registers back down to the device. Useful for after a device restart
 * or power-down to avoid a complete reconfigure.
 * 
 * @param icm   Pointer to an ICM20602 struct
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t icm20602_config_send(ICM20602 *icm);

#endif