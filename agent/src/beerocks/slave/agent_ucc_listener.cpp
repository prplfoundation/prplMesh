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

using namespace beerocks;
using namespace net;

agent_ucc_listener::agent_ucc_listener(uint16_t port, const std::string &vendor,
                                       const std::string &model, const std::string &bridge_iface,
                                       SocketClient **controller_sd)
    : beerocks_ucc_listener(port), m_vendor(vendor), m_model(model), m_bridge_iface(bridge_iface),
      m_controller_sd(controller_sd)
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

    return std::string();
}

/**
 * @brief Clears configuration on Controller database.
 * 
 * @return None.
 */
void agent_ucc_listener::clear_configuration()
{
    // set flag that dev_reset_default is required.
    // son_slave will monitor this flag and trigger slave_reset.
    m_dev_reset_default = true;
    reply_ucc(eWfaCaStatus::RUNNING);
}

void agent_ucc_listener::update_vaps_list(std::string ruid, beerocks_message::sVapsList &vaps)
{
    LOG(INFO) << "Update VAP map for ruid " << ruid << " bssid "
              << network_utils::mac_to_string(vaps.vaps->mac) << " ssid "
              << std::string(vaps.vaps->ssid, 36);
    vaps_map[ruid] = vaps;
}

#include <stdio.h>
#include <inttypes.h>
#include <limits.h>
/** Convert uint64_t mac address to hex
 * @param[in] mac uint64_t mac address
 * @param[out] hwaddr hex mac address
 */
void int2mac(const uint64_t mac, uint8_t *hwaddr)
{
    int8_t i;
    uint8_t *p = hwaddr;

    for (i = 5; i >= 0; i--) {
        *p++ = mac >> (CHAR_BIT * i);
    }
}

void agent_ucc_listener::get_parameter(std::unordered_map<std::string, std::string> params)
{
    reply_ucc(eWfaCaStatus::RUNNING);
    if (params["parameter"] == "ALid") {
        std::string alid;
        if (!net::network_utils::linux_iface_get_mac("br-lan", alid)) {
            reply_ucc(eWfaCaStatus::INVALID, "FAIL");
            return;
        }
        reply_ucc(eWfaCaStatus::COMPLETE, std::string("aLid,") + alid);
    } else if (params["parameter"] == "macaddr") {
        // assume dev_get_parameter,program,map,ruid,riod
        uint64_t ruid_int = std::stoull(params["ruid"], nullptr, 16);
        LOG(DEBUG) << "ruid_int=" << ruid_int;
        uint8_t oct[6] = {0};
        int2mac(ruid_int, oct);
        auto ruid = network_utils::mac_to_string(oct);
        std::string ssid      = params["ssid"];
        LOG(INFO) << "dev_get_parameter: ruid " << ruid << " ssid " << ssid;

        for (auto vap : vaps_map[ruid].vaps) {
            if (std::string(vap.ssid) == ssid) {
                reply_ucc(eWfaCaStatus::COMPLETE,
                        std::string("macaddr,") + network_utils::mac_to_string(vap.mac));
                return;
            }
        }
        reply_ucc(eWfaCaStatus::ERROR,
                std::string("macaddr not found for ruid ") + ruid + std::string(" ssid ") + ssid);
    }
}
/**
 * @brief Return socket to Agent with bridge 'dest_alid` MAC address.
 * 
 * @param[in] dest_alid Agent bridge MAC address.
 * @return Socket* Socket to the Agent.
 */
Socket *agent_ucc_listener::get_dev_send_1905_destination_socket(const std::string &dest_alid)
{
    // On the agent side, the dest_alid is not really needed since the destination socket will
    // always be the controller socket.
    return *m_controller_sd;
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
 * @param[in] sd Agent socket
 * @param[in] cmdu_tx CMDU object
 * @return true if successful, false if not.
 */
bool agent_ucc_listener::send_cmdu_to_destination(Socket *sd, ieee1905_1::CmduMessageTx &cmdu_tx)
{
    if (*m_controller_sd == nullptr) {
        LOG(ERROR) << "socket to controller is nullptr";
        return false;
    }

    return message_com::send_cmdu(*m_controller_sd, cmdu_tx, (*m_controller_sd)->getPeerMac(),
                                  m_bridge_mac);
    return true;
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
    reply_ucc(eWfaCaStatus::RUNNING);
    if (params.find("bss_info") != params.end()) {
        err_string = "parameter 'bss_info' is not relevant to the agent";
        return false;
    }

    if (params.find("backhaul") == params.end()) {
        err_string = "parameter 'backhaul' is missing";
        return false;
    }

    m_dev_set_config_onboarding            = true;
    m_dev_set_config_onboarding_inprogress = true;
    return true;
}
