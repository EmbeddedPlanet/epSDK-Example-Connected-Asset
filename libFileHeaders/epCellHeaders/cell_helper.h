/****************************************************************************                                                                     *
 * Copyright (c) 2023 Embedded Planet, Inc.                                 *
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

/**
 * @brief Internal EP operation
 * 
 */
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
#define MQTT_MAX_TOPIC_LENGTH    250U

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
#define AUTO    0xFF // ME310G1 and ME910G1 support auto detect

/**
 * @brief Cell technology defines
 * 
 */
#define CELLULAR_IOT_CATM1      0   // CAT-M1 Only
#define CELLULAR_IOT_NBIOT      1   // NB-IoT only
#define CELLULAR_IOT_CATM1_PREF 2   // CAT-M1 preferred over NB-IoT
#define CELLULAR_IOT_NBIOT_PREF 3   // NB-IOT preferred over Cat-M1
#define CELLULAR_IOT_NBIOT_NTN  4   // NB-IoT NTN

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
 * @brief HTTP/CoAP Method defines
 * 
 */
 #define METHOD_POST    0
 #define METHOD_GET     1

/**
 * @brief Cellular carrier, ATT, Verizon, AU, or Auto. Only need for Agora 1.0
 * 
 */
#define CELLULAR_CARRIER    AUTO

/**
 * @brief TLS Security defined
 * 
 */
#define TLS_ANONYMOUS   0
#define TLS_SAS_TOKENS  1
#define TLS_X509        2

/**
 * @brief Cell shutdown state between data passes
 * 
 */
#define SOCKET_OPEN         0
#define MODEM_ON            1
#define MODEM_OFF           2

/**
 * @brief Protocol defined needed for cell library
 * 
 */
#define PROTOCOL_HTTPS                             0
#define PROTOCOL_MQTTS                             1

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
    CellFailToRec,          /*!< @brief Failed to receive data. */
    CellBusy                /*!< @brief Cell busy. */
} CellStatus_t;
extern CellStatus_t cellStatus;

/**
 * @brief Structure for storing cell parameters
 * 
 */
typedef struct cellParam {
    char appVersion[9];                             /* Application version, shared with cell library */
    uint8_t technology;                             /* Cellular technology */
    uint8_t payloadFormat;                          /* Cell queue payload format */
    char rootCA1[2000];                             /* SSL root CA */
    char cert[2000];                                /* SSL certificate */
    char pvtKey[2000];                              /* SSL private key */
    char symmetricKey[500];                         /* SSL SaS Token */
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
    float gtpAccuracy;                          /**< GTP Accuracy of location. */
    float gtpLongFloat;                         /**< GTP Longitude of location. */
    float gtpLatFloat;                          /**< GTP Latitude of location. */
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

typedef enum CellRegStatus
{
    REG_STATUS_NOT_REGISTERED = 0,               /**< Not registered searching network registration status. */
    REG_STATUS_REGISTERED_HOME = 1,              /**< Registered home network registration status. */
    REG_STATUS_NOT_REGISTERED_SEARCHING = 2,     /**< Not registered searching network registration status. */
    REG_STATUS_REGISTRATION_DENIED = 3,          /**< Registration denied network registration status. */
    REG_STATUS_UNKNOWN = 4,                      /**< Unknown network registration status. */
    REG_STATUS_ROAMING_REGISTERED = 5,           /**< Roaming registered network registration status. */
    REG_STATUS_HOME_SMS_ONLY_REGISTERED = 6,     /**< Home SMS only registered network registration status. */
    REG_STATUS_SMS_ONLY_ROAMING_REGISTERED = 7,  /**< SMS only roaming registered network registration status. */
    REG_STATUS_ATTACHED_EMERG_SERVICES_ONLY = 8, /**< Attached emergency service only network registration status. */
    REG_STATUS_MAX                               /**< The max supported number for registration status. */
} CellRegStatus_t;

/**
 * @brief Used to send data from the main application through the cellular queue
 * 
 */
typedef struct{
    char        data[CELLULAR_QUEUE_MSG_MAX_LENGTH];    /** < Pointer to the JSON string */
    uint16_t    size;   /** < Size of the JSON string */
} cell_queue_msg;

/**
 * @brief Function to register modem to cellular network. Will return when registered or timeout expires
 *
 * @param[in] cellCarrier Cell carrier.
 * @param[in] regTimeout Cell registration timeout.
 * 
 * @return enum return.
 */
extern CellStatus_t registerCellular( uint8_t cellCarrier, uint32_t regTimeout );

/**
 * @brief Function to check cell registration status
 * 
 * @return enum return.
 */
extern CellRegStatus_t regStatus( void );

/**
 * @brief Set GNSS operation
 *
 * @param[in] status Set GNSS status. True: GNSS enable, False: GNSS disable
 * 
 * @return enum return.
 */
extern CellStatus_t setGNSS( bool status );

/**
 * @brief Get GNSS Status
 *
 * @param[out] satellites Number of satellites vehicles seen by modem on fix attempt.
 * 
 * @return Fix. Fix >= 2, the fixed obtained
 */
extern uint8_t statusGNSS( uint8_t *satellites );

/**
 * @brief Function to open cellular connection on modem
 *
 * @param[in] endpointAddr Address to server
 * @param[in] gtpStatus Bool to let cell library know if gtp location should be attempted
 * 
 * @return enum return.
 */
extern CellStatus_t connectCell( uint8_t *endpointAddr, bool gtpStatus );

/**
 * @brief Function to close cellular connection on modem
 *
 * @param[in] sleepMode Sleep mode in between data transmissions.
 * 
 * @return N/A.
 */
extern void closeCell( uint8_t sleepMode );

/**
 * @brief Function to send/receive data over CoAP
 *
 * @param[in] method POST or GET
 * @param[in] endpointAddr Address to server
 * @param[in] pathPrefix Prefix of server path
 * @param[in] pathSuffix Suffix of server path (entire path if tokenInPath equals false)
 * @param[in] tokenInPath Access token in path (true) or not (false)
 * @param[out] fotaAvailable Bool return for if FOTA is available on server
 * 
 * @return enum return.
 */
extern CellStatus_t sendCoAPMsg(uint8_t method, uint8_t *endpointAddr, uint8_t *pathPrefix, uint8_t* pathSuffix, bool tokenInPath, bool *fotaAvailable);

/**
 * @brief Function to send/receive data over HTTP
 *
 * @param[in] method POST or GET
 * @param[in] endpointAddr Address to server
 * @param[in] pathPrefix Prefix of server path
 * @param[in] pathSuffix Suffix of server path (entire path if tokenInPath equals false)
 * @param[in] tokenInPath Access token in path (true) or not (false)
 * @param[out] fotaAvailable Bool return for if FOTA is available on server
 * 
 * @return enum return.
 */
extern CellStatus_t sendHttpMsg(uint8_t method, uint8_t *endpointAddr, uint8_t *pathPrefix, uint8_t* pathSuffix, bool tokenInPath, bool *fotaAvailable);

/**
 * @brief Function to send/receive data over HTTP with TLS
 *
 * @param[in] method POST or GET
 * @param[in] endpointAddr Address to server
 * @param[in] pathPrefix Prefix of server path
 * @param[in] pathSuffix Suffix of server path (entire path if tokenInPath equals false)
 * @param[in] tokenInPath Access token in path (true) or not (false)
 * @param[in] securityType SaS token, anonomyous, or x509
 * @param[out] respSize Size of response, if GET
 * @param[out] respBody Response contents, if GET
 * 
 * @return enum return.
 */
extern CellStatus_t sendHttpTLSMsg(uint8_t method, uint8_t *endpointAddr, uint8_t *pathPrefix, uint8_t* pathSuffix, bool tokenInPath, uint8_t securityType, int32_t *respSize, uint8_t **respBody);

/**
 * @brief Function to run FOTA from Thingsboard
 *
 * @param[in] endpointAddr Address to server
 * @param[in] pathPrefix Prefix of server path
 * @param[in] pathSuffix Suffix of server path
 * 
 * @return N/A
 */
extern void runThingsboardFOTA(uint8_t *endpointAddr, uint8_t *pathPrefix, uint8_t* pathSuffix);

/**
 * @brief Function to run AWS Task. Send data in cell queue. Receives data from cloud. Runs OTA.
 *
 * @param[in] endpointAddr Address to server
 * @param[in] pathPrefix Prefix of server path
 * @param[in] pathSuffix Suffix of server path (entire path if tokenInPath equals false)
 * @param[in] tokenInPath Access token in path (true) or not (false)
 * @param[in] persistent Persistent MQTT connection
 * @param[in] gnssInterval If persistent equals true, then interval between GNSS fixes, otherwise not used
 * @param[in] gnssTimeout If persistent equals true, then timeout for obtaining GNSS fix, otherwise not used
 * @param[in] gnssStatus If persistent equals true, then status of GNSS engine, otherwise not used
 * 
 * @return N/A.
 */
extern void runAwsMqttTlsTask( uint8_t *endpointAddr, uint8_t *pathPrefix, uint8_t* pathSuffix, bool tokenInPath, bool persistent, uint32_t gnssInterval, uint32_t gnssTimeout, bool gnssStatus );

/**
 * @brief Function to regenerate Azure SaS token
 *
 * @param[in] endpointAddr Address to server
 * @param[in] expireTime Time for SaS token to expire, in seconds
 * 
 * @return enum return.
 */
extern CellStatus_t azureRegenerateSaSToken(uint8_t *endpointAddr, uint32_t expireTime);

/**
 * @brief Function to run Azure Task. Send data in cell queue.
 *
 * @param[in] endpointAddr Address to server
 * @param[in] pathPrefix Prefix of server path
 * @param[in] pathSuffix Suffix of server path (entire path if tokenInPath equals false)
 * @param[in] tokenInPath Access token in path (true) or not (false)
 * @param[in] securityType SaS token or x509
 * @param[in] persistent Persistent MQTT connection
 * @param[in] gnssInterval If persistent equals true, then interval between GNSS fixes, otherwise not used
 * @param[in] gnssTimeout If persistent equals true, then timeout for obtaining GNSS fix, otherwise not used
 * @param[in] gnssStatus If persistent equals true, then status of GNSS engine, otherwise not used
 * 
 * @return N/A.
 */
extern void runAzureMqttTlsTask( uint8_t *endpointAddr, uint8_t *pathPrefix, uint8_t* pathSuffix, bool tokenInPath, uint8_t securityType, bool persistent, uint32_t gnssInterval, uint32_t gnssTimeout, bool gnssStatus );

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
 * @param[in] apnName APN Name.
 * @param[in] sleepMode Sleep mode selection from application
 *
 * @return enum return.
 */
extern CellStatus_t cellInit(cellParam *params, uint8_t *apnName, uint8_t sleepMode);

/**
 * @brief Function used to retrieve attributes.
 *
 * @param[in] index Index for attribute.
 * @param[in] value Value for attribute.
 *
 * @return N/A.
 */
extern void getAttributes( uint8_t index, char* value );

/**
 * @brief Cell libray callback to alert app that fix attained, app can add queue message if desired.
 *
 * @param[in] status Pass or fail fix attempt.
 * 
 * @return N/A.
 */
extern void addGnssQueueMsg( bool status );


/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* __CELL_HELPER_H__ */