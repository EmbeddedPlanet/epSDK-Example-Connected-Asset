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
 * @file    uart_helper.h
 * @version 2.0.0
 * @author  Embedded Planet, Inc.
 * @author  Dan Maher
 * @date    18 May 2023
 * 
 * @brief Contains function to setup LIBUARTE and swo for debug output
 * 
 * @see Error codes:    https://infocenter.nordicsemi.com/index.jsp?topic=%2Fsdk_nrf5_v17.1.0%2Fgroup__nrfx__error__codes.html
 * 
 * Built for use with the nRF SDK 17.1
 * 
 */

#ifndef UART_HELPER_H
#define UART_HELPER_H

#include "bsp.h"

#ifdef FREERTOS
#include "FreeRTOS.h"
#include "queue.h"

/* Queue to handle incoming debug UART data. This queue holds characters. */
extern QueueHandle_t xDebugUartRxQueue;
#define DEBUG_UART_RX_QUEUE_SIZE   256

/* Queue to handle outgoing debug UART data. This queue holds strings. */
extern QueueHandle_t xDebugUartTxQueue;
#define DEBUG_UART_TX_QUEUE_ITEM_SIZE   1024
#define DEBUG_UART_TX_QUEUE_SIZE   10

/* Semaphore to indicate that a TX transmission has been completed. This is needed to flag retarget.c that it can move on to the next */
extern SemaphoreHandle_t xDebugUartTxSemaphore;

/* Semaphore to lock out other tasks from using retarget.c until the full transmission is complete. This is needed in addition to 
xDebugUartTxSemaphore because sometimes a message is sent character by character. Without this semaphore, if two tasks are transmitting
at the same time they will step on eachother */
extern SemaphoreHandle_t xDebugUartTxLockoutSemaphore;
#endif

/** @brief Settings for the uart_helper debug print.
 * 
 * @param dbgi  Set true to print information messages
 * @param dbgw  Set true to print warning messages
 * @param dbei  Set true to print error messages
 * @param dbg_header_style Set the debug header style (use debug_header_style enum)
 */
typedef struct {
    uint8_t dbg_header_style;    
	bool	dbgi; 
	bool	dbgw;
	bool	dbge;    
} UART_HELPER_STRUCT;

/** @brief enumeration for setting the heading style of debug prints
 * 
 * @param DEBUG_HEADER_NONE No Header information
 * @param DEBUG_HEADER_COMPACT Print the file and line number
 * @param DEBUG_HEADER_FULL Print the function, file, and line number
 */
typedef enum{
    DEBUG_HEADER_NONE    = 0x00,      
    DEBUG_HEADER_COMPACT = 0x01,     
    DEBUG_HEADER_FULL    = 0x02,     
} debug_header_style;

// At this time give main and 4 tasks ability to control uart/swo
#define MAIN_LOOP   0 // main and initialization loop
#define TASK_1      1 // sensor_sample task
#define TASK_2      2 // cell task
#define TASK_3      3 // LED task
#define TASK_4      4 // BLE
#define NUM_OF_TASKS    5

//Expose uart_helper globally
volatile extern UART_HELPER_STRUCT uart_helper;

//Color escape definitions for printing
//Normal colors (suffix N)
#define ANSI_COLOR_REDN "\033[0;31m"
#define ANSI_COLOR_GRNN "\033[0;32m"
#define ANSI_COLOR_YELN "\033[0;33m"
#define ANSI_COLOR_BLUN "\033[0;34m"
#define ANSI_COLOR_MAGN "\033[0;35m"
#define ANSI_COLOR_CYNN "\033[0;36m"
//Bold colors (suffix B)
#define ANSI_COLOR_REDB "\033[1;31m"
#define ANSI_COLOR_GRNB "\033[1;32m"
#define ANSI_COLOR_YELB "\033[1;33m"
#define ANSI_COLOR_BLUB "\033[1;34m"
#define ANSI_COLOR_MAGB "\033[1;35m"
#define ANSI_COLOR_CYNB "\033[1;36m"
//Set to normal mode (remove color)
#define ANSI_COLOR_RST "\x1b[0m" 

//MACROS FOR DEBUG PRINTING
//Use like printf, for example: DBGI ("%s register returns: %d", reg_name, reg_val);
// Macros concatenate the full debug message, print the selected header, the message, then the newline and print the string
//
//fprintf to stderr is used like printf but sends data to stderr (printf sends data to stdout).  
// sdtedd and stdout currently default to the same output destination, but could be sent to seperate
// outputs with modification to:
// compenents/libraries/uart/retarget.c, "_write" function.
//
//The default printing enables are set in uart_helper.c in the initialization of the uart_helper struct
//The default enables can be overridden by updating the structure variables.  For example, by adding
// "uart_helper.dbgi = true;" to the projects main function the debug information printing will
// then be enabled for the project.  The structure variables can be set throughout the program to 
// enable or disable debug printing in specific areas. 
//
//Colors are added for warning and error levels through escape characters defined above.
//
//Turning off debug print can cause "-Werror=unused-but-set-variable" compile errors for variables
// that are defined only for debug printing.  To supress the warning these vaiables can be created with
// a "[[maybe_unused]]" suffix.  For example: "uint8_t rxdata [[maybe_unused]];" will prevent rxdata
// from throwing the "unused" variable warning
void tx_enqueue(const char* ansi_color, const char* msg_type, const char* func, int line, const char* format, ...);
#define DBGI(...) if (uart_helper.dbgi == true){tx_enqueue(ANSI_COLOR_RST, "[INF]", __func__, __LINE__, __VA_ARGS__);}
#define DBGW(...) if (uart_helper.dbgw == true){tx_enqueue(ANSI_COLOR_BLUB, "[WRN]", __func__, __LINE__, __VA_ARGS__);}
#define DBGE(...) if (uart_helper.dbge == true){tx_enqueue(ANSI_COLOR_REDB, "[ERR]", __func__, __LINE__, __VA_ARGS__);}


/**
 * @brief   Initializes UART port for application data or sending debug information.
 * 
 * @param[in] task Tracker for calling task.
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
int init_uart(uint8_t task);

/**
 * @brief   Uninitializes UART port for application data or sending debug information.
 * 
 * @param[in] task Tracker for calling task.
 * 
 * @return  void.
 */
void uninit_uart(uint8_t task);

/**
 * @brief   Initializes the SWO port for sending debug information.
 * 
 * 
 * @return  0 if successful. For all others, refer to the Global Error Codes document at the top of this file.
 */
int init_swo(void);


#endif
