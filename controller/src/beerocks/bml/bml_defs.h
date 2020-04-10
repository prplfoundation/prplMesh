/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_DEFS_H_
#define _BML_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/* Returns Codes */
#define BML_RET_OK 0               /* Operation succeeded */
#define BML_RET_OP_FAILED 1        /* Operation failed */
#define BML_RET_OP_NOT_SUPPORTED 2 /* Operation not supported */
#define BML_RET_INVALID_ARGS 3     /* Invalid arguments */
#define BML_RET_MEM_FAIL 4         /* Memory allocation failed */
#define BML_RET_INIT_FAIL 5        /* BML Initialization failed */
#define BML_RET_CONNECT_FAIL 6     /* Connection to Beerocks failed */
#define BML_RET_NO_DATA 7          /* No more data available */
#define BML_RET_INVALID_DATA 8     /* Unexpected data */
#define BML_RET_TIMEOUT 9          /* Operation timed out */
#define BML_RET_CMDU_FAIL 10       /* cmdu message failure */
#define BML_RET_END 11             /* end of error return codes */

/* General Definitions */
#define BML_MAC_ADDR_LEN 6          /* MAC Address Length */
#define BML_IP_ADDR_LEN 4           /* IP Address Length */
#define BML_VERSION_LEN 16          /* Version Length */
#define BML_IFACE_NAME_LEN (32 + 4) /* Maximal interface name length (32) + null termination */
#define BML_DEV_INFO_LEN 32         /* Device information string length */

/* BML Node Types */
#define BML_NODE_TYPE_GW 1     /* GW Node */
#define BML_NODE_TYPE_IRE 2    /* IRE Node */
#define BML_NODE_TYPE_CLIENT 3 /* Client/STA Node */

/* BML Node Definitions */
#define BML_NODE_NAME 64      /* Node hostname */
#define BML_NODE_MAX_RADIOS 3 /* Maximal number of radios */
#define BML_NODE_MAX_VAPS 16  /* Maximal number of VAPs */
#define BML_NODE_IFACE_NAME_LEN                                                                    \
    (32 +                                                                                          \
     4) /* Maximal interface name length (32) + 1 byte null termination + 3 bytes for alignment */
#define BML_NODE_SSID_LEN                                                                          \
    (32 + 4) /* Maximal SSID length 32 + 1 byte null termination + 3 bytes for alignment */
#define BML_NODE_PASS_LEN (64 + 1)          /* Maximal PASS length 32 + null termination */
#define BML_NODE_USER_PASS_LEN (64 + 1)     /* Maximal ADMIN_PASS length 64 + null termination */
#define BML_NODE_SERIAL_NUMBER_LEN (64 + 1) /* Maximal SERIAL_NUMBER length 64 + null termination */
#define BML_NODE_RESTRICTED_CHANNELS_LEN 40 /* Maximal number of channels  */

/* BML Node States */
#define BML_NODE_STATE_DISCONNECTED 0
#define BML_NODE_STATE_CONNECTING 1
#define BML_NODE_STATE_CONNECTED 2
#define BML_NODE_STATE_UNKNOWN 3

/* BML Wi-Fi Definitions */
#define BML_WLAN_DRIVER_VERSION_LEN (32 + 4) /* Maximal string length (32) + null termination */
#define BML_SSID_MAX_LENGTH                                                                        \
    (32 + 1 + 3) /* maximum SSID string length, including null termination */
#define BML_PASS_MAX_LENGTH                                                                        \
    (64 + 1 + 3) /* maximum network encryption key string length, including null termination */

/* BML Wi-Fi Vendor */
#define BML_WLAN_VENDOR_UNKNOWN 0 /* Unknown vendor */
#define BML_WLAN_VENDOR_INTEL 1   /* Intel Corporation */

/* BML Wi-Fi Security Modes */
#define BML_WLAN_SEC_NONE 0         /* No Wi-Fi Security */
#define BML_WLAN_SEC_WEP64 1        /* WEP-64 Encryption */
#define BML_WLAN_SEC_WEP128 2       /* WEP-128 Encryption */
#define BML_WLAN_SEC_WPA_PSK 3      /* WPA-Personal Encryption */
#define BML_WLAN_SEC_WPA2_PSK 4     /* WPA2-Personal Encryption */
#define BML_WLAN_SEC_WPA_WPA2_PSK 5 /* WPA-WPA2-Personal Encryption */

#define BML_WLAN_SEC_NONE_STR "None"
#define BML_WLAN_SEC_WEP64_STR "WEP-64"
#define BML_WLAN_SEC_WEP128_STR "WEP-128"
#define BML_WLAN_SEC_WPA_PSK_STR "WPA-Personal"
#define BML_WLAN_SEC_WPA2_PSK_STR "WPA2-Personal"
#define BML_WLAN_SEC_WPA_WPA2_PSK_STR "WPA-WPA2-Personal"

/* BML Statistic Types */
#define BML_STAT_TYPE_RADIO 1  /* Radio Statistics */
#define BML_STAT_TYPE_VAP 2    /* VAP Statistics */
#define BML_STAT_TYPE_CLIENT 3 /* Client/STA Statistics */

/* BML Event Types */
#define BML_EVENT_TYPE_BSS_TM_REQ 1                      /* BSS TM Request (11v) */
#define BML_EVENT_TYPE_BEACON_MEASUREMENT 2              /* Beacon Measurement Request (11k) */
#define BML_EVENT_TYPE_BH_ROAM_REQ 3                     /* Backhaul Roam Request */
#define BML_EVENT_TYPE_CLIENT_ALLOW_REQ 4                /* Client Allow Request */
#define BML_EVENT_TYPE_CLIENT_DISALLOW_REQ 5             /* Client Disallow Request */
#define BML_EVENT_TYPE_ACS_START 6                       /* ACS Start*/
#define BML_EVENT_TYPE_CSA_NOTIFICATION 7                /* CSA Notification*/
#define BML_EVENT_TYPE_CAC_STATUS_CHANGED_NOTIFICATION 8 /* CAC Completed Notification*/
/* THIS IS THE MAXIMUM FOR THIS FILE. LARGER VALUES WILL BE USED FOR OTHER BML EVENT TYPES IN OTHER FILES */
#define BML_EVENT_TYPE_MAXIMUM 49

/* BML Vap information (use with BML_VAP_INFO) */
#define BML_WLAN_VAP_TYPE_BAP 0  /* Back AP */
#define BML_WLAN_VAP_TYPE_FAP 1  /* Front AP */
#define BML_WLAN_AUTH_OPEN 0     /* OPEN */
#define BML_WLAN_AUTH_WPA2_PSK 1 /* WPA2_PSK */
#define BML_WLAN_ENC_NONE 0      /* no encryption */
#define BML_WLAN_ENC_TKIP 1      /* Temporal Key Integrity Protocol */
#define BML_WLAN_ENC_AES 2       /* Advanced Encryption Standart */

/* Channel Scan Parameters Placeholders */
#define BML_CHANNEL_SCAN_INVALID_PARAM -1         /* Invalid parameter placeholder */
#define BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE 32 /* Maximal size of the channel pool */
#define BML_CHANNEL_SCAN_ENUM_LIST_SIZE 8

/****************************************************************************/
/******************************* General Types ******************************/
/****************************************************************************/

/**
 * Internal beerocks context.
 */
typedef void *BML_CTX;

typedef unsigned char BML_MAC_ADDR[BML_MAC_ADDR_LEN];

/****************************************************************************/
/******************************** Structures ********************************/
/****************************************************************************/
/**
* Vap information.
*/
struct BML_VAP_INFO {
    /* Back or front AP */
    uint8_t type;
    /* open wpa2_psk or no authentication */
    uint8_t auth;
    /* AES TKIP or no encryption */
    uint8_t enc;
    /* padding to allign to 4 byte */
    uint8_t reserved_1;
    /* al MAC address */
    uint8_t al_mac[BML_MAC_ADDR_LEN];
    /* padding to allign to 4 byte */
    uint8_t reserved_2[2];
    /* radio uniq ID */
    uint8_t ruid[BML_MAC_ADDR_LEN];
    /* padding to allign to 4 byte */
    uint8_t reserved_3[2];
    /* BSSID */
    uint8_t bssid[BML_MAC_ADDR_LEN];
    /* padding to allign to 4 byte */
    uint8_t reserved_4[2];
    /* SSID */
    char ssid[BML_SSID_MAX_LENGTH];
    /* network encryption key */
    char key[BML_PASS_MAX_LENGTH];
};

/**
 * Device information.
 */
struct BML_DEVICE_INFO {

    /* Device manufacturer name (e.g. Intel Corporation) */
    char manufacturer[BML_DEV_INFO_LEN];

    /* Device model name */
    char model_name[BML_DEV_INFO_LEN];

    /* Device serial number */
    char serial_number[BML_DEV_INFO_LEN];

    /* LAN interface name */
    char lan_iface_name[BML_IFACE_NAME_LEN];

    /* LAN interface IP address */
    uint32_t lan_ip_address;

    /* LAN interface network mask */
    uint32_t lan_network_mask;

    /* WAN interface name */
    char wan_iface_name[BML_IFACE_NAME_LEN];

    /* WAN interface IP address */
    uint32_t wan_ip_address;

    /* WAN interface network mask */
    uint32_t wan_network_mask;
};

/**
 * A structure representing a node in the network map.
 */
struct BML_NODE {

    /* The type of the node (GW, IRE or CLIENT) */
    uint8_t type;

    /* Current node state (e.g. BLM_NODE_STATE_xxx) */
    uint8_t state;

    /* Wi-Fi channel or 0 for wired connection. */
    uint8_t channel;

    /* Bandwidth */
    uint8_t bw;

    /* channel_ext_above_secondary. */
    uint8_t channel_ext_above_secondary;

    /* The MAC address of the node. */
    uint8_t mac[BML_MAC_ADDR_LEN];

    /* The IP address of the node. */
    uint8_t ip_v4[BML_IP_ADDR_LEN];

    /* The MAC address of the node's parent bridge (Zeroed for the GW node). */
    uint8_t parent_bridge[BML_MAC_ADDR_LEN];

    /* The BSSID to which the client is connected. */
    uint8_t parent_bssid[BML_MAC_ADDR_LEN];

    /* Human readable node name. */
    char name[BML_NODE_NAME];

    /* DL_RSSI of the node*/
    int8_t rx_rssi;

    /* Reserved bytes for alignment */
    uint8_t reserved[3];

    /* Extra information for the specific node. */
    union N_DATA {

        /* GW or IRE node */
        struct N_GW_IRE {
            /* The backhaul_mac to which the bridge is useing (wireless or wired). */
            uint8_t backhaul_mac[BML_MAC_ADDR_LEN];
            uint8_t reserved[2];

            /* Wi-Fi Radio */
            struct N_RADIO {

                /* Interface name */
                char iface_name[BML_NODE_IFACE_NAME_LEN];

                /* The MAC address of the radio. */
                uint8_t radio_mac[BML_MAC_ADDR_LEN];

                /* Radio Vendor - BML_WLAN_VENDOR_XXX */
                uint8_t vendor;

                /* Radio channel. */
                uint8_t channel;

                /* CAC completed */
                uint8_t cac_completed;

                /* AP active state: 0=disabled, 1=enabled, */
                uint8_t ap_active;

                /* Bandwidth */
                uint8_t bw;

                /* channel_ext_above_secondary. */
                uint8_t channel_ext_above_secondary;

                /* Radio driver version */
                char driver_version[BML_WLAN_DRIVER_VERSION_LEN];

                /* Radio identifier */
                uint8_t radio_identifier[BML_MAC_ADDR_LEN];

                /* Reserved bytes for alignment */
                uint8_t reserved[2];

                /* Virtual AP */
                struct N_VAP {

                    /* Published AP SSID */
                    char ssid[BML_NODE_SSID_LEN];

                    /* VAP MAC address */
                    uint8_t bssid[BML_MAC_ADDR_LEN];

                    /* Backhaul VAP */
                    uint8_t backhaul_vap;

                    /* Reserved for future use. */
                    uint8_t reserved[1];

                } vap[BML_NODE_MAX_VAPS];

            } radio[BML_NODE_MAX_RADIOS];

        } gw_ire;

    } data;
};

/**
 * Beerocks statistics structure.
 */
struct BML_STATS {

    /**
     * The type of the node (Radio, VAP, Station etc.)
     */
    int type;

    /**
     * The MAC address of the node.
     */
    uint8_t mac[BML_MAC_ADDR_LEN];
    uint8_t reserved1[2];

    /**
     * Common stats among all types
     */
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t packets_sent;
    uint64_t packets_received;
    uint32_t errors_sent;
    uint32_t errors_received;
    uint32_t retrans_count;
    uint32_t measurement_window_msec;

    /**
     * A union for node specific parameters.
     * Should be accessed based on the type of the node.
     */
    union S_TYPE {

        /**
         * Structure representing the Radio specific statistics.
         */
        struct S_RADIO {
            uint32_t bss_load;
            int32_t noise; // Noise
        } radio;

        /**
         * Structure representing the VAP specific statistics.
         */
        struct S_VAP {
            uint64_t unicast_packets_sent;       // UnicastPacketsSent
            uint64_t unicast_packets_received;   // UnicastPacketsReceived
            uint64_t multicast_packets_sent;     // MulticastPacketsSent
            uint64_t multicast_packets_received; // MulticastPacketsReceived
            uint64_t broadcast_packets_sent;     // BroadcastPacketsSent
            uint64_t broadcast_packets_received; // BroadcastPacketsReceived
        } vap;

        /**
         * Structure representing the Client/STA specific statistics.
         */
        struct S_CLIENT {
            int16_t signal_strength; // SignalStrength
            uint8_t reserved[2];
            uint32_t last_data_downlink_rate; // LastDataDownlinkRate
            uint32_t last_data_uplink_rate;   // LastDataUplinkRate
            uint32_t retransmissions;         // Retransmissions
        } client;

    } uType;

    /* TBD */
};

/**
 * Beerocks event structure.
 */
struct BML_EVENT {

    /**
     * BML private context.
     */
    BML_CTX ctx;

    /**
     * Event type
     */
    int type;

    /**
     * Pointer to the event data, to be casted to the appropriate struct type
     */
    void *data;
};

/*
 * Beerocks BSS TM request (11v) event
 */
struct BML_EVENT_BSS_TM_REQ {
    uint8_t target_bssid[BML_MAC_ADDR_LEN];
    uint8_t disassoc_imminent;
    uint8_t reserved;
};

/*
 * Beerocks backhaul roam request event
 */
struct BML_EVENT_BH_ROAM_REQ {
    uint8_t bssid[BML_MAC_ADDR_LEN];
    uint8_t channel;
    uint8_t reserved;
};

/*
 * Beerocks client allow request event
 */
struct BML_EVENT_CLIENT_ALLOW_REQ {
    uint8_t sta_mac[BML_MAC_ADDR_LEN];
    uint8_t hostap_mac[BML_MAC_ADDR_LEN];
    uint8_t ip[BML_IP_ADDR_LEN];
};

/*
 * Beerocks client disallow request event
 */
struct BML_EVENT_CLIENT_DISALLOW_REQ {
    uint8_t sta_mac[BML_MAC_ADDR_LEN];
    uint8_t hostap_mac[BML_MAC_ADDR_LEN];
    uint8_t reserved[4];
};

/*
 * Beerocks ACS start event
 */
struct BML_EVENT_ACS_START {
    uint8_t hostap_mac[BML_MAC_ADDR_LEN];
    uint8_t reserved[2];
};

/*
 * Beerocks CSA notification event
 */
struct BML_EVENT_CSA_NOTIFICATION {
    uint8_t hostap_mac[BML_MAC_ADDR_LEN];
    uint8_t bandwidth;
    uint8_t channel;
    uint8_t channel_ext_above_primary;
    uint16_t vht_center_frequency;
    uint8_t reserved[5];
};

/*
 * Beerocks CAC completed notification event
 */
struct BML_EVENT_CAC_STATUS_CHANGED_NOTIFICATION {
    uint8_t hostap_mac[BML_MAC_ADDR_LEN];
    uint8_t cac_completed;
    uint8_t reserved;
};

/*
 * Beerocks beacon measurement (11k) event
 */
struct BML_EVENT_BEACON_MEASUREMENT {
    //TBD
};
/**
 * Iterator structure for exploring the network map received in the 
 * Network Map Query callback function.
 */
struct BML_NODE_ITER {

    /**
     * BML private context.
     */
    BML_CTX ctx;

    /**
     * Number of nodes available to the iterator.
     */
    int nodes_num;

    /**
     * '1'= last node , '0' = more nodes are pending in next callback.
     */
    int last_node;

    /**
     * Function for resetting the iterator position to the 
     * beginning of the network map.
     *
     * @return BML_RET_OK on success.
     */
    int (*first)();

    /**
     * Function for moving to the next node in the network map.
     *
     * @return BML_RET_OK on success.
     */
    int (*next)();

    /**
     * Function for retrieving the data of the current node.
     *
     * @return Pointer to the node parameters structure.
     */
    struct BML_NODE *(*get_node)();
};

/**
 * Iterator structure for exploring the statistics list received in the 
 * Statistics Update callback function.
 */
struct BML_STATS_ITER {

    /**
     * BML private context.
     */
    BML_CTX ctx;

    /**
     * Number of nodes available to the iterator.
     */
    int nodes_num;

    /**
     * Function for resetting the iterator position to the 
     * beginning of the statistics list.
     *
     * @return BML_RET_OK on success.
     */
    int (*first)();

    /**
     * Function for moving to the next node in the statistics list.
     *
     * @return BML_RET_OK on success.
     */
    int (*next)();

    /**
     * Function for retrieving the data of the current node.
     *
     * @return Pointer to the node statistics structure.
     */
    struct BML_STATS *(*get_node)();
};

struct BML_NEIGHBOR_AP {
    //The current service set identifier in use by the neighboring WiFi SSID. The value MAY be empty for hidden SSIDs.
    char ap_SSID[BML_SSID_MAX_LENGTH];
    //The BSSID used for the neighboring WiFi SSID.
    uint8_t ap_BSSID[BML_MAC_ADDR_LEN];
    //The mode the neighboring WiFi radio is operating in. Enumerate
    uint8_t ap_Mode;
    //The current radio channel used by the neighboring WiFi radio.
    uint32_t ap_Channel;
    //An indicator of radio signal strength (RSSI) of the neighboring WiFi radio measured in dBm, as an average of the last 100 packets received.
    int32_t ap_SignalStrength;
    //The type of encryption the neighboring WiFi SSID advertises. Enumerate List.
    uint8_t ap_SecurityModeEnabled[BML_CHANNEL_SCAN_ENUM_LIST_SIZE];
    //The type of encryption the neighboring WiFi SSID advertises. Enumerate List.
    uint8_t ap_EncryptionMode[BML_CHANNEL_SCAN_ENUM_LIST_SIZE];
    //Indicates the frequency band at which the radio this SSID instance is operating. Enumerate
    uint8_t ap_OperatingFrequencyBand;
    //List items indicate which IEEE 802.11 standards thisResultinstance can support simultaneously, in the frequency band specified byOperatingFrequencyBand. Enumerate List
    uint8_t ap_SupportedStandards[BML_CHANNEL_SCAN_ENUM_LIST_SIZE];
    //Indicates which IEEE 802.11 standard that is detected for this Result. Enumerate
    uint8_t ap_OperatingStandards;
    //Indicates the bandwidth at which the channel is operating. Enumerate
    uint8_t ap_OperatingChannelBandwidth;
    //Time interval (inms) between transmitting beacons.
    uint32_t ap_BeaconPeriod;
    //Indicator of average noise strength (indBm) received from the neighboring WiFi radio.
    int32_t ap_Noise;
    //Basic data transmit rates (in Kbps) for the SSID.
    uint32_t ap_BasicDataTransferRates[BML_CHANNEL_SCAN_ENUM_LIST_SIZE];
    //Data transmit rates (in Kbps) for unicast frames at which the SSID will permit a station to connect.
    uint32_t ap_SupportedDataTransferRates[BML_CHANNEL_SCAN_ENUM_LIST_SIZE];
    //The number of beacon intervals that elapse between transmission of Beacon frames containing a TIM element whose DTIM count field is 0. This value is transmitted in the DTIM Period field of beacon frames. [802.11-2012]
    uint32_t ap_DTIMPeriod;
    //Indicates the fraction of the time AP senses that the channel is in use by the neighboring AP for transmissions.
    uint32_t ap_ChannelUtilization;
};

/****************************************************************************/
/****************************** Callback Types ******************************/
/****************************************************************************/

/**
 * Network map query/update callback function. When registered, the 
 * function will be called on each query or update response of the 
 * networking map.
 * The callback receives an iterator to the received network map, which
 * allows easy access to the contents of the map.
 */
typedef void (*BML_NW_MAP_QUERY_CB)(const struct BML_NODE_ITER *);

/**
 * Statistics update callback function. When registered, the function
 * will be called on every update of the beerocks statistics.
 * The callback receives an iterator to the statistics list, which
 * allows easy access to the contents of the list.
 */
typedef void (*BML_STATS_UPDATE_CB)(const struct BML_STATS_ITER *);

/**
 * Event callback function. When registered, the function
 * will be called on every beerocks event that supports BML notifications
 * The callback receives a pointer to an event struct
 */
typedef void (*BML_EVENT_CB)(const struct BML_EVENT *);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BML_DEFS_H_ */
