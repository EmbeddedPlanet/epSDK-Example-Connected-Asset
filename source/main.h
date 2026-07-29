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
 * Created on: Sept 1, 2022
 * Created by: golobmichael
 * 
 * Copyright (c) Embedded Planet, Inc - All rights reserved
 *
 * This source file is private and confidential.
 * Unauthorized copying of this file is strictly prohibited.
 * 
 * Version 1.0 - 01SEPT22  Initial
 */
#ifndef __MAIN_H__
#define __MAIN_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
    extern "C" {
#endif
/* *INDENT-ON* */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* General defines */

/* Version needs to be defined as string and ints */
#define VERSION_NUM                         "0.00.02"                                   /**< Version. Will be passed to Device Information Service. */
#define VERSION_MAJOR                       0
#define VERSION_MINOR                       0
#define VERSION_BUILD                       2

/* Length of EP serial number is 6 characters */
#define SERIAL_LENGTH   6

/* Struct to hold system information */
typedef struct
{
    char updateVerStr[9];
    uint8_t updateVerMaj;
    uint8_t updateVerMin;
    uint16_t updateVerBui;
    uint8_t ep_serial[SERIAL_LENGTH + 1];
    char partNoStr[8];
    char hwIDStr[8];
} system_info_struct;
extern system_info_struct system_info;

//////////////////////////////////////////////////
/****                                        ****/
/****               WATCHDOG                 ****/
/****                                        ****/
//////////////////////////////////////////////////

/* Watchdog reload value in s */
#define WATCHDOG_RELOAD         60

/* How often the watchdog should be fed in percent of WATCHDOG_RELOAD */
#define WATCHDOG_RELOAD_RATE    50

//////////////////////////////////////////////////
/****                                        ****/
/****               CELLULAR                 ****/
/****                                        ****/
//////////////////////////////////////////////////

/**< Used to activate/deactivate cellular in the main application
 * 
 *   0 = Deactivated
 *   1 = Activated
 */
#define CELLULAR_ACTIVE     1

/* Cellular technology, NB-IoT or CAT-M1 */
#define CELLULAR_TECH   CELLULAR_IOT_CATM1//CELLULAR_IOT_NBIOT_NTN

/* GPS status, true or false */
#define TELIT_GNSS_STATUS                    true

/* GPS fix timout in 10s of seconds */
#define GNSS_TIMEOUT                  18

/* GNSS interval for attempting fix in seconds */
#define GNSS_ATTEMPT_INTERVAL     3600U

/* Low satellite timeout status */
#define LOW_SAT_STATUS          true

/* Low satellite timeout threshold in 10s of seconds */
#define LOW_SAT_TIMEOUT         30

/* Low satellite num of satellite threshold */
#define LOW_SAT_NUMBER          3

/* Cellular transmission interval in seconds */
#define MIN_TRANS_INTERVAL    300U

/** Time to wait on queue before moving on to the next sample */
#define CELL_QUEUE_TIMEOUT                  pdMS_TO_TICKS(3000) 

/* Payload format */
#define CELL_PAYLOAD_FORMAT      PLAIN_TEXT_PAYLOAD

/* Cellular queue size for transmissions */
#define CELL_QUEUE_SIZE                     7

/* Checksum type to be used for OTA */
#define otapal_CHECKSUM_METHOD               CRC32_CHKSM

/* Modem sleep state */
#define MODEM_SLEEP_STATE   MODEM_OFF

/* Enable GTP Location Functional */
#define GTP_STATUS          true

/* MQTT Persistent Connection Flag */
#define PERSISTENT_CONNECT_FLAG     false   // false: disconnect with empty queue, true: maintain connection

/* Select option for server, uncomment one of the following. Contact EP for additional options */
#define THINGSBOARD_HTTP_INTEGRATION    // Send to ThingsBoard HTPP Integration server
//#define THINGSBOARD_HTTPS_INTEGRATION     // Send to ThingsBoard HTTPs Integration server
//#define THINGSBOARD_COAP_INTEGRATION    // Send to ThingsBoard CoAP Integration server
//#define THINGSBOARD_COAPS_INTEGRATION   // Send to ThingsBoard CoAP w/ dtls Integration server, coap w/dtls not currently supported, contact EP to discuss
//#define AWS_HTTPS_X509                   // Send to AWS IoT Core over HTTP using X509 certificates
//#define AWS_MQTTS_X509                   // Send to AWS IoT Core over MQTT using X509 certificates
//#define AZURE_MQTTS_X509                 // Send to Azure Event Grid over MQTT using X509 certificates
//#define AZURE_MQTTS_SAS                 // Send to Azure IoT Hub over MQTT using SaS tokens
//#define AZURE_HTTPS_SAS                 // Send to Azure IoT Hub over HTTP using SaS tokens

/* Both addresses below must be the same protocol for now */
/* Currently MQTT/CoAP/HTTP supported for POST */
/* If different endpoint location is required, contact EP for guidance */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define IOT_BROKER_ADDRESS_POST "http://thingsboard.cloud:80"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define IOT_BROKER_ADDRESS_POST "https://thingsboard.cloud:443"
#elif defined(AWS_HTTPS_X509)
    #define IOT_BROKER_ADDRESS_POST "https://d020120426aa3nod5zht2-ats.iot.us-east-1.amazonaws.com:443"
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define IOT_BROKER_ADDRESS_POST    "coap://int.thingsboard.cloud:5683"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define IOT_BROKER_ADDRESS_POST    "coaps://int.thingsboard.cloud:5684"
#elif defined(AWS_MQTTS_X509)
    #define IOT_BROKER_ADDRESS_POST    "mqtts://a3idy3b326ldym-ats.iot.us-east-1.amazonaws.com:8883"
#elif defined(AZURE_MQTTS_X509)
    #define IOT_BROKER_ADDRESS_POST    "mqtts://epdemohub.eastus-1.ts.eventgrid.azure.net:8883"
#elif defined(AZURE_MQTTS_SAS)
    #define IOT_BROKER_ADDRESS_POST    "mqtts://ep-demo-hub.azure-devices.net:8883"
#elif defined(AZURE_HTTPS_SAS)
    #define IOT_BROKER_ADDRESS_POST    "https://ep-demo-hub.azure-devices.net:443"
#endif

/* Currently MQTT/CoAP/HTTP supported for GET */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define IOT_BROKER_ADDRESS_GET "http://thingsboard.cloud:80"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define IOT_BROKER_ADDRESS_GET "https://thingsboard.cloud:443"
#elif defined(AWS_HTTPS_X509)
    #define IOT_BROKER_ADDRESS_GET "https://d020120426aa3nod5zht2-ats.iot.us-east-1.amazonaws.com:443"
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define IOT_BROKER_ADDRESS_GET    "coap://coap.thingsboard.cloud:5683"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define IOT_BROKER_ADDRESS_GET    "coaps://coap.thingsboard.cloud:5684"
#elif defined(AWS_MQTTS_X509)
    #define IOT_BROKER_ADDRESS_GET    "mqtts://a3idy3b326ldym-ats.iot.us-east-1.amazonaws.com:8883"
#elif defined(AZURE_MQTTS_X509)
    #define IOT_BROKER_ADDRESS_GET    "mqtts://epdemohub.eastus-1.ts.eventgrid.azure.net:8883"
#elif defined(AZURE_MQTTS_SAS)
    #define IOT_BROKER_ADDRESS_GET    "mqtts://ep-demo-hub.azure-devices.net:8883"
#elif defined(AZURE_HTTPS_SAS)
    #define IOT_BROKER_ADDRESS_GET    "https://ep-demo-hub.azure-devices.net:443"
#endif

/**
 * @brief Server path includes access token for post or get. Access token in post is also used for signifying if access token is in MQTT topic
 */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   0
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    1
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   0
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    1
#elif defined(AWS_HTTPS_X509)
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   0
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    0
#elif defined(AWS_HTTPS_X509) || defined(AWS_MQTTS_X509)
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   1
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    0
#elif defined(AZURE_MQTTS_X509) || defined(AZURE_MQTTS_SAS) || defined(AZURE_HTTPS_SAS)
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   1
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    1
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   0
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    1
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define SERVER_PATH_ACCESS_TOKEN_POST_ENABLED   0
    #define SERVER_PATH_ACCESS_TOKEN_GET_ENABLED    1
#endif


/**
 * @brief Server Address Path Prefix for POST or sending data to server on HTTP and CoAP. Use as topic prefix for MQTT
 */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define SERVER_ADDR_PREFIX_POST    "/api/v1/"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define SERVER_ADDR_PREFIX_POST    "/api/v1/"
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define SERVER_ADDR_PREFIX_POST    "/api/v1/"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define SERVER_ADDR_PREFIX_POST    "/api/v1/"
#elif defined(AWS_MQTTS_X509) || defined(AWS_HTTPS_X509)
    #define SERVER_ADDR_PREFIX_POST    "/api/v1/"
#elif defined(AZURE_MQTTS_X509)
    #define SERVER_ADDR_PREFIX_POST    "/api/v1/"
#elif defined(AZURE_MQTTS_SAS) || defined(AZURE_HTTPS_SAS)
    #define SERVER_ADDR_PREFIX_POST    "/devices/"
#endif


/**
 * @brief Server Address Path Prefix for GET or getting data from server. Used for HTTP and CoAP, ignored for MQTT
 */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define SERVER_ADDR_PREFIX_GET    "/api/v1/"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define SERVER_ADDR_PREFIX_GET    "/api/v1/"
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define SERVER_ADDR_PREFIX_GET    "/api/v1/"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define SERVER_ADDR_PREFIX_GET    "/api/v1/"
#elif defined(AWS_MQTTS_X509) || defined(AWS_HTTPS_X509)
    #define SERVER_ADDR_PREFIX_GET    "/api/v1/"
#elif defined(AZURE_MQTTS_SAS) || defined(AZURE_HTTPS_SAS) || defined(AZURE_MQTTS_X509)
    #define SERVER_ADDR_PREFIX_GET    "/devices/"
#endif

/**
 * @brief Server Address for sending Telemetry Path Suffix. If SERVER_PATH_ACCESS_TOKEN_POST_ENABLED is equal to 0, 
 *        then the complete address for sending data to the cloud is IOT_BROKER_ADDRESS_POST+SERVER_ADDR_SUFFIX_TELE.
 *        If SERVER_PATH_ACCESS_TOKEN_POST_ENABLED is equal to 1, then the complete address is 
 *        IOT_BROKER_ADDRESS_POST+SERVER_ADDR_PREFIX_POST+IMEI+SERVER_ADDR_SUFFIX_TELE.
 *        Only used for CoAP and HTTP. Value ignored for MQTT.
 */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define SERVER_ADDR_SUFFIX_TELE "/api/v1/integrations/http/6accb121-2cb5-787b-c645-2f5021538a25"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_TELE "/api/v1/integrations/http/bcf7402f-7c5b-b294-6724-eafd10f69f72"    
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_TELE "/i/1fe11c23-a3d0-2863-63b2-87d67cc52f6d"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_TELE "/i/1fe11c23-a3d0-2863-63b2-87d67cc52f6d"
#elif defined(AWS_MQTTS_X509)
    #define SERVER_ADDR_SUFFIX_TELE    ""
#elif defined(AWS_HTTPS_X509)
    #define SERVER_ADDR_SUFFIX_TELE    "/topics/topic?qos=1"
#elif  defined(AZURE_HTTPS_SAS)
    #define SERVER_ADDR_SUFFIX_TELE    "/messages/events?api-version=2021-04-12"
#elif defined(AZURE_MQTTS_SAS) || defined(AZURE_MQTTS_X509)
    #define SERVER_ADDR_SUFFIX_TELE    "/messages/events?api-version=2021-04-12"
#endif

/**
 * @brief Server Address Attribute Path Suffix. Used for ThingsBoard HTTP GET or getting data from server. Value ignored for MQTT.
 */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define SERVER_ADDR_SUFFIX_ATTR    "/attributes"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_ATTR    "/attributes"
#elif defined(AWS_HTTPS_X509) || defined(AWS_MQTTS_X509)
    #define SERVER_ADDR_SUFFIX_ATTR    ""
#elif defined(AZURE_MQTTS_X509) || defined(AZURE_MQTTS_SAS)
    #define SERVER_ADDR_SUFFIX_ATTR    "/messages/devicebound/#"
#elif defined(AZURE_HTTPS_SAS)
    #define SERVER_ADDR_SUFFIX_ATTR    "/messages/deviceBound?api-version=2021-04-12"
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_ATTR    "/attributes"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_ATTR    "/attributes"
#endif


/**
 * @brief Server Address FOTA Path Suffix. Used for ThingsBoard HTTP. Value ignored if not using HTTP.
 */
#ifdef THINGSBOARD_HTTP_INTEGRATION
    #define SERVER_ADDR_SUFFIX_FOTA    "/firmware?"
#elif defined(THINGSBOARD_HTTPS_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_FOTA    "/firmware?"
#elif defined(AWS_HTTPS_X509) || defined(AWS_MQTTS_X509) || defined(AZURE_MQTTS_X509) || defined(AZURE_MQTTS_SAS) || defined(AZURE_HTTPS_SAS)
    #define SERVER_ADDR_SUFFIX_FOTA    ""
#elif defined(THINGSBOARD_COAP_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_FOTA    "/firmware?"
#elif defined(THINGSBOARD_COAPS_INTEGRATION)
    #define SERVER_ADDR_SUFFIX_FOTA    "/firmware?"
#endif

/* x509 defines */
#define ROOT_CA1   "-----BEGIN CERTIFICATE-----\n" \
                    "MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF\n" \
                    "ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6\n" \
                    "b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL\n" \
                    "MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv\n" \
                    "b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj\n" \
                    "ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM\n" \
                    "9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw\n" \
                    "IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6\n" \
                    "VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L\n" \
                    "93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm\n" \
                    "jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC\n" \
                    "AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA\n" \
                    "A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI\n" \
                    "U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs\n" \
                    "N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv\n" \
                    "o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU\n" \
                    "5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy\n" \
                    "rqXRfboQnoZsG4q5WTP468SQvvG5\n" \
                    "-----END CERTIFICATE-----\n"

#if defined(THINGSBOARD_HTTP_INTEGRATION)||defined(THINGSBOARD_HTTPS_INTEGRATION)||defined(THINGSBOARD_COAP_INTEGRATION)||defined(THINGSBOARD_COAPS_INTEGRATION)
    #define CERTIFICATE     NULL
    #define PVT_KEY         NULL
    #define SYMMETRIC_KEY   NULL
    #define SAS_EXPIRE_TIME_S   0
    #define SECURITY_TYPE   TLS_ANONYMOUS
#elif defined(AZURE_HTTPS_SAS) || defined(AZURE_MQTTS_SAS)
    #define CERTIFICATE     NULL
    #define PVT_KEY         NULL
    #define SYMMETRIC_KEY   "r/IeIdKZ1nUeC+uTAW3l/hfBVDs3c8kGK8IjhpVvY2Q="
    #define SAS_EXPIRE_TIME_S   43200 // 30 days of seconds
    #define SECURITY_TYPE   TLS_SAS_TOKENS
#elif defined(AWS_HTTPS_X509) || defined(AWS_MQTTS_X509) || defined(AZURE_MQTTS_X509) 
    #define CERTIFICATE    "-----BEGIN CERTIFICATE-----\n" \
                            "MIIDWjCCAkKgAwIBAgIVALvbsEN71DO++QVVrGeRkoK8dkiSMA0GCSqGSIb3DQEB\n" \
                            "CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n" \
                            "IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0yMzAxMDYxODA4\n" \
                            "NDNaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n" \
                            "dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDrsc7DQVsmdYk3+AI2\n" \
                            "RtqKEj0tSp7VIrziPYg0LFAydpj84WxgSN+l90eMD9/t2SwymqKREfGAgy1jpmQQ\n" \
                            "Gf/hJVYEb1rHrJlzMlqnOBEF+lS16+ca8kio4oNUF6NiLJfn2kT/VXj1Gztk0tzI\n" \
                            "tiGKy3GBWi43Ihhta+2t3cSKirqHKkxEmSH8V+4lQM2PBYz4momQC0s/0CCccfnb\n" \
                            "6k1qU28uHseFK6x/4m4hdmh88KatqOXlFfOslH97PFO3YUhf5OVKG264600auJME\n" \
                            "dNX84Io3TO7ZSHxWRbHBCT4JjMGc0Ce8daEMuv569ROzQAlNrV+qdiLgbWzm3r3x\n" \
                            "Es0/AgMBAAGjYDBeMB8GA1UdIwQYMBaAFGA6X971iOXLY9sfOsnlstSFSErSMB0G\n" \
                            "A1UdDgQWBBRdzaKsHE9+w68oNJmc5iAEOOvbADAMBgNVHRMBAf8EAjAAMA4GA1Ud\n" \
                            "DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAndb4f87cY0GRQdo9UnucHNXW\n" \
                            "jHQ9Rnpc3D6Tx97KOcidFAc5DdytczaogL+l9W5CcNT+XiO94mthXIMlB5WY894O\n" \
                            "IMHb/289l3y/cj0NFi0u5DVhqAYjD+zGrCazhdk1A9Udf1X5BHPREYC2QUWoKScX\n" \
                            "w4rw9qcHPP8XZcRGqKKsZHbpMgtneI3gmBFIAlz5T4bDFDiw8yuOQCuqHxKsXQ45\n" \
                            "SIYzFvGhSGnKV25DiAhIOcQKWV1VFEuf2vQud+iyIToiL5lladDu/N/aG2oHBuh7\n" \
                            "KdE3JleSVocHgxX/fEhAWE5kkARMjis8KtjCifc4UGhA74oPUu6M1U3PsAH29w==\n" \
                            "-----END CERTIFICATE-----\n" // EPI AWS Instance


    #define PVT_KEY     "-----BEGIN RSA PRIVATE KEY-----\n" \
                        "MIIEpAIBAAKCAQEA67HOw0FbJnWJN/gCNkbaihI9LUqe1SK84j2INCxQMnaY/OFs\n" \
                        "YEjfpfdHjA/f7dksMpqikRHxgIMtY6ZkEBn/4SVWBG9ax6yZczJapzgRBfpUtevn\n" \
                        "GvJIqOKDVBejYiyX59pE/1V49Rs7ZNLcyLYhistxgVouNyIYbWvtrd3Eioq6hypM\n" \
                        "RJkh/FfuJUDNjwWM+JqJkAtLP9AgnHH52+pNalNvLh7HhSusf+JuIXZofPCmrajl\n" \
                        "5RXzrJR/ezxTt2FIX+TlShtuuOtNGriTBHTV/OCKN0zu2Uh8VkWxwQk+CYzBnNAn\n" \
                        "vHWhDLr+evUTs0AJTa1fqnYi4G1s5t698RLNPwIDAQABAoIBABEKH8qZ9P8IzEzR\n" \
                        "j0dhQ/drbiTSGj2Kb2Fj1W8ALSQY0uKlYXJskk6rW+7STvwhEULvTwVx6KXD1go4\n" \
                        "Q0+usYMTce6MJmH9JnLflxIzyXdHK8yjK2gVUA91oMz5kIROeQT4ELjs8vu1ZkvZ\n" \
                        "usB/+ljQcHpLjL/LpTxz6xEA0W66iUwtjWVdaAo2d8JvnlQMy61udwz4peXp4IeQ\n" \
                        "7aMaIQS+jQrWheJ1OCb9UoTHu+dhm+4atQDlA+PpuciMm3UHGKbRdSg+5716aS0U\n" \
                        "iEhOXS0o/4x1f08jc4n3klMx3biIcW6UIiHPrJpwD2NqHH7cpsFghybcuN+Fe156\n" \
                        "1jj6xuECgYEA/f1fw9zeT3AglhvdHbvL/G5shVsdGc1xl1wkFNXbRdRrAdaeP6bn\n" \
                        "N/R4QQ0zExRzt8Vqv+QsbfQzk8FWU4HN37FxoH4pGR4EvEGqqREFzZnhqMFE1rhz\n" \
                        "0xuMv/RukmnOyyC0FK8KJ8U81GjkpamvX0iX7YJtBInHcEFr9Hb7aGcCgYEA7Y9d\n" \
                        "TgB8nFIowcfv6R26LHYuNN2GMKjz9myjoZdOrAJtCAMg9udI9f39RpOMBdBcxI/r\n" \
                        "2mGgiasbj6RfdnMIkOKjd0cibEcJN/Cgsng1I6TngDvyD+GStNLQfIKSMSrq6zDr\n" \
                        "8r6nGhH3T9AQppwJLKp1RM2YGXZmSjy5h61CTWkCgYEA3lSlN7Zng/ILFFtfu19g\n" \
                        "uJ+Qr0uKtcN4453sl7B8OSwwX3OXIvDfBcQiYA9F3jXQ9dUCFOePXNCfNX/QKVk+\n" \
                        "9clGRc8p+qqkSobQ9R3JjqhdOHO15p+gA/PhyUYWZGPqeTUvbcurgBTPosAPJlTb\n" \
                        "BvVsyKOa+pYA1urtrRlaCZECgYEAyNzAYrSsqCwbxAV+x9fX08I1PYPU843XgZPI\n" \
                        "I1hhL7V5ZR3oIHvcyAnyvlsBOOU9mwGpxWNorx6bVjAAe5G2O3M2DiN6ap0BoWf3\n" \
                        "KA/Vtoa3K0kEWgM73WKm37AzVhlYunYJ+pzTg4qPVs+xVH54j2itcPh5U4Y15S1F\n" \
                        "HgAUIIkCgYAE9WGhc8+JtLkYMkuLaw38Zvm+apoAhJ/bQEJUhlHPYpH+3ciiDtVo\n" \
                        "8Zj2dF2/q9LayvPUr67vTakyXC+C1G/OM0HHZB8Ni3axpg8oE7drcv3zA65BrmOy\n" \
                        "KOFNLCzM+AKRfm36FHQNmNqCQyzktJ2eLVVUr6V+xj4RmHz8nHo+7w==\n" \
                        "-----END RSA PRIVATE KEY-----\n" // EPI AWS Key
    #define SYMMETRIC_KEY   NULL
    #define SAS_EXPIRE_TIME_S   0
    #define SECURITY_TYPE   TLS_X509
#endif

/**
 * @ingroup cellular_datatypes_paramstructs
 * @brief Represents a Cellular BLE config.
 */
#define APN_MAX_SIZE    64
typedef struct CellularBLEConfig
{
    char apnName[ APN_MAX_SIZE + 1 ];                 /**< APN name. */
} CellularBLEConfig_t;

extern CellularBLEConfig_t pdnConfig;

extern void parse_downlink(void);

typedef struct
{
    uint8_t  serialNumber[6];                    /* 8 byte serial number */
    uint32_t ulHardwareID;                       /* 32 bit ID that can be generated unique for a particular platform. */
    uint8_t  partNumber[6];                      /* 6 byte part number, ie. 800263 for P800000000263. */
    uint8_t  loraSubband;                        /* 8 bit lora sub-band */
    uint8_t  loraDevEUI[8];                      /* 8 byte app key for lora */
    uint8_t  loraJoinEUI[8];                     /* 8 byte app key for lora */
    uint8_t  loraAppKey[16];                     /* 16 byte app key for lora */
} ProductionTable_t;

/* OTA Defines */
#define otapal_FLASH_START               ( 0x1000 )
#define otapal_DESCRIPTOR_START          ( 0x0000 )
#define otapal_BANK_SIZE                 ( 0xA0000 )         /* Flash size available */
#define otapal_BLE_TBL_START             ( 0x90000 )
#define otapal_PROD_TBL_START            ( 0xA0000 )

/* Flag for signaling new data has been added to cell queue */
extern bool newDataAdded;

/* The task function to setup cellular with thread ready environment. */
extern void CellularTask( void * pvParameters );

/* The task function to manage the mqtt connection. */
extern void mqttTask( void * pvParameters );

//////////////////////////////////////////////////
/****                                        ****/
/****               BLUETOOTH                ****/
/****                                        ****/
//////////////////////////////////////////////////

/**< Used to activate/deactivate BLE in the main application
 * 
 *   0 = Deactivated
 *   1 = Activated
 */
#define BLE_ACTIVE     0

/** < BLE TX power boost in dBm. For details, see sd_ble_gap_tx_power_set() in ble_gap.h */
#define BLE_TX_POWER_BOOST      8


//////////////////////////////////////////////////
/****                                        ****/
/****                BUTTON                  ****/
/****                                        ****/
//////////////////////////////////////////////////

/* < Delay in ms for debouncing the button */
#define DEBOUNCE_DELAY      pdMS_TO_TICKS(80)        

/* < Upon button release, the timer is compared with this value to determine whether to trigger
    the short button push callback or the long push callback */
#define BTN_LONG_PUSH_DURATION          3000

/* < The timer is compared with this value to determine whether to trigger the very long push callback*/
#define BTN_VERY_LONG_PUSH_DURATION     10000 


//////////////////////////////////////////////////
/****                                        ****/
/****                  LED                   ****/
/****                                        ****/
//////////////////////////////////////////////////

#define LED_CELL_OK             LED_SINGLE_BLINK
#define LED_FAIL_TO_SEND        LED_DOUBLE_BLINK
#define LED_FAIL_TO_REG         LED_TRIPLE_BLINK
#define ENABLE_LED_OPERATION    true

/** < Amount of time, in milliseconds, that the LED remains on during slow blinks*/
#define LED_SLOW_BLINK_LENGTH               pdMS_TO_TICKS(1000)

/** < Amount of time, in milliseconds, between slow blinks */
#define LED_SLOW_BLINK_INTERVAL             pdMS_TO_TICKS(1000)

/** < Amount of time, in milliseconds, that the LED remains on during fast blinks */
#define LED_FAST_BLINK_LENGTH               pdMS_TO_TICKS(200)

/** < Amount of time, in milliseconds, between fast blinks */
#define LED_FAST_BLINK_INTERVAL             pdMS_TO_TICKS(200)

/** < Amount of time, in milliseconds, that the LED remains on during extra fast blinks */
#define LED_EXTRA_FAST_BLINK_LENGTH         pdMS_TO_TICKS(75)

/** < Amount of time, in milliseconds, between extra fast blinks */
#define LED_EXTRA_FAST_BLINK_INTERVAL       pdMS_TO_TICKS(75)

/** < Amount of time, in milliseconds, that the LED remains on during alive blinks */
#define LED_ALIVE_BLINK_LENGTH              pdMS_TO_TICKS(100)

/** < Amount of time, in milliseconds, between alive blinks */
#define LED_ALIVE_BLINK_INTERVAL            pdMS_TO_TICKS(15000)

/** < Amount of time, in milliseconds, that the LED remains on during multi-blink patterns */
#define LED_MULTI_BLINK_LENGTH              pdMS_TO_TICKS(135)

/** < Amount of time, in milliseconds, between slow multi-blinks */
#define LED_MULTI_BLINK_INTERVAL            pdMS_TO_TICKS(1000)

extern TaskHandle_t ledTaskHandle;

///////////////////////////////////////
/**         GLOBAL FUNCTIONS        **/
///////////////////////////////////////
/* Function for handling sensor power enable operation */
extern void sensorPwrEnConfig( bool status );

/* *INDENT-OFF* */
#ifdef __cplusplus
    }
#endif
/* *INDENT-ON* */

#endif /* __MAIN_H__ */