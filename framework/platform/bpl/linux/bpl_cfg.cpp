/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../common/utils/utils.h"
#include "../common/utils/utils_net.h"
#include "mapf/common/logger.h"
#include <bpl/bpl_cfg.h>

using namespace mapf;

#ifndef PLATFORM_DB_PATH
#error "PLATFORM_DB_PATH not defined!"
#endif

#ifndef PLATFORM_DB_PATH_TEMP
#define PLATFORM_DB_PATH_TEMP "/tmp/prplmesh_platform_db"
#endif

#define RETURN_OK 0
#define RETURN_ERR -1

namespace beerocks {
namespace bpl {

int cfg_get_param(const std::string &param, std::string &value)
{
    std::ifstream in_conf_file;
    std::string line;
    in_conf_file.open(PLATFORM_DB_PATH_TEMP);
    if (!in_conf_file.is_open()) {
        in_conf_file.open(PLATFORM_DB_PATH);
        if (!in_conf_file.is_open()) {
            MAPF_ERR("Failed oppening file " << PLATFORM_DB_PATH);
            return RETURN_ERR;
        }
    }

    while (std::getline(in_conf_file, line)) {
        utils::trim(line);
        if (line.empty())
            continue; // Empty line
        if (line.at(0) == '#')
            continue; // Commented line
        if (line.compare(0, param.size(), param) != 0)
            continue; // Not the param we look for

        std::string line_arg = line.substr(param.size(), line.size());
        auto pos             = line_arg.find("#");
        if (pos != std::string::npos) {
            line_arg.erase(pos, line_arg.size());
            utils::rtrim(line_arg);
        }
        if (line_arg.size() >= 1) {
            value.assign(line_arg);
            return RETURN_OK;
        }
        break;
    }

    return RETURN_ERR;
}

int cfg_get_param_int(const std::string &param, int &value)
{
    std::string str_value;

    if (cfg_get_param(param, str_value) < 0) {
        MAPF_ERR("Failed reading param " << param);
        return RETURN_ERR;
    }

    value = utils::stoi(str_value);

    return RETURN_OK;
}

int cfg_is_enabled() { return 1; }

int cfg_is_master()
{
    switch (cfg_get_management_mode()) {
    case BPL_MGMT_MODE_MULTIAP_CONTROLLER_AGENT:
        return 1;
    case BPL_MGMT_MODE_MULTIAP_CONTROLLER:
        return 1;
    case BPL_MGMT_MODE_MULTIAP_AGENT:
        return 0;
    case BPL_MGMT_MODE_NOT_MULTIAP:
        return (cfg_get_operating_mode() == BPL_OPER_MODE_GATEWAY) ? 1 : 0;
    default:
        return RETURN_ERR;
    }
}

int cfg_get_management_mode()
{
    int retVal = RETURN_ERR;
    std::string mgmt_mode;
    if (cfg_get_param("management_mode=", mgmt_mode) < 0) {
        MAPF_ERR("cfg_get_management_mode: Failed to read management_mode");
        return -1;
    }

    if (mgmt_mode == "Multi-AP-Controller-and-Agent") {
        return BPL_MGMT_MODE_MULTIAP_CONTROLLER_AGENT;
    } else if (mgmt_mode == "Multi-AP-Controller") {
        return BPL_MGMT_MODE_MULTIAP_CONTROLLER;
    } else if (mgmt_mode == "Multi-AP-Agent") {
        return BPL_MGMT_MODE_MULTIAP_AGENT;
    } else if (mgmt_mode == "Not-Multi-AP") {
        return BPL_MGMT_MODE_NOT_MULTIAP;
    }

    MAPF_ERR("cfg_get_management_mode: Unexpected management_mode");
    return retVal;
}

int cfg_get_operating_mode()
{
    int retVal = 0;
    std::string op_mode;
    if (cfg_get_param("operating_mode=", op_mode) < 0) {
        MAPF_ERR("cfg_get_operating_mode: Failed to read operating_mode");
        return -1;
    }

    if (op_mode == "Gateway") {
        return BPL_OPER_MODE_GATEWAY;
    } else if (op_mode == "Gateway-WISP") {
        return BPL_OPER_MODE_GATEWAY_WISP;
    } else if (op_mode == "WDS-Extender") {
        return BPL_OPER_MODE_WDS_EXTENDER;
    } else if (op_mode == "WDS-Repeater") {
        return BPL_OPER_MODE_WDS_REPEATER;
    } else if (op_mode == "L2NAT-Client") {
        return BPL_OPER_MODE_L2NAT_CLIENT;
    }

    MAPF_ERR("cfg_get_operating_mode: Unexpected operating_mode");
    return retVal;
}

int cfg_get_certification_mode()
{
    int retVal = 0;
    std::string certification_mode;
    if (cfg_get_param("certification_mode=", certification_mode) < 0) {
        MAPF_ERR("cfg_get_certification_mode: Failed to read certification_mode");
        retVal = RETURN_ERR;
    } else if (certification_mode == "0") {
        retVal = BPL_CERTIFICATION_MODE_OFF;
    } else {
        // if "0" then disabled, anything else for enabled
        retVal = BPL_CERTIFICATION_MODE_ON;
    }

    return retVal;
}

int cfg_get_stop_on_failure_attempts()
{
    int retVal = -1;
    if (cfg_get_param_int("stop_on_failure_attempts", retVal) == RETURN_ERR) {
        retVal = RETURN_ERR;
    }
    return retVal;
}

int cfg_is_onboarding() { return 0; }

int cfg_get_rdkb_extensions() { return 0; }

int cfg_get_band_steering() { return 1; }

int cfg_get_dfs_reentry() { return 0; }

int cfg_get_client_roaming() { return 1; }

int cfg_get_device_info(BPL_DEVICE_INFO *device_info) { return RETURN_ERR; }

int cfg_get_wifi_params(const char *iface, struct BPL_WLAN_PARAMS *wlan_params)
{
    if (!iface || !wlan_params) {
        return RETURN_ERR;
    }
    wlan_params->enabled = 1;
    wlan_params->channel = 0;
    utils::copy_string(wlan_params->ssid, "test_ssid", BPL_SSID_LEN);
    utils::copy_string(wlan_params->passphrase, "test_pass", BPL_PASS_LEN);
    utils::copy_string(wlan_params->security, "None", BPL_SEC_LEN);

    return RETURN_OK;
}

int cfg_get_backhaul_params(int *max_vaps, int *network_enabled, int *preferred_radio_band)
{
    *max_vaps             = 0;
    *network_enabled      = 0;
    *preferred_radio_band = 0;
    return RETURN_OK;
}

int cfg_get_backhaul_vaps(char *backhaul_vaps_buf, const int buf_len)
{
    memset(backhaul_vaps_buf, 0, buf_len);
    return RETURN_OK;
}

int cfg_get_beerocks_credentials(const int radio_dir, char ssid[BPL_SSID_LEN],
                                 char pass[BPL_PASS_LEN], char sec[BPL_SEC_LEN])
{
    utils::copy_string(ssid, "test_beerocks_ssid", BPL_SSID_LEN);
    utils::copy_string(sec, "None", BPL_SEC_LEN);
    return RETURN_OK;
}

int cfg_get_security_policy() { return 0; }

int cfg_set_onboarding(int enable) { return RETURN_ERR; }

int cfg_notify_onboarding_completed(const char ssid[BPL_SSID_LEN], const char pass[BPL_PASS_LEN],
                                    const char sec[BPL_SEC_LEN],
                                    const char iface_name[BPL_IFNAME_LEN], const int success)
{
    return RETURN_ERR;
}

int cfg_notify_fw_version_mismatch() { return RETURN_ERR; }

int cfg_notify_error(int code, const char data[BPL_ERROR_STRING_LEN]) { return RETURN_ERR; }

int cfg_notify_iface_status(const BPL_INTERFACE_STATUS_NOTIFICATION *status_notif)
{
    return RETURN_ERR;
}

int cfg_get_administrator_credentials(char pass[BPL_PASS_LEN]) { return RETURN_ERR; }

int cfg_get_sta_iface(const char iface[BPL_IFNAME_LEN], char sta_iface[BPL_IFNAME_LEN])
{
    if (iface == NULL || sta_iface == NULL) {
        MAPF_ERR("cfg_get_sta_iface: invalid input: iface or sta_iface are NULL");
        return RETURN_ERR;
    }

    // return empty STA interface name
    sta_iface[0] = '\0';
    return RETURN_OK;
}

} // namespace bpl
} // namespace beerocks
