/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../../common/utils/utils.h"
#include "../../common/utils/utils_net.h"
#include <bpl/bpl_cfg.h>

#include "cal/cal_settings.h"
#include "cal/msg_api.h"
using namespace beerocks::bpl;

#include <cstring>

#include <mapf/common/logger.h>

const char *s_error_strings[] = {FOREACH_ERROR_CODE(GENERATE_ERROR_STRING)};

int bpl_cfg_is_enabled()
{
    cal_settings cal;
    return (cal.beerocks_is_enabled());
}

int bpl_cfg_is_master()
{
    cal_settings cal;
    return (cal.beerocks_is_master());
}

int bpl_cfg_get_operating_mode()
{
    cal_settings cal;
    return (cal.beerocks_get_operating_mode());
}

int bpl_cfg_is_onboarding()
{
    cal_settings cal;
    return (cal.beerocks_is_onboarding());
}

int bpl_cfg_is_wired_backhaul()
{
    cal_settings cal;
    return (cal.beerocks_is_wired_backhaul());
}

int bpl_cfg_get_rdkb_extensions() { return 0; }

int bpl_cfg_get_band_steering()
{
    cal_settings cal;
    return (cal.beerocks_get_band_steering());
}

int bpl_cfg_get_dfs_reentry() { return 1; }

int bpl_cfg_get_passive_mode() { return 0; }

int bpl_cfg_get_client_roaming()
{
    cal_settings cal;
    return (cal.beerocks_get_client_roaming());
}

int bpl_cfg_get_device_info(BPL_DEVICE_INFO *device_info)
{
    cal_settings cal;
    cal_settings::SDeviceInfo cal_device_info;

    if (!device_info || cal.beerocks_get_device_info(cal_device_info) < 0) {
        if (device_info) {
            memset(device_info, 0, sizeof(BPL_DEVICE_INFO));
        }

        return -1;
    }

    // Copy the value
    utils::copy_string(device_info->manufacturer, cal_device_info.manufacturer.c_str(),
                       BPL_DEV_INFO_LEN);
    utils::copy_string(device_info->model_name, cal_device_info.model_name.c_str(),
                       BPL_DEV_INFO_LEN);
    utils::copy_string(device_info->serial_number, cal_device_info.serial_number.c_str(),
                       BPL_DEV_INFO_LEN);

    // LAN
    if (!cal_device_info.lan_interface_name.empty()) {
        utils::copy_string(device_info->lan_iface_name, cal_device_info.lan_interface_name.c_str(),
                           BPL_IFNAME_LEN);
        device_info->lan_ip_address =
            utils::uint_ipv4_from_string(cal_device_info.lan_interface_ip);
        device_info->lan_network_mask =
            utils::uint_ipv4_from_string(cal_device_info.lan_interface_netmask);
    }

    // WAN
    if (!cal_device_info.wan_interface_name.empty()) {
        utils::copy_string(device_info->wan_iface_name, cal_device_info.wan_interface_name.c_str(),
                           BPL_IFNAME_LEN);
        device_info->wan_ip_address =
            utils::uint_ipv4_from_string(cal_device_info.wan_interface_ip);
        device_info->wan_network_mask =
            utils::uint_ipv4_from_string(cal_device_info.wan_interface_netmask);
    }

    return 0;
}

int bpl_cfg_get_wifi_params(const char *iface, struct BPL_WLAN_PARAMS *wlan_params)
{
    cal_settings cal;
    cal_settings::SAPSettings ap_settings;

    if (cal.beerocks_get_wifi_params(iface, ap_settings) < 0) {
        return -1;
    }

    wlan_params->enabled = ap_settings.enabled ? 1 : 0;
    wlan_params->acs     = ap_settings.acs ? 1 : 0;
    utils::copy_string(wlan_params->ssid, ap_settings.ssid.c_str(), BPL_SSID_LEN);
    utils::copy_string(wlan_params->passphrase, ap_settings.pass.c_str(), BPL_PASS_LEN);
    utils::copy_string(wlan_params->security, ap_settings.sec_mode.c_str(), BPL_SEC_LEN);

    return 0;
}

int bpl_cfg_get_backhaul_params(int *max_vaps, int *network_enabled, int *prefered_radio_band)
{
    cal_settings cal;
    if (cal.beerocks_get_backhaul_params(*max_vaps, *network_enabled, *prefered_radio_band) < 0) {
        return -1;
    }
    return 0;
}
int bpl_cfg_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len)
{
    cal_settings cal;
    if (cal.beerocks_get_backhaul_vaps(backhaul_vaps_buf, buf_len) < 0) {
        return -1;
    }
    return 0;
}

int bpl_cfg_get_beerocks_credentials(const int radio_dir, char ssid[BPL_SSID_LEN],
                                     char pass[BPL_PASS_LEN], char sec[BPL_SEC_LEN])
{
    cal_settings cal;
    std::string str_ssid, str_pass, str_sec;

    if (cal.beerocks_get_wifi_credentials(radio_dir, str_ssid, str_pass, str_sec) < 0)
        return -1;

    // TODO: Use constants for sizes
    if (ssid)
        utils::copy_string(ssid, str_ssid.c_str(), BPL_SSID_LEN);
    if (pass)
        utils::copy_string(pass, str_pass.c_str(), BPL_PASS_LEN);
    if (sec)
        utils::copy_string(sec, str_sec.c_str(), BPL_SEC_LEN);

    return 0;
}

int bpl_cfg_set_wifi_credentials(const char iface[BPL_IFNAME_LEN], const char ssid[BPL_SSID_LEN],
                                 const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN])
{
    cal_settings cal;

    std::string ssid_str(ssid);
    std::string pass_str(pass);
    std::string sec_str(sec);
    return (cal.beerocks_set_wifi_credentials(iface, ssid_str, pass_str, sec_str));
}

int bpl_cfg_set_beerocks_credentials(const int radio_dir, const char ssid[BPL_SSID_LEN],
                                     const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN])
{
    if (bpl_cfg_is_onboarding() > 0) {
        cal_settings cal;

        std::string ssid_str(ssid);
        std::string pass_str(pass);
        std::string sec_str(sec);
        return (cal.beerocks_set_beerocks_credentials(radio_dir, ssid_str, pass_str, sec_str));
    }

    return (sl_beerocks_set_wifi_credentials(radio_dir, ssid, pass, sec) ? 0 : -1);
}

int bpl_cfg_set_onboarding(int enable)
{
    cal_settings cal;
    return cal.beerocks_set_onboarding(enable);
}
int bpl_cfg_notify_onboarding_completed(const char ssid[BPL_SSID_LEN],
                                        const char pass[BPL_PASS_LEN], const char sec[BPL_SEC_LEN],
                                        const char iface_name[BPL_IFNAME_LEN], const int success)
{
    return (sl_beerocks_notify_onboarding_completed(ssid, pass, sec, iface_name, success) ? 0 : -1);
}

int bpl_cfg_notify_fw_version_mismatch() { return (sl_beerocks_notify_fw_mismatch() ? 0 : -1); }

int bpl_cfg_notify_error(int code, const char data[BPL_ERROR_STRING_LEN])
{
    if (code >= BPL_ERR_LAST)
        return -1;

    std::string full_data(s_error_strings[code]);
    full_data += (": " + std::string(data));

    return (sl_beerocks_notify_error(code, full_data.c_str()) ? 0 : -1);
}

int bpl_cfg_set_wifi_iface_state(const char iface[BPL_IFNAME_LEN], int op)
{
    return (sl_beerocks_set_wifi_iface_state(iface, op) ? 0 : -1);
}

int bpl_cfg_set_wifi_radio_tx_state(const char iface[BPL_IFNAME_LEN], int enable)
{
    return (sl_beerocks_set_wifi_radio_tx_state(iface, enable) ? 0 : -1);
}

int bpl_cfg_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif)
{
    return (sl_beerocks_notify_iface_status(status_notif) ? 0 : -1);
}

int bpl_cfg_get_administrator_credentials(char pass[BPL_PASS_LEN])
{
    cal_settings cal;
    std::string str_pass;

    if (cal.beerocks_get_administrator_credentials(str_pass) < 0)
        return -1;

    // TODO: Use constants for sizes
    if (pass)
        utils::copy_string(pass, str_pass.c_str(), BPL_USER_PASS_LEN);

    return 0;
}
