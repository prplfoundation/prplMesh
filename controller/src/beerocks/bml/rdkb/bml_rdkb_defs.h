/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_RDKB_DEFS_H_
#define _BML_RDKB_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bml_defs.h"
#include <stdint.h>

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/
#define BML_EVENT_TYPE_STEERING                                                                    \
    BML_EVENT_TYPE_MAXIMUM +                                                                       \
        1 /* General steering event (refer to enum BML_STEERING_EVENT_TYPE for subevents) */

#define BML_RET_INVALID_CONFIGURATION BML_RET_END + 1 /* 13 - rdkb steering invalid configuration */
#define BML_RET_CLIENT_NOT_FOUND                                                                   \
    BML_RET_END + 2 /* 14 - rdkb steering client not found on DB(beerocks) */
#define BML_RET_CLIENT_NOT_CONFIGURED                                                              \
    BML_RET_END + 3                                  /* 15 - rdkb steering client not configured  */
#define BML_RET_CLIENT_NOT_CONNECTED BML_RET_END + 4 /* 16 - rdkb steering client not connected  */
#define BML_RET_REGISTERTION_FAIL BML_RET_END + 5    /* 17 - rdkb steering registration failure  */

/****************************************************************************/
/******************************** Structures ********************************/
/****************************************************************************/

struct BML_STEERING_AP_CONFIG {
    BML_MAC_ADDR bssid;
    uint32_t utilCheckIntervalSec;   /**< Channel utilization check interval     */
    uint32_t utilAvgCount;           /**< Number of samples to average           */
    uint32_t inactCheckIntervalSec;  /**< Client inactive check internval        */
    uint32_t inactCheckThresholdSec; /**< Client inactive threshold              */
};

struct BML_STEERING_CLIENT_CONFIG {
    uint32_t snrProbeHWM;      /**< Probe response SNR high water mark    */
    uint32_t snrProbeLWM;      /**< Probe response SNR low water mark     */
    uint32_t snrAuthHWM;       /**< Auth response SNR high water mark     */
    uint32_t snrAuthLWM;       /**< Auth response SNR low water mark      */
    uint32_t snrInactXing;     /**< Inactive SNR crossing threshold       */
    uint32_t snrHighXing;      /**< High SNR crossing threshold           */
    uint32_t snrLowXing;       /**< Low SNR crossing threshold            */
    uint32_t authRejectReason; /**< Inactive RSSI crossing threshold      */
};

typedef enum {
    WIFI_STEERING_SNR_UNCHANGED = 0, /**< SNR hasn't crossed        */
    WIFI_STEERING_SNR_HIGHER,        /**< SNR went higher           */
    WIFI_STEERING_SNR_LOWER          /**< SNR went lower            */
} BML_STEERING_SNR_CHANGE;

typedef enum {
    BML_STEERING_EVENT_PROBE_REQ = 1,     /**< Probe Request Event        */
    BML_STEERING_EVENT_CLIENT_CONNECT,    /**< Client Connect Event       */
    BML_STEERING_EVENT_CLIENT_DISCONNECT, /**< Client Disconnect Event    */
    BML_STEERING_EVENT_CLIENT_ACTIVITY,   /**< Client Active Change Event */
    BML_STEERING_EVENT_CHAN_UTILIZATION,  /**< Channel Utilization Event  */
    BML_STEERING_EVENT_SNR_XING,          /**< Client SNR Crossing Event  */
    BML_STEERING_EVENT_SNR,               /**< Instant Measurement Event  */
    BML_STEERING_EVENT_AUTH_FAIL          /**< Client Auth Failure Event  */
} BML_STEERING_EVENT_TYPE;

//events structs
struct BML_STEERING_DATA_RATE_INFO {
    uint8_t maxChwidth;        /**< Max bandwidth supported                */
    uint8_t maxStreams;        /**< Max spatial streams supported          */
    uint8_t phyMode;           /**< PHY Mode supported                     */
    uint8_t maxMCS;            /**< Max MCS  supported                     */
    uint8_t maxTxpower;        /**< Max TX power supported                 */
    uint8_t isStaticSmps;      /**< Operating in Static SM Power Save Mode */
    uint8_t isMUMimoSupported; /**< Supports MU-MIMO                       */
};
struct BML_STEERING_RRM_CAPS {
    uint8_t linkMeas;      /**< Supports link measurement      */
    uint8_t neighRpt;      /**< Supports neighbor reports      */
    uint8_t bcnRptPassive; /**< Supports Passive 11k scans     */
    uint8_t bcnRptActive;  /**< Supports Active 11k scans      */
    uint8_t bcnRptTable;   /**< Supports beacon report table   */
    uint8_t lciMeas;       /**< Supports LCI measurement       */
    uint8_t ftmRangeRpt;   /**< Supports FTM Range report      */
};

typedef enum {
    DISCONNECT_SOURCE_UNKNOWN = 0, /**< Unknown source             */
    DISCONNECT_SOURCE_LOCAL,       /**< Initiated locally          */
    DISCONNECT_SOURCE_REMOTE       /**< Initiated remotely         */
} BML_DISCONNECT_SOURCE;

typedef enum {
    DISCONNECT_TYPE_UNKNOWN = 0, /**< Unknown type               */
    DISCONNECT_TYPE_DISASSOC,    /**< Disassociation             */
    DISCONNECT_TYPE_DEAUTH       /**< Deauthentication           */
} BML_DISCONNECT_TYPE;

/**
 * Probe Request Event Data
 */
struct BML_STEERING_EV_PROB_REQ {
    BML_MAC_ADDR client_mac; /**< Client MAC Address         */
    uint8_t snr;             /**< SNR of probe frame         */
    uint8_t broadcast;       /**< True if broadcast probe    */
    uint8_t blocked;         /**< True if response blocked   */
};
/**
 * Client Connect Event Data
 */
struct BML_STEERING_EV_CONNECT {
    BML_MAC_ADDR client_mac;                         /**< Client MAC Address                     */
    uint8_t isBTMSupported;                          /**< Client supports BSS TM                 */
    uint8_t isRRMSupported;                          /**< Client supports RRM                    */
    uint8_t bandCap2G;                               /**< Client is 2.4GHz capable               */
    uint8_t bandCap5G;                               /**< Client is 5GHz capable                 */
    struct BML_STEERING_DATA_RATE_INFO datarateInfo; /**< Client supported datarate information  */
    struct BML_STEERING_RRM_CAPS rmCaps;             /**< Client supported RRM capabilites       */
};
/**
 * Client Disconnect Event Data
 */
struct BML_STEERING_EV_DISCONNECT {
    BML_MAC_ADDR client_mac;      /**< Client MAC Address         */
    uint8_t reason;               /**< Reason code of disconnect  */
    BML_DISCONNECT_SOURCE source; /**< Source of disconnect       */
    BML_DISCONNECT_TYPE type;     /**< Disconnect Type            */
};
/**
 * Client Activity Change Event Data
 */
struct BML_STEERING_EV_ACTIVITY {
    BML_MAC_ADDR client_mac; /**< Client MAC Address         */
    uint8_t active;          /**< True if client is active   */
};
/**
 * Client SNR Crossing Event Data
 */
struct BML_STEERING_EV_SNR_XING {
    BML_MAC_ADDR client_mac;             /**< Client MAC Address         */
    uint8_t snr;                         /**< Clients current SNR        */
    BML_STEERING_SNR_CHANGE inactveXing; /**< Inactive threshold Value   */
    BML_STEERING_SNR_CHANGE highXing;    /**< High threshold Value       */
    BML_STEERING_SNR_CHANGE lowXing;     /**< Low threshold value        */
};
/**
 * Client SNR Measurement Event Data
 */
struct BML_STEERING_EV_SNR {
    BML_MAC_ADDR client_mac; /**< Client MAC Address         */
    uint8_t snr;             /**< Clients current SNR        */
};
/**
 * @brief Auth Failure Event Data
 * This data is provided with @b WIFI_STEERING_EVENT_AUTH_FAIL
 */
struct BML_STEERING_EV_AUTH_FAIL {
    BML_MAC_ADDR client_mac; /**< Client MAC Address         */
    uint8_t snr;             /**< SNR of auth frame          */
    uint8_t reason;          /**< Reject Reason              */
    uint8_t bsBlocked;       /**< True if purposely blocked  */
    uint8_t bsRejected;      /**< True if rejection sent     */
};

struct BML_EVENT_STEERING {
    BML_STEERING_EVENT_TYPE type;    /**< Event Type                 */
    uint32_t steeringGroupIndex;     /**< steering_group index       */
    BML_MAC_ADDR bssid;              /**< bssid event is from        */
    unsigned long long timestamp_ms; /**< Optional: Event Timestamp  */
    union {
        struct BML_STEERING_EV_PROB_REQ probeReq;     /**< Probe Request Data         */
        struct BML_STEERING_EV_CONNECT connect;       /**< Client Connect Data        */
        struct BML_STEERING_EV_DISCONNECT disconnect; /**< Client Disconnect Data     */
        struct BML_STEERING_EV_ACTIVITY activity;     /**< Client Active Change Data  */
        struct BML_STEERING_EV_SNR_XING snrXing;      /**< Client SNR Crossing Data   */
        struct BML_STEERING_EV_SNR snr;               /**< Client Measured SNR Data   */
        struct BML_STEERING_EV_AUTH_FAIL authFail;    /**< Auth Failure Data          */
    } data;
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BML__RDKB_DEFS_H_ */
