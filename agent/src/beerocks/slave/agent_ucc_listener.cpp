/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "agent_ucc_listener.h"

#include <bcl/network/network_utils.h>
#include <beerocks/tlvf/beerocks_message.h>

#include <easylogging++.h>

#include "backhaul_manager/backhaul_manager_thread.h"

using namespace beerocks;
using namespace net;

agent_ucc_listener::agent_ucc_listener(backhaul_manager &backhaul_manager_ctx, uint16_t port,
                                       const std::string &vendor, const std::string &model,
                                       const std::string &bridge_iface)
    : beerocks_ucc_listener(port), m_backhaul_manager_ctx(backhaul_manager_ctx), m_vendor(vendor),
      m_model(model), m_bridge_iface(bridge_iface)
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
    m_onboarding_state = eOnboardingState::WAIT_FOR_RESET;

    auto timeout =
        std::chrono::steady_clock::now() + std::chrono::seconds(UCC_REPLY_COMPLETE_TIMEOUT_SEC);

    while (m_onboarding_state != eOnboardingState::WAIT_FOR_CONFIG) {

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
 * @brief Update list of VAPs for ruid.
 * 
 * @return None.
 */

typedef struct {
    std::string mac;
    std::string ssid;
    bool backhaul_vap;
} sVapElement;
void agent_ucc_listener::update_vaps_list(std::string ruid, beerocks_message::sVapsList &vaps)
{
    LOG(INFO) << "Update VAP map for ruid " << ruid << " bssid "
              << network_utils::mac_to_string(vaps.vaps->mac) << " ssid "
              << std::string(vaps.vaps->ssid, 36);
    vaps_map[ruid] = vaps;
}

/**
 * @brief Validate if the 'dest_alid` MAC address matches the controllers MAC address.
 * 
 * @param[in] dest_alid Agent bridge MAC address.
 * @return true if successful, false if not.
 */
bool agent_ucc_listener::validate_destination_alid(const std::string &dest_alid)
{
    // On the agent side, the dest_alid is not really needed since the destination socket will
    // always be the controller socket.
    return m_backhaul_manager_ctx.get_controller_bridge_mac() == dest_alid;
}

/**
 * @brief Get preprepared buffer with CMDU in it.
 * 
 * @return std::shared_ptr<uint8_t> Buffer pointer.
 */
std::shared_ptr<uint8_t> agent_ucc_listener::get_buffer_filled_with_cmdu()
{
    // Currently, no such buffer on agent side.
    return std::shared_ptr<uint8_t>(nullptr);
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
    return m_backhaul_manager_ctx.send_cmdu_to_bus(cmdu_tx, dest_mac, m_bridge_mac);
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
    m_selected_backhaul = backhaul_param;

    auto timeout =
        std::chrono::steady_clock::now() + std::chrono::seconds(UCC_REPLY_COMPLETE_TIMEOUT_SEC);

    while (m_onboarding_state == eOnboardingState::WAIT_FOR_CONFIG ||
           m_onboarding_state == eOnboardingState::IN_PROGRESS) {
        if (std::chrono::steady_clock::now() > timeout) {
            err_string         = "onboarding timeout";
            m_onboarding_state = eOnboardingState::NOT_IN_PROGRESS;
            m_selected_backhaul.clear();
            return false;
        }
        // Unlock the thread mutex and allow the Agent thread to work while this thread sleeps
        unlock();
        UTILS_SLEEP_MSEC(1000);
    }

    if (m_onboarding_state != eOnboardingState::SUCCESS) {
        err_string         = "onboarding failed";
        m_onboarding_state = eOnboardingState::NOT_IN_PROGRESS;
        m_selected_backhaul.clear();
        return false;
    }

    m_onboarding_state = eOnboardingState::NOT_IN_PROGRESS;
    m_selected_backhaul.clear();
    return true;
}

/**
 * @brief Get the onboarding state, and update the state if needed. 
 * 
 * @return eOnboardingState The onboarding state.
 */
eOnboardingState agent_ucc_listener::get_and_update_onboarding_state()
{
    if (m_onboarding_state == eOnboardingState::WAIT_FOR_RESET) {
        m_onboarding_state = eOnboardingState::RESET_TO_DEFAULT;
    } else if (m_onboarding_state == eOnboardingState::RESET_TO_DEFAULT) {
        m_onboarding_state = eOnboardingState::WAIT_FOR_CONFIG;
        return eOnboardingState::RESET_TO_DEFAULT;
    } else if (m_onboarding_state == eOnboardingState::WAIT_FOR_CONFIG) {
        if (!m_selected_backhaul.empty()) {
            m_onboarding_state = eOnboardingState::IN_PROGRESS;
        }
    }

    return m_onboarding_state;
}

/**
 * @brief Set the Agent onboarding status. Calling this function will trigger sending the UCC
 * COMPLETE/ERROR status.
 * 
 * @param success Onboarding success status.
 * @return None.
 */
void agent_ucc_listener::set_onboarding_status(bool success)
{
    m_onboarding_state = success ? eOnboardingState::SUCCESS : eOnboardingState::FAIL;
}

/**
 * @brief Get the selected backhaul which has been received on "DEV_SET_CONFIG" command from UCC.
 * 
 * @return std::string "eth" or RUID of selected radio or empty string if "DEV_SET_CONFIG" has not 
 * been received.
 */
std::string agent_ucc_listener::get_selected_backhaul() { return m_selected_backhaul; }
