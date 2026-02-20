/**
 * Copyright (c) 2016 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include "nrf_bootloader.h"

#include "compiler_abstraction.h"
#include "nrf.h"
#include "boards.h"
#include "sdk_config.h"
#include "nrf_power.h"
#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_dfu.h"
#include "nrf_error.h"
#include "nrf_dfu_settings.h"
#include "nrf_dfu_utils.h"
#include "nrf_bootloader_wdt.h"
#include "nrf_bootloader_info.h"
#include "nrf_bootloader_app_start.h"
#include "nrf_bootloader_fw_activation.h"
#include "nrf_bootloader_dfu_timers.h"
#include "app_scheduler.h"
#include "nrf_dfu_validation.h"

#include "main.h"
#include "uart_helper.h"

#if defined (QSPI_USED)
#include "qspi_helper.h"
#endif

static nrf_dfu_observer_t m_user_observer; //<! Observer callback set by the user.
static volatile bool m_flash_write_done;

//Added by EP to track DFU enter time
uint32_t dfu_time_ms;

#define SCHED_QUEUE_SIZE      32          /**< Maximum number of events in the scheduler queue. */
#define SCHED_EVENT_DATA_SIZE NRF_DFU_SCHED_EVENT_DATA_SIZE /**< Maximum app_scheduler event size. */

#if !(defined(NRF_BL_DFU_ENTER_METHOD_BUTTON)    && \
      defined(NRF_BL_DFU_ENTER_METHOD_PINRESET)  && \
      defined(NRF_BL_DFU_ENTER_METHOD_GPREGRET)  && \
      defined(NRF_BL_DFU_ENTER_METHOD_BUTTONLESS)&& \
      defined(NRF_BL_RESET_DELAY_MS)             && \
      defined(NRF_BL_DEBUG_PORT_DISABLE))
    #error Configuration file is missing flags. Update sdk_config.h.
#endif

//Used in EP modifications to determine how long to enter DFU mode
#define DFU_NONE        -1
#define SHORT_DFU_TIME  4500
#define LONG_DFU_TIME   60000

//Project-specific DFU timeouts
#define CLEVEMED_SHORT_DFU_TIME     3000
#define ANL_SHORT_DFU_TIME          10000

STATIC_ASSERT((NRF_BL_DFU_INACTIVITY_TIMEOUT_MS >= 100) || (NRF_BL_DFU_INACTIVITY_TIMEOUT_MS == 0),
             "NRF_BL_DFU_INACTIVITY_TIMEOUT_MS must be 100 ms or more, or 0 to indicate that it is disabled.");

#if defined(NRF_LOG_BACKEND_FLASH_START_PAGE)
STATIC_ASSERT(NRF_LOG_BACKEND_FLASH_START_PAGE != 0,
    "If nrf_log flash backend is used it cannot use space after code because it would collide with settings page.");
#endif

/**@brief Weak implemenation of nrf_dfu_init
 *
 * @note   This function will be overridden if nrf_dfu.c is
 *         compiled and linked with the project
 */
 #if (__LINT__ != 1)
__WEAK uint32_t nrf_dfu_init(nrf_dfu_observer_t observer)
{
    NRF_LOG_DEBUG("in weak nrf_dfu_init");
    return NRF_SUCCESS;
}
#endif


/**@brief Weak implementation of nrf_dfu_init
 *
 * @note    This function must be overridden in application if
 *          user-specific initialization is needed.
 */
__WEAK uint32_t nrf_dfu_init_user(void)
{
    NRF_LOG_DEBUG("in weak nrf_dfu_init_user");
    return NRF_SUCCESS;
}


static void flash_write_callback(void * p_context)
{
    UNUSED_PARAMETER(p_context);
    m_flash_write_done = true;
}


static void do_reset(void * p_context)
{
    UNUSED_PARAMETER(p_context);

    NRF_LOG_FINAL_FLUSH();

    nrf_delay_ms(NRF_BL_RESET_DELAY_MS);

    NVIC_SystemReset();
}


static void bootloader_reset(bool do_backup)
{
    DBGI("Resetting bootloader!");
    nrf_delay_ms(50);

    if (do_backup)
    {
        m_flash_write_done = false;
        nrf_dfu_settings_backup(do_reset);
    }
    else
    {
        do_reset(NULL);
    }
}


static void inactivity_timeout_cb(void)
{
    DBGI("Inactivity timeout.");

    //Skip DFU on restart
    s_dfu_settings.enter_buttonless_dfu = 0;
    APP_ERROR_CHECK(nrf_dfu_settings_write(NULL));
    
    bootloader_reset(true);
}


/**@brief Function for handling DFU events.
 */
static void dfu_observer(nrf_dfu_evt_type_t evt_type)
{
    switch (evt_type)
    {
        case NRF_DFU_EVT_DFU_STARTED:
        case NRF_DFU_EVT_OBJECT_RECEIVED:
            DBGI("DFU Object Received");
            nrf_bootloader_dfu_inactivity_timer_restart(
                        NRF_BOOTLOADER_MS_TO_TICKS(dfu_time_ms),
                        inactivity_timeout_cb);
            break;
        case NRF_DFU_EVT_DFU_COMPLETED:
        case NRF_DFU_EVT_DFU_ABORTED:
            DBGI("DFU Aborted");
            bootloader_reset(true);
            break;
        case NRF_DFU_EVT_TRANSPORT_DEACTIVATED:
            // Reset the internal state of the DFU settings to the last stored state.
            nrf_dfu_settings_reinit();
            break;
        default:
            break;
    }

    if (m_user_observer)
    {
        m_user_observer(evt_type);
    }
}


/**@brief Function for initializing the event scheduler.
 */
static void scheduler_init(void)
{
    APP_SCHED_INIT(SCHED_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
}


/**@brief Suspend the CPU until an interrupt occurs.
 */
static void wait_for_event(void)
{
#if defined(BLE_STACK_SUPPORT_REQD) || defined(ANT_STACK_SUPPORT_REQD)
    (void)sd_app_evt_wait();
#else
    // Wait for an event.
    __WFE();
    // Clear the internal event register.
    __SEV();
    __WFE();
#endif
}


/**@brief Continually sleep and process tasks whenever woken.
 */
static void loop_forever(void)
{
    while (true)
    {
        //feed the watchdog if enabled.
        nrf_bootloader_wdt_feed();

        app_sched_execute();

        if (!NRF_LOG_PROCESS())
        {
            wait_for_event();
        }
    }
}

#if NRF_BL_DFU_ENTER_METHOD_BUTTON
#ifndef BUTTON_PULL
    #error NRF_BL_DFU_ENTER_METHOD_BUTTON is enabled but not buttons seem to be available on the board.
#endif
/**@brief Function for initializing button used to enter DFU mode.
 */
static void dfu_enter_button_init(void)
{
    nrf_gpio_cfg_sense_input(NRF_BL_DFU_ENTER_METHOD_BUTTON_PIN,
                             BUTTON_PULL,
                             NRF_GPIO_PIN_SENSE_LOW);
}
#endif


static bool crc_on_valid_app_required(void)
{
    bool ret = true;
    if (NRF_BL_APP_CRC_CHECK_SKIPPED_ON_SYSTEMOFF_RESET &&
        (nrf_power_resetreas_get() & NRF_POWER_RESETREAS_OFF_MASK))
    {
        nrf_power_resetreas_clear(NRF_POWER_RESETREAS_OFF_MASK);
        ret = false;
    }
    else if (NRF_BL_APP_CRC_CHECK_SKIPPED_ON_GPREGRET2 &&
            ((nrf_power_gpregret2_get() & BOOTLOADER_DFU_SKIP_CRC_MASK) == BOOTLOADER_DFU_SKIP_CRC))
    {
        nrf_power_gpregret2_set(nrf_power_gpregret2_get() & ~BOOTLOADER_DFU_SKIP_CRC);
        ret = false;
    }
    else
    {
    }

    return ret;
}



static bool boot_validate(boot_validation_t const * p_validation, uint32_t data_addr, uint32_t data_len, bool do_crc)
{
    if (!do_crc && (p_validation->type == VALIDATE_CRC))
    {
        return true;
    }
    return nrf_dfu_validation_boot_validate(p_validation, data_addr, data_len);
}


/** @brief Function for checking if the main application is valid.
 *
 * @details     This function checks if there is a valid application
 *              located at Bank 0.
 *
 * @param[in]   do_crc Perform CRC check on application. Only CRC checks
                       can be skipped. For other boot validation types,
                       this parameter is ignored.
 *
 * @retval  true  If a valid application has been detected.
 * @retval  false If there is no valid application.
 */
static bool app_is_valid(bool do_crc)
{
    if (s_dfu_settings.bank_0.bank_code != NRF_DFU_BANK_VALID_APP)
    {
        NRF_LOG_INFO("Boot validation failed. No valid app to boot.");
        return false;
    }
    else if (NRF_BL_APP_SIGNATURE_CHECK_REQUIRED &&
        (s_dfu_settings.boot_validation_app.type != VALIDATE_ECDSA_P256_SHA256))
    {
        NRF_LOG_WARNING("Boot validation failed. The boot validation of the app must be a signature check.");
        return false;
    }
    else if (SD_PRESENT && !boot_validate(&s_dfu_settings.boot_validation_softdevice, MBR_SIZE, s_dfu_settings.sd_size, do_crc))
    {
        NRF_LOG_WARNING("Boot validation failed. SoftDevice is present but invalid.");
        return false;
    }
    else if (!boot_validate(&s_dfu_settings.boot_validation_app, nrf_dfu_bank0_start_addr(), s_dfu_settings.bank_0.image_size, do_crc))
    {
        NRF_LOG_WARNING("Boot validation failed. App is invalid.");
        return false;
    }
    // The bootloader itself is not checked, since a self-check of this kind gives little to no benefit
    // compared to the cost incurred on each bootup.

    NRF_LOG_DEBUG("App is valid");
    return true;
}



/**@brief Function for clearing all DFU enter flags that
 *        preserve state during reset.
 *
 * @details This is used to make sure that each of these flags
 *          is checked only once after reset.
 */
static void dfu_enter_flags_clear(void)
{
    if (NRF_BL_DFU_ENTER_METHOD_PINRESET &&
       (NRF_POWER->RESETREAS & POWER_RESETREAS_RESETPIN_Msk))
    {
        // Clear RESETPIN flag.
        NRF_POWER->RESETREAS |= POWER_RESETREAS_RESETPIN_Msk;
    }

    if (NRF_BL_DFU_ENTER_METHOD_GPREGRET &&
       ((nrf_power_gpregret_get() & BOOTLOADER_DFU_START_MASK) == BOOTLOADER_DFU_START))
    {
        // Clear DFU mark in GPREGRET register.
        nrf_power_gpregret_set(nrf_power_gpregret_get() & ~BOOTLOADER_DFU_START);
    }

    if (NRF_BL_DFU_ENTER_METHOD_BUTTONLESS &&
       (s_dfu_settings.enter_buttonless_dfu == 1))
    {
        // Clear DFU flag in flash settings.
        s_dfu_settings.enter_buttonless_dfu = 0;
        APP_ERROR_CHECK(nrf_dfu_settings_write(NULL));
    }
}


/**@brief Function for checking whether to enter DFU mode or not.
 */
static bool dfu_enter_check(void)
{
    if (!app_is_valid(crc_on_valid_app_required()))
    {
        DBGE("DFU mode because app is not valid.");
        return true;
    }

    if (NRF_BL_DFU_ENTER_METHOD_BUTTON &&
       (nrf_gpio_pin_read(NRF_BL_DFU_ENTER_METHOD_BUTTON_PIN) == 0))
    {
        DBGE("DFU mode requested via button.");
        return true;
    }

    if (NRF_BL_DFU_ENTER_METHOD_PINRESET &&
       (NRF_POWER->RESETREAS & POWER_RESETREAS_RESETPIN_Msk))
    {
        DBGE("DFU mode requested via pin-reset.");
        return true;
    }

    if (NRF_BL_DFU_ENTER_METHOD_GPREGRET &&
       ((nrf_power_gpregret_get() & BOOTLOADER_DFU_START_MASK) == BOOTLOADER_DFU_START))
    {
        DBGE("DFU mode requested via GPREGRET.");
        return true;
    }

    if (NRF_BL_DFU_ENTER_METHOD_BUTTONLESS &&
       (s_dfu_settings.enter_buttonless_dfu == 1))
    {
        DBGI("Moving to BLE bootloader...");
        //NRF_LOG_DEBUG("DFU mode requested via bootloader settings.");
        //DBGI("Enter DFU mode for %d seconds.", NRF_BL_DFU_INACTIVITY_TIMEOUT_MS / 1000);
        return true;
    }

    return false;
}


#if NRF_BL_DFU_ALLOW_UPDATE_FROM_APP
static void postvalidate(void)
{
    NRF_LOG_INFO("Postvalidating update after reset.");
    nrf_dfu_validation_init();

    if (nrf_dfu_validation_init_cmd_present())
    {
        uint32_t firmware_start_addr;
        uint32_t firmware_size;

        // Execute a previously received init packed. Subsequent executes will have no effect.
        if (nrf_dfu_validation_init_cmd_execute(&firmware_start_addr, &firmware_size) == NRF_DFU_RES_CODE_SUCCESS)
        {
            if (nrf_dfu_validation_prevalidate() == NRF_DFU_RES_CODE_SUCCESS)
            {
                if (nrf_dfu_validation_activation_prepare(firmware_start_addr, firmware_size) == NRF_DFU_RES_CODE_SUCCESS)
                {
                    NRF_LOG_INFO("Postvalidation successful.");
                }
            }
        }
    }

    s_dfu_settings.bank_current = NRF_DFU_CURRENT_BANK_0;
    UNUSED_RETURN_VALUE(nrf_dfu_settings_write_and_backup(flash_write_callback));
}
#endif

ret_code_t nrf_bootloader_init(nrf_dfu_observer_t observer)
{
    ret_code_t                            ret_val;
    nrf_bootloader_fw_activation_result_t activation_result;    
    bool                                  dfu_enter = false;

    m_user_observer = observer;

    if (NRF_BL_DEBUG_PORT_DISABLE)
    {
        nrf_bootloader_debug_port_disable();
    }

    ret_val = nrf_dfu_settings_init(false);
    if (ret_val != NRF_SUCCESS)
    {
        return NRF_ERROR_INTERNAL;
    }

#if NRF_BL_DFU_ENTER_METHOD_BUTTON
    dfu_enter_button_init();
#endif

#if NRF_BL_DFU_ALLOW_UPDATE_FROM_APP
    //Check to see if update is in QSPI by seeing if first 256 bytes are empty
    DBGI("Checking QSPI for pending update...");

    ImageDescriptor_t xDescriptor;
    nrfx_err_t err_code = qspi_read((uint8_t*)&xDescriptor, sizeof(xDescriptor), 0);
    if(err_code){
        DBGE("QSPI read failed with error code %d", err_code);
    }else if(xDescriptor.usImageFlags == otapalIMAGE_FLAG_NEW){
        DBGI("QSPI update found!");
        DBGI("Size: 0x%x  |  Checksum: 0x%x  |  Flag: 0x%x  |  Version: 0x%x", 
            xDescriptor.ulImageSize, xDescriptor.checksum, xDescriptor.usImageFlags, xDescriptor.updateVersion);

        //Load update from QSPI into flash program memory
        err_code = image_copy_from_qspi(xDescriptor.ulImageSize);

        //Check if successful
        if(err_code){
            DBGE("Image copy from QSPI Failed! Err: %d", err_code);
        }else{
            DBGI("Image copy from QSPI Successful! Writing app as valid...");
            xDescriptor.usImageFlags = otapalIMAGE_FLAG_VALID;
            err_code = qspi_write( (uint8_t * ) &xDescriptor, otapalDESCRIPTOR_SIZE, otapal_DESCRIPTOR_START );
            if(err_code){
                DBGE("Failed to write QSPI descriptor table!");
            }else{
                DBGI("Write successful! Restarting...")
                nrf_delay_ms(10);
                qspi_uninit();
                bootloader_reset(true);
            }
        }
    }else{
        DBGI("No QSPI update found!");
         DBGI("Size: 0x%x  |  Checksum: 0x%x  |  Flag: 0x%x  |  Version: 0x%x", 
            xDescriptor.ulImageSize, xDescriptor.checksum, xDescriptor.usImageFlags, xDescriptor.updateVersion);
        qspi_uninit();
    }

#endif

    // Get reboot reason. If the WDT has fired, we enter the DFU bootloader for a longer period of time
    uint32_t rebootReason = NRF_POWER->RESETREAS;
    DBGI("Reboot register: %x", rebootReason);

    if((rebootReason & resetWatchdog) == resetWatchdog ){
        DBGI("Reboot caused by WDT! Entering %2.1fs BLE DFU...", (float)(LONG_DFU_TIME/1000.0));
        dfu_time_ms = LONG_DFU_TIME;
    }else if (s_dfu_settings.enter_buttonless_dfu == 1){        
        #ifdef CLEVEMED_SLEEPVIEW
        dfu_time_ms = CLEVEMED_SHORT_DFU_TIME;
        #elif BOARD_ANL_ACTIVE_SEAL
        dfu_time_ms = ANL_SHORT_DFU_TIME;
        #else
        dfu_time_ms = SHORT_DFU_TIME;
        #endif
        DBGI("Entering %2.1fs BLE DFU...", (float)(dfu_time_ms/1000.0));
    }else{
        DBGI("Skipping BLE DFU...");
        dfu_time_ms = DFU_NONE;
    }

    // Check if an update needs to be activated and activate it.
    activation_result = nrf_bootloader_fw_activate();

    switch (activation_result)
    {
        case ACTIVATION_NONE:            
            // Do not jump to corrupted app
            if (!app_is_valid(crc_on_valid_app_required()))
            {
                DBGE("DFU mode because app is not valid.");
                dfu_time_ms = LONG_DFU_TIME;
                dfu_enter = true;
            }
            #ifdef CLEVEMED_SLEEPVIEW
            // Get reset reason(s) from micro, if watchdog then fast timeout
            else if((rebootReason & resetWatchdog) != resetWatchdog){
                //If not watchdog, then just delay 3s then jump                
                DBGE("Reboot reason%d", NRF_POWER->RESETREAS);
            }
            #endif
            if(dfu_time_ms == DFU_NONE){
                dfu_enter = false;
            }else{
                dfu_enter = true;
            }
            break;

        case ACTIVATION_SUCCESS_EXPECT_ADDITIONAL_UPDATE:
            dfu_enter       = true;
            break;

        case ACTIVATION_SUCCESS:
            bootloader_reset(true);
            NRF_LOG_ERROR("Unreachable");
            return NRF_ERROR_INTERNAL; // Should not reach this.

        case ACTIVATION_ERROR:
        default:
            return NRF_ERROR_INTERNAL;
    }

    NRF_POWER->RESETREAS = 0xFFFFFFFF; //Needs to be cleared or else the system will keep entering DFU because of WDT

    if (dfu_enter)
    {
        nrf_bootloader_wdt_init();
        scheduler_init();
        dfu_enter_flags_clear();

        // Call user-defined init function if implemented
        ret_val = nrf_dfu_init_user();
        if (ret_val != NRF_SUCCESS)
        {
            return NRF_ERROR_INTERNAL;
        }

        uint32_t dfu_time_ticks = NRF_BOOTLOADER_MS_TO_TICKS(dfu_time_ms);
        nrf_bootloader_dfu_inactivity_timer_restart(dfu_time_ticks, inactivity_timeout_cb);

        ret_val = nrf_dfu_init(dfu_observer);
        if (ret_val != NRF_SUCCESS)
        {
            return NRF_ERROR_INTERNAL;
        }

        NRF_LOG_DEBUG("Enter main loop");
        loop_forever(); // This function will never return.
        NRF_LOG_ERROR("Unreachable");
    }
    else
    {
        // Erase additional data like peer data or advertisement name
        ret_val = nrf_dfu_settings_additional_erase();
        if (ret_val != NRF_SUCCESS)
        {
            DBGE("Settings erase failed!");
        }

        m_flash_write_done = false;
        DBGI("Flash Settings Backup");
        nrf_dfu_settings_backup(flash_write_callback);
        ASSERT(m_flash_write_done);

        //Flag to start DFU on next restart
        s_dfu_settings.enter_buttonless_dfu = 1;
        APP_ERROR_CHECK(nrf_dfu_settings_write(NULL));

        DBGI("Exiting bootloader and starting application!");
        nrf_delay_ms(50);
        nrf_bootloader_app_start();
        NRF_LOG_ERROR("Unreachable");
    }

    // Should not be reached.
    return NRF_ERROR_INTERNAL;
}
