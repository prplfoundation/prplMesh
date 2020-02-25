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

namespace beerocks {
namespace bpl {

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

/* Platform Management Mode */
#define BPL_MGMT_MODE_MULTIAP_CONTROLLER_AGENT 0 /* EasyMesh controller and agent */
#define BPL_MGMT_MODE_MULTIAP_CONTROLLER 1       /* EasyMesh controller */
#define BPL_MGMT_MODE_MULTIAP_AGENT 2            /* EasyMesh agent */
#define BPL_MGMT_MODE_NOT_MULTIAP 3              /* Non EasyMesh */

/* Platform Certification Mode */
#define BPL_CERTIFICATION_MODE_OFF 0
#define BPL_CERTIFICATION_MODE_ON 1

/* Platform Information */
#define BPL_USER_PASS_LEN (64 + 1)     /* Maximal length of USER_PASS */
#define BPL_SERIAL_NUMBER_LEN (64 + 1) /* Maximal length of SERIAL_NUMBER */

/* Platform notification types */
#define BPL_NOTIF_VER_MISMATCH 1 /* Version mismatch detected */
#define BPL_NOTIF_ERROR 2        /* Error notification */
#define BPL_NOTIF_WPS_START 3    /* WPS button pressed */
#define BPL_NOTIF_MNS_OPER 4     /* MNS operation request */
#define BPL_NOTIF_WPS_COMPLETE 5 /* Set WPS Status */

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

/* Default values */
#define DEFAULT_STOP_ON_FAILURE_ATTEMPTS 1
#define DEFAULT_RDKB_EXTENSIONS 0
#define DEFAULT_BAND_STEERING 1
#define DEFAULT_DFS_REENTRY 1
#define DEFAULT_CLIENT_ROAMING 1
#define DEFAULT_NO_VENDOR_SPECIFIC 0

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
};

/**
 * a structure to couple together radio-number (also correlates to slave-number) and interface name
 */
struct BPL_WLAN_IFACE {
    int radio_num;
    char ifname[BPL_IFNAME_LEN];
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
int cfg_is_enabled();

/**
 * Returns whether the current platform is configured as Master.
 *
 * @return 1 Master.
 * @return 0 IRE.
 * @return -1 Error.
 */
int cfg_is_master();

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
int cfg_get_operating_mode();

/**
 * Returns the current management mode configuration.
 *
 * @return valid possibilities:
 *   BPL_MGMT_MODE_MULTIAP_CONTROLLER_AGENT,
 *   BPL_MGMT_MODE_MULTIAP_CONTROLLER,
 *   BPL_MGMT_MODE_MULTIAP_AGENT,
 *   BPL_MGMT_MODE_NOT_MULTIAP
 * @return -1 Error.
 */
int cfg_get_management_mode();

/**
 * Returns certification mode value.
 *
 * @return BPL_CERTIFICATION_MODE_ON Certification mode is ON.
 * @return BPL_CERTIFICATION_MODE_OFF Certification mode is OFF.
 * @return -1 Error.
 */
int cfg_get_certification_mode();

/**
 * Returns the maximum number of failures allowed on agent before stopping its execution.
 *
 * @return Maximum number of failures allowed or 0 to retry indefinitely.
 * @return -1 Error.
 */
int cfg_get_stop_on_failure_attempts();

/**
 * Returns whether the vendor specific messages are turned off
 * @return 1 Messages are enabled
 * @return 0 Messages are disabled
 * @return -1 Error
 */
int cfg_get_no_vendor_specific();

/**
 * Returns whether the platform is in onboarding state.
 *
 * @return 1 Platform is in onboarding state.
 * @return 0 Platform is NOT in onboarding state.
 * @return -1 Error.
 */
int cfg_is_onboarding();

/**
 * Checks the state of the RDKB Extensions feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int cfg_get_rdkb_extensions();

/**
 * Checks the state of the Band Steering feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int cfg_get_band_steering();

/**
 * Checks the state of the DFS Reentry feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int cfg_get_dfs_reentry();

/**
 * Checks the state of the Client Roaming feature.
 *
 * @return 1 Enabled.
 * @return 0 Disabled.
 * @return -1 Error.
 */
int cfg_get_client_roaming();

/**
 * Returns generic device information.
 * 
 * @param [out] Device information structure.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int cfg_get_device_info(struct BPL_DEVICE_INFO *device_info);

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
int cfg_get_wifi_params(const char *iface, struct BPL_WLAN_PARAMS *wlan_params);

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
int cfg_get_backhaul_params(int *max_vaps, int *network_enabled, int *preferred_radio_band);

/**
 * Returns backhaul vaps list. 
 * 
 * @param [out] backhaul_vaps_buf buffer for backhaul vaps list.
 * @param [in]  buf_len buffer length.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int cfg_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len);

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
int cfg_get_beerocks_credentials(const int radio_dir, char ssid[BPL_SSID_LEN],
                                 char pass[BPL_PASS_LEN], char sec[BPL_SEC_LEN]);

/**
 * @brief Returns the platform SDL policy which is represented by 'mem_only_psk' flag.
 * 'mem_only_psk' flag sets whether the platform shall save the credentials in some encrypted DB so 
 * the wpa-supplicant will get from there in runtime, without supply it on `connect` API, or not.
 *
 * @return mem_only_psk flag on success.
 * @return -1 Error.
 */
int cfg_get_security_policy();

/**
 * Set platform onboarding state.
 *
 * @param [in] enable, 0= disable, 1 = enable.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int cfg_set_onboarding(int enable);

/**
 * Notify the platform on onboarding completed.
 *
 * @param [in] ssid SSID (up to 32 bytes in length).
 * @param [in] pass Password (up to 64 bytes in length).
 * @param [in] sec Security Mode (up to 32 bytes in length).
 * @param [in] iface_name Interface name (up to 32 bytes in length).
 * @param [in] success Success of onboarding (0 - failure, 1 - success).
 */
int cfg_notify_onboarding_completed(const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN],
                                    const char sec[BPL_SEC_LEN],
                                    const char iface_name[BPL_IFNAME_LEN], const int success);

/**
 * Notify the platform about a FW version mismatch.
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int cfg_notify_fw_version_mismatch();

/**
 * Notify the platform about an error.
 * 
 * @param [in] error_code Error code (of type BPL_ERR...)
 * @param [in] error_str "printf" style formatted string.
 *
 * @return 0 Success.
 * @return -1 Error. 
 */
int cfg_notify_error(int code, const char str[BPL_ERROR_STRING_LEN]);

/**
 * Returns the platform administrator password.
 *
 * @param [out] pass Password (up to 64 bytes in length).
 *
 * @return 0 Success.
 * @return -1 Error.
 */
int cfg_get_administrator_credentials(char pass[BPL_USER_PASS_LEN]);

/**
 * Returns the STA interface for the specified radio id.
 *
 * @param [in] iface Interface name for the requested parameters.
 * @param [out] sta_iface name of STA interface (up to 32 bytes in length).
 *
 * @return 0 Success.
 * @return -1 Error, or no sta_iface is configured.
 */
int cfg_get_sta_iface(const char iface[BPL_IFNAME_LEN], char sta_iface[BPL_IFNAME_LEN]);

/**
 * Returns the HOSTAP interface for the specified radio id.
 *
 * @param [in] radio_num radio number in prplmesh UCI for the requested parameters.
 * @param [out] hostap_iface name of HOSTAP interface (up to 32 bytes in length).
 *
 * @return 0 Success.
 * @return -1 Error, or no hostap_iface is configured.
 */
int cfg_get_hostap_iface(int32_t radio_num, char hostap_iface[BPL_IFNAME_LEN]);

/**
 * Returns all the HOSTAP interfaces available in prplmesh config file
 *
 * @param [out] interfaces list of HOSTAP interfaces of type BPL_WLAN_IFACE.
 * @param [int/out] num_of_interfaces in:max num of interfaces, out:actual num of interfaces.
 *
 * @return 0 Success.
 * @return -1 Error, or no hostap_iface is configured.
 */
int cfg_get_all_prplmesh_wifi_interfaces(BPL_WLAN_IFACE *interfaces, int *num_of_interfaces);

} // namespace bpl
} // namespace beerocks

#endif /* _BPL_CFG_H_ */
