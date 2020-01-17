/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <string.h>

#include "../../common/utils/utils.h"
#include "../../common/utils/utils_net.h"
#include <bpl/bpl_cfg.h>

#include "bpl_cfg_helper.h"
#include "bpl_cfg_uci.h"

#include "mapf/common/logger.h"

using namespace mapf;

namespace beerocks {
namespace bpl {

int cfg_is_enabled()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("enable", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_is_enabled: Failed to read Enable parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_is_master()
{
    int retVal                               = 0;
    char man_mode[BPL_GW_DB_MANAGE_MODE_LEN] = {0};
    if (cfg_get_prplmesh_param("management_mode", man_mode, BPL_GW_DB_MANAGE_MODE_LEN) < 0) {
        MAPF_ERR("cfg_is_master: Failed to read ManagementMode\n");
        retVal = -1;
    } else {
        std::string mode_str(man_mode);
        if (mode_str == "Multi-AP-Controller-and-Agent") {
            retVal = 1;
        } else if (mode_str == "Proprietary-Mesh") {
            retVal = 1;
        } else if (mode_str == "Multi-AP-Agent") {
            retVal = 0;
        } else {
            MAPF_ERR("cfg_is_master: Unexpected ManagementMode\n");
            retVal = -1;
        }
    }
    return retVal;
}

int cfg_get_operating_mode()
{
    int retVal                            = 0;
    char op_mode[BPL_GW_DB_OPER_MODE_LEN] = {0};
    if (cfg_get_prplmesh_param("operating_mode", op_mode, BPL_GW_DB_OPER_MODE_LEN) < 0) {
        MAPF_ERR("cfg_get_operating_mode: Failed to read OperatingMode\n");
        retVal = -1;
    } else {
        std::string mode_str(op_mode);
        if (mode_str == "Gateway") {
            retVal = BPL_OPER_MODE_GATEWAY;
        } else if (mode_str == "Gateway-WISP") {
            retVal = BPL_OPER_MODE_GATEWAY_WISP;
        } else if (mode_str == "WDS-Extender") {
            retVal = BPL_OPER_MODE_WDS_EXTENDER;
        } else if (mode_str == "WDS-Repeater") {
            retVal = BPL_OPER_MODE_WDS_REPEATER;
        } else if (mode_str == "L2NAT-Client") {
            retVal = BPL_OPER_MODE_L2NAT_CLIENT;
        } else {
            MAPF_ERR("cfg_get_operating_mode: Unexpected OperatingMode\n");
            retVal = -1;
        }
    }
    return retVal;
}

int cfg_get_certification_mode()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("certification_mode", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_get_certification_mode: Failed to read certification_mode parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_get_stop_on_failure_attempts()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("stop_on_failure_attempts", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_get_stop_on_failure_attempts: Failed to read stop_on_failure_attempts "
                 "parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_is_onboarding()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("onboarding", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_is_onboarding: Failed to read Onboarding parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_get_rdkb_extensions()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("rdkb_extensions", &retVal) == RETURN_ERR) {
        printf("cfg_get_rdkb_extensions: Failed to read RDKB Extensions parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_get_band_steering()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("band_steering", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_get_band_steering: Failed to read BandSteering parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_get_dfs_reentry()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("dfs_reentry", &retVal) == RETURN_ERR) {
        printf("cfg_get_dfs_reentry: Failed to read DfsReentry parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_get_client_roaming()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("client_roaming", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_get_client_roaming: Failed to read ClientRoaming parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_get_device_info(BPL_DEVICE_INFO *device_info) { return 0; }

int cfg_get_wifi_params(const char iface[BPL_IFNAME_LEN], struct BPL_WLAN_PARAMS *wlan_params)
{
    int retVal = 0;
    int index  = 0;

    if (!iface || !wlan_params) {
        return RETURN_ERR;
    }

    retVal = cfg_get_index_from_interface(iface, &index);
    if (retVal) {
        return retVal;
    }

    // The UCI "disabled" setting is optional, defaults to false if not present
    bool disabled = false;
    cfg_uci_get_wireless_bool(TYPE_RADIO, index, "disabled", &disabled);
    wlan_params->enabled = !disabled;

    // The UCI "channel" setting is not documented as optional, but for Intel
    // wireless (as probably for other drivers) it is. We do not want to
    // fail when wifi still works fine, so default to "auto" (0) and if
    // can't get the channel from UCI just move on.
    wlan_params->channel = 0;
    if (!cfg_get_channel(index, &wlan_params->channel)) {
        MAPF_INFO("UCI: radio" << index << ": channel is not configured assuming auto\n");
    }

    char ssid[MAX_UCI_BUF_LEN] = {0}, security[MAX_UCI_BUF_LEN] = {0},
         passphrase[MAX_UCI_BUF_LEN] = {0};
    retVal |= cfg_uci_get_wireless(TYPE_VAP, index, "ssid", ssid);
    retVal |= cfg_uci_get_wireless(TYPE_VAP, index, "wav_security_mode", security);
    std::string mode = std::string(security);
    if (mode == BPL_WLAN_SEC_WEP64_STR || mode == BPL_WLAN_SEC_WEP128_STR) {
        retVal |= cfg_get_wep_key(index, -1, passphrase);
    } else if (mode != BPL_WLAN_SEC_NONE_STR) {
        retVal |= cfg_uci_get_wireless(TYPE_VAP, index, "key", passphrase);
    }

    utils::copy_string(wlan_params->ssid, ssid, BPL_SSID_LEN);
    utils::copy_string(wlan_params->security, security, BPL_SEC_LEN);
    utils::copy_string(wlan_params->passphrase, passphrase, BPL_PASS_LEN);

    return retVal;
}

int cfg_get_backhaul_params(int *max_vaps, int *network_enabled, int *preferred_radio_band)
{
    int retVal = 0;

    if (max_vaps) {
        //get max_vaps
    }

    if (network_enabled) {
        //get network_enabled
    }

    if (preferred_radio_band) {
        char backhaul_band[BPL_BACKHAUL_BAND_LEN] = {0};
        //get preferred_radio_band
        retVal = cfg_get_prplmesh_param("backhaul_band", backhaul_band, BPL_BACKHAUL_BAND_LEN);
        if (retVal == RETURN_ERR) {
            MAPF_ERR("cfg_get_backhaul_params: Failed to read backhaul_band parameter\n");
            return RETURN_ERR;
        }
        std::string preferred_bh_band(backhaul_band);
        if (preferred_bh_band.compare("2.4GHz") == 0) {
            *preferred_radio_band = BPL_RADIO_BAND_2G;
        } else if (preferred_bh_band.compare("5GHz") == 0) {
            *preferred_radio_band = BPL_RADIO_BAND_5G;
        } else if (preferred_bh_band.compare("auto") == 0) {
            *preferred_radio_band = BPL_RADIO_BAND_AUTO;
        } else {
            MAPF_ERR("cfg_get_backhaul_params: unknown backhaul_band parameter value\n");
            return RETURN_ERR;
        }
    }

    return RETURN_OK;
}

int cfg_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len) { return 0; }

int cfg_get_beerocks_credentials(const int radio_dir, char ssid[BPL_SSID_LEN],
                                 char pass[BPL_PASS_LEN], char sec[BPL_SEC_LEN])
{
    int retVal = 0;

    retVal |= cfg_get_prplmesh_param("ssid", ssid, BPL_SSID_LEN);
    retVal |= cfg_get_prplmesh_param("mode_enabled", sec, BPL_SEC_LEN);
    if (!strcmp(sec, "WEP-64") || !strcmp(sec, "WEP-128")) {
        retVal |= cfg_get_prplmesh_param("wep_key", pass, BPL_PASS_LEN);
    } else {
        retVal |= cfg_get_prplmesh_param("key_passphrase", pass, BPL_PASS_LEN);
    }

    return retVal;
}

int cfg_get_security_policy()
{
    int retVal = -1;
    if (cfg_get_prplmesh_param_int("mem_only_psk", &retVal) == RETURN_ERR) {
        MAPF_ERR("cfg_get_security_policy: Failed to read mem_only_psk parameter\n");
        return RETURN_ERR;
    }
    return retVal;
}

int cfg_notify_onboarding_completed(const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN],
                                    const char sec[BPL_SEC_LEN],
                                    const char iface_name[BPL_IFNAME_LEN], const int success)
{
    //return (sl_beerocks_notify_onboarding_completed(ssid, pass, sec, iface_name, success) ? 0 : -1);
    return 0;
}

int cfg_notify_fw_version_mismatch() { return 0; }

int cfg_notify_error(int code, const char data[BPL_ERROR_STRING_LEN]) { return 0; }

int cfg_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif) { return 0; }

int cfg_get_administrator_credentials(char pass[BPL_PASS_LEN]) { return 0; }

int cfg_get_sta_iface(const char iface[BPL_IFNAME_LEN], char sta_iface[BPL_IFNAME_LEN])
{
    if (iface == NULL || sta_iface == NULL) {
        MAPF_ERR("cfg_get_sta_iface: invalid input: iface or sta_iface are NULL");
        return RETURN_ERR;
    }

    int index = -1;
    if (cfg_get_index_from_interface(iface, &index) == RETURN_ERR) {
        MAPF_ERR("cfg_get_sta_iface: Failed to get radio index from iface\n");
        return RETURN_ERR;
    }

    return cfg_get_prplmesh_radio_param(index, "sta_iface", sta_iface, BPL_IFNAME_LEN);
}

} // namespace bpl
} // namespace beerocks
