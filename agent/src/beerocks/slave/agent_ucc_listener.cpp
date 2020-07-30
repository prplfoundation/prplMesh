/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "agent_ucc_listener.h"
#include "agent_db.h"

#include <bcl/network/network_utils.h>
#include <beerocks/tlvf/beerocks_message.h>

#include <easylogging++.h>

#include "backhaul_manager/backhaul_manager_thread.h"

using namespace beerocks;
using namespace net;

agent_ucc_listener::agent_ucc_listener(backhaul_manager &backhaul_manager_ctx, uint16_t port,
                                       const std::string &vendor, const std::string &model,
                                       const std::string &bridge_iface,
                                       ieee1905_1::CmduMessageTx &cmdu)
    : beerocks_ucc_listener(port, cmdu), m_backhaul_manager_ctx(backhaul_manager_ctx),
      m_vendor(vendor), m_model(model), m_bridge_iface(bridge_iface)
{
    m_ucc_listener_run_on = eUccListenerRunOn::AGENT;
}

bool agent_ucc_listener::init()
{
    network_utils::iface_info bridge_info;
    if (network_utils::get_iface_info(bridge_info, m_bridge_iface) < 0) {
        LOG(ERROR) << " failed getting iface info on bridge_mac '" << m_bridge_iface << "'";
        should_stop = true;
        return false;
    }

    m_bridge_mac = bridge_info.mac;

    return beerocks_ucc_listener::init();
}

/**
 * @brief Returns string filled with reply to "DEVICE_GET_INFO" command.
 *
 * @return const std::string Device info in UCC reply format.
 */
std::string agent_ucc_listener::fill_version_reply_string()
{
    return std::string("vendor,") + m_vendor + std::string(",model,") + m_model +
           std::string(",version,") + BEEROCKS_VERSION;
}

/**
 * @brief Clear configuration on Agent, and initiate onboarding sequence.
 *
 * @return None.
 */
void agent_ucc_listener::clear_configuration()
{
    m_in_reset        = true;
    m_reset_completed = false;

    auto timeout =
        std::chrono::steady_clock::now() + std::chrono::seconds(UCC_REPLY_COMPLETE_TIMEOUT_SEC);

    while (!m_reset_completed) {

        if (std::chrono::steady_clock::now() > timeout) {
            LOG(ERROR) << "Reached timeout!";
            return;
        }

        // Unlock the thread mutex and allow the Agent thread to work while this thread sleeps
        unlock();
        UTILS_SLEEP_MSEC(1000);
    }
}

/**
 * @brief get parameter command
 *
 * get agent parameter
 *
 * @param[in] params command parsed parameter map
 * @param[out] value returned parameter or error on failure
 * @return true on success
 * @return false on failure
 */
bool agent_ucc_listener::handle_dev_get_param(std::unordered_map<std::string, std::string> &params,
                                              std::string &value)
{
    auto parameter     = params["parameter"];
    auto db            = AgentDB::get();
    sMacAddr mac_value = net::network_utils::ZERO_MAC;

    std::transform(parameter.begin(), parameter.end(), parameter.begin(), ::tolower);
    if (parameter == "alid") {
        value = m_bridge_mac;
        return true;
    } else if (parameter == "macaddr") {
        if (params.find("ruid") == params.end()) {
            value = "missing ruid";
            return false;
        }
        auto ruid_str = tlvf::mac_to_string(std::strtoull(params["ruid"].c_str(), nullptr, 16));
        auto ruid     = tlvf::mac_from_string(ruid_str);
        if (params.find("ssid") == params.end()) {
            // No ssid was given, return the radio MAC.
            // We use MAC address as Radio UID, so we can just return it here.
            value = ruid_str;
            return true;
        }
        auto ssid = params["ssid"];
        // there is an ssid, lookup the corresponding mac address
        if (!db->get_mac_by_ssid(ruid, ssid, mac_value)) {
            LOG(ERROR) << " failed to find the MAC address for ruid '" << ruid_str << "'"
                       << " ssid '" << ssid << "'";
            value = "macaddr/bssid not found for ruid " + ruid_str + " ssid " + ssid;
            return false;
        }
        value = tlvf::mac_to_string(mac_value);
        return true;
    } else if (parameter == "bssid") {
        if (params.find("ruid") == params.end()) {
            value = "missing ruid";
            return false;
        }
        if (params.find("ssid") == params.end()) {
            value = "missing ssid";
            return false;
        }
        auto ruid_str = tlvf::mac_to_string(std::strtoull(params["ruid"].c_str(), nullptr, 16));
        auto ruid     = tlvf::mac_from_string(ruid_str);
        auto ssid     = params["ssid"];

        if (!db->get_mac_by_ssid(ruid, ssid, mac_value)) {
            LOG(ERROR) << " failed to find the BSSID for ruid '" << ruid_str << "'"
                       << " ssid '" << ssid << "'";
            value = "macaddr/bssid not found for ruid " + ruid_str + " ssid " + ssid;
            return false;
        }
        value = tlvf::mac_to_string(mac_value);
        return true;
    }
    value = "parameter " + parameter + " not supported";
    return false;
}

/**
 * @brief Send CMDU to destined Agent.
 *
 * @param[in] dest_mac Controllers mac address
 * @param[in] cmdu_tx CMDU object
 * @return true if successful, false if not.
 */
bool agent_ucc_listener::send_cmdu_to_destination(ieee1905_1::CmduMessageTx &cmdu_tx,
                                                  const std::string &dest_mac)
{
    return m_backhaul_manager_ctx.send_cmdu_to_broker(cmdu_tx, dest_mac, m_bridge_mac);
}

static enum eFreqType band_to_freq(const std::string &band)
{
    if (band == "24G") {
        return eFreqType::FREQ_24G;
    } else if (band == "5GL") {
        return eFreqType::FREQ_5G;
    } else if (band == "5GH") {
        return eFreqType::FREQ_5G;
    } else {
        return eFreqType::FREQ_UNKNOWN;
    }
}

bool agent_ucc_listener::handle_start_wps_registration(const std::string &band,
                                                       std::string &err_string)
{
    auto freq          = band_to_freq(band);
    auto radio_mac_str = m_backhaul_manager_ctx.freq_to_radio_mac(freq);
    if (radio_mac_str.empty()) {
        err_string = "Failed to get radio for " + band;
        return false;
    }

    LOG(DEBUG) << "Trigger WPS PBC on radio mac " << radio_mac_str;
    err_string = "Failed to start wps pbc";
    return m_backhaul_manager_ctx.start_wps_pbc(tlvf::mac_from_string(radio_mac_str));
}

/**
 * @brief Handle DEV_SET_CONFIG command. Parse the command and save the parameters on the agent.
 *
 * @param[in] params Command parameters.
 * @param[out] err_string Contains an error description if the function fails.
 * @return true if successful, false if not.
 */
bool agent_ucc_listener::handle_dev_set_config(std::unordered_map<std::string, std::string> &params,
                                               std::string &err_string)
{

    if (params.find("bss_info") != params.end()) {
        err_string = "parameter 'bss_info' is not relevant to the agent";
        return false;
    }

    if (params.find("backhaul") == params.end()) {
        err_string = "parameter 'backhaul' is missing";
        return false;
    }

    auto &backhaul_param = params["backhaul"];
    std::transform(backhaul_param.begin(), backhaul_param.end(), backhaul_param.begin(), ::tolower);
    if (backhaul_param == DEV_SET_ETH) {
        m_selected_backhaul = DEV_SET_ETH;
    } else {
        // backhaul param must be a radio UID, in hex, starting with 0x
        if (backhaul_param.substr(0, 2) != "0x" || backhaul_param.size() != 14 ||
            backhaul_param.find_first_not_of("0123456789abcdef", 2) != std::string::npos) {
            err_string = "parameter 'backhaul' is not 'eth' or MAC address";
            return false;
        }
        sMacAddr backhaul_radio_uid = net::network_utils::ZERO_MAC;
        for (size_t idx = 0; idx < 6; idx++) {
            backhaul_radio_uid.oct[idx] = std::stoul(backhaul_param.substr(2 + 2 * idx, 2), 0, 16);
        }
        m_selected_backhaul = tlvf::mac_to_string(backhaul_radio_uid);
    }

    // Signal to backhaul that it can continue onboarding.
    m_in_reset = false;
    return true;
}

/**
 * @brief Get the selected backhaul which has been received on "DEV_SET_CONFIG" command from UCC.
 *
 * @return std::string "eth" or RUID of selected radio or empty string if "DEV_SET_CONFIG" has not
 * been received.
 */
std::string agent_ucc_listener::get_selected_backhaul() { return m_selected_backhaul; }
