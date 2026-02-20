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
 * \file htu21d.h
 *
 * \brief htu21 Temperature & Humidity sensor driver header file
 * 
 * @see Error codes:    https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v17.1.0%2Fgroup__nrfx__error__codes.html
 *
 * Copyright (c) 2016 Measurement Specialties. All rights reserved.
 *
 */


#ifndef HTU21_H_INCLUDED
#define HTU21_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "nrfx_twi.h"

// Enums
enum htu21_i2c_master_mode {
	htu21_i2c_hold,
	htu21_i2c_no_hold
};

enum htu21_status {
	htu21_status_ok,
	htu21_status_no_i2c_acknowledge,
	htu21_status_i2c_transfer_error,
	htu21_status_crc_error
};

enum htu21_resolution {
	htu21_resolution_t_14b_rh_12b = 0,
	htu21_resolution_t_12b_rh_8b,
	htu21_resolution_t_13b_rh_10b,
	htu21_resolution_t_11b_rh_11b
};

enum htu21_battery_status {
	htu21_battery_ok,
	htu21_battery_low
};

enum htu21_heater_status {
	htu21_heater_off,
	htu21_heater_on
};

// Functions

/**
 * \brief Configures the SERCOM I2C master to be used with the HTU21 device.
 */
void htu21_init(nrfx_twi_t twi);

/**
 * \brief Check whether HTU21 device is connected
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_is_connected(void);

/**
 * \brief Reset the HTU21 device
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_reset(void);

/**
 * \brief Reads the htu21 serial number.
 *
 * \param[out] uint64_t* : Serial number
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_read_serial_number(uint64_t *);

/**
 * \brief Set temperature and humidity ADC resolution.
 *
 * \param[in] htu21_resolution : Resolution requested
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_set_resolution(enum htu21_resolution);

/**
 * \brief Set I2C master mode. 
 *        This determines whether the program will hold while ADC is accessed or will wait some time
 *
 * \param[in] htu21_i2c_master_mode : I2C mode
 *
 */
void htu21_set_i2c_master_mode(enum htu21_i2c_master_mode);

/**
 * \brief Reads the relative humidity value.
 *
 * \param[out] float* : Celsius Degree temperature value
 * \param[out] float* : %RH Relative Humidity value
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_read_temperature_and_relative_humidity( float *temperature, float *humidity);

/**
 * \brief Provide battery status
 *
 * \param[out] htu21_battery_status* : Battery status
 *                      - htu21_battery_ok,
 *                      - htu21_battery_low
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_get_battery_status(enum htu21_battery_status*);

/**
 * \brief Enable heater
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_enable_heater(void);

/**
 * \brief Disable heater
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_disable_heater(void);

/**
 * \brief Get heater status
 *
 * \param[in] htu21_heater_status* : Return heater status (above or below 2.5V)
 *	                    - htu21_heater_off,
 *                      - htu21_heater_on
 *
 * \return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t htu21_get_heater_status(enum htu21_heater_status*);

/**
 * \brief Returns result of compensated humidity
 *
 * \param[in] float - Actual temperature measured (degC)
 * \param[in] float - Actual relative humidity measured (%RH)
 *
 * \return float - Compensated humidity (%RH).
 */
float htu21_compute_compensated_humidity(float,float);

/**
 * \brief Returns the computed dew point
 *
 * \param[in] float - Actual temperature measured (degC)
 * \param[in] float - Actual relative humidity measured (%RH)
 *
 * \return float - Dew point temperature (DegC).
 */
float htu21_compute_dew_point(float,float);

#endif /* HTU21_H_INCLUDED */