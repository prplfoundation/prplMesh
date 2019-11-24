/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_CFG_H_
#define _BPL_CFG_H_

#include "bpl.h"
#include "bpl_err.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/* Wi-Fi Settings */
#define BPL_DEV_INFO_LEN (32 + 4) /* Device information string length */
#define BPL_SSID_LEN (32 + 1)     /* Maximal length of Wi-Fi SSID */
#define BPL_PASS_LEN (64 + 1)     /* Maximal length of Wi-Fi password */
#define BPL_SEC_LEN 32            /* Maximal length of Wi-Fi security string */
#define BPL_IFNAME_LEN 32         /* Maximal length of Wi-Fi interface name */
#define BPL_NUM_OF_INTERFACES 7   /* Maximal number of Interfaces: (3 APs + 3 STAs) + 1 Wired */
#define BPL_MNS_DATA_LEN 256      /* Maximal length of BPL MNS data */
#define BPL_BACK_VAPS_GROUPS 4 /* Backhaul VAPs Groups size, group contain 1 Vap for every radio */
#define BPL_BACK_VAPS_IN_GROUP 3   /* Number of VAPs in 1 VAP group */
#define BPL_MAC_ADDR_OCTETS_LEN 6  /* Number of octets in mac address */
#define BPL_IPV4_ADDR_OCTETS_LEN 4 /* Number of octets in ipv4 address */
#define BPL_BACKHAUL_BAND_LEN 7    /* Max length of backhaul preferred band: 2.4GHz/5GHz/auto */

/* Radio Direction */
#define BPL_RADIO_BAND_AUTO 0
#define BPL_RADIO_FRONT 0
#define BPL_RADIO_BACK 1

/* Radio Band */
#define BPL_RADIO_BAND_5G 5
#define BPL_RADIO_BAND_2G 2

/* Platform Operating Mode */
#define BPL_OPER_MODE_GATEWAY 0
#define BPL_OPER_MODE_GATEWAY_WISP 1
#define BPL_OPER_MODE_WDS_EXTENDER 2
#define BPL_OPER_MODE_WDS_REPEATER 3
#define BPL_OPER_MODE_L2NAT_CLIENT 4

/* Platform Information */
#define BPL_USER_PASS_LEN (64 + 1)     /* Maximal length of USER_PASS */
#define BPL_SERIAL_NUMBER_LEN (64 + 1) /* Maximal length of SERIAL_NUMBER */

/* Platform notification types */
#define BPL_NOTIF_VER_MISMATCH 1 /* Version mismatch detected */
#define BPL_NOTIF_ERROR 2        /* Error notification */
#define BPL_NOTIF_WPS_START 3    /* WPS button pressed */
#define BPL_NOTIF_MNS_OPER 4     /* MNS operation request */
#define BPL_NOTIF_WPS_COMPLETE 5 /* Set WPS Status */

/* Platform messages types */
#define BPL_MSG_SET_WIFI_CRED 10             /* Set Wi-Fi Credentials */
#define BPL_MSG_SET_WIFI_IFACE_STATE 11      /* Set Wi-Fi Interface state (ON/OFF/RESET) */
#define BPL_MSG_SET_WIFI_RADIO_TX_STATE 12   /* Set Wi-Fi Radio TX state (ON/OFF) */
#define BPL_MSG_SET_WIFI_INTERFACE_STATUS 13 /* Set the Radio status */
#define BPL_MSG_SET_WIFI_ADVERTISE_SSID 14   /* Set Wi-Fi SSID Advertisement */

/* Wi-Fi Security Mode Strings */
#define BPL_WLAN_SEC_NONE_STR "None"
#define BPL_WLAN_SEC_WEP64_STR "WEP-64"
#define BPL_WLAN_SEC_WEP128_STR "WEP-128"
#define BPL_WLAN_SEC_WPA_PSK_STR "WPA-Personal"
#define BPL_WLAN_SEC_WPA2_PSK_STR "WPA2-Personal"
#define BPL_WLAN_SEC_WPA_WPA2_PSK_STR "WPA-WPA2-Personal"

/* Gateway database */
#define BPL_GW_DB_MANAGE_MODE_LEN (127 + 1) /* Maximal length of MANAGEMENT MODE string */
#define BPL_GW_DB_OPER_MODE_LEN (127 + 1)   /* Maximal length of OPERATING MODE string */

/****************************************************************************/
/******************************* Structures *********************************/
/****************************************************************************/

/* Error Notification */
struct BPL_ERROR {

    /* Error code of type BPL_ERR_... */
    int code;

    /* Custom string data reported by the module */
    char data[BPL_ERROR_STRING_LEN];
};

/* Generic device information */
struct BPL_DEVICE_INFO {

    /* Device manufacturer name (e.g. Intel Corporation) */
    char manufacturer[BPL_DEV_INFO_LEN];

    /* Device model name */
    char model_name[BPL_DEV_INFO_LEN];

    /* Device serial number */
    char serial_number[BPL_DEV_INFO_LEN];

    /* LAN interface name */
    char lan_iface_name[BPL_IFNAME_LEN];

    /* LAN interface IP address */
    uint32_t lan_ip_address;

    /* LAN interface network mask */
    uint32_t lan_network_mask;

    /* WAN interface name */
    char wan_iface_name[BPL_IFNAME_LEN];

    /* WAN interface IP address */
    uint32_t wan_ip_address;

    /* WAN interface network mask */
    uint32_t wan_network_mask;
};

/* Wi-Fi Credentials */
struct BPL_WIFI_CREDENTIALS {

    /* Wi-Fi SSID */
    char ssid[BPL_SSID_LEN];

    /* Wi-Fi KeyPassphrase */
    char pass[BPL_PASS_LEN];

    /* Wi-Fi Security Mode */
    char sec[BPL_SEC_LEN];
};

/* Wi-Fi Interface state  */
struct BPL_WIFI_IFACE_STATE {

    /* Wi-Fi interface name */
    char ifname[BPL_IFNAME_LEN];

    /* Wi-Fi interface operation */
    int op;
};

/* Wi-Fi Radio enable state  */
struct BPL_WIFI_RADIO_TX_STATE {

    /* Wi-Fi interface name */
    char ifname[BPL_IFNAME_LEN];

    /* Wi-Fi Radio operation */
    int enable;
};

/* Wi-Fi Access point SSID advertisement */
struct BPL_WIFI_ADVERTISE_SSID_FLAG {

    /* Wi-Fi interface name */
    char ifname[BPL_IFNAME_LEN];

    /* Wi-Fi SSID advertisement flag */
    int advertise_ssid;
};

/* Interface state for the platform*/
typedef struct {

    /* interface's name */
    char ifname[BPL_NUM_OF_INTERFACES][BPL_IFNAME_LEN];

    /* status from the BPL_WIFI_INTERFACE_STATUS enum*/
    int status[BPL_NUM_OF_INTERFACES];

    /* is platform on operational mode (true/false) */
    int operational_status;

} BPL_INTERFACE_STATUS_NOTIFICATION;

enum BPL_INTERFACE_STATUS {
    INVALID = 0,
    OFF,
    AP_OK,
    AP_DFS_CAC,
    BH_SIGNAL_OK,
    BH_SIGNAL_TOO_LOW,
    BH_SIGNAL_TOO_HIGH,
    BH_WIRED,
    BH_SCAN
};

/* WPS Trigger params */
struct BPL_WPS_PARAMS {

    /* Wi-Fi interface name */
    char ifname[BPL_IFNAME_LEN];

    /* wps type (0-pbc, 1-pin) */
    int wps_type;
};

enum BPL_WPS_TYPE { BPL_WPS_PBC = 0, BPL_WPS_PIN };

/* MNS operation params */
struct BPL_MNS_PARAMS {
    /* operation of type BPL_MNS_OP_... */
    int op;

    /* data */
    char data[BPL_MNS_DATA_LEN];
};

enum BPL_MNS_OP {
    BPL_MNS_OP_RESET = 0,
};

/* Interface state for the platform*/
struct BPL_NOTIF_WPS_COMPLETE_NOTIFICATION {

    /* Wi-Fi SSID */
    char ssid[BPL_SSID_LEN];

    /* Wi-Fi KeyPassphrase */
    char pass[BPL_PASS_LEN];

    /* Wi-Fi Security Mode */
    char sec[BPL_SEC_LEN];

    /* Wi-Fi interface name */
    char ifname[BPL_IFNAME_LEN];

    /* WPS Status success=0/fail=1*/
    int status;
};

/* WLAN params */
struct BPL_WLAN_PARAMS {

    /* Wi-Fi AP enable */
    int enabled;

    /* Wi-Fi Channel (0 for ACS) */
    int channel;

    /* Wi-Fi SSID advertising */
    int advertise_ssid;

    /* Wi-Fi SSID */
    char ssid[BPL_SSID_LEN];

    /* Wi-Fi KeyPassphrase */
    char passphrase[BPL_PASS_LEN];

    /* Wi-Fi Securirt Mode */
    char security[BPL_SEC_LEN];
};

/****************************************************************************/
/******************************** Functions *********************************/
/****************************************************************************/

/**
 * Returns the beerocks state.
 *
 * @return 1 if enabled.
 * @return 0 if disabled.
 * @return -1 Error.
 */
int bpl_cfg_is_enabled();

/**
 * Returns whether the current platform is configured as Master.
 *
 * @return 1 Master.
 * @return 0 IRE.
 * @return -1 Error.
 */
int bpl_cfg_is_master();

/**
 * Returns whether the current platform is configured as Gateway.
 *
 * @return valid possibilities:
 *   BPL_OPER_MODE_GATEWAY,
 *   BPL_OPER_MODE_GATEWAY_WISP,
 *   BPL_OPER_MODE_WDS_EXTENDER,
 *   BPL_OPER_MODE_WDS_REPEATER,
 *   BPL_OPER_MODE_L2NAT_CLIENT
 * @return -1 Error.
 */
int bpl_cfg_get_operating_mode();

/**
 * Returns whether the platform is in onboarding state.
 *
 * @return 1 Platform is in onboarding state.
 * @return 0 Platform is NOT in onboarding state.
 * @return -1 Error.
 */
int bpl_cfg_is_onboarding();

/**
 * Returns the state of the wired_backhaul.
 *
 * @return 1 wired_backhaul is enabled.
 * @return 0 wired_backhaul is enabled..
 * @return -1 Error.
 */
int bpl_cfg_is_wired_backhaul();

/**
 * Checks the state of the RDKB Extensions feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int bpl_cfg_get_rdkb_extensions();

/**
 * Checks the state of the Band Steering feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int bpl_cfg_get_band_steering();

/**
 * Checks the state of the DFS Reentry feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int bpl_cfg_get_dfs_reentry();

/**
 * Checks the state of the Passive Mode feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int bpl_cfg_get_passive_mode();

/**
 * Checks the state of the Client Roaming feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int bpl_cfg_get_client_roaming();

/**
 * Returns generic device information.
 * 
 * @param [out] Device information structure.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_get_device_info(struct BPL_DEVICE_INFO *device_info);

/**
 * Returns miscellaneous Wi-Fi parameters. 
 * 
 * @param [in] iface Interface name for the requested parameters.
 * @param [out] WLAN parameters structure.
 *
 * NOTE: NULL output parameters should not be filled.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_get_wifi_params(const char *iface, struct BPL_WLAN_PARAMS *wlan_params);

/**
 * Returns backhaul vaps configuration. 
 * 
 * @param [out] max_vaps an int.
 * @param [out] network_enabled 1 if network is enabled or 0 otherwise.
 * @param [out] preferred_radio_band BPL_RADIO_BAND_5G or BPL_RADIO_BAND_2G or BPL_RADIO_BAND_AUTO
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_get_backhaul_params(int *max_vaps, int *network_enabled, int *preferred_radio_band);

/**
 * Returns backhaul vaps list. 
 * 
 * @param [out] backhaul_vaps_buf buffer for backhaul vaps list.
 * @param [in]  buf_len buffer length.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len);

/**
 * Update the SSID advertisement flag
 *
 * @param [in] iface Interface name for the requested parameters.
 * @param [in] advertise_ssid SSID Advertisement flag
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_set_wifi_advertise_ssid(const char *iface, int advertise_ssid);

/**
 * Returns the platform Wi-Fi settings.
 *
 * @param [in] radio_dir radio direction (BPL_RADIO_FRONT/BPL_RADIO_BACK)
 * @param [out] ssid SSID (up to 32 bytes in length).
 * @param [out] pass Password (up to 64 bytes in length).
 * @param [out] sec Security Mode (up to 32 bytes in length).
 *
 * NOTE: NULL output parameters should not be filled.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_get_beerocks_credentials(const int radio_dir, char ssid[BPL_SSID_LEN],
                                     char pass[BPL_PASS_LEN], char sec[BPL_SEC_LEN]);

/**
 * Update the platform Wi-Fi settings.
 *
 * @param [in] radio_dir radio direction (BPL_RADIO_FRONT/BPL_RADIO_BACK)
 * @param [in] ssid SSID (up to 32 bytes in length).
 * @param [in] pass Password (up to 64 bytes in length).
 * @param [in] sec Secutiry Mode (up to 32 bytes in length).
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_set_beerocks_credentials(const int radio_dir, const char ssid[BPL_SSID_LEN],
                                     const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN]);

/**
 * @brief Returns the platform SDL policy which is represented by 'mem_only_psk' flag.
 * 'mem_only_psk' flag sets whether the platform shall save the credentials in some encrypted DB so 
 * the wpa-supplicant will get from there in runtime, without supply it on `connect` API, or not.
 *
 * @return mem_only_psk flag on success.
 * @return -1 Error.
 */
int bpl_cfg_get_security_policy();

/**
 * Update the Wi-Fi settings.
 *
 * @param [in] radio_dir radio direction (BPL_RADIO_FRONT/BPL_RADIO_BACK)
 * @param [in] ssid SSID (up to 32 bytes in length).
 * @param [in] pass Password (up to 64 bytes in length).
 * @param [in] sec Security Mode (up to 32 bytes in length).
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_set_wifi_credentials(const char iface[BPL_IFNAME_LEN], const char ssid[BPL_SSID_LEN],
                                 const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN]);

/**
 * Set platform onboarding state.
 *
 * @param [in] enable, 0= disable, 1 = enable.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_set_onboarding(int enable);

/**
 * Notify the platform on onboarding completed.
 *
 * @param [in] ssid SSID (up to 32 bytes in length).
 * @param [in] pass Password (up to 64 bytes in length).
 * @param [in] sec Security Mode (up to 32 bytes in length).
 * @param [in] iface_name Interface name (up to 32 bytes in length).
 * @param [in] success Success of onboarding (0 - failure, 1 - success).
 */
int bpl_cfg_notify_onboarding_completed(const char ssid[BPL_SSID_LEN],
                                        const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN],
                                        const char iface_name[BPL_IFNAME_LEN], const int success);

/**
 * Set Wi-Fi interface (wpa_supplicant or hostapd) up or down.
 *
 * @param [in] iface, the interface name.
 * @param [in] op, 0= disable, 1 = enable 
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_set_wifi_iface_state(const char iface[BPL_IFNAME_LEN], int op);

/**
 * Set Wi-Fi radio tx (wpa_supplicant or hostapd) enable or disable.
 *
 * @param [in] iface, the interface name.
 * @param [in] enable, 0= disable, 1 = enable 
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_set_wifi_radio_tx_state(const char iface[BPL_IFNAME_LEN], int enable);

/**
 * Notify the platform about a FW version mismatch.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_notify_fw_version_mismatch();

/**
 * Notify the platform about an error.
 * 
 * @param [in] error_code Error code (of type BPL_ERR...)
 * @param [in] error_str "printf" style formatted string.
 *
 * @return 0 Success.
 * @return -1 Error. 
 */
int bpl_cfg_notify_error(int code, const char str[BPL_ERROR_STRING_LEN]);

/**
 * Notify the platform about the interface status.
 *
 * @param [in] BPL_INTERFACE_STATUS_NOTIFICATION (status for intrfaces)
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif);

/**
 * Returns the platform administrator password.
 *
 * @param [out] pass Password (up to 64 bytes in length).
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int bpl_cfg_get_administrator_credentials(char pass[BPL_USER_PASS_LEN]);

/**
 * @brief Return 1 BPL supports monitoring (ARP and DHCP)
 * 
 * @return 1 if monitoring supported, 0 otherwise
 */
int bpl_cfg_monitoring_supported(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BPL_CFG_H_ */
