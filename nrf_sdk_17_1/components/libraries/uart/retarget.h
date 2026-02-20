#ifndef RETARGET_H
#define RETARGET_H

#include "nrf_libuarte_async.h"

#ifdef FREERTOS
#include "FreeRTOS.h"
#include "semphr.h"

/**
 * @brief Added by EP for use of retarget with libuarte for the debug uart. This function initializes
 * retarget.c with the libuarte interface used for debug print / printf as well as the tx semaphore
 * needed for tx done events. Refer to the uart-helper utility in the ep-nrf repo for more information.
 * 
 * @param p_libuarte 
 * @param dbg_tx_semaphore 
 */
void retarget_init(const nrf_libuarte_async_t * const p_libuarte, SemaphoreHandle_t dbg_tx_semaphore, SemaphoreHandle_t dbg_tx_lock_semaphore, volatile bool *tx_done);

#else
/**
 * @brief Added by EP for use of retarget with libuarte for the debug uart. This function initializes
 * retarget.c with the libuarte interface used for debug print / printf.
 * Refer to the uart-helper utility in the ep-nrf repo for more information.
 * 
 * @param p_libuarte 
 * @param dbg_tx_semaphore 
 */
void retarget_init(const nrf_libuarte_async_t* const p_libuarte, bool *tx_done);

#endif
#endif