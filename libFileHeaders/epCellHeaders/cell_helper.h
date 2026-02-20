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
 * @file    cell_helper.h
 * @version 1.0.0
 * @author  Embedded Planet, Inc.
 * @author  golobmichael
 * @date    01 SEPT 2022
 * 
 * @brief Header for cellular library APIs and configurations.
 * versions:
 *  1.0.0 - 28SEPT2023  Initial - golobmichael
 * 
 * Built for use with the nRF SDK 17.1 and FreeRTOS.
 */

/**
 * @file cell_helper.h
 */

#ifndef __CELL_HELPER_H__
#define __CELL_HELPER_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

/* Standard includes. */
#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#define CELL_SKIP_SECURITY_CHECK

/**
 * @brief Define for size of descriptor table
 * 
 */
#define otapalDESCRIPTOR_SIZE           sizeof( ImageDescriptor_t )     /* The size of the firmware descriptor */

/**
 * @brief MQTT Max Topic Length.
 * 
 */
#define MQTT_MAX_TOPIC_LENGTH    50U

/**
 * @brief Maximum length of the data packet allowed in a cell queue message
 *  EPCP_SYSTEM_SIZE        = 19,
    EPCP_SYSTEM_V2_SIZE     = 9,
    EPCP_CELL_SIZE          = 10,
    EPCP_GNSS_SIZE          = 7,
    EPCP_SI7021_SIZE        = 6,
    EPCP_BME680_SIZE        = 25,
    EPCP_ICM20602_SIZE      = 14,
    EPCP_LSM9DS1_SIZE       = 21,
    EPCP_VL53L0X_SIZE       = 6,
    TOTOAL                  = 130, with 13 bytes overhead
 */
#define CELLULAR_QUEUE_MSG_MAX_LENGTH   130U

/**
 * @brief Cell carrier defines
 * 
 */
#define ATT     0
#define VERIZON 1
#define ROW     2
#define AU      3

/**
 * @brief Cell technology defines
 * 
 */
#define CELLULAR_IOT_CATM1      0   // CAT-M1 Only
#define CELLULAR_IOT_NBIOT      1   // NB-IoT only
#define CELLULAR_IOT_CATM1_PREF 2   // CAT-M1 preferred over NB-IoT
#define CELLULAR_IOT_NBIOT_PREF 3   // NB-IOT preferred over Cat-M1

/**
 * @brief FOTA Checksum defines
 * 
 */
#define SUM_ALL         0
#define CRC32_CHKSM     1

/**
 * @brief Size of buffer that holds downlink (shared) attribute string
 */
#define HTTP_ATTR_BUFF_SIZE         1024

/**
 * @brief Define number of HTTP Attributes needed
 * 
 */
#define num_HTTP_ATTRIBUTES             5

/**
 * @brief Max attribute length for cell http get
 * 
 */
#define MAX_ATTR_VALUE_LENGTH     10

/**
 * @brief IMEI max size.<br>
 *
 */
#define CELLULAR_IMEI_MAX_SIZE    15U

/**
 * @brief Integrate circuit card identity max size.<br>
 * 
 */
#define CELLULAR_ICCID_MAX_SIZE    20U

/**
 * @brief Define for skipping socket query, if applicable
 * 
 */
//#define CELL_SKIP_SOCKET_QUERY     1   // True equals skip, not defined equals don't skip

/**
 * @brief Threshold for transmission interval to power off cell versus just shutdown socket
 * 
 */
#define CELL_PWR_OFF_THRESH     300U    // 300s

/**
 * @brief Max GNSS Direction length.<br>
 *
 */
#define MAX_GNSS_LAT_LENGTH    10U

/**
 * @brief Max GNSS Direction length.<br>
 *
 */
#define MAX_GNSS_LONG_LENGTH    11U

/**
 * @brief Cell carrier AT&T.<br>
 *
 */
#define ATT_PDN    1U /* AT&T and Telit AT&T */

/**
 * @brief Cell carrier Verizon.<br>
 *
 */
#define VERIZON_PDN    3U /* Verizon */

/**
 * @brief Access point name max size.<br>
 *
 */
#define CELLULAR_APN_MAX_SIZE    ( 64U )

/**
 * @brief Payload format.<br>
 *
 */
#define PLAIN_TEXT_PAYLOAD      0 //Compact payload
#define JSON_PAYLOAD            1 //Json format payload

/**
 * @brief Endpoint address max size
 * 
 */
#define ENDPOINT_ADDR_MAX_SIZE ( 75U )

/**
 * @ingroup cellular_datatypes_enums
 * @brief Represents PDN context type.
 */
typedef enum CellularPdnContextType
{
    CELLULAR_PDN_CONTEXT_IPV4 = 1,   /**< IPV4 PDN CONTEXT. */
    CELLULAR_PDN_CONTEXT_IPV6 = 2,   /**< IPV6 PDN CONTEXT. */
    CELLULAR_PDN_CONTEXT_IPV4V6 = 3, /**< IPV4V6 PDN CONTEXT. */
    CELLULAR_PDN_CONTEXT_TYPE_MAX    /**< The max number of supported PDN CONTEXT. */
} CellularPdnContextType_t;

/**
 * @ingroup cellular_datatypes_enums
 * @brief Represents PDN authentication type.
 */
typedef enum CellularPdnAuthType
{
    CELLULAR_PDN_AUTH_NONE = 0,   /**< No authentication. */
    CELLULAR_PDN_AUTH_PAP,        /**< Password Authentication Protocol (PAP). */
    CELLULAR_PDN_AUTH_CHAP,       /**< Challenge Handshake Authentication Protocol (CHAP). */
    CELLULAR_PDN_AUTH_PAP_OR_CHAP /**< PAP or CHAP. */
} CellularPdnAuthType_t;

/**
 * @ingroup cellular_datatypes_paramstructs
 * @brief Represents a Cellular BLE config.
 */
typedef struct CellularBLEConfig
{
    uint8_t CellularSocketPdnContextId;                        /**< PDN Context ID. */
    CellularPdnContextType_t pdnContextType;                   /**< PDN Context type. */
    char apnName[ CELLULAR_APN_MAX_SIZE + 1 ];                 /**< APN name. */
    char endpointAddr[ ENDPOINT_ADDR_MAX_SIZE + 1 ];           /**< Endpoint Address. */
} CellularBLEConfig_t;

/**
 * @brief gnss Status.
 */
extern bool gnssStatus;

/**
 * @brief Cell semaphore and queue
 * 
 */
extern SemaphoreHandle_t cellularSemaphore;
extern QueueHandle_t xCellQueue;

/**
 * @brief FOTA variables.
 */
extern uint32_t otapalBlockSize;
extern uint8_t otapalChecksumMethod;
extern uint32_t otapalFlashStart;
extern uint32_t otapalDescTblStart;
extern uint32_t otapalBankSize;
extern char appVersion[9];

/**
 * @ingroup cell_enum_types
 * @brief The cell library return status.
 */
typedef enum CellStatus
{
    CellGeneralFail = 0,    /*!< @brief General Failure. */
    CellSuccess,            /*!< @brief Success, no errors. */
    CellInitFail,           /*!< @brief Initialization Library Failed, or modem Init failed. */
    CellSimFail,            /*!< @brief SIM failure. */
    CellRegisterFail,       /*!< @brief Failure to register to network during initialization. */
    CellSocketFail,         /*!< @brief Failed to configure and connect to socket. */
    CellLostService,        /*!< @brief Cell service lost during runtime. */
    CellFailToSend,         /*!< @brief Failed to send data. */
    CellFailToRec           /*!< @brief Failed to receive data. */
} CellStatus_t;
extern CellStatus_t cellStatus;

/**
 * @ingroup cellular_datatypes_enums
 * @brief Status code returns from APIs.
 */
typedef enum CellularError
{
    CELLULAR_SUCCESS = 0,            /**< The operation was successful. */
    CELLULAR_INVALID_HANDLE,         /**< Invalid handle. */
    CELLULAR_MODEM_NOT_READY,        /**< The modem is not ready yet. */
    CELLULAR_LIBRARY_NOT_OPEN,       /**< The cellular library is not open yet. */
    CELLULAR_LIBRARY_ALREADY_OPEN,   /**< The cellular library is already open. */
    CELLULAR_BAD_PARAMETER,          /**< One or more of the input parameters is not valid. */
    CELLULAR_NO_MEMORY,              /**< Memory allocation failure. */
    CELLULAR_TIMEOUT,                /**< The operation timed out. */
    CELLULAR_SOCKET_CLOSED,          /**< The supplied socket is already closed. */
    CELLULAR_SOCKET_NOT_CONNECTED,   /**< The supplied socket is not connected. */
    CELLULAR_INTERNAL_FAILURE,       /**< The Cellular library internal failure. */
    CELLULAR_RESOURCE_CREATION_FAIL, /**< Resource creation for Cellular library failed. */
    CELLULAR_UNSUPPORTED,            /**< The operation is not supported. */
    CELLULAR_NOT_ALLOWED,            /**< The operation is not allowed. */
    CELLULAR_UNKNOWN                 /**< Any other error other than the above mentioned ones. */
} CellularError_t;

/**
 * @brief Structure for storing cell parameters
 * 
 */
typedef struct cellParam {
    char appVersion[9];                             /* Application version, shared with cell library */
    uint8_t carrier;                                /* Cellular SIM carrier */
    uint8_t technology;                             /* Cellular technology */
    uint32_t gnssInterval;                          /* GNSS interval */
    uint32_t gnssTimeout;                           /* GNSS timeout in 10s of seconds */
    uint8_t payloadFormat;                          /* Cell queue payload format */
    char brokerAddrPost[100];                       /* Cellular broker address for POST */
    char brokerAddrGet[100];                        /* Cellular broker address for GET */
    bool gnssStatus;                                /* GNSS enable/disable */
    char serverAddrPrefixPost[50];                  /* HTTP address prefix Post */
    char serverAddrPrefixGet[50];                   /* HTTP address prefix Get */
    char serverDownloadSuffix[50];                  /* HTTP download suffix */
    char serverTelemSuffix[65];                     /* HTTP telemetry suffix */
    char serverAttribSuffix[50];                    /* HTTP attribute suffix */
    bool accessTokenInPathPost;                     /* Flag to mark whether access token in server url path for POST */
    bool accessTokenInPathGet;                      /* Flag to mark whether access token in server url path for GET */
    char rootCA1[2000];                             /* SSL root CA */
    char cert[2000];                                /* SSL certificate */
    char pvtKey[2000];                              /* SSL private key */
    char mqttTopicString[MQTT_MAX_TOPIC_LENGTH];    /* MQTT topic string */
    uint8_t checksumMethod;                         /* FOTA checksum method */
    uint32_t fotaFlashStart;                        /* FOTA flash address start */
    uint32_t fotaDescrTblStart;                     /* FOTA descriptor table start */
    uint32_t fotaBankSize;                          /* FOTA bank start */
    uint32_t cellPwrEnPin;                          /* Cellular power enable pin */
    uint32_t cellOnOffPin;                          /* Cellular On/Off pin */
    uint32_t cellRTSPin;                            /* Cellular RTS pin */
} cellParam;

/**
 * @brief Structure for storing cell diagnostics
 * 
 */
typedef struct cellDiag {
    int16_t rssi;                               /**< RSSI Value. */
    int16_t rsrq;                               /**< RSRQ Value. */
    char longitude[MAX_GNSS_LONG_LENGTH+1];     /**< GNSS longitude value, represented as string. */
    char latitude[MAX_GNSS_LAT_LENGTH+1];       /**< GNSS latitude value represented as a string. */
    float long_float;                           /**< GNSS longitude value, represented as a float. */
    float lat_float;                            /**< GNSS latitude value represented as a float. */
    uint8_t fix;                                /**< GNSS fix value. */
    uint32_t fixTime;                           /**< GNSS fix time in epoch seconds */
    uint8_t satellites;                         /**< GNSS number of satellites. */
    uint16_t restartCounter;                    /**< Modem transmit issue counter. */
    char iccid[ CELLULAR_ICCID_MAX_SIZE + 1 ];  /**< SIM ICCID. */
    char imei[ CELLULAR_IMEI_MAX_SIZE + 1 ];    /**< Modem IMEI. */
} cellDiag;

/**
 * @brief Enum for OTA status flags
 * 
 */
typedef enum
{
    otapalIMAGE_FLAG_NEW = 0xFE,            /* If the application image is running for the first time and never executed before. */
    otapalIMAGE_FLAG_VALID = 0xFC,          /* The application image is marked valid and committed. */
    otapalIMAGE_FLAG_INVALID = 0xF8         /* The application image is marked invalid. */
} ImageFlags_t;

/**
 * @brief Structure for storing descriptor tables information
 * 
 */
typedef struct
{
    uint8_t usImageFlags;                        /* Image flags. */
    uint32_t ulImageStartAddress;                /* Starting address of the application image. */
    uint32_t ulImageSize;                        /* End address of the application image. */
    uint32_t ulHardwareID;                       /* 32 bit ID that can be generated unique for a particular platform. */
    uint32_t updateVersion;                      /* 32 bit ID for version number */
    uint32_t checksum;                           /* 32 bit checksum. */
} ImageDescriptor_t;

/**
 * @brief Used to send data from the main application through the cellular queue
 * 
 */
typedef struct{
    char        data[CELLULAR_QUEUE_MSG_MAX_LENGTH];  /** < Pointer to the JSON string */
    uint16_t    size;   /** < Size of the JSON string */
} cell_queue_msg;

/**
 * @brief Callback used to inform about the response of an AT command sent
 * using Cellular_ATCommandRaw API.
 *
 * @param[in] cellTransInt Interval to transmit cellular data.
 * @param[in] regTimeout Cell registration timeout.
 * 
 * @return enum return.
 */
extern CellStatus_t runCellular( uint32_t cellTransInt, uint32_t regTimeout );

/**
 * @brief Query cellular diagnostics
 *
 * @param[in] cellTransInt Interval to transmit cellular data.
 * 
 * @return enum return.
 */
extern CellStatus_t queryCellularDiag( cellDiag *params );

/**
 * @brief Function used to set cellular parameters.
 *
 * @param[in] params Pointer to structure containing all cell initialization parameters.
 * @param[in] pdnConfig Cell device pdp configuration to send and receive data from in the cloud.
 *
 * @return True if successful, false if failed.
 */
extern bool cellInit(cellParam *params, CellularBLEConfig_t *pdnConfig);

/**
 * @brief Function used to retrieve HTTP attributes.
 *
 * @param[in] index Index for HTTP attribute.
 * @param[in] value Value for HTTP attribute.
 *
 * @return N/A.
 */
extern void getHTTPAttributes( uint8_t index, char* value );

/**
 * @brief Cell libray callback to alert app that fix attained, app can append to message if desired.
 *
 * @return N/A.
 */
extern void appendMsgWithGNSS( void );


/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* __CELL_HELPER_H__ */