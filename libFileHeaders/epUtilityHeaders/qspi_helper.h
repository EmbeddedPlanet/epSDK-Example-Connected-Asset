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
 * @file    qspi_helper.h
 * @version 0.0.1
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    1 March 2022
 * 
 * @brief Contains function definitions for the configuration and control of the nRF QSPI peripheral
 * 
 * @see Error codes:    https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v17.1.0%2Fgroup__nrfx__error__codes.html
 * 
 * Built for use with the nRF SDK 17.1
 */


#ifndef QSPI_HELPER_H
#define QSPI_HELPER_H

#include "nrf_drv_qspi.h"

/**
 * @brief Enum for QSPI read/write options
 */
typedef enum{
    QSPI_ERASE_LEN_4KB = NRF_QSPI_ERASE_LEN_4KB,
    QSPI_ERASE_LEN_64KB = NRF_QSPI_ERASE_LEN_64KB,
    QSPI_ERASE_LEN_ALL = NRF_QSPI_ERASE_LEN_ALL,
} qspi_erase_len_enum;

/**
 * @brief Initializes the NRF QSPI driver
 * 
 * @return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t qspi_init();

/**
 * @brief Unitializes the NRF QSPI driver
 * 
 */
void qspi_uninit();

/**
 * @brief Erase a sector or block of QSPI memory
 * 
 * @param erase_len         Size to erase. QSPI_ERASE_LEN_4KB, QSPI_ERASE_LEN_16KB, or QSPI_ERASE_LEN_ALL
 * @param start_address     Start address to erase. Must be in multiples of 256 (0, 256, etc)
 * @return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t qspi_erase(qspi_erase_len_enum erase_len, uint32_t start_address);

/**
 * @brief Write to QSPI memory
 * 
 * @param tx_buffer     Pointer to a buffer of data to write
 * @param length        Length of buffer
 * @param start_address Start address to write. Must be in multiples of 256 (0, 256, etc)
 * @return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t qspi_write(void const *tx_buffer, size_t length, uint32_t start_address);

/**
 * @brief Read from QSPI memory
 * 
 * @param rx_buffer     Pointer to a buffer to read to
 * @param length        Length of the buffer
 * @param start_address Start address to write
 * @return 0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
nrfx_err_t qspi_read(void *rx_buffer, size_t length, uint32_t start_address);

#endif