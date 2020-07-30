/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

/**
 * This file uses code copied from `iw` (http://git.sipsolutions.net/iw.git/)
 *
 * Copyright (c) 2007, 2008 Johannes Berg
 * Copyright (c) 2007    Andy Lutomirski
 * Copyright (c) 2007    Mike Kershaw
 * Copyright (c) 2008-2009   Luis R. Rodriguez
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "backhaul_manager_thread.h"
#include "../agent_db.h"

#include "../link_metrics/ieee802_11_link_metrics_collector.h"
#include "../link_metrics/ieee802_3_link_metrics_collector.h"
#include "../tlvf_utils.h"

#include <bcl/beerocks_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_backhaul.h>
#include <beerocks/tlvf/beerocks_message_control.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

/*
 * TODO:
 * move all these includes into a separate header because
 * over time this list is going to get very long
 */
#include <tlvf/ieee_1905_1/s802_11SpecificInformation.h>
#include <tlvf/ieee_1905_1/tlv1905NeighborDevice.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricResultCode.h>
#include <tlvf/ieee_1905_1/tlvMacAddress.h>
#include <tlvf/ieee_1905_1/tlvReceiverLinkMetric.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/ieee_1905_1/tlvTransmitterLinkMetric.h>
#include <tlvf/wfa_map/tlvApCapability.h>
#include <tlvf/wfa_map/tlvApHeCapabilities.h>
#include <tlvf/wfa_map/tlvApHtCapabilities.h>
#include <tlvf/wfa_map/tlvApMetricQuery.h>
#include <tlvf/wfa_map/tlvApMetrics.h>
#include <tlvf/wfa_map/tlvApOperationalBSS.h>
#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>
#include <tlvf/wfa_map/tlvApVhtCapabilities.h>
#include <tlvf/wfa_map/tlvAssociatedClients.h>
#include <tlvf/wfa_map/tlvAssociatedStaLinkMetrics.h>
#include <tlvf/wfa_map/tlvAssociatedStaTrafficStats.h>
#include <tlvf/wfa_map/tlvBackhaulSteeringRequest.h>
#include <tlvf/wfa_map/tlvBackhaulSteeringResponse.h>
#include <tlvf/wfa_map/tlvBeaconMetricsQuery.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvClientCapabilityReport.h>
#include <tlvf/wfa_map/tlvClientInfo.h>
#include <tlvf/wfa_map/tlvErrorCode.h>
#include <tlvf/wfa_map/tlvHigherLayerData.h>
#include <tlvf/wfa_map/tlvMetricReportingPolicy.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvStaMacAddressType.h>
#include <tlvf/wfa_map/tlvSteeringPolicy.h>
#include <tlvf/wfa_map/tlvSupportedService.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>

// BPL Error Codes
#include <bpl/bpl_cfg.h>
#include <bpl/bpl_err.h>

// SPEED values
#include <linux/ethtool.h>

using namespace beerocks::net;

namespace beerocks {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define FSM_MOVE_STATE(eNewState)                                                                  \
    ({                                                                                             \
        LOG(TRACE) << "FSM: " << s_arrStates[int(m_eFSMState)] << " --> "                          \
                   << s_arrStates[int(EState::eNewState)];                                         \
        m_eFSMState = EState::eNewState;                                                           \
    })

#define FSM_IS_IN_STATE(eState) (m_eFSMState == EState::eState)
#define FSM_CURR_STATE_STR s_arrStates[int(m_eFSMState)]

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Static Members ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

const char *backhaul_manager::s_arrStates[] = {FOREACH_STATE(GENERATE_STRING)};

/**
 * IEEE Std 1905.1, Table 6-12—Media type (intfType) for IEEE 802.11 interfaces
 *
 * Value and Description         Frequency  MaxBandwith  Comments
 * 0 IEEE 802.11b (2.4 GHz)      2.4 GHz    22 MHz       Not supported
 * 1 IEEE 802.11g (2.4 GHz)      2.4 GHz    20 MHz
 * 2 IEEE 802.11a (5 GHz)        5 GHz      20 MHz
 * 3 IEEE 802.11n (2.4 GHz)      2.4 GHz    40 MHz
 * 4 IEEE 802.11n (5 GHz)        5 GHz      40 MHz
 * 5 IEEE 802.11ac (5 GHz)       5 GHz      80 MHz
 * 6 IEEE 802.11ad (60 GHz)      60 GHz     160 MHz      Not supported
 * 7 IEEE 802.11af (whitespace)                          Not supported
 * 8 IEEE 802.11ax (2.4 GHz)     2.4 GHz    160 MHz      Not included in Table 6-12—Media type (intfType), WiFi6 is specified to use 0x0108 in R2
 * 8 IEEE 802.11ax (5 GHz)       5 GHz      160 MHz      Not included in Table 6-12—Media type (intfType), WiFi6 is specified to use 0x0108 in R2
 */
static const std::vector<std::tuple<eFreqType, eWiFiBandwidth, ieee1905_1::eMediaType>>
    table_6_12_media_type_802_11{
        std::make_tuple(eFreqType::FREQ_24G, eWiFiBandwidth::BANDWIDTH_20,
                        ieee1905_1::eMediaType::IEEE_802_11G_2_4_GHZ),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_20,
                        ieee1905_1::eMediaType::IEEE_802_11A_5_GHZ),

        std::make_tuple(eFreqType::FREQ_24G, eWiFiBandwidth::BANDWIDTH_40,
                        ieee1905_1::eMediaType::IEEE_802_11N_2_4_GHZ),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_40,
                        ieee1905_1::eMediaType::IEEE_802_11N_5_GHZ),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_80,
                        ieee1905_1::eMediaType::IEEE_802_11AC_5_GHZ),

        std::make_tuple(eFreqType::FREQ_24G, eWiFiBandwidth::BANDWIDTH_160,
                        ieee1905_1::eMediaType::IEEE_802_11AX),

        std::make_tuple(eFreqType::FREQ_5G, eWiFiBandwidth::BANDWIDTH_160,
                        ieee1905_1::eMediaType::IEEE_802_11AX),

    };

/**
 * @brief Gets media type from given frequency band and max bandwidth values.
 *
 * Media type value is obtained by looking up into table_6_12_media_type_802_11 table.
 * Returns UNKNOWN_MEDIA if frequency band and max bandwidth are not found in table.
 *
 * @param frequency_band Frequency band
 * @param max_bandwidth Maximum bandwidth
 * @return Media type value as per table_6_12_media_type_802_11 table.
 */
static ieee1905_1::eMediaType get_802_11_media_type(eFreqType frequency_band,
                                                    eWiFiBandwidth max_bandwidth)
{
    for (const auto &tuple : table_6_12_media_type_802_11) {
        if ((std::get<0>(tuple) == frequency_band) && (std::get<1>(tuple) == max_bandwidth)) {
            return std::get<2>(tuple);
        }
    }

    return ieee1905_1::eMediaType::UNKNOWN_MEDIA;
}

/**
 * @brief Gets the MAC address of given interface.
 *
 * @param[in] iface_name Name of the network interface.
 * @param[out] iface_mac MAC address of the network interface on success and ZERO_MAC on error.
 *
 * @return True on success and false otherwise.
 */
static bool get_iface_mac(const std::string &iface_name, sMacAddr &iface_mac)
{
    std::string mac;
    if (network_utils::linux_iface_get_mac(iface_name, mac)) {
        iface_mac = tlvf::mac_from_string(mac);
        return true;
    }

    LOG(ERROR) << "Failed getting MAC address for interface: " << iface_name;
    iface_mac = network_utils::ZERO_MAC;

    return false;
}

/**
 * @brief Gets the name of the interface with given MAC address.
 *
 * @param[in] iface_mac MAC address of the network interface.
 * @param[out] iface_name Name of the network interface on success and empty string on error.
 *
 * @return True on success and false otherwise.
 */
static bool get_iface_name(const sMacAddr &iface_mac, std::string &iface_name)
{
    if (network_utils::linux_iface_get_name(iface_mac, iface_name)) {
        return true;
    }

    LOG(ERROR) << "Failed getting interface name for MAC address: " << iface_mac;
    iface_name.clear();

    return false;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

backhaul_manager::backhaul_manager(const config_file::sConfigSlave &config,
                                   const std::set<std::string> &slave_ap_ifaces_,
                                   const std::set<std::string> &slave_sta_ifaces_,
                                   int stop_on_failure_attempts_)
    : transport_socket_thread(config.temp_path + std::string(BEEROCKS_BACKHAUL_MGR_UDS)),
      beerocks_temp_path(config.temp_path), slave_ap_ifaces(slave_ap_ifaces_),
      slave_sta_ifaces(slave_sta_ifaces_), config_const_bh_slave(config.const_backhaul_slave)
{
    thread_name                            = "backhaul_manager";
    pending_slave_ifaces                   = slave_ap_ifaces_;
    configuration_stop_on_failure_attempts = stop_on_failure_attempts_;
    stop_on_failure_attempts               = stop_on_failure_attempts_;
    LOG(DEBUG) << "stop_on_failure_attempts=" << stop_on_failure_attempts;
    m_sConfig.ucc_listener_port = string_utils::stoi(config.ucc_listener_port);
    m_sConfig.vendor            = config.vendor;
    m_sConfig.model             = config.model;

    // Create a local copy on this process database instance. Will be removed on PPM-83 phase 5
    auto db               = AgentDB::get();
    db->bridge.iface_name = config.bridge_iface;

    m_eFSMState = EState::INIT;
    set_select_timeout(SELECT_TIMEOUT_MSC);
}

backhaul_manager::~backhaul_manager() { backhaul_manager::on_thread_stop(); }

bool backhaul_manager::init()
{
    on_thread_stop();
    if (!transport_socket_thread::init()) {
        LOG(ERROR) << "Failed init!";
        stop();
        return false;
    }

    if (!broker_subscribe(std::vector<ieee1905_1::eMessageType>{
            ieee1905_1::eMessageType::ACK_MESSAGE,
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE,
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE,
            ieee1905_1::eMessageType::AP_CAPABILITY_QUERY_MESSAGE,
            ieee1905_1::eMessageType::AP_METRICS_QUERY_MESSAGE,
            ieee1905_1::eMessageType::ASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE,
            ieee1905_1::eMessageType::BEACON_METRICS_QUERY_MESSAGE,
            ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE,
            ieee1905_1::eMessageType::CHANNEL_SELECTION_REQUEST_MESSAGE,
            ieee1905_1::eMessageType::CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE,
            ieee1905_1::eMessageType::CLIENT_CAPABILITY_QUERY_MESSAGE,
            ieee1905_1::eMessageType::CLIENT_STEERING_REQUEST_MESSAGE,
            ieee1905_1::eMessageType::COMBINED_INFRASTRUCTURE_METRICS_MESSAGE,
            ieee1905_1::eMessageType::HIGHER_LAYER_DATA_MESSAGE,
            ieee1905_1::eMessageType::LINK_METRIC_QUERY_MESSAGE,
            ieee1905_1::eMessageType::MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE,
            ieee1905_1::eMessageType::TOPOLOGY_DISCOVERY_MESSAGE,
            ieee1905_1::eMessageType::TOPOLOGY_QUERY_MESSAGE,
            ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE,
            ieee1905_1::eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE,
        })) {
        LOG(ERROR) << "Failed to init mapf_bus";
        return false;
    }

    return true;
}

bool backhaul_manager::work()
{
    bool skip_select = false;

    if (!backhaul_fsm_main(skip_select))
        return false;

    if (!skip_select)
        if (!transport_socket_thread::work())
            return false;

    return true;
}

void backhaul_manager::on_thread_stop()
{
    // Close the socket with the platform manager
    if (m_scPlatform) {
        m_scPlatform.reset();
    }

    while (slaves_sockets.size() > 0) {
        auto soc          = slaves_sockets.back();
        std::string iface = soc->sta_iface;
        LOG(DEBUG) << "Closing iface " << iface << " sockets";
        if (soc) {
            if (soc->slave) {
                remove_socket(soc->slave);
                delete soc->slave;
            }
            if (soc->sta_wlan_hal) {
                soc->sta_wlan_hal.reset();
            }
            if (soc->sta_hal_ext_events) {
                remove_socket(soc->sta_hal_ext_events);
                delete soc->sta_hal_ext_events;
                soc->sta_hal_ext_events = nullptr;
            }
            if (soc->sta_hal_int_events) {
                remove_socket(soc->sta_hal_int_events);
                delete soc->sta_hal_int_events;
                soc->sta_hal_int_events = nullptr;
            }
        }
        slaves_sockets.pop_back();
    }
}

void backhaul_manager::socket_connected(Socket *sd)
{
    LOG(DEBUG) << "new slave_socket, sd=" << intptr_t(sd);
    auto soc   = std::make_shared<sRadioInfo>();
    soc->slave = sd;
    slaves_sockets.push_back(soc);
    add_socket(soc->slave);
}

bool backhaul_manager::socket_disconnected(Socket *sd)
{
    if (from_broker(sd)) {
        LOG(ERROR) << "broker socket to the controller disconnected " << intptr_t(sd)
                   << " restarting backhaul manager";
        FSM_MOVE_STATE(RESTART);
        return true;
    }

    if (m_scPlatform.get() == sd) {
        LOG(ERROR) << "platform manager socket disconnected " << intptr_t(sd)
                   << " restarting backhaul manager";
        m_scPlatform.reset();
        FSM_MOVE_STATE(RESTART);
        return true;
    }

    auto db = AgentDB::get();

    for (auto it = slaves_sockets.begin(); it != slaves_sockets.end();) {
        auto soc          = *it;
        std::string iface = soc->hostap_iface;
        if (soc->slave == sd) {
            LOG(INFO) << "slave disconnected, iface=" << iface
                      << " backhaul_manager=" << int(soc->slave_is_backhaul_manager);
            if (soc->slave) {
                LOG(INFO) << "removing slave socket";
                remove_socket(soc->slave);
                delete soc->slave;
            }
            if (soc->sta_wlan_hal) {
                LOG(INFO) << "dereferencing sta_wlan_hal";
                soc->sta_wlan_hal.reset();
            }
            if (soc->sta_hal_ext_events) {
                LOG(INFO) << "removing sta_hal_ext_events socket";
                remove_socket(soc->sta_hal_ext_events);
                delete soc->sta_hal_ext_events;
                soc->sta_hal_ext_events = nullptr;
            }
            if (soc->sta_hal_int_events) {
                LOG(INFO) << "removing sta_hal_int_events socket";
                remove_socket(soc->sta_hal_int_events);
                delete soc->sta_hal_int_events;
                soc->sta_hal_int_events = nullptr;
            }

            // Remove the socket reference from the backhaul configuration
            m_sConfig.slave_iface_socket.erase(iface);

            if (!m_agent_ucc_listener) {
                LOG(INFO) << "sending platform_notify: slave socket disconnected " << iface;
                platform_notify_error(bpl::eErrorCode::BH_SLAVE_SOCKET_DISCONNECTED,
                                      "slave socket disconnected " + iface);
            }

            it = slaves_sockets.erase(it);
            if ((m_eFSMState > EState::_WIRELESS_START_ && m_eFSMState < EState::_WIRELESS_END_) ||
                (soc->slave_is_backhaul_manager &&
                 db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless)) {
                LOG(INFO) << "Not in operational state OR backhaul manager slave disconnected, "
                             "restarting backhaul manager. Backhaul connection is probably lost";
                FSM_MOVE_STATE(RESTART);
            } else if (soc->slave_is_backhaul_manager &&
                       !slaves_sockets.empty()) { // bh_manager socket in Wired backhaul mode
                LOG(INFO)
                    << "backhaul manager slave disconnected on wired backhaul, Replacing it...";
                finalize_slaves_connect_state(true, slaves_sockets.front());
            }
            LOG(INFO) << "disconnected slave sockets has been deleted";

            if (m_eFSMState >= EState::CONNECT_TO_MASTER) {
                LOG(INFO) << "Sending topology notification on son_slave disconnect";
                auto cmdu_header =
                    cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
                if (!cmdu_header) {
                    LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
                    return false;
                }
                send_cmdu_to_broker(cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                    tlvf::mac_to_string(db->bridge.mac));
            }
            return false;
        } else {
            ++it;
        }
    }

    return true;
}

std::string backhaul_manager::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

void backhaul_manager::platform_notify_error(bpl::eErrorCode code, const std::string &error_data)
{
    if (!m_scPlatform) {
        LOG(ERROR) << "Invalid Platform Manager socket!";
        return;
    }

    auto error =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ERROR_NOTIFICATION>(
            cmdu_tx);

    if (error == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return;
    }

    error->code() = uint32_t(code);

    string_utils::copy_string(error->data(0), error_data.c_str(),
                              message::PLATFORM_ERROR_DATA_SIZE);

    LOG(ERROR) << "platform_notify_error: " << error_data;

    // Send the message
    message_com::send_cmdu(m_scPlatform.get(), cmdu_tx);
}

void backhaul_manager::before_select()
{
    if (m_agent_ucc_listener) {
        m_agent_ucc_listener->unlock();
    }
}

void backhaul_manager::after_select(bool timeout)
{
    if (m_agent_ucc_listener) {
        m_agent_ucc_listener->lock();
    }

    for (auto &soc : slaves_sockets) {
        if (soc->sta_iface.empty() || !soc->sta_wlan_hal)
            continue;

        // Process external events
        if (read_ready(soc->sta_hal_ext_events)) {
            soc->sta_wlan_hal->process_ext_events();
            clear_ready(soc->sta_hal_ext_events);
        }

        // Process internal events
        if (read_ready(soc->sta_hal_int_events)) {
            // A callback (hal_event_handler()) will invoked for pending events
            soc->sta_wlan_hal->process_int_events();
            clear_ready(soc->sta_hal_int_events);
        }
    }

    auto db = AgentDB::get();

    // Send topology discovery every 60 seconds according to IEEE_Std_1905.1-2013 specification
    static std::chrono::steady_clock::time_point discovery_timestamp =
        std::chrono::steady_clock::now();
    auto now     = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - discovery_timestamp);
    if ((m_eFSMState == EState::CONNECTED) ||
        (m_eFSMState == EState::OPERATIONAL && elapsed.count() == 60)) {
        discovery_timestamp = now;
        send_1905_topology_discovery_message();
    }

    // Each platform must send Topology Discovery message every 60 seconds to its first circle
    // 1905.1 neighbors.
    // Iterate on each of known 1905.1 neighbors and check if we have received in the last
    // 60 seconds a Topology Discovery message from it. If not, remove this neighbor from our list
    // and send a Topology Notification message.
    bool neighbors_list_changed = false;
    for (auto &neighbors_on_local_iface_entry : db->neighbor_devices) {
        auto &neighbors_on_local_iface = neighbors_on_local_iface_entry.second;
        for (auto it = neighbors_on_local_iface.begin(); it != neighbors_on_local_iface.end();) {
            auto &last_topology_discovery = it->second.timestamp;
            if (now - last_topology_discovery >
                std::chrono::seconds(DISCOVERY_NEIGHBOUR_REMOVAL_TIMEOUT)) {
                auto &device_al_mac = it->first;
                LOG(INFO) << "Removed 1905.1 device " << device_al_mac << " from neighbors list";
                it                     = neighbors_on_local_iface.erase(it);
                neighbors_list_changed = true;
                continue;
            }
            it++;
        }
    }

    if (neighbors_list_changed) {
        LOG(INFO) << "Sending topology notification on removeing of 1905.1 neighbors";
        auto cmdu_header =
            cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
            return;
        }
        send_cmdu_to_broker(cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                            tlvf::mac_to_string(db->bridge.mac));
    }

    // Run Tasks
    m_task_pool.run_tasks();
}

bool backhaul_manager::finalize_slaves_connect_state(bool fConnected,
                                                     std::shared_ptr<sRadioInfo> pSocket)
{
    LOG(TRACE) << __func__ << ": fConnected=" << int(fConnected) << std::hex
               << ", pSocket=" << pSocket;
    // Backhaul Connected Notification
    if (fConnected) {

        // Build the notification message
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_BACKHAUL_CONNECTED_NOTIFICATION>(cmdu_tx);

        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        auto db = AgentDB::get();

        network_utils::iface_info iface_info;
        bool backhaul_manager_exist = false;

        notification->params().is_prplmesh_controller = is_prplmesh_controller;
        notification->params().controller_bridge_mac = tlvf::mac_from_string(controller_bridge_mac);

        if (!db->device_conf.local_gw) {
            // Read the IP addresses of the bridge interface
            if (network_utils::get_iface_info(iface_info, db->backhaul.selected_iface_name) != 0) {
                LOG(ERROR) << "Failed reading addresses for: " << db->backhaul.selected_iface_name;
                return false;
            }

            notification->params().gw_ipv4 = network_utils::ipv4_from_string(bridge_info.ip_gw);
            notification->params().gw_bridge_mac = tlvf::mac_from_string(bssid_bridge_mac);
            notification->params().bridge_mac    = db->bridge.mac;
            notification->params().bridge_ipv4   = network_utils::ipv4_from_string(bridge_info.ip);
            notification->params().backhaul_mac  = tlvf::mac_from_string(iface_info.mac);
            notification->params().backhaul_ipv4 = network_utils::ipv4_from_string(iface_info.ip);

            if (db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wired) {
                notification->params().backhaul_bssid =
                    tlvf::mac_from_string(network_utils::ZERO_MAC_STRING);
                notification->params().backhaul_iface_type  = IFACE_TYPE_ETHERNET;
                notification->params().backhaul_is_wireless = 0;
                for (auto soc : slaves_sockets) {

                    if (soc->sta_wlan_hal) {
                        soc->sta_wlan_hal.reset();
                    }
                    if (soc->sta_hal_ext_events) {
                        remove_socket(soc->sta_hal_ext_events);
                        delete soc->sta_hal_ext_events;
                        soc->sta_hal_ext_events = nullptr;
                    }
                    if (soc->sta_hal_int_events) {
                        remove_socket(soc->sta_hal_int_events);
                        delete soc->sta_hal_int_events;
                        soc->sta_hal_int_events = nullptr;
                    }
                }

            } else {

                // Find the slave handling the wireless interface
                for (auto soc : slaves_sockets) {
                    if (soc->sta_iface == db->backhaul.selected_iface_name) {

                        // Mark the slave as the backhaul manager
                        soc->slave_is_backhaul_manager = true;
                        backhaul_manager_exist         = true;

                        notification->params().backhaul_bssid =
                            tlvf::mac_from_string(soc->sta_wlan_hal->get_bssid());
                        // notification->params().backhaul_freq          = son::wireless_utils::channel_to_freq(soc->sta_wlan_hal->get_channel()); // HACK temp disabled because of a bug on endian converter
                        notification->params().backhaul_channel = soc->sta_wlan_hal->get_channel();
                        // TODO - Specify true WiFi model from config (safe to derive from hostap_iface_type?)
                        notification->params().backhaul_iface_type  = IFACE_TYPE_WIFI_INTEL;
                        notification->params().backhaul_is_wireless = 1;
                    } else {
                        // HACK - needs to be controlled from slave

                        // Mark the slave as non backhaul manager
                        soc->slave_is_backhaul_manager = false;
                        // detach from unused stations
                        if (soc->sta_wlan_hal) {
                            soc->sta_wlan_hal.reset();
                        }
                        if (soc->sta_hal_ext_events) {
                            remove_socket(soc->sta_hal_ext_events);
                            delete soc->sta_hal_ext_events;
                            soc->sta_hal_ext_events = nullptr;
                        }
                        if (soc->sta_hal_int_events) {
                            remove_socket(soc->sta_hal_int_events);
                            delete soc->sta_hal_int_events;
                            soc->sta_hal_int_events = nullptr;
                        }
                    }
                }
            }
        }

        int i = 0;
        memset(notification->params().backhaul_scan_measurement_list, 0,
               sizeof(beerocks_message::sBackhaulParams::backhaul_scan_measurement_list));
        for (auto scan_measurement_entry : scan_measurement_list) {
            LOG(DEBUG) << "copy scan list to slaves = " << scan_measurement_entry.first
                       << " channel = " << int(scan_measurement_entry.second.channel)
                       << " rssi = " << int(scan_measurement_entry.second.rssi);
            notification->params().backhaul_scan_measurement_list[i].mac =
                scan_measurement_entry.second.mac;
            i++;
        }

        // handle case when backhaul manager slave is not selected
        if (!backhaul_manager_exist) {
            if (!config_const_bh_slave.empty()) {
                for (auto &soc : slaves_sockets) {
                    if (soc->hostap_iface == config_const_bh_slave) {
                        LOG(INFO) << "Configured slave sfor constant BH manager was found: "
                                  << config_const_bh_slave;
                        soc->slave_is_backhaul_manager = true;
                        break;
                    }
                }
            } else {
                if (!slaves_sockets.empty()) {
                    LOG(WARNING)
                        << "backhaul_manager slave was not found, select first connected slave: "
                        << "hostap_iface=" << slaves_sockets.front()->hostap_iface
                        << ", sta_iface=" << slaves_sockets.front()->hostap_iface;
                    slaves_sockets.front()->slave_is_backhaul_manager = true;
                }
            }
        }

        // Send the message(s)
        for (auto sc : slaves_sockets) {

            LOG(DEBUG) << "Iterating on slave " << sc->hostap_iface;
            //if no controller was discovered for that slave's band, skip
            if (!sc->controller_discovered) {
                LOG(DEBUG) << "controller not discovered, skipping...";
                continue;
            }

            // If the notification should be sent to a specific socket, skip all other
            if (pSocket != nullptr && pSocket != sc) {
                LOG(DEBUG) << "notification should be sent to slave " << pSocket->hostap_iface
                           << " skipping " << sc->hostap_iface;
                continue;
            }
            if (sc->slave == nullptr) {
                LOG(ERROR) << "slave " << sc->hostap_iface << " socket is nullptr!";
                continue;
            }

            // note: On wired connections ore GW, the first connected slave is selected as the backhaul manager
            notification->params().is_backhaul_manager = sc->slave_is_backhaul_manager;

            if (db->device_conf.local_gw) {
                LOG(DEBUG) << "Sending GW_MASTER CONNECTED notification to slave of '"
                           << sc->hostap_iface << "'";
            } else {

                LOG(DEBUG) << "Sending CONNECTED notification to slave of '" << sc->sta_iface
                           << "' - Mac: " << iface_info.mac << ", IP: " << bridge_info.ip
                           << ", GW_IP: " << bridge_info.ip_gw
                           << ", Slave is Backhaul Manager: " << int(sc->slave_is_backhaul_manager);
            }
            message_com::send_cmdu(sc->slave, cmdu_tx);

        } // end for (auto sc : slaves_sockets)

        // Backhaul Disconnected Notification
    } else {

        for (auto sc : slaves_sockets) {
            // If the notification should be sent to a specific socket, skip all other
            if (pSocket != nullptr && pSocket != sc) {
                continue;
            }

            if (sc->slave == nullptr) {
                continue;
            }

            sc->controller_discovered = false;

            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            notification->stopped() =
                (uint8_t)(configuration_stop_on_failure_attempts && !stop_on_failure_attempts);
            LOG(DEBUG) << "Sending DISCONNECTED notification to slave of " << sc->hostap_iface;
            message_com::send_cmdu(sc->slave, cmdu_tx);
        }
    }

    return true;
}

bool backhaul_manager::backhaul_fsm_main(bool &skip_select)
{
    // Process internal FSMs before the main one, to prevent
    // falling into the "default" case...

    // UCC FSM. If UCC is in RESET, we have to stay in (or move to) ENABLED state.
    if (m_agent_ucc_listener && m_agent_ucc_listener->is_in_reset()) {
        if (m_eFSMState == EState::ENABLED) {
            m_agent_ucc_listener->reset_completed();
            // Stay in ENABLE state until onboarding_state will change
            return true;
        } else if (m_eFSMState > EState::ENABLED) {
            auto active_hal = get_wireless_hal();
            if (active_hal) {
                active_hal->disconnect();
            }
            FSM_MOVE_STATE(RESTART);
        }
    }

    // Wireless FSM
    if (m_eFSMState > EState::_WIRELESS_START_ && m_eFSMState < EState::_WIRELESS_END_) {
        return backhaul_fsm_wireless(skip_select);
    }

    switch (m_eFSMState) {
    // Initialize the module
    case EState::INIT: {
        state_time_stamp_timeout = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(STATE_WAIT_ENABLE_TIMEOUT_SECONDS);
        FSM_MOVE_STATE(WAIT_ENABLE);
        break;
    }
    // Wait for Enable command
    case EState::WAIT_ENABLE: {
        auto db = AgentDB::get();
        if (!onboarding && !db->device_conf.local_gw &&
            std::chrono::steady_clock::now() > state_time_stamp_timeout) {
            LOG(ERROR) << STATE_WAIT_ENABLE_TIMEOUT_SECONDS
                       << " seconds has passed on state WAIT_ENABLE, stopping thread!";
            return false;
        } else if (pending_enable) {
            LOG(DEBUG) << "pending_enable = " << int(pending_enable);
            pending_enable = false;
            FSM_MOVE_STATE(ENABLED);
        }
        break;
    }
    // Received Backhaul Enable command
    case EState::ENABLED: {

        std::string selected_backhaul;
        if (m_agent_ucc_listener) {
            selected_backhaul = m_agent_ucc_listener->get_selected_backhaul();
        }
        // If reached here without getting DEV_RESET_DEFAULT from UCC, 'selected_backhaul' will stay
        // empty, and the state machine will not be affected.

        LOG(TRACE) << "backhaul manager state=ENABLED";

        // Connect/Reconnect to the platform manager
        if (!m_scPlatform) {
            m_scPlatform = std::make_shared<SocketClient>(
                SocketClient(beerocks_temp_path + std::string(BEEROCKS_PLAT_MGR_UDS)));
            std::string err = m_scPlatform->getError();
            if (!err.empty()) {
                m_scPlatform.reset();
                LOG(ERROR) << "Failed connecting to Platform Manager: " << err;
            }
        } else {
            LOG(DEBUG) << "Using existing platform_manager_socket=0x"
                       << intptr_t(m_scPlatform.get());
        }

        auto db = AgentDB::get();

        // Ignore 'selected_backhaul' since this case is not covered by certification flows
        if (db->device_conf.local_controller && db->device_conf.local_gw) {
            LOG(DEBUG) << "local controller && local gw";
            FSM_MOVE_STATE(MASTER_DISCOVERY);
            db->backhaul.connection_type = AgentDB::sBackhaul::eConnectionType::Invalid;
            db->backhaul.selected_iface_name.clear();
        } else { // link establish

            auto ifaces = network_utils::linux_get_iface_list_from_bridge(db->bridge.iface_name);

            // If a wired (WAN) interface was provided, try it first, check if the interface is UP
            wan_monitor::ELinkState wired_link_state = wan_monitor::ELinkState::eInvalid;
            if (!db->ethernet.iface_name.empty()) {
                wired_link_state = wan_mon.initialize(db->ethernet.iface_name);
                // Failure might be due to insufficient permissions, datailed error message is being
                // printed inside.
                if (wired_link_state == wan_monitor::ELinkState::eInvalid) {
                    LOG(WARNING) << "wan_mon.initialize() failed, skip wired link establishment";
                }
            }
            if ((wired_link_state == wan_monitor::ELinkState::eUp) &&
                (selected_backhaul.empty() || selected_backhaul == DEV_SET_ETH)) {

                auto it = std::find(ifaces.begin(), ifaces.end(), db->ethernet.iface_name);
                if (it == ifaces.end()) {
                    LOG(ERROR) << "wire iface " << db->ethernet.iface_name
                               << " is not on the bridge";
                    FSM_MOVE_STATE(RESTART);
                    break;
                }

                // Mark the connection as WIRED
                db->backhaul.connection_type     = AgentDB::sBackhaul::eConnectionType::Wired;
                db->backhaul.selected_iface_name = db->ethernet.iface_name;

            } else {
                auto selected_ruid = db->get_radio_by_mac(tlvf::mac_from_string(selected_backhaul),
                                                          AgentDB::eMacType::RADIO);

                if (!selected_backhaul.empty() && !selected_ruid) {
                    LOG(ERROR) << "UCC configured backhaul RUID which is not enabled";
                    // Restart state will update the onboarding status to failure.
                    FSM_MOVE_STATE(RESTART);
                    break;
                }

                // Override backhaul_preferred_radio_band if UCC set it
                if (!selected_ruid) {
                    m_sConfig.backhaul_preferred_radio_band = selected_ruid->front.freq_type;
                }

                // Mark the connection as WIRELESS
                db->backhaul.connection_type = AgentDB::sBackhaul::eConnectionType::Wireless;
            }

            // Move to the next state immediately
            if (db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless) {
                FSM_MOVE_STATE(INIT_HAL);
            } else { // EType::Wired
                FSM_MOVE_STATE(MASTER_DISCOVERY);
            }

            skip_select = true;
        }
        break;
    }
    case EState::MASTER_DISCOVERY: {

        auto db = AgentDB ::get();
        if (network_utils::get_iface_info(bridge_info, db->bridge.iface_name) != 0) {
            LOG(ERROR) << "Failed reading addresses from the bridge!";
            platform_notify_error(bpl::eErrorCode::BH_READING_DATA_FROM_THE_BRIDGE, "");
            stop_on_failure_attempts--;
            FSM_MOVE_STATE(RESTART);
            break;
        }

        // Update bridge parameters on AgentDB.
        db->bridge.mac = tlvf::mac_from_string(bridge_info.mac);

        auto ifaces = network_utils::linux_get_iface_list_from_bridge(db->bridge.iface_name);
        if (!configure_ieee1905_transport_interfaces(db->bridge.iface_name, ifaces)) {
            LOG(ERROR) << "configure_ieee1905_transport_interfaces() failed!";
            FSM_MOVE_STATE(RESTART);
            break;
        }

        if (!broker_connect(beerocks_temp_path, db->device_conf.local_controller)) {
            platform_notify_error(bpl::eErrorCode::BH_CONNECTING_TO_MASTER, "");
            FSM_MOVE_STATE(RESTART);
            break;
        }

        state_attempts = 0;
        FSM_MOVE_STATE(SEND_AUTOCONFIG_SEARCH_MESSAGE);
        break;
    }
    case EState::SEND_AUTOCONFIG_SEARCH_MESSAGE: {
        if (state_attempts++ > MAX_FAILED_AUTOCONFIG_SEARCH_ATTEMPTS) {
            LOG(ERROR) << "exceeded maximum attempts for autoconfig discovery, state_attempts="
                       << state_attempts;
            FSM_MOVE_STATE(RESTART);
            break;
        }

        bool sent_for_2g = false;
        bool sent_for_5g = false;
        auto db          = AgentDB::get();

        for (const auto &radio : db->get_radios_list()) {
            if (!radio) {
                continue;
            }
            //for (auto soc : slaves_sockets) {
            if (radio->front.freq_type == eFreqType::FREQ_24G) {
                if (sent_for_2g)
                    continue;
                if (send_autoconfig_search_message(radio->front.iface_name)) {
                    sent_for_2g = true;
                }
            } else if (radio->front.freq_type == eFreqType::FREQ_5G) {
                if (sent_for_5g)
                    continue;
                if (send_autoconfig_search_message(radio->front.iface_name)) {
                    sent_for_5g = true;
                }
            }
        }

        state_time_stamp_timeout = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(AUTOCONFIG_DISCOVERY_TIMEOUT_SECONDS);

        FSM_MOVE_STATE(WAIT_FOR_AUTOCONFIG_RESPONSE_MESSAGE);
        break;
    }
    case EState::WAIT_FOR_AUTOCONFIG_RESPONSE_MESSAGE: {
        if (std::chrono::steady_clock::now() > state_time_stamp_timeout) {
            FSM_MOVE_STATE(SEND_AUTOCONFIG_SEARCH_MESSAGE);
        }
        break;
    }
    case EState::CONNECT_TO_MASTER: {
        FSM_MOVE_STATE(CONNECTED);
        break;
    }
    // Successfully connected to the master
    case EState::CONNECTED: {

        stop_on_failure_attempts = configuration_stop_on_failure_attempts;

        LOG(DEBUG) << "clearing blacklist";
        ap_blacklist.clear();

        auto db = AgentDB::get();

        eth_link_poll_timer = std::chrono::steady_clock::now();
        m_eth_link_up       = network_utils::linux_iface_is_up_and_running(db->ethernet.iface_name);
        FSM_MOVE_STATE(OPERATIONAL);

        // This event may come as a result of enabling the backhaul, but also as a result
        // of steering. *Only* in case it was the result of steering, we need to send a steering
        // response.
        if (m_backhaul_sta_steering_enable) {
            m_backhaul_sta_steering_enable = false;

            create_backhaul_steering_response(wfa_map::tlvErrorCode::eReasonCode::RESERVED);

            LOG(DEBUG) << "Sending BACKHAUL_STA_STEERING_RESPONSE_MESSAGE";
            send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, bridge_info.mac);
        }
        break;
    }
    // Backhaul manager is OPERATIONAL!
    case EState::OPERATIONAL: {

        /**
         * Get current time. It is later used to compute elapsed time since some start time and
         * check if a timeout has expired to perform periodic actions.
         */
        auto now = std::chrono::steady_clock::now();

        /*
        * TODO
        * This code segment is commented out since wireless-backhaul is not yet supported and
        * the current implementation causes high CPU load on steady-state.
        * The high CPU load is due to a call to linux_iface_is_up_and_running() performed every
        * second to check if the wired interface changed its state. The implementation of the above
        * polls the interface flags using ioctl() which is very costly (~120 milliseconds).
        *
        * An event-driven solution will be implemented as part of the task:
        * [TASK] Dynamic switching between wired and wireless
        * https://github.com/prplfoundation/prplMesh/issues/866
        */
        // auto db = AgentDB::get();
        // int time_elapsed_ms =
        //     std::chrono::duration_cast<std::chrono::milliseconds>(now - eth_link_poll_timer)
        //         .count();
        // //pooling eth link status every second to notice if there been a change.
        // if (time_elapsed_ms > POLL_TIMER_TIMEOUT_MS) {

        //     eth_link_poll_timer = now;
        //     bool eth_link_up = network_utils::linux_iface_is_up_and_running(db->ethernet.iface_name);
        //     if (eth_link_up != m_eth_link_up) {
        //         m_eth_link_up = network_utils::linux_iface_is_up_and_running(db->ethernet.iface_name);
        //         FSM_MOVE_STATE(RESTART);
        //     }
        // } else {
        auto db = AgentDB::get();
        if (pending_enable &&
            db->backhaul.connection_type != AgentDB::sBackhaul::eConnectionType::Invalid) {
            pending_enable = false;
        }

        /**
         * If periodic AP metrics reporting is enabled, check if time interval has elapsed and if
         * so, then report AP metrics.
         */
        if (0 != ap_metrics_reporting_info.reporting_interval_s) {
            int elapsed_time_s = std::chrono::duration_cast<std::chrono::seconds>(
                                     now - ap_metrics_reporting_info.last_reporting_time_point)
                                     .count();

            if (elapsed_time_s >= ap_metrics_reporting_info.reporting_interval_s) {
                ap_metrics_reporting_info.last_reporting_time_point = now;

                // We must generate a new MID for the periodic AP Metrics Response messages that
                // do not correspond to an AP Metrics Query message.
                // We cannot set MID to 0 here because we must also differentiate periodic
                // AP Metrics Response messages and messages received from monitor thread
                // due to channel utilization crossed configured threshold value.
                // As a temporary solution, set MID to UINT16_MAX here.
                // TODO: to be fixed as part of #1328

                // Send ap_metrics query on all bssids exists on the Agent.
                send_slave_ap_metric_query_message(UINT16_MAX);
            }
        }

        break;
    }
    case EState::RESTART: {

        LOG(DEBUG) << "Restarting ...";

        auto db = AgentDB::get();

        for (auto soc : slaves_sockets) {
            std::string iface = soc->sta_iface;

            if (soc->sta_wlan_hal) {
                soc->sta_wlan_hal.reset();
            }
            if (soc->sta_hal_ext_events) {
                remove_socket(soc->sta_hal_ext_events);
                delete soc->sta_hal_ext_events;
                soc->sta_hal_ext_events = nullptr;
            }
            if (soc->sta_hal_int_events) {
                remove_socket(soc->sta_hal_int_events);
                delete soc->sta_hal_int_events;
                soc->sta_hal_int_events = nullptr;
            }

            soc->slave_is_backhaul_manager = false;
        }

        finalize_slaves_connect_state(false); //send disconnect to all connected slaves

        // wait again for enable from each slave before proceeding to attach
        pending_slave_sta_ifaces.clear();
        pending_slave_sta_ifaces = slave_sta_ifaces;

        pending_slave_ifaces.clear();
        pending_slave_ifaces = slave_ap_ifaces;
        pending_enable       = false;

        db->backhaul.connection_type = AgentDB::sBackhaul::eConnectionType::Invalid;

        controller_bridge_mac.clear();

        if (configuration_stop_on_failure_attempts && !stop_on_failure_attempts) {
            LOG(ERROR) << "Reached to max stop on failure attempts!";
            platform_notify_error(bpl::eErrorCode::BH_STOPPED, "backhaul manager stopped");
            FSM_MOVE_STATE(STOPPED);
        } else {
            FSM_MOVE_STATE(INIT);
        }

        ap_blacklist.clear();

        break;
    }
    case EState::STOPPED: {
        break;
    }
    default: {
        LOG(ERROR) << "Undefined state: " << int(m_eFSMState);
        return false;
    }
    }

    return (true);
} // namespace beerocks

bool backhaul_manager::send_1905_topology_discovery_message()
{
    // TODO: get the list of interfaces that are up_and_running using the event-driven mechanism
    // to be implemented in #866

    /**
     * Transmission type of Topology Discovery message is 'neighbor multicast'.
     * That is, the CMDU must be transmitted once on each and every of its 1905.1 interfaces.
     * Also, according to IEEE1905.1, the message should include a MAC Address TLV which contains
     * the address of the interface on which the message is sent. Thus, a different message should
     * be sent on each interface.
     */
    auto db     = AgentDB::get();
    auto ifaces = network_utils::linux_get_iface_list_from_bridge(db->bridge.iface_name);
    for (const auto &iface_name : ifaces) {
        if (!network_utils::linux_iface_is_up_and_running(iface_name)) {
            continue;
        }

        send_1905_topology_discovery_message(iface_name);
    }

    return true;
}

bool backhaul_manager::send_1905_topology_discovery_message(const std::string &iface_name)
{
    sMacAddr iface_mac;
    if (!get_iface_mac(iface_name, iface_mac)) {
        return false;
    }

    auto cmdu_hdr = cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_DISCOVERY_MESSAGE);
    if (!cmdu_hdr) {
        LOG(ERROR) << "Failed to create TOPOLOGY_DISCOVERY_MESSAGE cmdu";
        return false;
    }

    auto db = AgentDB::get();

    auto tlvAlMacAddress = cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMacAddress) {
        LOG(ERROR) << "Failed to create tlvAlMacAddress tlv";
        return false;
    }
    tlvAlMacAddress->mac() = db->bridge.mac;

    auto tlvMacAddress = cmdu_tx.addClass<ieee1905_1::tlvMacAddress>();
    if (!tlvMacAddress) {
        LOG(ERROR) << "Failed to create tlvMacAddress tlv";
        return false;
    }
    tlvMacAddress->mac() = iface_mac;

    LOG(DEBUG) << "send_1905_topology_discovery_message, bridge_mac=" << db->bridge.mac
               << ", iface=" << iface_name;
    return send_cmdu_to_broker(cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                               tlvf::mac_to_string(db->bridge.mac), iface_name);
}

bool backhaul_manager::send_autoconfig_search_message(const std::string &front_radio_iface_name)
{
    auto db = AgentDB::get();

    ieee1905_1::tlvAutoconfigFreqBand::eValue freq_band =
        ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    /*
     * TODO
     * this is a workaround, need to find a better way to know each slave's band
     */
    auto radio = db->radio(front_radio_iface_name);
    if (!radio) {
        LOG(DEBUG) << "Radio of iface " << front_radio_iface_name << " does not exist on the db";
        return false;
    }
    if (radio->front.freq_type == beerocks::eFreqType::FREQ_24G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    } else if (radio->front.freq_type == beerocks::eFreqType::FREQ_5G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_5_GHZ;
    } else {
        LOG(ERROR) << "unsupported freq_type=" << int(radio->front.freq_type)
                   << ", iface=" << front_radio_iface_name;
        return false;
    }
    auto p_cmdu_header =
        cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE);

    auto tlvAlMacAddress = cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMacAddress) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAlMacAddress failed";
        return false;
    }
    tlvAlMacAddress->mac() = db->bridge.mac;

    auto tlvSearchedRole = cmdu_tx.addClass<ieee1905_1::tlvSearchedRole>();
    if (!tlvSearchedRole) {
        LOG(ERROR) << "addClass ieee1905_1::tlvSearchedRole failed";
        return false;
    }
    tlvSearchedRole->value() = ieee1905_1::tlvSearchedRole::REGISTRAR;

    auto tlvAutoconfigFreqBand = cmdu_tx.addClass<ieee1905_1::tlvAutoconfigFreqBand>();
    if (!tlvAutoconfigFreqBand) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAutoconfigFreqBand failed";
        return false;
    }
    tlvAutoconfigFreqBand->value() = freq_band;

    auto tlvSupportedService = cmdu_tx.addClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedService) {
        LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed";
        return false;
    }
    if (!tlvSupportedService->alloc_supported_service_list()) {
        LOG(ERROR) << "alloc_supported_service_list failed";
        return false;
    }
    auto supportedServiceTuple = tlvSupportedService->supported_service_list(0);
    if (!std::get<0>(supportedServiceTuple)) {
        LOG(ERROR) << "Failed accessing supported_service_list";
        return false;
    }
    std::get<1>(supportedServiceTuple) =
        wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT;

    auto tlvSearchedService = cmdu_tx.addClass<wfa_map::tlvSearchedService>();
    if (!tlvSearchedService) {
        LOG(ERROR) << "addClass wfa_map::tlvSearchedService failed";
        return false;
    }
    if (!tlvSearchedService->alloc_searched_service_list()) {
        LOG(ERROR) << "alloc_searched_service_list failed";
        return false;
    }
    auto searchedServiceTuple = tlvSearchedService->searched_service_list(0);
    if (!std::get<0>(searchedServiceTuple)) {
        LOG(ERROR) << "Failed accessing searched_service_list";
        return false;
    }
    std::get<1>(searchedServiceTuple) =
        wfa_map::tlvSearchedService::eSearchedService::MULTI_AP_CONTROLLER;

    // Add prplMesh handshake in a vendor specific TLV.
    // If the controller is prplMesh, it will reply to the autoconfig search with
    // handshake response.
    auto request =
        message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST>(cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed adding cACTION_CONTROL_SLAVE_HANDSHAKE_REQUEST";
        return false;
    }
    auto beerocks_header                      = message_com::get_beerocks_header(cmdu_tx);
    beerocks_header->actionhdr()->direction() = beerocks::BEEROCKS_DIRECTION_CONTROLLER;
    LOG(DEBUG) << "sending autoconfig search message, bridge_mac=" << db->bridge.mac;
    return send_cmdu_to_broker(cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                               tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::send_slave_ap_metric_query_message(
    uint16_t mid, const std::unordered_set<sMacAddr> &bssid_list)
{
    auto db = AgentDB::get();

    for (const auto &radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }
        for (const auto &bssid : radio->front.bssids) {
            if (!bssid_list.empty() && bssid_list.find(bssid.mac) == bssid_list.end()) {
                continue;
            }
            LOG(DEBUG) << "Forwarding AP_METRICS_QUERY_MESSAGE message to son_slave, bssid: "
                       << bssid.mac;

            if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::AP_METRICS_QUERY_MESSAGE)) {
                LOG(ERROR) << "Failed to create AP_METRICS_QUERY_MESSAGE";
                return false;
            }

            auto query = cmdu_tx.addClass<wfa_map::tlvApMetricQuery>();
            if (!query) {
                LOG(ERROR) << "Failed addClass<wfa_map::tlvApMetricQuery>";
                return false;
            }

            if (!query->alloc_bssid_list(1)) {
                LOG(ERROR) << "Failed to allocate memory for bssid_list";
                return false;
            }

            auto list = query->bssid_list(0);
            if (!std::get<0>(list)) {
                LOG(ERROR) << "Failed to get element of bssid_list";
            }
            std::get<1>(list) = bssid.mac;

            auto radio_info = get_radio(radio->front.iface_mac);
            if (!radio_info) {
                LOG(ERROR) << "Failed to get radio info for " << radio->front.iface_mac;
                return false;
            }

            if (!message_com::send_cmdu(radio_info->slave, cmdu_tx)) {
                LOG(ERROR) << "Failed forwarding AP_METRICS_QUERY_MESSAGE message to son_slave";
            }

            m_ap_metric_query.push_back({radio_info->slave, bssid.mac});
        }
    }
    return true;
}

bool backhaul_manager::backhaul_fsm_wireless(bool &skip_select)
{
    switch (m_eFSMState) {
    case EState::INIT_HAL: {
        skip_select = true;
        state_time_stamp_timeout =
            std::chrono::steady_clock::now() + std::chrono::seconds(WPA_ATTACH_TIMEOUT_SECONDS);
        FSM_MOVE_STATE(WPA_ATTACH);
        break;
    }
    case EState::WPA_ATTACH: {

        bool success = true;

        auto db = AgentDB::get();

        for (auto soc : slaves_sockets) {
            std::string iface = soc->sta_iface;
            if (iface.empty())
                continue;

            LOG(DEBUG) << FSM_CURR_STATE_STR << " iface: " << iface;

            // Create a HAL instance if doesn't exists
            if (!soc->sta_wlan_hal) {

                bwl::hal_conf_t hal_conf;

                if (!beerocks::bpl::bpl_cfg_get_wpa_supplicant_ctrl_path(iface,
                                                                         hal_conf.wpa_ctrl_path)) {
                    LOG(ERROR) << "Couldn't get hostapd control path";
                    return false;
                }

                using namespace std::placeholders; // for `_1`
                soc->sta_wlan_hal = bwl::sta_wlan_hal_create(
                    iface, std::bind(&backhaul_manager::hal_event_handler, this, _1, iface),
                    hal_conf);
                LOG_IF(!soc->sta_wlan_hal, FATAL) << "Failed creating HAL instance!";
            } else {
                LOG(DEBUG) << "STA HAL exists...";
            }

            // Attach in BLOCKING mode
            auto attach_state = soc->sta_wlan_hal->attach(true);
            if (attach_state == bwl::HALState::Operational) {

                // Events
                int ext_events_fd = soc->sta_wlan_hal->get_ext_events_fd();
                int int_events_fd = soc->sta_wlan_hal->get_int_events_fd();
                if (ext_events_fd >= 0 && int_events_fd) {
                    soc->sta_hal_ext_events = new Socket(ext_events_fd);
                    add_socket(soc->sta_hal_ext_events);

                    soc->sta_hal_int_events = new Socket(int_events_fd);
                    add_socket(soc->sta_hal_int_events);
                } else {
                    LOG(ERROR) << "Invalid event file descriptors - "
                               << "External = " << ext_events_fd
                               << ", Internal = " << int_events_fd;

                    success = false;
                    break;
                }

                /**
                 * This code was disabled as part of the effort to pass certification flow 
                 * (PR #1469), and broke wireless backhual flow.
                 * If a connected backhual interface has been discovered, the backhaul fsm was set
                 * to MASTER_DISCOVERY state, otherwise to INITIATE_SCAN.
                 */

                // if (!roam_flag && soc->sta_wlan_hal->is_connected()) {
                //     if (!soc->sta_wlan_hal->update_status()) {
                //         LOG(ERROR) << "failed to update sta status";
                //         success = false;
                //         break;
                //     }
                //     connected                        = true;
                //     db->backhaul.selected_iface_name = iface;
                //     db->backhaul.connection_type   = AgentDB::sBackhaul::eConnectionType::Wireless;
                //     selected_bssid                 = soc->sta_wlan_hal->get_bssid();
                //     selected_bssid_channel         = soc->sta_wlan_hal->get_channel();
                //     soc->slave_is_backhaul_manager = true;
                //     break;
                // }

            } else if (attach_state == bwl::HALState::Failed) {
                // Delete the HAL instance
                soc->sta_wlan_hal.reset();
                success = false;
                break;
            }
        }

        if (!success) {
            if (std::chrono::steady_clock::now() > state_time_stamp_timeout) {
                LOG(ERROR) << "attach wpa timeout";
                platform_notify_error(bpl::eErrorCode::BH_TIMEOUT_ATTACHING_TO_WPA_SUPPLICANT, "");
                stop_on_failure_attempts--;
                FSM_MOVE_STATE(RESTART);
            } else {
                UTILS_SLEEP_MSEC(1000);
            }
            break;
        }

        state_attempts = 0; // for next state

        state_time_stamp_timeout =
            std::chrono::steady_clock::now() + std::chrono::seconds(STATE_WAIT_WPS_TIMEOUT_SECONDS);
        FSM_MOVE_STATE(WAIT_WPS);
        break;
    }
    // Wait for WPS command
    case EState::WAIT_WPS: {
        auto db = AgentDB::get();
        if (!onboarding && !db->device_conf.local_gw &&
            std::chrono::steady_clock::now() > state_time_stamp_timeout) {
            LOG(ERROR) << STATE_WAIT_WPS_TIMEOUT_SECONDS
                       << " seconds has passed on state WAIT_WPS, stopping thread!";
            return false;
        }
        break;
    }
    case EState::INITIATE_SCAN: {

        hidden_ssid            = false;
        selected_bssid_channel = 0;
        selected_bssid.clear();

        if (state_attempts > MAX_FAILED_SCAN_ATTEMPTS && !roam_flag) {
            LOG(DEBUG)
                << "exceeded maximum failed scan attempts, attempting hidden ssid connection";
            hidden_ssid              = true;
            pending_slave_sta_ifaces = slave_sta_ifaces;

            FSM_MOVE_STATE(WIRELESS_CONFIG_4ADDR_MODE);
            break;
        }

        if ((state_attempts > MAX_FAILED_ROAM_SCAN_ATTEMPTS) && roam_flag) {
            LOG(DEBUG) << "exceeded MAX_FAILED_ROAM_SCAN_ATTEMPTS";
            roam_flag                   = false;
            roam_selected_bssid_channel = 0;
            roam_selected_bssid.clear();
            state_attempts = 0;
            FSM_MOVE_STATE(RESTART);
            break;
        }

        bool preferred_band_is_available = false;

        // Check if backhaul preferred band is supported (supporting radio is available)
        if (m_sConfig.backhaul_preferred_radio_band == beerocks::eFreqType::FREQ_AUTO) {
            preferred_band_is_available = true;
        } else {
            for (auto soc : slaves_sockets) {
                if (soc->sta_iface.empty())
                    continue;
                if (!soc->sta_wlan_hal) {
                    LOG(WARNING) << "Sta_hal of " << soc->sta_iface << " is null";
                    continue;
                }
                if (m_sConfig.backhaul_preferred_radio_band == soc->freq_type) {
                    preferred_band_is_available = true;
                }
            }
        }

        LOG_IF(!preferred_band_is_available, DEBUG) << "Preferred backhaul band is not available";

        bool success        = true;
        bool scan_triggered = false;

        for (auto soc : slaves_sockets) {
            if (soc->sta_iface.empty())
                continue;

            if (!soc->sta_wlan_hal) {
                LOG(WARNING) << "Sta_hal of " << soc->sta_iface << " is null";
                continue;
            }

            if (preferred_band_is_available &&
                m_sConfig.backhaul_preferred_radio_band != beerocks::eFreqType::FREQ_AUTO &&
                m_sConfig.backhaul_preferred_radio_band != soc->freq_type) {
                LOG(DEBUG) << "slave iface=" << soc->sta_iface
                           << " is not of the preferred backhaul band";
                continue;
            }

            std::string iface = soc->sta_iface;
            pending_slave_sta_ifaces.insert(iface);

            if (!soc->sta_wlan_hal->initiate_scan()) {
                LOG(ERROR) << "initiate_scan for iface " << iface << " failed!";
                platform_notify_error(bpl::eErrorCode::BH_SCAN_FAILED_TO_INITIATE_SCAN,
                                      "iface='" + iface + "'");
                success = false;
                break;
            }
            scan_triggered = true;
            LOG(INFO) << "wait for scan results on iface " << iface;
        }

        if (!success || !scan_triggered) {
            LOG_IF(!scan_triggered, DEBUG) << "no sta hal is available for scan";
            FSM_MOVE_STATE(RESTART);
        } else {
            FSM_MOVE_STATE(WAIT_FOR_SCAN_RESULTS);
            skip_select              = true;
            state_time_stamp_timeout = std::chrono::steady_clock::now() +
                                       std::chrono::seconds(WAIT_FOR_SCAN_RESULTS_TIMEOUT_SECONDS);
        }
        break;
    }
    case EState::WAIT_FOR_SCAN_RESULTS: {
        if (std::chrono::steady_clock::now() > state_time_stamp_timeout) {
            LOG(DEBUG) << "scan timed out";
            platform_notify_error(bpl::eErrorCode::BH_SCAN_TIMEOUT,
                                  "SSID='" + m_sConfig.ssid + "'");

            state_attempts++;
            FSM_MOVE_STATE(INITIATE_SCAN);
            break;
        }

        skip_select = false;
        break;
    }
    case EState::WIRELESS_CONFIG_4ADDR_MODE: {

        // Disconnect is necessary before changing 4addr mode, to make sure wpa_supplicant is not using the iface
        if (hidden_ssid) {
            for (auto soc : slaves_sockets) {
                if (!soc->sta_wlan_hal || soc->sta_iface.empty())
                    continue;
                std::string iface = soc->sta_iface;
                soc->sta_wlan_hal->disconnect();
                soc->sta_wlan_hal->set_4addr_mode(true);
            }
        } else {
            auto active_hal = get_wireless_hal();
            active_hal->disconnect();
            active_hal->set_4addr_mode(true);
        }
        FSM_MOVE_STATE(WIRELESS_ASSOCIATE_4ADDR);
        skip_select = true;
        break;
    }
    case EState::WIRELESS_ASSOCIATE_4ADDR: {

        // Get the HAL for the connected interface
        auto active_hal = get_wireless_hal();

        if (roam_flag) {
            selected_bssid         = roam_selected_bssid;
            selected_bssid_channel = roam_selected_bssid_channel;
            if (!active_hal->roam(tlvf::mac_from_string(selected_bssid), selected_bssid_channel)) {
                platform_notify_error(bpl::eErrorCode::BH_ROAMING,
                                      "BSSID='" + selected_bssid + "'");
                stop_on_failure_attempts--;
                FSM_MOVE_STATE(RESTART);
                break;
            }
        }

        auto db = AgentDB::get();

        if (hidden_ssid) {
            std::string iface;

            std::shared_ptr<bwl::sta_wlan_hal> selected_hal;
            for (auto it = pending_slave_sta_ifaces.cbegin();
                 it != pending_slave_sta_ifaces.end();) {
                iface          = *it;
                auto iface_hal = get_wireless_hal(iface);

                if (!iface_hal) {
                    LOG(ERROR) << "Slave for iface " << iface << " not found!";
                    break;
                }

                iface_hal->refresh_radio_info();

                if (son::wireless_utils::which_freq(iface_hal->get_radio_info().channel) ==
                        beerocks::FREQ_24G &&
                    pending_slave_sta_ifaces.size() > 1) {
                    ++it;
                    LOG(DEBUG) << "skipping 2.4GHz iface " << iface
                               << " while other ifaces are available";
                    continue;
                }

                it = pending_slave_sta_ifaces.erase(it);
                break;
            }

            db->backhaul.selected_iface_name = iface;
            active_hal                       = get_wireless_hal();
        }

        if (active_hal->connect(m_sConfig.ssid, m_sConfig.pass, m_sConfig.security_type,
                                m_sConfig.mem_only_psk, selected_bssid, selected_bssid_channel,
                                hidden_ssid)) {
            LOG(DEBUG) << "successful call to active_hal->connect(), bssid=" << selected_bssid
                       << ", channel=" << selected_bssid_channel
                       << ", iface=" << db->backhaul.selected_iface_name;
        } else {
            LOG(ERROR) << "connect command failed for iface " << db->backhaul.selected_iface_name;
            FSM_MOVE_STATE(INITIATE_SCAN);
            break;
        }

        FSM_MOVE_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT);
        state_attempts           = 0;
        skip_select              = true;
        state_time_stamp_timeout = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(MAX_WIRELESS_ASSOCIATE_TIMEOUT_SECONDS);
        break;
    }
    case EState::WIRELESS_ASSOCIATE_4ADDR_WAIT: {

        auto db  = AgentDB::get();
        auto now = std::chrono::steady_clock::now();
        if (now > state_time_stamp_timeout) {
            LOG(ERROR) << "associate wait timeout";
            if (hidden_ssid) {
                if (pending_slave_sta_ifaces.empty()) {
                    LOG(ERROR) << "hidden ssid association failed for all ifaces";
                    platform_notify_error(
                        bpl::eErrorCode::BH_SCAN_EXCEEDED_MAXIMUM_FAILED_SCAN_ATTEMPTS,
                        "attempts=" + std::to_string(MAX_FAILED_SCAN_ATTEMPTS) + ", SSID='" +
                            m_sConfig.ssid + "'");
                } else {
                    FSM_MOVE_STATE(WIRELESS_ASSOCIATE_4ADDR);
                    break;
                }
            } else {

                if (roam_flag) {
                    FSM_MOVE_STATE(RESTART);
                    roam_flag = false;
                    break;
                }

                stop_on_failure_attempts--;
                platform_notify_error(bpl::eErrorCode::BH_ASSOCIATE_4ADDR_TIMEOUT,
                                      "SSID='" + m_sConfig.ssid + "', iface='" +
                                          db->backhaul.selected_iface_name + "'");

                if (!selected_bssid.empty()) {
                    ap_blacklist_entry &entry = ap_blacklist[selected_bssid];
                    entry.timestamp           = now;
                    entry.attempts++;
                    LOG(DEBUG) << "updating bssid " << selected_bssid
                               << " blacklist entry, attempts=" << entry.attempts;
                }
                roam_flag = false;
            }
            FSM_MOVE_STATE(INITIATE_SCAN);
        }
        break;
    }
    case EState::WIRELESS_WAIT_FOR_RECONNECT: {
        auto now = std::chrono::steady_clock::now();
        if (now > state_time_stamp_timeout) {
            LOG(DEBUG) << "reconnect wait timed out";

            // increment attempts count in blacklist
            if (!selected_bssid.empty()) {
                auto &entry     = ap_blacklist[selected_bssid];
                entry.timestamp = now;
                entry.attempts++;
                LOG(DEBUG) << "updating bssid " << selected_bssid
                           << " blacklist entry, attempts=" << entry.attempts
                           << ", max_allowed attempts=" << AP_BLACK_LIST_FAILED_ATTEMPTS_THRESHOLD;
            }

            FSM_MOVE_STATE(INITIATE_SCAN);
        }
        break;
    }
    default: {
        LOG(ERROR) << "backhaul_fsm_wireless() Invalid state: " << int(m_eFSMState);
        return false;
    }
    }
    return (true);
}

bool backhaul_manager::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto uds_header = message_com::get_uds_header(cmdu_rx);

    if (!uds_header) {
        LOG(ERROR) << "uds_header=nullptr";
        return false;
    }

    uint16_t length     = uds_header->length;
    std::string src_mac = tlvf::mac_to_string(uds_header->src_bridge_mac);
    std::string dst_mac = tlvf::mac_to_string(uds_header->dst_bridge_mac);

    auto db = AgentDB::get();

    if (from_broker(sd)) {

        // Filter messages which are not destined to this agent
        if (dst_mac != network_utils::MULTICAST_1905_MAC_ADDR &&
            dst_mac != tlvf::mac_to_string(db->bridge.mac)) {
            LOG(DEBUG) << "handle_cmdu() - dropping msg, dst_mac=" << dst_mac
                       << ", local_bridge_mac=" << db->bridge.mac;
            return true;
        }

        // TODO: Add optimization of PID filtering for cases like the following:
        // 1. If VS message was sent by Controllers local agent to the controller, it is looped back.
        // 2. If IRE is sending message to the Controller of the Controller, it will be received in
        //    Controllers backhaul manager as well, and should ignored.

        // Handle the CMDU message. If the message was processed locally
        // (by the Backhaul Manager), this function will return 'true'.
        // Otherwise, it should be forwarded to the slaves.

        // the destination slave is used to forward the cmdu
        // only to the desired slave.
        // handle_1905_1_message has the opportunity to set it
        // to a speficic slave. In this case the cmdu is forward only
        // to this slave. when dest_slave is left as nullptr
        // the cmdu is forwarded to all slaves
        Socket *dest_slave = nullptr;
        if (handle_1905_1_message(cmdu_rx, src_mac, dest_slave)) {
            //function returns true if message doesn't need to be forwarded
            return true;
        }

        ////////// If got here, message needs to be forwarded //////////

        // Message from controller (bus) to agent (uds)
        // Send the data (uds_header + cmdu) how it is on UDS, without changing it

        cmdu_rx.swap(); // swap back before forwarding

        if (dest_slave) {
            // Forward only to the desired destination
            if (!message_com::forward_cmdu_to_uds(dest_slave, cmdu_rx, length)) {
                LOG(ERROR) << "forward_cmdu_to_uds() failed - " << print_cmdu_types(uds_header)
                           << " sd=" << intptr_t(dest_slave);
            }
        } else {
            // Forward cmdu to all slaves how it is on UDS, without changing it
            for (auto soc_iter : slaves_sockets) {
                if (!message_com::forward_cmdu_to_uds(soc_iter->slave, cmdu_rx, length)) {
                    LOG(ERROR) << "forward_cmdu_to_uds() failed - " << print_cmdu_types(uds_header)
                               << " sd=" << intptr_t(soc_iter->slave);
                }
            }
        }
    } else { // from uds to bus or local handling (ACTION_BACKHAUL)

        // Check for local handling
        if (dst_mac == network_utils::ZERO_MAC_STRING) {
            std::shared_ptr<sRadioInfo> soc;
            for (auto soc_iter : slaves_sockets) {
                if (soc_iter->slave == sd) {
                    soc = soc_iter;
                    break;
                }
            }

            if (soc && soc->slave == sd) {
                if (cmdu_rx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
                    return handle_slave_backhaul_message(soc, cmdu_rx);
                } else {
                    return handle_slave_1905_1_message(cmdu_rx, src_mac);
                }
            } else {
                LOG(ERROR) << "ACTION_BACKHAUL from none slave socket!";
                return false;
            }

        } else { // Forward the data (cmdu) to bus
            // LOG(DEBUG) << "forwarding slave->master message, controller_bridge_mac=" << controller_bridge_mac;
            cmdu_rx.swap(); //swap back before forwarding
            send_cmdu_to_broker(cmdu_rx, dst_mac, tlvf::mac_to_string(db->bridge.mac), length);
        }
    }

    return true;
}

bool backhaul_manager::handle_slave_backhaul_message(std::shared_ptr<sRadioInfo> soc,
                                                     ieee1905_1::CmduMessageRx &cmdu_rx)
{
    // Validate Socket
    if (!soc) {
        LOG(ERROR) << "slave socket is nullptr!";
        return false;
    }

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(WARNING) << "Not a beerocks vendor specific message";
        return true;
    }

    // Validate BACKHAUL action
    if (beerocks_header->action() != beerocks_message::ACTION_BACKHAUL) {
        LOG(ERROR) << "Invalid message action received: action=" << int(beerocks_header->action())
                   << ", action_op=" << int(beerocks_header->action_op());
        return false;
    }

    // Handle messages
    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_BACKHAUL_REGISTER_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_BACKHAUL_REGISTER_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BACKHAUL_REGISTER_REQUEST failed";
            return false;
        }

        auto db = AgentDB::get();

        soc->sta_iface.assign(request->sta_iface(message::IFACE_NAME_LENGTH));
        soc->hostap_iface.assign(request->hostap_iface(message::IFACE_NAME_LENGTH));
        // Create a local copy on this process database instance. Will be removed on PPM-83 phase 5
        db->add_radio(request->hostap_iface(), request->sta_iface());
        soc->sta_iface_filter_low = request->sta_iface_filter_low();
        onboarding                = request->onboarding();

        // Add the slave socket to the backhaul configuration
        m_sConfig.slave_iface_socket[soc->sta_iface] = soc;

        if (!m_agent_ucc_listener && request->certification_mode() &&
            m_sConfig.ucc_listener_port != 0 && !db->device_conf.local_controller) {
            m_agent_ucc_listener = std::make_unique<agent_ucc_listener>(
                *this, m_sConfig.ucc_listener_port, m_sConfig.vendor, m_sConfig.model,
                db->bridge.iface_name, cert_cmdu_tx);
            if (m_agent_ucc_listener && !m_agent_ucc_listener->start("ucc_listener")) {
                LOG(ERROR) << "failed start agent_ucc_listener";
                return false;
            }
        }

        LOG(DEBUG) << "ACTION_BACKHAUL_REGISTER_REQUEST sta_iface=" << soc->sta_iface
                   << " hostap_iface=" << soc->hostap_iface;

        auto register_response =
            message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_REGISTER_RESPONSE>(
                cmdu_tx);

        if (register_response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        message_com::send_cmdu(soc->slave, cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_BACKHAUL_ENABLE: {

        auto request = beerocks_header->addClass<beerocks_message::cACTION_BACKHAUL_ENABLE>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BACKHAUL_ENABLE failed";
            return false;
        }

        auto db    = AgentDB::get();
        auto radio = db->radio(soc->hostap_iface);
        if (!radio) {
            LOG(DEBUG) << "Radio of iface " << soc->hostap_iface << " does not exist on the db";
            return false;
        }

        auto tuple_preferred_channels = request->preferred_channels(0);
        if (!std::get<0>(tuple_preferred_channels)) {
            LOG(ERROR) << "access to supported channels list failed!";
            return false;
        }

        auto channels = &std::get<1>(tuple_preferred_channels);

        std::copy_n(channels, request->preferred_channels_size(), soc->preferred_channels.begin());

        soc->radio_mac = request->iface_mac();
        // Create a local copy on this process database instance. Will be removed on PPM-83 phase 5
        radio->front.iface_mac = soc->radio_mac;

        soc->freq_type = request->frequency_band();
        // Create a local copy on this process database instance. Will be removed on PPM-83 phase 5
        radio->front.freq_type     = request->frequency_band();
        soc->controller_discovered = false;
        soc->max_bandwidth         = request->max_bandwidth();
        // Create a local copy on this process database instance. Will be removed on PPM-83 phase 5
        radio->front.max_supported_bw = request->max_bandwidth();

        soc->ht_supported  = request->ht_supported();
        soc->ht_capability = request->ht_capability();
        std::copy_n(request->ht_mcs_set(), soc->ht_mcs_set.size(), soc->ht_mcs_set.begin());
        soc->vht_supported  = request->vht_supported();
        soc->vht_capability = request->vht_capability();
        std::copy_n(request->vht_mcs_set(), soc->vht_mcs_set.size(), soc->vht_mcs_set.begin());

        LOG(DEBUG) << "ACTION_BACKHAUL_ENABLE hostap_iface=" << soc->hostap_iface
                   << " sta_iface=" << soc->sta_iface << " band=" << int(request->frequency_band());

        if (m_eFSMState >= EState::CONNECT_TO_MASTER) {
            LOG(INFO) << "Sending topology notification on reconnected son_slave";
            auto cmdu_header =
                cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
            if (!cmdu_header) {
                LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
                return false;
            }
            send_cmdu_to_broker(cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                tlvf::mac_to_string(db->bridge.mac));
        }

        // If we're already connected, send a notification to the slave
        if (FSM_IS_IN_STATE(OPERATIONAL)) {
            send_autoconfig_search_message(radio->front.iface_name);
        } else if (pending_enable) {
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_BUSY_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building cACTION_BACKHAUL_BUSY_NOTIFICATION message!";
                break;
            }
            message_com::send_cmdu(soc->slave, cmdu_tx);
        } else {
            pending_slave_ifaces.erase(soc->hostap_iface);

            if (pending_slave_ifaces.empty()) {

                LOG(DEBUG) << "All pending slaves have sent us backhaul enable!";

                /* All pending slaves have sent us backhaul enable
                     * which means we can proceed to the scan->connect->operational flow
                     */
                pending_enable = true;

                if (db->device_conf.local_gw) {
                    LOG(DEBUG) << "All slaves ready, proceeding, local GW, Bridge: "
                               << db->bridge.iface_name;
                } else {

                    m_sConfig.preferred_bssid = tlvf::mac_to_string(request->preferred_bssid());
                    m_sConfig.ssid.assign(request->ssid(message::WIFI_SSID_MAX_LENGTH));
                    m_sConfig.pass.assign(request->pass(message::WIFI_PASS_MAX_LENGTH));
                    m_sConfig.security_type = static_cast<bwl::WiFiSec>(request->security_type());
                    m_sConfig.mem_only_psk  = request->mem_only_psk();
                    if (request->backhaul_preferred_radio_band() ==
                        beerocks::eFreqType::FREQ_UNKNOWN) {
                        LOG(DEBUG) << "Unknown backhaul preferred radio band, setting to auto";
                        m_sConfig.backhaul_preferred_radio_band = beerocks::eFreqType::FREQ_AUTO;
                    } else {
                        m_sConfig.backhaul_preferred_radio_band =
                            (beerocks::eFreqType)request->backhaul_preferred_radio_band();
                    }

                    // Change mixed state to WPA2
                    if (m_sConfig.security_type == bwl::WiFiSec::WPA_WPA2_PSK) {
                        m_sConfig.security_type = bwl::WiFiSec::WPA2_PSK;
                    }
                    db->ethernet.iface_name.assign(request->wire_iface(message::IFACE_NAME_LENGTH));
                    m_sConfig.wire_iface_type = (beerocks::eIfaceType)request->wire_iface_type();

                    LOG(DEBUG) << "All slaves ready, proceeding" << std::endl
                               << "SSID: " << m_sConfig.ssid << ", Pass: ****"
                               << ", Security: " << int(m_sConfig.security_type)
                               << ", Bridge: " << db->bridge.iface_name
                               << ", Wired: " << db->ethernet.iface_name;
                }
            }
        }
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_ROAM_REQUEST: {
        auto request = beerocks_header->addClass<beerocks_message::cACTION_BACKHAUL_ROAM_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass ACTION_BACKHAUL_ROAM_REQUEST failed";
            return false;
        }
        roam_selected_bssid_channel = request->params().channel;
        roam_selected_bssid         = tlvf::mac_to_string(request->params().bssid);
        roam_flag                   = true;
        LOG(DEBUG) << "ACTION_BACKHAUL_ROAM_REQUEST to bssid=" << roam_selected_bssid
                   << " on channel=" << int(roam_selected_bssid_channel);

        /*
        * NOTE: Why moving to RESTART state? It causing BACKHAUL DISCONNECTED to the son slaves,
        * which in the past (when we had TCP socket to the controller instead of the bus)
        * disconnected the socket, but now when we have bus it is not happening anyway.
        * Another thing it is causing is to re-scan the for networks. Is it really necessary?
        * Can we just move immediately to WIRELESS_ASSOCIATE_4ADDR, and then when successfully
        * connected, notify the agent about the ne bssid?
        */
        FSM_MOVE_STATE(RESTART);
        break;
    }

    case beerocks_message::ACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST: {
        LOG(DEBUG) << "ACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST received from iface "
                   << soc->sta_iface;
        auto request_in = beerocks_header->addClass<
            beerocks_message::cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST>();
        if (!request_in) {
            LOG(ERROR)
                << "addClass cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST failed";
            return false;
        }
        configuration_stop_on_failure_attempts = request_in->attempts();
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_RESET: {
        LOG(DEBUG) << "ACTION_CONTROL_BACKHAUL_RESET";
        FSM_MOVE_STATE(RESTART);
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST: {
        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST";

        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST failed";
            return false;
        }
        std::string sta_mac = tlvf::mac_to_string(request->params().mac);
        bool ap_busy        = false;
        bool bwl_error      = false;
        if (unassociated_rssi_measurement_header_id == -1) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building "
                              "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE message!";
                break;
            }
            response->mac() = tlvf::mac_from_string(sta_mac);
            message_com::send_cmdu(soc->slave, cmdu_tx);
            LOG(DEBUG) << "send ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE, sta_mac = "
                       << sta_mac;
            int bandwidth = beerocks::utils::convert_bandwidth_to_int(
                (beerocks::eWiFiBandwidth)request->params().bandwidth);
            if (get_wireless_hal()->unassoc_rssi_measurement(
                    sta_mac, request->params().channel, bandwidth,
                    request->params().vht_center_frequency, request->params().measurement_delay,
                    request->params().mon_ping_burst_pkt_num)) {
                unassociated_measurement_slave_soc = soc->slave;
            } else {
                bwl_error = true;
                LOG(ERROR) << "unassociated_sta_rssi_measurement failed!";
            }

            unassociated_rssi_measurement_header_id = beerocks_header->id();
            LOG(DEBUG) << "CLIENT_RX_RSSI_MEASUREMENT_REQUEST, mac = " << sta_mac
                       << " channel = " << int(request->params().channel) << " bandwidth="
                       << beerocks::utils::convert_bandwidth_to_int(
                              (beerocks::eWiFiBandwidth)request->params().bandwidth);
        } else {
            ap_busy = true;
            LOG(WARNING)
                << "busy!, send response to retry CLIENT_RX_RSSI_MEASUREMENT_REQUEST, mac = "
                << sta_mac;
        }

        if (ap_busy || bwl_error) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE "
                              "message!";
                break;
            }
            response->params().result.mac = request->params().mac;
            response->params().rx_rssi    = beerocks::RSSI_INVALID;
            response->params().rx_snr     = beerocks::SNR_INVALID;
            response->params().rx_packets = -1;
            message_com::send_cmdu(soc->slave, cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION received from iface "
                   << soc->hostap_iface;
        auto msg = beerocks_header->addClass<
            beerocks_message::cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>();
        if (!msg) {
            LOG(ERROR) << "Failed parsing BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION message!";
            return false;
        }

        // Create a local copy on this process database instance. Will be removed on PPM-83 phase 5
        auto db    = AgentDB::get();
        auto radio = db->radio(soc->hostap_iface);
        if (!radio) {
            LOG(DEBUG) << "Radio of iface " << soc->hostap_iface << " does not exist on the db";
            return false;
        }
        for (uint8_t vap_idx = 0; vap_idx < eBeeRocksIfaceIds::IFACE_TOTAL_VAPS; vap_idx++) {
            radio->front.bssids[vap_idx].mac  = msg->params().vaps[vap_idx].mac;
            radio->front.bssids[vap_idx].ssid = msg->params().vaps[vap_idx].ssid;
            radio->front.bssids[vap_idx].type = msg->params().vaps[vap_idx].backhaul_vap
                                                    ? AgentDB::sRadio::sFront::sBssid::eType::bAP
                                                    : AgentDB::sRadio::sFront::sBssid::eType::fAP;
        }
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION";
        auto msg =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION>();
        if (!msg) {
            LOG(ERROR) << "Failed building ACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION message!";
            break;
        }

        // Remove this client from other radios.
        auto db = AgentDB::get();
        db->erase_client(msg->client_mac());

        // Set client association information for associated client
        auto radio = db->get_radio_by_mac(msg->bssid(), AgentDB::eMacType::BSSID);
        if (!radio) {
            LOG(DEBUG) << "Radio containing bssid " << msg->bssid() << " not found";
            break;
        }

        radio->associated_clients.emplace(msg->client_mac(),
                                          AgentDB::sRadio::sClient{msg->bssid(),
                                                                   msg->association_frame_length(),
                                                                   msg->association_frame()});

        break;
    }
    case beerocks_message::ACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION";
        auto msg =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION>();
        if (!msg) {
            LOG(ERROR)
                << "Failed building ACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION message!";
            return false;
        }

        // If exists, remove client association information for disconnected client.
        auto db = AgentDB::get();
        db->erase_client(msg->client_mac(), msg->bssid());
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE: {
        auto response_in = beerocks_header->addClass<
            beerocks_message::cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE>();
        if (!response_in) {
            LOG(ERROR) << "addClass ACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE failed";
            return false;
        }

        auto mid = beerocks_header->id();

        if (!cmdu_tx.create(
                mid, ieee1905_1::eMessageType::ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE)) {
            LOG(ERROR) << "cmdu creation of type ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE "
                          "has failed";
            return false;
        }

        auto response_out = cmdu_tx.addClass<wfa_map::tlvAssociatedStaLinkMetrics>();

        if (!response_out->alloc_bssid_info_list(response_in->bssid_info_list_length())) {
            LOG(ERROR) << "alloc_per_bss_sta_link_metrics failed";
            return false;
        }

        response_out->sta_mac() = response_in->sta_mac();

        auto db = AgentDB::get();

        for (size_t i = 0; i < response_out->bssid_info_list_length(); ++i) {
            auto &bss_in  = std::get<1>(response_in->bssid_info_list(i));
            auto &bss_out = std::get<1>(response_out->bssid_info_list(i));

            auto &client_mac = response_out->sta_mac();

            auto radio = db->get_radio_by_mac(client_mac, AgentDB::eMacType::CLIENT);
            if (!radio) {
                LOG(ERROR) << "radio for client mac " << client_mac << " not found";
                return false;
            }

            // If get_radio_by_mac() found the radio, it means that 'client_mac' is on the radio
            // 'associated_clients' list.
            bss_out.bssid = radio->associated_clients.at(client_mac).bssid;
            if (bss_out.bssid == network_utils::ZERO_MAC) {
                LOG(ERROR) << "bssid is ZERO_MAC";
                return false;
            }

            bss_out.earliest_measurement_delta = bss_in.earliest_measurement_delta;
            bss_out.downlink_estimated_mac_data_rate_mbps =
                bss_in.downlink_estimated_mac_data_rate_mbps;
            bss_out.uplink_estimated_mac_data_rate_mbps =
                bss_in.uplink_estimated_mac_data_rate_mbps;
            bss_out.sta_measured_uplink_rssi_dbm_enc = bss_in.sta_measured_uplink_rssi_dbm_enc;
        }

        LOG(DEBUG) << "Send AssociatedStaLinkMetrics to controller, mid = " << mid;
        send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, tlvf::mac_to_string(db->bridge.mac));
        break;
    }
    default: {
        bool handled = m_task_pool.handle_cmdu(cmdu_rx, sMacAddr(), beerocks_header);
        if (!handled) {
            LOG(ERROR) << "Unhandled message received from the Controller: "
                       << int(beerocks_header->action_op());
            return false;
        }
        return true;
    }
    }

    return true;
}

bool backhaul_manager::handle_1905_1_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                                             const std::string &src_mac, Socket *&forward_to)
{
    /*
     * return values:
     * true if the message was handled by the backhaul manager
     * false if the message needs to be forwarded by the calling function
     */
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::TOPOLOGY_DISCOVERY_MESSAGE:
        return handle_1905_topology_discovery(src_mac, cmdu_rx);
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE: {
        return handle_1905_autoconfiguration_response(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE: {
        if (src_mac != controller_bridge_mac) {
            LOG(INFO) << "current controller_bridge_mac=" << controller_bridge_mac
                      << " but renew came from src_mac=" << src_mac << ", ignoring";
            return true;
        }
        // According to IEEE 1905.1, there should be a separate renew per frequency band. However,
        // Multi-AP overrides this and says that all radios have to restart WSC when a renew is
        // received. The actual handling is done in the slaves, so forward it to the slaves by
        // returning false.
        return false;
    }
    case ieee1905_1::eMessageType::TOPOLOGY_QUERY_MESSAGE: {
        return handle_1905_topology_query(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::AP_CAPABILITY_QUERY_MESSAGE: {
        return handle_ap_capability_query(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::HIGHER_LAYER_DATA_MESSAGE: {
        return handle_1905_higher_layer_data_message(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::LINK_METRIC_QUERY_MESSAGE: {
        return handle_1905_link_metric_query(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::COMBINED_INFRASTRUCTURE_METRICS_MESSAGE: {
        return handle_1905_combined_infrastructure_metrics(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::CLIENT_CAPABILITY_QUERY_MESSAGE: {
        return handle_client_capability_query(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::ASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE: {
        return handle_associated_sta_link_metrics_query(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE: {
        return handle_multi_ap_policy_config_request(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::AP_METRICS_QUERY_MESSAGE: {
        return handle_ap_metrics_query(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::BEACON_METRICS_QUERY_MESSAGE: {
        return handle_1905_beacon_metrics_query(cmdu_rx, src_mac, forward_to);
    }
    case ieee1905_1::eMessageType::CHANNEL_SELECTION_REQUEST_MESSAGE: {
        return handle_channel_selection_request(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::BACKHAUL_STEERING_REQUEST_MESSAGE: {
        return handle_backhaul_steering_request(cmdu_rx, src_mac);
    }
    default: {
        // TODO add a warning once all vendor specific flows are replaced with EasyMesh
        // flows, since we won't expect a 1905 message not handled in this function
        return m_task_pool.handle_cmdu(cmdu_rx, tlvf::mac_from_string(src_mac));
    }
    }
}

bool backhaul_manager::handle_slave_1905_1_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                   const std::string &src_mac)
{
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::AP_METRICS_RESPONSE_MESSAGE: {
        return handle_slave_ap_metrics_response(cmdu_rx, src_mac);
    }
    case ieee1905_1::eMessageType::CHANNEL_SELECTION_RESPONSE_MESSAGE: {
        return handle_slave_channel_selection_response(cmdu_rx, src_mac);
    }
    default: {
        bool handled = m_task_pool.handle_cmdu(cmdu_rx, tlvf::mac_from_string(src_mac));
        if (!handled) {
            LOG(DEBUG) << "Unexpected 1905 message " << int(cmdu_rx.getMessageType());
            return false;
        }
        return true;
    }
    }
}

std::shared_ptr<backhaul_manager::sRadioInfo>
backhaul_manager::get_radio(const sMacAddr &radio_mac) const
{
    auto it = std::find_if(slaves_sockets.begin(), slaves_sockets.end(),
                           [&radio_mac](const std::shared_ptr<sRadioInfo> &radio_info) {
                               return radio_info->radio_mac == radio_mac;
                           });
    return it != slaves_sockets.end() ? *it : nullptr;
}

bool backhaul_manager::handle_multi_ap_policy_config_request(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                             const std::string &src_mac)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE, mid=" << std::hex << int(mid);

    auto steering_policy_tlv = cmdu_rx.getClass<wfa_map::tlvSteeringPolicy>();
    if (steering_policy_tlv) {
        // For the time being, agent doesn't do steering so steering policy is ignored.
    }

    auto db = AgentDB::get();

    auto metric_reporting_policy_tlv = cmdu_rx.getClass<wfa_map::tlvMetricReportingPolicy>();
    if (metric_reporting_policy_tlv) {
        /**
         * The Multi-AP Policy Config Request message containing a Metric Reporting Policy TLV is
         * sent by the controller and received by the backhaul manager.
         * The backhaul manager forwards the request message "as is" to all the slaves managing the
         * radios which Radio Unique Identifier has been specified.
         */
        for (size_t i = 0; i < metric_reporting_policy_tlv->metrics_reporting_conf_list_length();
             i++) {
            auto tuple = metric_reporting_policy_tlv->metrics_reporting_conf_list(i);
            if (!std::get<0>(tuple)) {
                LOG(ERROR) << "Failed to get metrics_reporting_conf[" << i
                           << "] from TLV_METRIC_REPORTING_POLICY";
                return false;
            }

            auto metrics_reporting_conf = std::get<1>(tuple);

            std::shared_ptr<sRadioInfo> radio = get_radio(metrics_reporting_conf.radio_uid);
            if (radio) {
                uint16_t length = message_com::get_uds_header(cmdu_rx)->length;
                cmdu_rx.swap(); // swap back before forwarding
                if (!message_com::forward_cmdu_to_uds(radio->slave, cmdu_rx, length)) {
                    LOG(ERROR) << "Failed to forward message to slave " << radio->radio_mac;
                }
                cmdu_rx.swap(); // swap back to normal after forwarding, for next iteration
            } else {
                LOG(INFO) << "Radio Unique Identifier " << metrics_reporting_conf.radio_uid
                          << " not found";
            }
        }

        /**
         * The AP Metrics Reporting Interval field indicates if periodic AP metrics reporting is
         * to be enabled, and if so the cadence.
         *
         * Store configured interval value and restart the timer.
         *
         * Reporting interval value works just for enabling/disabling auto sending AP Metrics Response,
         * which will be send every 500 ms.
         */
        ap_metrics_reporting_info.reporting_interval_s =
            metric_reporting_policy_tlv->metrics_reporting_interval_sec();
        ap_metrics_reporting_info.last_reporting_time_point = std::chrono::steady_clock::now();
    }

    // send ACK_MESSAGE back to the controller
    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_associated_sta_link_metrics_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                                const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received ASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE , mid=" << std::dec
               << int(mid);

    if (!cmdu_tx.create(mid,
                        ieee1905_1::eMessageType::ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE)) {
        LOG(ERROR)
            << "cmdu creation of type ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE, has failed";
        return false;
    }

    auto mac = cmdu_rx.getClass<wfa_map::tlvStaMacAddressType>();
    if (!mac) {
        LOG(ERROR) << "Failed to get mac address";
        return false;
    }

    auto assoc_link_metrics = cmdu_tx.addClass<wfa_map::tlvAssociatedStaLinkMetrics>();
    if (!assoc_link_metrics) {
        LOG(ERROR) << "Failed to create tlvAssociatedStaLinkMetrics tlv";
        return false;
    }

    auto db = AgentDB::get();

    // Check if it is an error scenario - if the STA specified in the STA link Query message is not associated
    // with any of the BSS operated by the Multi-AP Agent
    auto radio = db->get_radio_by_mac(mac->sta_mac(), AgentDB::eMacType::CLIENT);
    if (!radio) {
        LOG(ERROR) << "client with mac address " << mac->sta_mac() << " not found";
        //Add an Error Code TLV
        auto error_code_tlv = cmdu_tx.addClass<wfa_map::tlvErrorCode>();
        if (!error_code_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvErrorCode has failed";
            return false;
        }
        error_code_tlv->reason_code() =
            wfa_map::tlvErrorCode::STA_NOT_ASSOCIATED_WITH_ANY_BSS_OPERATED_BY_THE_AGENT;
        error_code_tlv->sta_mac() = mac->sta_mac();

        LOG(DEBUG) << "Send a ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE back to controller";
        return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
    }
    auto client_it = radio->associated_clients.find(mac->sta_mac());
    if (client_it == radio->associated_clients.end()) {
        LOG(ERROR) << "Cannot find sta sta " << mac->sta_mac();
        return false;
    }
    if (client_it->second.bssid == network_utils::ZERO_MAC) {
        LOG(ERROR) << "Cannot find sta bssid";
        return false;
    }
    LOG(DEBUG) << "Client with mac address " << mac->sta_mac() << " connected to "
               << client_it->second.bssid;

    auto request_out = message_com::create_vs_message<
        beerocks_message::cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST>(cmdu_tx, mid);

    if (!request_out) {
        LOG(ERROR) << "Failed to build ACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST";
        return false;
    }

    request_out->sync()    = true;
    request_out->sta_mac() = mac->sta_mac();

    auto radio_info = get_radio(radio->front.iface_mac);
    if (!radio_info) {
        LOG(ERROR) << "Failed to get radio info for " << radio->front.iface_mac;
        return false;
    }
    return message_com::send_cmdu(radio_info->slave, cmdu_tx);
}

bool backhaul_manager::handle_client_capability_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                      const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CLIENT_CAPABILITY_QUERY_MESSAGE , mid=" << std::dec << int(mid);

    auto client_info_tlv_r = cmdu_rx.getClass<wfa_map::tlvClientInfo>();
    if (!client_info_tlv_r) {
        LOG(ERROR) << "getClass wfa_map::tlvClientInfo failed";
        return false;
    }

    // send CLIENT_CAPABILITY_REPORT_MESSAGE back to the controller
    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::CLIENT_CAPABILITY_REPORT_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type CLIENT_CAPABILITY_REPORT_MESSAGE, has failed";
        return false;
    }

    auto client_info_tlv_t = cmdu_tx.addClass<wfa_map::tlvClientInfo>();
    if (!client_info_tlv_t) {
        LOG(ERROR) << "addClass wfa_map::tlvClientInfo has failed";
        return false;
    }
    client_info_tlv_t->bssid()      = client_info_tlv_r->bssid();
    client_info_tlv_t->client_mac() = client_info_tlv_r->client_mac();

    auto client_capability_report_tlv = cmdu_tx.addClass<wfa_map::tlvClientCapabilityReport>();
    if (!client_capability_report_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvClientCapabilityReport has failed";
        return false;
    }

    auto db = AgentDB::get();

    // Check if it is an error scenario - if the STA specified in the Client Capability Query
    // message is not associated with any of the BSS operated by the Multi-AP Agent [ though the
    // TLV does contain a BSSID, the specification says that we should answer if the client is
    // associated with any BSS on this agent.]
    auto radio = db->get_radio_by_mac(client_info_tlv_r->client_mac(), AgentDB::eMacType::CLIENT);
    if (!radio) {
        LOG(ERROR) << "radio for client mac " << client_info_tlv_r->client_mac() << " not found";

        // If it is an error scenario, set Success status to 0x01 = Failure and do nothing after it.
        client_capability_report_tlv->result_code() = wfa_map::tlvClientCapabilityReport::FAILURE;

        LOG(DEBUG) << "Result Code: FAILURE";
        LOG(DEBUG) << "STA specified in the Client Capability Query message is not associated with "
                      "any of the BSS operated by the Multi-AP Agent ";
        // Add an Error Code TLV
        auto error_code_tlv = cmdu_tx.addClass<wfa_map::tlvErrorCode>();
        if (!error_code_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvErrorCode has failed";
            return false;
        }
        error_code_tlv->reason_code() =
            wfa_map::tlvErrorCode::STA_NOT_ASSOCIATED_WITH_ANY_BSS_OPERATED_BY_THE_AGENT;
        error_code_tlv->sta_mac() = client_info_tlv_r->client_mac();
        return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
    }

    client_capability_report_tlv->result_code() = wfa_map::tlvClientCapabilityReport::SUCCESS;
    LOG(DEBUG) << "Result Code: SUCCESS";

    // Add frame body of the most recently received (Re)Association Request frame from this client.
    auto &client_info = radio->associated_clients.at(client_info_tlv_r->client_mac());
    client_capability_report_tlv->set_association_frame(client_info.association_frame.data(),
                                                        client_info.association_frame_length);

    LOG(DEBUG) << "Send a CLIENT_CAPABILITY_REPORT_MESSAGE back to controller";
    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_ap_capability_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                  const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received AP_CAPABILITY_QUERY_MESSAGE, mid=" << std::dec << int(mid);

    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::AP_CAPABILITY_REPORT_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type AP_CAPABILITY_REPORT_MESSAGE, has failed";
        return false;
    }

    auto ap_capability_tlv = cmdu_tx.addClass<wfa_map::tlvApCapability>();
    if (!ap_capability_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvApCapability has failed";
        return false;
    }

    auto db = AgentDB::get();

    // Capability bitmask is set to 0 because neither unassociated STA link metrics
    // reporting or agent-initiated RCPI-based steering are supported

    for (const auto &slave : slaves_sockets) {
        // TODO skip slaves that are not operational
        auto radio_mac          = slave->radio_mac;
        auto preferred_channels = slave->preferred_channels;

        if (!tlvf_utils::add_ap_radio_basic_capabilities(cmdu_tx, radio_mac, preferred_channels)) {
            return false;
        }

        if (!add_ap_ht_capabilities(*slave)) {
            return false;
        }

        if (!add_ap_vht_capabilities(*slave)) {
            return false;
        }

        if (!add_ap_he_capabilities(*slave)) {
            return false;
        }
    }

    LOG(DEBUG) << "Sending AP_CAPABILITY_REPORT_MESSAGE , mid: " << std::hex << (int)mid;
    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_ap_metrics_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                               const std::string &src_mac)
{
    const auto mid           = cmdu_rx.getMessageId();
    auto ap_metric_query_tlv = cmdu_rx.getClass<wfa_map::tlvApMetricQuery>();
    if (!ap_metric_query_tlv) {
        LOG(ERROR) << "AP Metrics Query CMDU mid=" << mid << " does not have AP Metric Query TLV";
        return false;
    }

    std::unordered_set<sMacAddr> bssids;
    for (size_t bssid_idx = 0; bssid_idx < ap_metric_query_tlv->bssid_list_length(); bssid_idx++) {
        auto bssid_tuple = ap_metric_query_tlv->bssid_list(bssid_idx);
        if (!std::get<0>(bssid_tuple)) {
            LOG(ERROR) << "Failed to get bssid " << bssid_idx << " from AP_METRICS_QUERY";
            return false;
        }
        bssids.insert(std::get<1>(bssid_tuple));
        LOG(DEBUG) << "Received AP_METRICS_QUERY_MESSAGE, mid=" << std::hex << int(mid)
                   << "  bssid " << std::get<1>(bssid_tuple);
    }

    if (!send_slave_ap_metric_query_message(mid, bssids)) {
        LOG(ERROR) << "Failed to forward AP_METRICS_RESPONSE to the son_slave_thread";
        return false;
    }

    return true;
}

bool backhaul_manager::handle_slave_ap_metrics_response(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                        const std::string &src_mac)
{
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received AP_METRICS_RESPONSE_MESSAGE, mid=" << std::hex << int(mid);

    auto db = AgentDB::get();

    /**
     * If AP Metrics Response message does not correspond to a previously received and forwarded
     * AP Metrics Query message (which we know because message id is not set), then forward message
     * to controller.
     * This might happen when channel utilization value has crossed configured threshold or when
     * periodic metrics reporting interval has elapsed.
     */
    if (0 == mid) {
        uint16_t length = message_com::get_uds_header(cmdu_rx)->length;
        cmdu_rx.swap(); //swap back before forwarding
        return send_cmdu_to_broker(cmdu_rx, controller_bridge_mac,
                                   tlvf::mac_to_string(db->bridge.mac), length);
    }

    /**
     * When periodic metrics reporting interval has elapsed, we emulate that we have received an
     * AP Metrics Query message from controller. To differentiate real queries from emulated ones,
     * we use a "special" mid value.
     * Note that this design is flaw as a real query might also have this special mid value. This
     * is just a quick and dirty fix to pass 4.7.5 and 4.7.6 for M1
     * TODO: to be fixed as part of #1328
     */
    if (UINT16_MAX == mid) {
        mid = 0;
    }

    auto ap_metrics_tlv = cmdu_rx.getClass<wfa_map::tlvApMetrics>();
    if (!ap_metrics_tlv) {
        LOG(ERROR) << "Failed cmdu_rx.getClass<wfa_map::tlvApMetrics>(), mid=" << std::hex
                   << int(mid);
        return false;
    }

    auto bssid_tlv = ap_metrics_tlv->bssid();
    auto mac       = std::find_if(
        m_ap_metric_query.begin(), m_ap_metric_query.end(),
        [&bssid_tlv](sApMetricsQuery const &query) { return query.bssid == bssid_tlv; });

    if (mac == m_ap_metric_query.end()) {
        LOG(ERROR) << "Failed search in ap_metric_query for bssid: " << bssid_tlv
                   << " from mid=" << std::hex << int(mid);
        return false;
    }

    sApMetrics metric;
    // Copy data to the response vector
    metric.bssid               = ap_metrics_tlv->bssid();
    metric.channel_utilization = ap_metrics_tlv->channel_utilization();
    metric.number_of_stas_currently_associated =
        ap_metrics_tlv->number_of_stas_currently_associated();
    metric.estimated_service_parameters = ap_metrics_tlv->estimated_service_parameters();
    auto info                           = ap_metrics_tlv->estimated_service_info_field();
    for (size_t i = 0; i < ap_metrics_tlv->estimated_service_info_field_length(); i++) {
        metric.estimated_service_info_field.push_back(info[i]);
    }
    std::vector<sStaTrafficStats> traffic_stats_response;

    for (auto &sta_traffic : cmdu_rx.getClassList<wfa_map::tlvAssociatedStaTrafficStats>()) {
        if (!sta_traffic) {
            LOG(ERROR) << "Failed to get class list for tlvAssociatedStaTrafficStats";
            continue;
        }

        traffic_stats_response.push_back(
            {sta_traffic->sta_mac(), sta_traffic->byte_sent(), sta_traffic->byte_recived(),
             sta_traffic->packets_sent(), sta_traffic->packets_recived(),
             sta_traffic->tx_packets_error(), sta_traffic->rx_packets_error(),
             sta_traffic->retransmission_count()});
    }

    std::vector<sStaLinkMetrics> link_metrics_response;
    for (auto &sta_link_metric : cmdu_rx.getClassList<wfa_map::tlvAssociatedStaLinkMetrics>()) {
        if (!sta_link_metric) {
            LOG(ERROR) << "Failed getClassList<wfa_map::tlvAssociatedStaLinkMetrics>";
            continue;
        }
        if (sta_link_metric->bssid_info_list_length() != 1) {
            LOG(ERROR) << "sta_link_metric->bssid_info_list_length() should be equal to 1";
            continue;
        }
        auto response_list = sta_link_metric->bssid_info_list(0);
        link_metrics_response.push_back({sta_link_metric->sta_mac(), std::get<1>(response_list)});
    }

    // Fill a response vector
    m_ap_metric_response.push_back({metric, traffic_stats_response, link_metrics_response});

    // Remove an entry from the processed query
    m_ap_metric_query.erase(
        std::remove_if(m_ap_metric_query.begin(), m_ap_metric_query.end(),
                       [&](sApMetricsQuery const &query) { return mac->bssid == query.bssid; }),
        m_ap_metric_query.end());

    if (!m_ap_metric_query.empty()) {
        return true;
    }

    // We received all responses - prepare and send response message to the controller
    auto cmdu_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::AP_METRICS_RESPONSE_MESSAGE);

    if (!cmdu_header) {
        LOG(ERROR) << "Failed building IEEE1905 AP_METRICS_RESPONSE_MESSAGE";
        return false;
    }

    // Prepare tlvApMetrics for each processed query
    for (const auto &response : m_ap_metric_response) {
        auto ap_metrics_response_tlv = cmdu_tx.addClass<wfa_map::tlvApMetrics>();
        if (!ap_metrics_response_tlv) {
            LOG(ERROR) << "Failed addClass<wfa_map::tlvApMetrics>";
            return false;
        }

        ap_metrics_response_tlv->bssid()               = response.metric.bssid;
        ap_metrics_response_tlv->channel_utilization() = response.metric.channel_utilization;
        ap_metrics_response_tlv->number_of_stas_currently_associated() =
            response.metric.number_of_stas_currently_associated;
        ap_metrics_response_tlv->estimated_service_parameters() =
            response.metric.estimated_service_parameters;
        if (!ap_metrics_response_tlv->alloc_estimated_service_info_field(
                response.metric.estimated_service_info_field.size())) {
            LOG(ERROR) << "Couldn't allocate "
                          "ap_metrics_response_tlv->alloc_estimated_service_info_field";
            return false;
        }
        std::copy_n(response.metric.estimated_service_info_field.begin(),
                    response.metric.estimated_service_info_field.size(),
                    ap_metrics_response_tlv->estimated_service_info_field());

        for (auto &stat : response.sta_traffic_stats) {
            auto sta_traffic_response_tlv =
                cmdu_tx.addClass<wfa_map::tlvAssociatedStaTrafficStats>();

            if (!sta_traffic_response_tlv) {
                LOG(ERROR) << "Failed addClass<wfa_map::tlvAssociatedStaTrafficStats>";
                continue;
            }

            sta_traffic_response_tlv->sta_mac()              = stat.sta_mac;
            sta_traffic_response_tlv->byte_sent()            = stat.byte_sent;
            sta_traffic_response_tlv->byte_recived()         = stat.byte_recived;
            sta_traffic_response_tlv->packets_sent()         = stat.packets_sent;
            sta_traffic_response_tlv->packets_recived()      = stat.packets_recived;
            sta_traffic_response_tlv->tx_packets_error()     = stat.tx_packets_error;
            sta_traffic_response_tlv->rx_packets_error()     = stat.rx_packets_error;
            sta_traffic_response_tlv->retransmission_count() = stat.retransmission_count;
        }

        for (auto &link_metric : response.sta_link_metrics) {
            auto sta_link_metric_response_tlv =
                cmdu_tx.addClass<wfa_map::tlvAssociatedStaLinkMetrics>();

            if (!sta_link_metric_response_tlv) {
                LOG(ERROR) << "Failed addClass<wfa_map::tlvAssociatedStaLinkMetrics>";
                continue;
            }

            sta_link_metric_response_tlv->sta_mac() = link_metric.sta_mac;
            if (!sta_link_metric_response_tlv->alloc_bssid_info_list(1)) {
                LOG(ERROR) << "Failed alloc_bssid_info_list";
                continue;
            }
            auto &sta_link_metric_response =
                std::get<1>(sta_link_metric_response_tlv->bssid_info_list(0));
            sta_link_metric_response = link_metric.bssid_info;
        }
    }

    // Clear the m_ap_metric_response vector after preparing response to the controller
    m_ap_metric_response.clear();

    LOG(DEBUG) << "Sending AP_METRICS_RESPONSE_MESSAGE, mid=" << std::hex << int(mid);
    return send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, tlvf::mac_to_string(db->bridge.mac));
}

/**
 * @brief Handles 1905 Topology Query message
 * @param cmdu_rx Received CMDU (containing Topology Query)
 * @param src_mac MAC address of the message sender
 * @return true on success
 * @return false on failure
 */
bool backhaul_manager::handle_1905_topology_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                  const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received TOPOLOGY_QUERY_MESSAGE , mid=" << std::dec << int(mid);
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::TOPOLOGY_RESPONSE_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "Failed creating topology response header! mid=" << std::hex << (int)mid;
        return false;
    }

    auto db = AgentDB::get();

    auto tlvDeviceInformation = cmdu_tx.addClass<ieee1905_1::tlvDeviceInformation>();
    if (!tlvDeviceInformation) {
        LOG(ERROR) << "addClass ieee1905_1::tlvDeviceInformation failed, mid=" << std::hex
                   << (int)mid;
        return false;
    }

    /**
     * 1905.1 AL MAC address of the device.
     */
    tlvDeviceInformation->mac() = db->bridge.mac;

    /**
     * Set the number of local interfaces and fill info of each of the local interfaces, according
     * to IEEE_1905 section 6.4.5
     */

    /**
     * Add a LocalInterfaceInfo field for the wired interface, if any.
     */
    std::string local_interface_name = db->ethernet.iface_name;
    if (!local_interface_name.empty() &&
        network_utils::linux_iface_is_up_and_running(local_interface_name)) {
        ieee1905_1::eMediaType media_type = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
        if (!get_media_type(local_interface_name, ieee1905_1::eMediaTypeGroup::IEEE_802_3,
                            media_type)) {
            LOG(ERROR) << "Unable to compute media type for interface " << local_interface_name;
            return false;
        }

        std::shared_ptr<ieee1905_1::cLocalInterfaceInfo> localInterfaceInfo =
            tlvDeviceInformation->create_local_interface_list();

        // default to zero mac if get_mac fails.
        std::string wire_iface_mac = network_utils::ZERO_MAC_STRING;
        network_utils::linux_iface_get_mac(local_interface_name, wire_iface_mac);
        localInterfaceInfo->mac()               = tlvf::mac_from_string(wire_iface_mac);
        localInterfaceInfo->media_type()        = media_type;
        localInterfaceInfo->media_info_length() = 0;

        tlvDeviceInformation->add_local_interface_list(localInterfaceInfo);
    }

    /**
     * Add a LocalInterfaceInfo field for each wireless interface.
     */
    for (const auto &soc : slaves_sockets) {
        // Iterate on front radio iface and then switch to back radio iface
        auto fill_radio_iface_info = [&](ieee1905_1::eMediaType media_type, bool front_iface) {
            LOG(DEBUG) << "filling interface information on radio="
                       << (front_iface ? soc->hostap_iface : soc->sta_iface);

            // Skip Backhaul iteration iface when STA BWL is not allocated (Eth connection or GW).
            if (!front_iface && !soc->sta_wlan_hal) {
                LOG(TRACE) << "Skip radio interface with no active STA BWL, front_radio="
                           << soc->hostap_iface;
                return true;
            }

            auto localInterfaceInfo = tlvDeviceInformation->create_local_interface_list();

            localInterfaceInfo->mac() =
                front_iface ? soc->radio_mac
                            : tlvf::mac_from_string(soc->sta_wlan_hal->get_radio_mac());

            LOG(DEBUG) << "Added radio interface to tlvDeviceInformation: "
                       << localInterfaceInfo->mac();

            localInterfaceInfo->media_type() = media_type;

            ieee1905_1::s802_11SpecificInformation media_info = {};
            localInterfaceInfo->alloc_media_info(sizeof(media_info));

            // BSSID field is not defined well for interface. The common definition is in simple
            // words "the AP/ETH mac that we are connected to".
            // For fronthaul radio interface or unused backhaul interface put zero mac.
            if (db->device_conf.local_gw ||
                db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wired ||
                front_iface ||
                (db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless &&
                 soc->sta_iface != db->backhaul.selected_iface_name)) {
                media_info.network_membership = network_utils::ZERO_MAC;
            } else {
                media_info.network_membership =
                    tlvf::mac_from_string(soc->sta_wlan_hal->get_bssid());
            }

            media_info.role =
                front_iface ? ieee1905_1::eRole::AP : ieee1905_1::eRole::NON_AP_NON_PCP_STA;

            // TODO: The Backhaul manager does not hold the information on the front radios.
            // For now, put zeros and when the Agent management will be move to unified Agent thread
            // this field will be filled. #435
            media_info.ap_channel_bandwidth               = 0;
            media_info.ap_channel_center_frequency_index1 = 0;
            media_info.ap_channel_center_frequency_index2 = 0;

            auto *media_info_ptr = localInterfaceInfo->media_info(0);
            if (media_info_ptr == nullptr) {
                LOG(ERROR) << "media_info is nullptr";
                return false;
            }

            std::copy_n(reinterpret_cast<uint8_t *>(&media_info), sizeof(media_info),
                        media_info_ptr);

            tlvDeviceInformation->add_local_interface_list(localInterfaceInfo);

            return true;
        };

        std::string local_interface_name = soc->hostap_iface;

        ieee1905_1::eMediaTypeGroup media_type_group = ieee1905_1::eMediaTypeGroup::IEEE_802_11;
        ieee1905_1::eMediaType media_type            = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
        if (!get_media_type(local_interface_name, media_type_group, media_type)) {
            LOG(ERROR) << "Unable to compute media type for interface " << local_interface_name;
            return false;
        }

        if (!fill_radio_iface_info(media_type, true)) {
            LOG(DEBUG) << "filling interface information on radio=" << soc->hostap_iface
                       << " has failed!";
            return true;
        }

        if (!fill_radio_iface_info(media_type, false)) {
            LOG(DEBUG) << "filling interface information on radio=" << soc->hostap_iface
                       << " backhaul has failed!";
            return true;
        }
    }

    /**
     * Add a 1905.1 neighbor device TLV for each local interface for which this management entity
     * has inferred the presence of a 1905.1 neighbor device. Include each discovered neighbor
     * device in its corresponding 1905.1 neighbor device TLV.
     *
     * First, group known 1905 neighbor devices by the local interface that links to them. Create
     * a map which key is the name of the local interface and the value is the list of neighbor
     * devices inferred from that interface.
     */
    for (auto &neighbors_on_local_iface_entry : db->neighbor_devices) {
        auto tlv1905NeighborDevice = cmdu_tx.addClass<ieee1905_1::tlv1905NeighborDevice>();
        if (!tlv1905NeighborDevice) {
            LOG(ERROR) << "addClass ieee1905_1::tlv1905NeighborDevice failed, mid=" << std::hex
                       << mid;
            return false;
        }

        tlv1905NeighborDevice->mac_local_iface() = neighbors_on_local_iface_entry.first;
        auto &neighbors_on_local_iface           = neighbors_on_local_iface_entry.second;

        if (!tlv1905NeighborDevice->alloc_mac_al_1905_device(neighbors_on_local_iface.size())) {
            LOG(ERROR) << "alloc_mac_al_1905_device() has failed";
            return false;
        }

        size_t index = 0;
        for (const auto &neighbor_on_local_iface_entry : neighbors_on_local_iface) {
            auto &neighbor_al_mac = neighbor_on_local_iface_entry.first;

            auto mac_al_1905_device_tuple = tlv1905NeighborDevice->mac_al_1905_device(index);
            if (!std::get<0>(mac_al_1905_device_tuple)) {
                LOG(ERROR) << "getting mac_al_1905_device element has failed";
                return true;
            }

            auto &mac_al_1905_device = std::get<1>(mac_al_1905_device_tuple);
            mac_al_1905_device.mac   = neighbor_al_mac;
            mac_al_1905_device.bridges_exist =
                ieee1905_1::tlv1905NeighborDevice::eBridgesExist::AT_LEAST_ONE_BRIDGES_EXIST;
            index++;
        }
    }

    auto tlvSupportedService = cmdu_tx.addClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedService) {
        LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed, mid=" << std::hex << (int)mid;
        return false;
    }

    size_t number_of_supported_services = 1;
    if (db->device_conf.local_controller) {
        number_of_supported_services++;
    }

    if (!tlvSupportedService->alloc_supported_service_list(number_of_supported_services)) {
        LOG(ERROR) << "alloc_supported_service_list failed";
        return false;
    }

    auto supportedServiceTuple = tlvSupportedService->supported_service_list(0);
    if (!std::get<0>(supportedServiceTuple)) {
        LOG(ERROR) << "Failed accessing supported_service_list(0)";
        return false;
    }

    std::get<1>(supportedServiceTuple) =
        wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT;

    if (db->device_conf.local_controller) {
        auto supportedServiceTuple = tlvSupportedService->supported_service_list(1);
        if (!std::get<0>(supportedServiceTuple)) {
            LOG(ERROR) << "Failed accessing supported_service_list(1)";
            return false;
        }

        std::get<1>(supportedServiceTuple) =
            wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER;
    }

    auto tlvApOperationalBSS = cmdu_tx.addClass<wfa_map::tlvApOperationalBSS>();
    if (!tlvApOperationalBSS) {
        LOG(ERROR) << "addClass wfa_map::tlvApOperationalBSS failed, mid=" << std::hex << (int)mid;
        return false;
    }

    for (const auto &radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }

        auto radio_list         = tlvApOperationalBSS->create_radio_list();
        radio_list->radio_uid() = radio->front.iface_mac;
        for (const auto &bssid : radio->front.bssids) {
            if (bssid.mac == network_utils::ZERO_MAC) {
                continue;
            }
            if (bssid.ssid.empty()) {
                continue;
            }
            auto radio_bss_list           = radio_list->create_radio_bss_list();
            radio_bss_list->radio_bssid() = bssid.mac;
            radio_bss_list->set_ssid(bssid.ssid);

            radio_list->add_radio_bss_list(radio_bss_list);
        }
        tlvApOperationalBSS->add_radio_list(radio_list);
    }

    // The Multi-AP Agent shall include an Associated Clients TLV in the message if there is at
    // least one 802.11 client directly associated with any of the BSS(s) that is operated by the
    // Multi-AP Agent
    bool include_associated_clients_tlv = false;
    for (const auto &radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }
        if (radio->associated_clients.size() > 0) {
            include_associated_clients_tlv = true;
            break;
        }
    }

    if (include_associated_clients_tlv) {
        auto tlvAssociatedClients = cmdu_tx.addClass<wfa_map::tlvAssociatedClients>();
        if (!tlvAssociatedClients) {
            LOG(ERROR) << "addClass wfa_map::tlvAssociatedClients failed, mid=" << std::hex << mid;
            return false;
        }

        // Get current time to compute elapsed time since last client association
        auto now = std::chrono::steady_clock::now();

        // Fill in Associated Clients TLV
        for (const auto &radio : db->get_radios_list()) {
            if (!radio) {
                continue;
            }

            for (const auto &bssid : radio->front.bssids) {
                auto bss_list     = tlvAssociatedClients->create_bss_list();
                bss_list->bssid() = bssid.mac;

                for (const auto &associated_client_entry : radio->associated_clients) {
                    if (associated_client_entry.second.bssid != bssid.mac) {
                        continue;
                    }

                    auto client_info = bss_list->create_clients_associated_list();

                    auto &association_time = associated_client_entry.second.association_time;
                    auto elapsed =
                        std::chrono::duration_cast<std::chrono::seconds>(now - association_time)
                            .count();
                    if ((elapsed < 0) || (elapsed > UINT16_MAX)) {
                        elapsed = UINT16_MAX;
                    }

                    client_info->mac()                             = associated_client_entry.first;
                    client_info->time_since_last_association_sec() = elapsed;

                    bss_list->add_clients_associated_list(client_info);
                }
                tlvAssociatedClients->add_bss_list(bss_list);
            }
        }
    }

    LOG(DEBUG) << "Sending topology response message, mid: " << std::hex << (int)mid;
    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_1905_higher_layer_data_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                             const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received HIGHER_LAYER_DATA_MESSAGE , mid=" << std::hex << int(mid);

    auto tlvHigherLayerData = cmdu_rx.getClass<wfa_map::tlvHigherLayerData>();
    if (!tlvHigherLayerData) {
        LOG(ERROR) << "addClass wfa_map::tlvHigherLayerData failed";
        return false;
    }

    const auto protocol       = tlvHigherLayerData->protocol();
    const auto payload_length = tlvHigherLayerData->payload_length();
    LOG(DEBUG) << "protocol: " << std::hex << int(protocol);
    LOG(DEBUG) << "payload_length: " << std::hex << int(payload_length);

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message to the originator, mid=" << std::hex << int(mid);
    auto db = AgentDB::get();
    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_1905_link_metric_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                     const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received LINK_METRIC_QUERY_MESSAGE, mid=" << std::hex << int(mid);

    /**
     * The IEEE 1905.1 standard says about the Link Metric Query TLV and the neighbor type octet
     * that "If the value is 0, then the EUI48 field is not present; if the value is 1, then the
     * EUI-48 field shall be present."
     *
     * However, optional fields are not currently supported by TLVF.
     *
     * As a workaround, instead of defining a tlvLinkMetricQuery TLV with an optional field, we
     * have defined two different TLVs: tlvLinkMetricQuery with the optional field and
     * tlvLinkMetricQueryAllNeighbors without it. Application must check which of both TLVs has
     * been received inside the message.
     */
    std::shared_ptr<ieee1905_1::tlvLinkMetricQueryAllNeighbors> tlvLinkMetricQueryAllNeighbors;
    std::shared_ptr<ieee1905_1::tlvLinkMetricQuery> tlvLinkMetricQuery;

    tlvLinkMetricQueryAllNeighbors = cmdu_rx.getClass<ieee1905_1::tlvLinkMetricQueryAllNeighbors>();
    if (!tlvLinkMetricQueryAllNeighbors) {
        tlvLinkMetricQuery = cmdu_rx.getClass<ieee1905_1::tlvLinkMetricQuery>();
        if (!tlvLinkMetricQuery) {
            LOG(ERROR) << "getClass ieee1905_1::tlvLinkMetricQueryAllNeighbors and "
                          "ieee1905_1::tlvLinkMetricQuery failed";
            return false;
        }
    }

    auto db = AgentDB::get();

    /**
     * 1905.1 AL MAC address of the device that transmits the response message.
     */
    sMacAddr reporter_al_mac = db->bridge.mac;

    /**
     * 1905.1 AL MAC address of a neighbor of the receiving device.
     * Query can specify a particular neighbor device or all neighbor devices.
     */
    sMacAddr neighbor_al_mac = network_utils::ZERO_MAC;

    /**
     * Obtain link metrics for either all neighbors or a specific neighbor
     */
    ieee1905_1::eLinkMetricNeighborType neighbor_type;

    /**
     * The link metrics type requested: TX, RX or both
     */
    ieee1905_1::eLinkMetricsType link_metrics_type;

    if (tlvLinkMetricQuery) {
        /**
    	   * If tlvLinkMetricQuery has been included in message, we will be permissive enough to
    	   * allow it specify ALL_NEIGHBORS and if so, then we will just ignore the field
    	   * containing the MAC address of neighbor.
    	   */
        neighbor_type     = tlvLinkMetricQuery->neighbor_type();
        neighbor_al_mac   = tlvLinkMetricQuery->mac_al_1905_device();
        link_metrics_type = tlvLinkMetricQuery->link_metrics_type();
    } else {
        neighbor_type = tlvLinkMetricQueryAllNeighbors->neighbor_type();
        if (ieee1905_1::eLinkMetricNeighborType::ALL_NEIGHBORS != neighbor_type) {
            LOG(ERROR) << "Unexpected neighbor type: " << std::hex << int(neighbor_type);
            return false;
        }
        link_metrics_type = tlvLinkMetricQueryAllNeighbors->link_metrics_type();
    }

    /**
     * Set alias flag to true if link metrics for a specific neighbor have been requested
     */
    bool specific_neighbor =
        ieee1905_1::eLinkMetricNeighborType::SPECIFIC_NEIGHBOR == neighbor_type;

    /**
     * Create response message
     */
    auto cmdu_tx_header =
        cmdu_tx.create(mid, ieee1905_1::eMessageType::LINK_METRIC_RESPONSE_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "Failed creating LINK_METRIC_RESPONSE_MESSAGE header! mid=" << std::hex
                   << (int)mid;
        return false;
    }

    /**
     * Get the list of neighbor links from the topology database.
     * Neighbors are grouped by the interface that connects to them.
     */
    std::map<sLinkInterface, std::vector<sLinkNeighbor>> neighbor_links_map;
    if (!get_neighbor_links(neighbor_al_mac, neighbor_links_map)) {
        LOG(ERROR) << "Failed to get the list of neighbor links";
        return false;
    }

    /**
     * If the specified neighbor 1905.1 AL ID does not identify a neighbor of the receiving 1905.1
     * AL, then a link metric ResultCode TLV (see Table 6-21) with a value set to “invalid
     * neighbor” shall be included in this message.
     */
    bool invalid_neighbor = specific_neighbor && neighbor_links_map.empty();
    if (invalid_neighbor) {
        auto tlvLinkMetricResultCode = cmdu_tx.addClass<ieee1905_1::tlvLinkMetricResultCode>();
        if (!tlvLinkMetricResultCode) {
            LOG(ERROR) << "addClass ieee1905_1::tlvLinkMetricResultCode failed, mid=" << std::hex
                       << (int)mid;
            return false;
        }

        LOG(INFO) << "Invalid neighbor 1905.1 AL ID specified: "
                  << tlvf::mac_to_string(neighbor_al_mac);

        tlvLinkMetricResultCode->value() = ieee1905_1::tlvLinkMetricResultCode::INVALID_NEIGHBOR;
    } else {

        /**
         * Report link metrics for the link with specific neighbor or for all neighbors, as
         * obtained from topology database
         */
        for (const auto &entry : neighbor_links_map) {
            auto interface        = entry.first;
            const auto &neighbors = entry.second;

            std::unique_ptr<link_metrics_collector> collector =
                create_link_metrics_collector(interface);
            if (!collector) {
                continue;
            }

            for (const auto &neighbor : neighbors) {

                LOG(TRACE) << "Getting link metrics for interface " << interface.iface_name
                           << " (MediaType = " << std::hex << (int)interface.media_type
                           << ") and neighbor " << neighbor.iface_mac;

                sLinkMetrics link_metrics;
                if (!collector->get_link_metrics(interface.iface_name, neighbor.iface_mac,
                                                 link_metrics)) {
                    LOG(ERROR) << "Unable to get link metrics for interface "
                               << interface.iface_name << " and neighbor " << neighbor.iface_mac;
                    return false;
                }

                if (!add_link_metrics(reporter_al_mac, interface, neighbor, link_metrics,
                                      link_metrics_type)) {
                    return false;
                }
            }
        }
    }

    LOG(DEBUG) << "Sending LINK_METRIC_RESPONSE_MESSAGE, mid: " << std::hex << (int)mid;
    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_1905_combined_infrastructure_metrics(
    ieee1905_1::CmduMessageRx &cmdu_rx, const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received COMBINED_INFRASTRUCTURE_METRICS message, mid=" << std::hex << int(mid);

    if (cmdu_rx.getClass<ieee1905_1::tlvReceiverLinkMetric>())
        LOG(DEBUG) << "Received TLV_RECEIVER_LINK_METRIC";
    if (cmdu_rx.getClass<ieee1905_1::tlvTransmitterLinkMetric>())
        LOG(DEBUG) << "Received TLV_TRANSMITTER_LINK_METRIC";

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message to the originator, mid=" << std::hex << int(mid);
    auto db = AgentDB::get();
    return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_1905_topology_discovery(const std::string &src_mac,
                                                      ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto tlvAlMac = cmdu_rx.getClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMac) {
        LOG(ERROR) << "getClass tlvAlMacAddress failed";
        return false;
    }

    auto db = AgentDB::get();

    // Filter out the messages we have sent.
    if (tlvAlMac->mac() == db->bridge.mac) {
        return true;
    }

    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received TOPOLOGY_DISCOVERY_MESSAGE from AL MAC=" << tlvAlMac->mac()
              << ", mid=" << std::hex << mid;

    auto tlvMac = cmdu_rx.getClass<ieee1905_1::tlvMacAddress>();
    if (!tlvMac) {
        LOG(ERROR) << "getClass tlvMacAddress failed";
        return false;
    }

    uint32_t if_index                      = message_com::get_uds_header(cmdu_rx)->if_index;
    std::string local_receiving_iface_name = network_utils::linux_get_iface_name(if_index);
    if (local_receiving_iface_name.empty()) {
        LOG(ERROR) << "Failed getting interface name for index: " << if_index;
        return false;
    }

    std::string local_receiving_iface_mac_str;
    if (!network_utils::linux_iface_get_mac(local_receiving_iface_name,
                                            local_receiving_iface_mac_str)) {
        LOG(ERROR) << "Failed getting MAC address for interface: " << local_receiving_iface_name;
        return false;
    }

    // Check if it is a new device so if it does, we will send Topology Notification.
    bool new_device = false;
    for (auto &neighbors_on_local_iface_entry : db->neighbor_devices) {
        auto &neighbors_on_local_iface = neighbors_on_local_iface_entry.second;
        new_device =
            neighbors_on_local_iface.find(tlvAlMac->mac()) == neighbors_on_local_iface.end();
        if (new_device) {
            break;
        }
    }

    // Add/Update the device on our list.
    auto &neighbor_devices_by_al_mac =
        db->neighbor_devices[tlvf::mac_from_string(local_receiving_iface_mac_str)];

    // Update an exist neighbor.
    neighbor_devices_by_al_mac[tlvAlMac->mac()].transmitting_iface_mac = tlvMac->mac();
    neighbor_devices_by_al_mac[tlvAlMac->mac()].timestamp = std::chrono::steady_clock::now();

    // If it is a new device, then our 1905.1 neighbors list has changed and we are required to send
    // Topology Notification Message.
    if (new_device) {
        LOG(INFO) << "Sending Topology Notification on newly discovered 1905.1 device";
        auto cmdu_header =
            cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
            return false;
        }
        send_cmdu_to_broker(cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                            tlvf::mac_to_string(db->bridge.mac));
    }
    return true;
}

bool backhaul_manager::handle_1905_autoconfiguration_response(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                              const std::string &src_mac)
{
    LOG(DEBUG) << "received autoconfiguration response message";

    if (!controller_bridge_mac.empty() && src_mac != controller_bridge_mac) {
        LOG(INFO) << "current controller_bridge_mac=" << controller_bridge_mac
                  << " but response came from src_mac=" << src_mac << ", ignoring";
        return true;
    }

    auto tlvSupportedRole = cmdu_rx.getClass<ieee1905_1::tlvSupportedRole>();
    if (tlvSupportedRole) {
        LOG(DEBUG) << "tlvSupportedRole->value()=" << int(tlvSupportedRole->value());
        if (tlvSupportedRole->value() != ieee1905_1::tlvSupportedRole::REGISTRAR) {
            LOG(ERROR) << "invalid tlvSupportedRole value";
            return true;
        }
    } else {
        LOG(ERROR) << "tlvSupportedRole missing - ignoring autoconfiguration response message";
        return true;
    }

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header &&
        beerocks_header->action_op() == beerocks_message::ACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE) {
        // mark controller as prplMesh
        LOG(DEBUG) << "prplMesh controller: received ACTION_CONTROL_SLAVE_HANDSHAKE_RESPONSE from "
                   << src_mac;
        LOG_IF(is_prplmesh_controller, ERROR)
            << src_mac << " already marked as prplmesh controller!";
        is_prplmesh_controller = true;
    } else {
        LOG(DEBUG) << "Not prplMesh controller " << src_mac;
        is_prplmesh_controller = false;
    }
    controller_bridge_mac = src_mac;
    LOG(INFO) << "update controller_bridge_mac=" << controller_bridge_mac;

    auto tlvSupportedFreqBand = cmdu_rx.getClass<ieee1905_1::tlvSupportedFreqBand>();
    if (tlvSupportedFreqBand) {
        LOG(DEBUG) << "tlvSupportedFreqBand->value()=" << int(tlvSupportedFreqBand->value());
        std::string band_name;
        beerocks::eFreqType freq_type = beerocks::eFreqType::FREQ_UNKNOWN;
        switch (tlvSupportedFreqBand->value()) {
        case ieee1905_1::tlvSupportedFreqBand::BAND_2_4G:
            band_name = "2.4GHz";
            freq_type = beerocks::eFreqType::FREQ_24G;
            break;
        case ieee1905_1::tlvSupportedFreqBand::BAND_5G:
            band_name = "5GHz";
            freq_type = beerocks::eFreqType::FREQ_5G;
            break;
        case ieee1905_1::tlvSupportedFreqBand::BAND_60G:
            LOG(DEBUG) << "received autoconfiguration response for 60GHz band, unsupported";
            return true;
        default:
            LOG(ERROR) << "invalid tlvSupportedFreqBand value";
            return true;
        }
        LOG(DEBUG) << "received autoconfiguration response for " << band_name << " band";
        for (auto soc : slaves_sockets) {
            if (soc->freq_type == freq_type) {
                LOG(DEBUG) << band_name << " band socket found, iface=" << soc->hostap_iface;
                if (!soc->controller_discovered) {
                    LOG(DEBUG) << "set controller_discovered to true for " << band_name
                               << " band, iface=" << soc->hostap_iface;
                    LOG(DEBUG) << FSM_CURR_STATE_STR;
                    soc->controller_discovered = true;
                    controller_bridge_mac      = src_mac;
                    finalize_slaves_connect_state(true, soc);
                }
            }
        }

    } else {
        LOG(ERROR) << "tlvSupportedFreqBand missing - ignoring autoconfiguration response message";
        return true;
    }

    auto tlvSupportedService = cmdu_rx.getClass<wfa_map::tlvSupportedService>();
    if (tlvSupportedService) {
        bool controllerFound = false;
        for (int i = 0; i < tlvSupportedService->supported_service_list_length(); i++) {
            auto supportedServiceTuple = tlvSupportedService->supported_service_list(i);
            if (!std::get<0>(supportedServiceTuple)) {
                LOG(ERROR) << "Invalid tlvSupportedService";
                return true;
            }
            if (std::get<1>(supportedServiceTuple) ==
                wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER) {
                controllerFound = true;
            }
        }
        if (!controllerFound) {
            LOG(WARNING)
                << "Invalid tlvSupportedService - supported service is not MULTI_AP_CONTROLLER";
            return true;
        }
    } else {
        LOG(ERROR) << "tlvSupportedService missing - ignoring autoconfiguration response message";
        return true;
    }

    if (FSM_IS_IN_STATE(WAIT_FOR_AUTOCONFIG_RESPONSE_MESSAGE)) {
        FSM_MOVE_STATE(CONNECT_TO_MASTER);
    } else {
        LOG(TRACE) << "no state change";
    }
    return true;
}

bool backhaul_manager::handle_1905_beacon_metrics_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                        const std::string &src_mac,
                                                        Socket *&forward_to)
{
    LOG(DEBUG) << "now going to handle BEACON METRICS QUERY";

    // extract the desired STA mac
    auto tlvBeaconMetricsQuery = cmdu_rx.getClass<wfa_map::tlvBeaconMetricsQuery>();
    if (!tlvBeaconMetricsQuery) {
        LOG(ERROR) << "handle_1905_beacon_metrics_query should handle only tlvBeaconMetrics, but "
                      "got something else: 0x"
                   << std::hex << (uint16_t)cmdu_rx.getMessageType();
        return false;
    }

    const sMacAddr &requested_sta_mac = tlvBeaconMetricsQuery->associated_sta_mac();
    LOG(DEBUG) << "the requested STA mac is: " << requested_sta_mac;

    // build ACK message CMDU
    const auto mid      = cmdu_rx.getMessageId();
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    auto db    = AgentDB::get();
    auto radio = db->get_radio_by_mac(requested_sta_mac, AgentDB::eMacType::CLIENT);
    if (!radio) {
        LOG(DEBUG) << "STA with MAC [" << requested_sta_mac
                   << "] is not associated with any BSS operated by the agent";

        // add an Error Code TLV
        auto error_code_tlv = cmdu_tx.addClass<wfa_map::tlvErrorCode>();
        if (!error_code_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvErrorCode has failed";
            return false;
        }

        error_code_tlv->reason_code() =
            wfa_map::tlvErrorCode::STA_NOT_ASSOCIATED_WITH_ANY_BSS_OPERATED_BY_THE_AGENT;

        error_code_tlv->sta_mac() = requested_sta_mac;

        // report the error
        std::stringstream errorSS;
        auto error_tlv = cmdu_tx.getClass<wfa_map::tlvErrorCode>();
        if (error_tlv) {
            errorSS << "0x" << error_tlv->reason_code();
        } else {
            errorSS << "note: error constructing the error itself";
        }

        LOG(DEBUG) << "sending ACK message to the originator with an error, mid: " << std::hex
                   << int(mid) << " tlv error code: " << errorSS.str();

        // send the error
        return send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));
    }

    auto radio_info = get_radio(radio->front.iface_mac);
    if (!radio_info) {
        LOG(ERROR) << "Failed to get radio info for " << radio->front.iface_mac;
        return false;
    }
    forward_to = radio_info->slave;

    LOG(DEBUG) << "Found the radio that has the sation. radio: " << radio->front.iface_mac
               << "; station: " << requested_sta_mac;

    LOG(DEBUG) << "BEACON METRICS QUERY: sending ACK message to the originator mid: "
               << int(mid); // USED IN TESTS

    send_cmdu_to_broker(cmdu_tx, src_mac, tlvf::mac_to_string(db->bridge.mac));

    // continue processing
    return false;
}

bool backhaul_manager::send_slaves_enable()
{
    auto iface_hal = get_wireless_hal();

    auto db = AgentDB::get();
    for (auto soc : slaves_sockets) {
        auto notification =
            message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_ENABLE_APS_REQUEST>(
                cmdu_tx);

        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        if (soc->sta_iface == db->backhaul.selected_iface_name) {
            notification->channel() = iface_hal->get_channel();
        }
        LOG(DEBUG) << "Sending enable to slave " << soc->hostap_iface
                   << ", channel=" << int(notification->channel());

        message_com::send_cmdu(soc->slave, cmdu_tx);
    }

    return true;
}

bool backhaul_manager::hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr,
                                         std::string iface)
{
    if (!event_ptr) {
        LOG(ERROR) << "Invalid event!";
        return false;
    }

    // TODO: TEMP!
    LOG(DEBUG) << "Got event " << int(event_ptr->first) << " from iface " << iface;

    // AP Event & Data
    typedef bwl::sta_wlan_hal::Event Event;
    auto event = (Event)(event_ptr->first);
    auto data  = event_ptr->second.get();

    switch (event) {

    case Event::Connected: {

        auto iface_hal = get_wireless_hal(iface);

        LOG(DEBUG) << "WPA EVENT_CONNECTED on iface=" << iface;
        LOG(DEBUG) << "successfully connected to bssid=" << (iface_hal->get_bssid())
                   << " on channel=" << (iface_hal->get_channel()) << " on iface=" << iface;

        auto db = AgentDB::get();

        if (iface == db->backhaul.selected_iface_name && !hidden_ssid) {
            //this is generally not supposed to happen
            LOG(WARNING) << "event iface != wireless iface!";
        }
        if (FSM_IS_IN_STATE(WAIT_WPS)) {
            db->backhaul.selected_iface_name = iface;
            FSM_MOVE_STATE(MASTER_DISCOVERY);
        }
        if (FSM_IS_IN_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)) {
            LOG(DEBUG) << "successful connect on iface=" << iface;
            if (hidden_ssid) {
                iface_hal->refresh_radio_info();
                const auto &radio_info = iface_hal->get_radio_info();
                for (auto soc : slaves_sockets) {
                    if (soc->sta_iface == iface) {
                        /* prevent low filter radio from connecting to high band in any case */
                        if (son::wireless_utils::which_freq(radio_info.channel) ==
                                beerocks::FREQ_5G &&
                            soc->sta_iface_filter_low &&
                            !son::wireless_utils::is_low_subband(radio_info.channel)) {
                            LOG(DEBUG) << "iface " << iface
                                       << " is connected on low 5G band with filter, aborting";
                            FSM_MOVE_STATE(WIRELESS_CONFIG_4ADDR_MODE);
                            return true;
                        }
                        /* prevent unfiltered ("high") radio from connecting to low band, unless we have only 2 radios */
                        int sta_iface_count_5ghz = 0;
                        for (const auto &sta_iface : slave_sta_ifaces) {
                            auto iface_hal = get_wireless_hal(sta_iface);
                            if (!iface_hal)
                                break;

                            iface_hal->refresh_radio_info();
                            if (son::wireless_utils::which_freq(
                                    iface_hal->get_radio_info().channel) == beerocks::FREQ_5G) {
                                sta_iface_count_5ghz++;
                            }
                        }
                        if (son::wireless_utils::which_freq(radio_info.channel) ==
                                beerocks::FREQ_5G &&
                            !soc->sta_iface_filter_low &&
                            son::wireless_utils::is_low_subband(radio_info.channel) &&
                            sta_iface_count_5ghz > 1) {
                            LOG(DEBUG) << "iface " << iface
                                       << " is connected on low 5G band with filter, aborting";
                            FSM_MOVE_STATE(WIRELESS_CONFIG_4ADDR_MODE);
                            return true;
                        }
                    }
                }
            }
            roam_flag      = false;
            state_attempts = 0;

            // Send slaves to enable the AP's
            send_slaves_enable();

            FSM_MOVE_STATE(MASTER_DISCOVERY);
        } else if (FSM_IS_IN_STATE(WIRELESS_WAIT_FOR_RECONNECT)) {
            LOG(DEBUG) << "reconnected successfully, continuing";

            // IRE running controller
            if (db->device_conf.local_controller && !db->device_conf.local_gw) {
                FSM_MOVE_STATE(CONNECT_TO_MASTER);
            } else {
                FSM_MOVE_STATE(OPERATIONAL);
            }
        }
    } break;

    case Event::Disconnected: {
        if (FSM_IS_IN_STATE(WAIT_WPS)) {
            return true;
        }
        auto db = AgentDB::get();
        if (iface == db->backhaul.selected_iface_name) {
            if (FSM_IS_IN_STATE(OPERATIONAL) || FSM_IS_IN_STATE(CONNECTED)) {
                platform_notify_error(bpl::eErrorCode::BH_DISCONNECTED,
                                      "Backhaul disconnected on operational state");
                stop_on_failure_attempts--;
                state_time_stamp_timeout =
                    std::chrono::steady_clock::now() +
                    std::chrono::seconds(WIRELESS_WAIT_FOR_RECONNECT_TIMEOUT);
                FSM_MOVE_STATE(WIRELESS_WAIT_FOR_RECONNECT);
            } else if (FSM_IS_IN_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)) {
                if (!data) {
                    LOG(ERROR) << "Disconnected event without data!";
                    return false;
                }
                roam_flag = false;
                auto msg =
                    static_cast<bwl::sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION *>(data);
                if (msg->disconnect_reason == uint32_t(DEAUTH_REASON_PASSPHRASE_MISMACH)) {
                    //enter bssid to black_list trigger timer
                    auto local_time_stamp = std::chrono::steady_clock::now();
                    auto local_bssid      = tlvf::mac_to_string(msg->bssid);
                    LOG(DEBUG) << "insert bssid = " << local_bssid << " to backhaul blacklist";
                    ap_blacklist_entry entry;
                    entry.timestamp           = local_time_stamp;
                    entry.attempts            = AP_BLACK_LIST_FAILED_ATTEMPTS_THRESHOLD;
                    ap_blacklist[local_bssid] = entry;
                    platform_notify_error(bpl::eErrorCode::BH_ASSOCIATE_4ADDR_FAILURE,
                                          "SSID='" + m_sConfig.ssid + "', BSSID='" + local_bssid +
                                              "', DEAUTH_REASON='" +
                                              std::to_string(msg->disconnect_reason));
                    stop_on_failure_attempts--;
                    FSM_MOVE_STATE(INITIATE_SCAN);
                }

            } else {
                platform_notify_error(bpl::eErrorCode::BH_DISCONNECTED,
                                      "Backhaul disconnected non operational state");
                stop_on_failure_attempts--;
                FSM_MOVE_STATE(RESTART);
            }
        }

    } break;

    case Event::Terminating: {

        LOG(DEBUG) << "wpa_supplicant terminated, restarting";
        platform_notify_error(bpl::eErrorCode::BH_WPA_SUPPLICANT_TERMINATED,
                              "wpa_supplicant terminated");
        stop_on_failure_attempts--;
        FSM_MOVE_STATE(RESTART);

    } break;

    case Event::ScanResults: {
        if (FSM_IS_IN_STATE(WAIT_WPS)) {
            return true;
        }
        if (!FSM_IS_IN_STATE(WAIT_FOR_SCAN_RESULTS)) {
            LOG(DEBUG) << "not waiting for scan results, ignoring event";
            return true;
        }

        LOG(DEBUG) << "scan results available for iface " << iface;
        pending_slave_sta_ifaces.erase(iface);

        if (pending_slave_sta_ifaces.empty()) {
            LOG(DEBUG) << "scan results ready";
            get_scan_measurement();
            if (!select_bssid()) {
                LOG(DEBUG) << "couldn't find a suitable BSSID";
                FSM_MOVE_STATE(INITIATE_SCAN);
                state_attempts++;
                return false;
            } else {
                FSM_MOVE_STATE(WIRELESS_CONFIG_4ADDR_MODE);
            }
        }

    } break;

    case Event::ChannelSwitch: {

    } break;

    case Event::STA_Unassoc_RSSI: {

        if (!data) {
            LOG(ERROR) << "STA_Unassoc_RSSI without data!";
            return false;
        }

        auto msg = static_cast<bwl::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE *>(data);

        LOG(DEBUG) << "ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE for mac "
                   << msg->params.result.mac << " id = " << unassociated_rssi_measurement_header_id;

        if (unassociated_rssi_measurement_header_id > -1) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(cmdu_tx);

            if (response == nullptr) {
                LOG(ERROR) << "Failed building message!";
                break;
            }
            std::copy_n(msg->params.result.mac.oct, sizeof(msg->params.result.mac.oct),
                        response->params().result.mac.oct);
            response->params().result.channel    = msg->params.result.channel;
            response->params().result.rssi       = msg->params.result.rssi;
            response->params().rx_phy_rate_100kb = msg->params.rx_phy_rate_100kb;
            response->params().tx_phy_rate_100kb = msg->params.tx_phy_rate_100kb;
            response->params().rx_rssi           = msg->params.rx_rssi;
            response->params().rx_snr            = msg->params.rx_snr;
            response->params().rx_packets        = msg->params.rx_packets;
            response->params().src_module        = msg->params.src_module;

            if (unassociated_measurement_slave_soc) {
                message_com::send_cmdu(unassociated_measurement_slave_soc, cmdu_tx);
            } else {
                LOG(ERROR) << "unassociated_measurement_slave_soc = nullptr!!!";
            }
        } else {
            LOG(ERROR) << "sta_unassociated_rssi_measurement_header_id == -1";
        }

        unassociated_rssi_measurement_header_id = -1;
        unassociated_measurement_slave_soc      = nullptr;

    } break;

    // Unhandled events
    default: {
        LOG(ERROR) << "Unhandled event: " << int(event);
        return false;
    }
    }

    return true;
} // namespace beerocks

bool backhaul_manager::select_bssid()
{
    int max_rssi_24     = beerocks::RSSI_INVALID;
    int max_rssi_5_best = beerocks::RSSI_INVALID;
    int max_rssi_5_high = beerocks::RSSI_INVALID;
    int max_rssi_5_low  = beerocks::RSSI_INVALID;
    std::string best_bssid_5, best_bssid_5_high, best_bssid_5_low, best_bssid_24;
    int best_bssid_channel_5 = 0, best_bssid_channel_5_high = 0, best_bssid_channel_5_low = 0,
        best_bssid_channel_24 = 0;
    std::string best_24_sta_iface, best_5_high_sta_iface, best_5_low_sta_iface, best_5_sta_iface;

    // Support up to 256 scan results
    std::vector<bwl::SScanResult> scan_results;

    auto db = AgentDB::get();

    LOG(DEBUG) << "select_bssid: SSID = " << m_sConfig.ssid;

    for (auto soc : slaves_sockets) {

        if (soc->sta_iface.empty() || !soc->sta_wlan_hal) {
            LOG(DEBUG) << "skipping empty iface";
            continue;
        }

        std::string iface = soc->sta_iface;

        LOG(DEBUG) << "select_bssid: iface  = " << iface;
        int num_of_results = soc->sta_wlan_hal->get_scan_results(m_sConfig.ssid, scan_results);
        LOG(DEBUG) << "Scan Results: " << num_of_results;

        for (auto &scan_result : scan_results) {

            auto bssid = tlvf::mac_to_string(scan_result.bssid);
            LOG(DEBUG) << "select_bssid: bssid = " << bssid
                       << ", channel = " << int(scan_result.channel) << " iface = " << iface
                       << ", rssi=" << int(scan_result.rssi);

            auto ap_blacklist_it = ap_blacklist.find(bssid);
            if (ap_blacklist_it != ap_blacklist.end()) {
                ap_blacklist_entry &entry = ap_blacklist_it->second;
                if (std::chrono::steady_clock::now() >
                    (entry.timestamp + std::chrono::seconds(AP_BLACK_LIST_TIMEOUT_SECONDS))) {
                    LOG(DEBUG) << " bssid = " << bssid
                               << " aged and removed from backhaul blacklist";
                    ap_blacklist.erase(bssid);
                } else if (entry.attempts >= AP_BLACK_LIST_FAILED_ATTEMPTS_THRESHOLD) {
                    LOG(DEBUG) << " bssid = " << bssid << " is blacklisted, skipping";
                    continue;
                }
            }
            if (roam_flag) {
                if ((bssid == roam_selected_bssid) &&
                    (scan_result.channel == roam_selected_bssid_channel)) {
                    LOG(DEBUG) << "roaming flag on  - found bssid match = " << roam_selected_bssid
                               << " roam_selected_bssid_channel = "
                               << int(roam_selected_bssid_channel);
                    db->backhaul.selected_iface_name = iface;
                    return true;
                }
            } else if (!m_sConfig.preferred_bssid.empty() && bssid == m_sConfig.preferred_bssid) {
                LOG(DEBUG) << "preferred bssid - found bssid match = " << bssid;
                selected_bssid_channel           = scan_result.channel;
                selected_bssid                   = bssid;
                db->backhaul.selected_iface_name = iface;
                return true;
            } else if (son::wireless_utils::which_freq(scan_result.channel) == eFreqType::FREQ_5G) {
                if (soc->sta_iface_filter_low &&
                    son::wireless_utils::which_subband(scan_result.channel) ==
                        beerocks::LOW_SUBBAND) {
                    // iface with low filter - best low
                    if (scan_result.rssi > max_rssi_5_low) {
                        max_rssi_5_low           = scan_result.rssi;
                        best_bssid_5_low         = bssid;
                        best_bssid_channel_5_low = scan_result.channel;
                        best_5_low_sta_iface     = iface;
                    }

                } else if (!soc->sta_iface_filter_low &&
                           son::wireless_utils::which_subband(scan_result.channel) ==
                               beerocks::HIGH_SUBBAND) {
                    // iface without low filter (high filter or bypass) - best high
                    if (scan_result.rssi > max_rssi_5_high) {
                        max_rssi_5_high           = scan_result.rssi;
                        best_bssid_5_high         = bssid;
                        best_bssid_channel_5_high = scan_result.channel;
                        best_5_high_sta_iface     = iface;
                    }
                }

                if (scan_result.rssi > max_rssi_5_best) {
                    // best 5G (low/high)
                    max_rssi_5_best      = scan_result.rssi;
                    best_bssid_5         = bssid;
                    best_bssid_channel_5 = scan_result.channel;
                    best_5_sta_iface     = iface;
                }

            } else {
                // best 2.4G
                if (scan_result.rssi > max_rssi_24) {
                    max_rssi_24           = scan_result.rssi;
                    best_bssid_24         = bssid;
                    best_bssid_channel_24 = scan_result.channel;
                    best_24_sta_iface     = iface;
                }
            }
        }
    }

    if (!best_bssid_24.empty()) {
        LOG(DEBUG) << "BEST - 2.4Ghz          - " << best_24_sta_iface
                   << " - BSSID: " << best_bssid_24 << ", Channel: " << int(best_bssid_channel_24)
                   << ", RSSI: " << int(max_rssi_24);
    } else {
        LOG(DEBUG) << "BEST - 2.4Ghz          - Not Found!";
    }

    if (!best_bssid_5_low.empty()) {
        LOG(DEBUG) << "BEST - 5Ghz (Low)      - " << best_5_low_sta_iface
                   << " - BSSID: " << best_bssid_5_low
                   << ", Channel: " << int(best_bssid_channel_5_low)
                   << ", RSSI: " << int(max_rssi_5_low);
    } else {
        LOG(DEBUG) << "BEST - 5Ghz (Low)      - Not Found!";
    }

    if (!best_bssid_5_high.empty()) {
        LOG(DEBUG) << "BEST - 5Ghz (High)     - " << best_5_high_sta_iface
                   << " - BSSID: " << best_bssid_5_high
                   << ", Channel: " << int(best_bssid_channel_5_high)
                   << ", RSSI: " << int(max_rssi_5_high);
    } else {
        LOG(DEBUG) << "BEST - 5Ghz (High)     - Not Found!";
    }

    if (!best_bssid_5.empty()) {
        LOG(DEBUG) << "BEST - 5Ghz (Absolute) - " << best_5_sta_iface
                   << " - BSSID: " << best_bssid_5 << ", Channel: " << int(best_bssid_channel_5)
                   << ", RSSI: " << int(max_rssi_5_best);
    } else {
        LOG(DEBUG) << "BEST - 5Ghz (Absolute) - Not Found!";
    }

    if (max_rssi_5_high != beerocks::RSSI_INVALID &&
        (best_5_sta_iface == best_5_low_sta_iface || best_5_low_sta_iface.empty()) &&
        son::wireless_utils::which_subband(best_bssid_channel_5) == beerocks::HIGH_SUBBAND) {

        max_rssi_5_best      = max_rssi_5_high;
        best_bssid_5         = best_bssid_5_high;
        best_bssid_channel_5 = best_bssid_channel_5_high;
        best_5_sta_iface     = best_5_high_sta_iface;

    } else if (max_rssi_5_low != beerocks::RSSI_INVALID &&
               (best_5_sta_iface == best_5_high_sta_iface || best_5_high_sta_iface.empty()) &&
               son::wireless_utils::which_subband(best_bssid_channel_5) == beerocks::LOW_SUBBAND) {

        max_rssi_5_best      = max_rssi_5_low;
        best_bssid_5         = best_bssid_5_low;
        best_bssid_channel_5 = best_bssid_channel_5_low;
        best_5_sta_iface     = best_5_low_sta_iface;
    }

    if (!best_bssid_5.empty()) {
        LOG(DEBUG) << "Selected 5Ghz - " << best_5_sta_iface << " - BSSID: " << best_bssid_5
                   << ", Channel: " << int(best_bssid_channel_5)
                   << ", RSSI: " << int(max_rssi_5_best);
    } else {
        LOG(DEBUG) << "Selected 5Ghz - Not Found!";
    }

    // Select the base backhaul interface
    if (((max_rssi_24 == beerocks::RSSI_INVALID) && (max_rssi_5_best == beerocks::RSSI_INVALID)) ||
        roam_flag) {
        // TODO: ???
        return false;
    } else if (max_rssi_24 == beerocks::RSSI_INVALID) {
        selected_bssid                   = best_bssid_5;
        selected_bssid_channel           = best_bssid_channel_5;
        db->backhaul.selected_iface_name = best_5_sta_iface;
    } else if (max_rssi_5_best == beerocks::RSSI_INVALID) {
        selected_bssid                   = best_bssid_24;
        selected_bssid_channel           = best_bssid_channel_24;
        db->backhaul.selected_iface_name = best_24_sta_iface;
    } else if ((max_rssi_5_best > RSSI_THRESHOLD_5GHZ)) {
        selected_bssid                   = best_bssid_5;
        selected_bssid_channel           = best_bssid_channel_5;
        db->backhaul.selected_iface_name = best_5_sta_iface;
    } else if (max_rssi_24 < max_rssi_5_best + RSSI_BAND_DELTA_THRESHOLD) {
        selected_bssid                   = best_bssid_5;
        selected_bssid_channel           = best_bssid_channel_5;
        db->backhaul.selected_iface_name = best_5_sta_iface;
    } else {
        selected_bssid                   = best_bssid_24;
        selected_bssid_channel           = best_bssid_channel_24;
        db->backhaul.selected_iface_name = best_24_sta_iface;
    }

    if (!get_wireless_hal()) {
        LOG(ERROR) << "Slave for interface " << db->backhaul.selected_iface_name << " NOT found!";
        return false;
    }

    return true;
}

void backhaul_manager::get_scan_measurement()
{
    // Support up to 256 scan results
    std::vector<bwl::SScanResult> scan_results;

    LOG(DEBUG) << "get_scan_measurement: SSID = " << m_sConfig.ssid;
    scan_measurement_list.clear();
    for (auto &soc : slaves_sockets) {

        if (soc->sta_iface.empty()) {
            LOG(DEBUG) << "skipping empty iface";
            continue;
        }
        if (!soc->sta_wlan_hal) {
            continue;
        }

        std::string iface = soc->sta_iface;
        LOG(DEBUG) << "get_scan_measurement: iface  = " << iface;
        int num_of_results = soc->sta_wlan_hal->get_scan_results(m_sConfig.ssid, scan_results);
        LOG(DEBUG) << "Scan Results: " << int(num_of_results);
        if (num_of_results < 0) {
            LOG(ERROR) << "get_scan_results failed!";
            return;
        } else if (num_of_results == 0) {
            continue;
        }

        for (auto &scan_result : scan_results) {

            auto bssid = tlvf::mac_to_string(scan_result.bssid);
            LOG(DEBUG) << "get_scan_measurement: bssid = " << bssid
                       << ", channel = " << int(scan_result.channel) << " iface = " << iface;

            auto it = scan_measurement_list.find(bssid);
            if (it != scan_measurement_list.end()) {
                //updating rssi if stronger
                if (scan_result.rssi > it->second.rssi) {
                    LOG(DEBUG) << "updating scan rssi for bssid = " << bssid
                               << " channel = " << int(scan_result.channel)
                               << " rssi = " << int(it->second.rssi) << " to -> "
                               << int(scan_result.rssi);
                    it->second.rssi = scan_result.rssi;
                }
            } else {
                //insert new entry
                sScanResult scan_measurement;

                std::copy_n(scan_result.bssid.oct, MAC_ADDR_LEN, scan_measurement.mac.oct);
                scan_measurement.channel     = scan_result.channel;
                scan_measurement.rssi        = scan_result.rssi;
                scan_measurement_list[bssid] = scan_measurement;
                LOG(DEBUG) << "insert scan to list bssid = " << bssid
                           << " channel = " << int(scan_result.channel)
                           << " rssi = " << int(scan_result.rssi);
            }
        }
    }
}

std::shared_ptr<bwl::sta_wlan_hal> backhaul_manager::get_wireless_hal(std::string iface)
{
    // If the iface argument is empty, use the default wireless interface
    auto db = AgentDB::get();
    if (iface.empty()) {
        iface = db->backhaul.selected_iface_name;
    }

    auto slave_sk = m_sConfig.slave_iface_socket.find(iface);
    if (slave_sk == m_sConfig.slave_iface_socket.end()) {
        return {};
    }

    return slave_sk->second->sta_wlan_hal;
}

bool backhaul_manager::get_media_type(const std::string &interface_name,
                                      ieee1905_1::eMediaTypeGroup media_type_group,
                                      ieee1905_1::eMediaType &media_type)
{
    bool result = false;

    if (ieee1905_1::eMediaTypeGroup::IEEE_802_3 == media_type_group) {
        uint32_t speed;
        if (network_utils::linux_iface_get_speed(interface_name, speed)) {
            if (SPEED_100 == speed) {
                media_type = ieee1905_1::eMediaType::IEEE_802_3U_FAST_ETHERNET;
            } else if (SPEED_1000 <= speed) {
                media_type = ieee1905_1::eMediaType::IEEE_802_3AB_GIGABIT_ETHERNET;
            } else {
                media_type = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
            }

            result = true;
        }
    } else if (ieee1905_1::eMediaTypeGroup::IEEE_802_11 == media_type_group) {

        auto db = AgentDB::get();

        auto radio = db->radio(interface_name);
        if (radio) {
            media_type =
                get_802_11_media_type(radio->front.freq_type, radio->front.max_supported_bw);
            result = true;
        }

    } else if (ieee1905_1::eMediaTypeGroup::IEEE_1901 == media_type_group) {
        // TODO: Not supported yet
        LOG(ERROR) << "IEEE_1901 media is not supported yet";
    } else if (ieee1905_1::eMediaTypeGroup::MoCA == media_type_group) {
        // TODO: Not supported yet
        LOG(ERROR) << "MoCA media is not supported yet";
    } else {
        media_type = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
        result     = true;
    }

    return result;
}

std::unique_ptr<link_metrics_collector>
backhaul_manager::create_link_metrics_collector(const sLinkInterface &link_interface) const
{
    ieee1905_1::eMediaType media_type = link_interface.media_type;
    ieee1905_1::eMediaTypeGroup media_type_group =
        static_cast<ieee1905_1::eMediaTypeGroup>(media_type >> 8);

    if (ieee1905_1::eMediaTypeGroup::IEEE_802_3 == media_type_group) {
        return std::make_unique<ieee802_3_link_metrics_collector>();
    }

    if (ieee1905_1::eMediaTypeGroup::IEEE_802_11 == media_type_group) {
        return std::make_unique<ieee802_11_link_metrics_collector>();
    }

    LOG(ERROR) << "Unable to create link metrics collector for interface '"
               << link_interface.iface_name << "' (unsupported media type " << std::hex
               << (int)media_type << ")";

    return nullptr;
}

bool backhaul_manager::get_neighbor_links(
    const sMacAddr &neighbor_mac_filter,
    std::map<sLinkInterface, std::vector<sLinkNeighbor>> &neighbor_links_map)
{
    // TODO: Topology Database is required to implement this method.

    // TODO: this is not accurate as we have made the assumption that there is a single interface.
    // Note that when processing Topology Discovery message we must store the IEEE 1905.1 AL MAC
    // address of the transmitting device together with the interface that such message is
    // received through.
    sLinkInterface wired_interface;
    auto db = AgentDB::get();

    wired_interface.iface_name = db->ethernet.iface_name;

    if (!get_media_type(wired_interface.iface_name, ieee1905_1::eMediaTypeGroup::IEEE_802_3,
                        wired_interface.media_type)) {
        LOG(ERROR) << "Unable to compute media type for interface " << wired_interface.iface_name;
        return false;
    }

    if (!get_iface_mac(wired_interface.iface_name, wired_interface.iface_mac)) {
        return false;
    }

    for (const auto &neighbors_on_local_iface : db->neighbor_devices) {
        auto &neighbors = neighbors_on_local_iface.second;
        for (const auto &neighbor_entry : neighbors) {
            sLinkNeighbor neighbor;
            neighbor.al_mac    = neighbor_entry.first;
            neighbor.iface_mac = neighbor_entry.second.transmitting_iface_mac;
            if ((neighbor_mac_filter == network_utils::ZERO_MAC) ||
                (neighbor_mac_filter == neighbor.al_mac)) {
                neighbor_links_map[wired_interface].push_back(neighbor);
            }
        }
    }

    // Also include a link for each associated client
    for (const auto radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }

        for (const auto &associated_client : radio->associated_clients) {
            auto &bssid = associated_client.second.bssid;

            sLinkInterface interface;
            if (!get_iface_name(bssid, interface.iface_name)) {
                LOG(ERROR) << "Unable to get interface name for BSSID " << bssid;
                return false;
            }

            interface.iface_mac  = bssid;
            interface.media_type = beerocks::get_802_11_media_type(radio->front.freq_type,
                                                                   radio->front.max_supported_bw);

            if (ieee1905_1::eMediaType::UNKNOWN_MEDIA == interface.media_type) {
                LOG(ERROR) << "Unknown media type for interface " << interface.iface_name;
                return false;
            }

            LOG(TRACE) << "Getting neighbors connected to interface " << interface.iface_name
                       << " with BSSID " << bssid;

            // TODO: This is not correct... We actually have to get this from the topology
            // discovery message, which will give us the neighbor interface and AL MAC addresses.
            sLinkNeighbor neighbor;
            neighbor.iface_mac = associated_client.first;
            neighbor.al_mac    = neighbor.iface_mac;

            if ((neighbor_mac_filter == network_utils::ZERO_MAC) ||
                (neighbor_mac_filter == neighbor.al_mac)) {
                neighbor_links_map[interface].push_back(neighbor);
            }
        }
    }

    return true;
}

bool backhaul_manager::add_ap_ht_capabilities(const sRadioInfo &radio_info)
{
    if (!radio_info.ht_supported) {
        return true;
    }

    auto tlv = cmdu_tx.addClass<wfa_map::tlvApHtCapabilities>();
    if (!tlv) {
        LOG(ERROR) << "Error creating TLV_AP_HT_CAPABILITIES";
        return false;
    }

    tlv->radio_uid() = radio_info.radio_mac;

    /**
     * See iw/util.c for details on how to compute fields.
     * Code has been preserved as close as possible to that in the iw command line tool.
     */
    bool tx_mcs_set_defined = !!(radio_info.ht_mcs_set[12] & (1 << 0));
    if (tx_mcs_set_defined) {
        tlv->flags().max_num_of_supported_tx_spatial_streams = (radio_info.ht_mcs_set[12] >> 2) & 3;
        tlv->flags().max_num_of_supported_rx_spatial_streams = 0; // TODO: Compute value (#1163)
    }
    tlv->flags().short_gi_support_20mhz = radio_info.ht_capability & BIT(5);
    tlv->flags().short_gi_support_40mhz = radio_info.ht_capability & BIT(6);
    tlv->flags().ht_support_40mhz       = radio_info.ht_capability & BIT(1);

    return true;
}

bool backhaul_manager::add_ap_vht_capabilities(const sRadioInfo &radio_info)
{
    if (!radio_info.vht_supported) {
        return true;
    }

    auto tlv = cmdu_tx.addClass<wfa_map::tlvApVhtCapabilities>();
    if (!tlv) {
        LOG(ERROR) << "Error creating TLV_AP_VHT_CAPABILITIES";
        return false;
    }

    tlv->radio_uid() = radio_info.radio_mac;

    /**
     * See iw/util.c for details on how to compute fields
     * Code has been preserved as close as possible to that in the iw command line tool.
     */
    tlv->supported_vht_tx_mcs() = radio_info.vht_mcs_set[4] | (radio_info.vht_mcs_set[5] << 8);
    tlv->supported_vht_rx_mcs() = radio_info.vht_mcs_set[0] | (radio_info.vht_mcs_set[1] << 8);
    tlv->flags1().max_num_of_supported_tx_spatial_streams = 0; // TODO: Compute value (#1163)
    tlv->flags1().max_num_of_supported_rx_spatial_streams = 0; // TODO: Compute value (#1163)
    tlv->flags1().short_gi_support_80mhz                  = radio_info.vht_capability & BIT(5);
    tlv->flags1().short_gi_support_160mhz_and_80_80mhz    = radio_info.vht_capability & BIT(6);
    tlv->flags2().vht_support_80_80mhz  = ((radio_info.vht_capability >> 2) & 3) == 2;
    tlv->flags2().vht_support_160mhz    = ((radio_info.vht_capability >> 2) & 3) == 1;
    tlv->flags2().su_beamformer_capable = radio_info.vht_capability & BIT(11);
    tlv->flags2().mu_beamformer_capable = radio_info.vht_capability & BIT(19);

    return true;
}

bool backhaul_manager::add_ap_he_capabilities(const sRadioInfo &radio_info)
{
    if (!radio_info.he_supported) {
        return true;
    }

    auto tlv = cmdu_tx.addClass<wfa_map::tlvApHeCapabilities>();
    if (!tlv) {
        LOG(ERROR) << "Error creating TLV_AP_HE_CAPABILITIES";
        return false;
    }

    tlv->radio_uid() = radio_info.radio_mac;

    // TODO: Fetch the AP HE Capabilities from the Wi-Fi driver via the Netlink socket and include
    // them into AP HE Capabilities TLV (#1162)

    return true;
}

bool backhaul_manager::add_link_metrics(const sMacAddr &reporter_al_mac,
                                        const sLinkInterface &link_interface,
                                        const sLinkNeighbor &link_neighbor,
                                        const sLinkMetrics &link_metrics,
                                        ieee1905_1::eLinkMetricsType link_metrics_type)
{
    /**
     * Add Transmitter Link Metric TLV if specifically requested or both requested
     */
    if ((ieee1905_1::eLinkMetricsType::TX_LINK_METRICS_ONLY == link_metrics_type) ||
        (ieee1905_1::eLinkMetricsType::BOTH_TX_AND_RX_LINK_METRICS == link_metrics_type)) {
        auto tlvTransmitterLinkMetric = cmdu_tx.addClass<ieee1905_1::tlvTransmitterLinkMetric>();
        if (!tlvTransmitterLinkMetric) {
            LOG(ERROR) << "addClass ieee1905_1::tlvTransmitterLinkMetric failed";
            return false;
        }

        tlvTransmitterLinkMetric->reporter_al_mac() = reporter_al_mac;
        tlvTransmitterLinkMetric->neighbor_al_mac() = link_neighbor.al_mac;

        if (!tlvTransmitterLinkMetric->alloc_interface_pair_info()) {
            LOG(ERROR) << "alloc_interface_pair_info failed";
            return false;
        }
        auto interface_pair_info = tlvTransmitterLinkMetric->interface_pair_info(0);
        if (!std::get<0>(interface_pair_info)) {
            LOG(ERROR) << "Failed accessing interface_pair_info";
            return false;
        }
        auto interfacePairInfo                      = std::get<1>(interface_pair_info);
        interfacePairInfo.rc_interface_mac          = link_interface.iface_mac;
        interfacePairInfo.neighbor_interface_mac    = link_neighbor.iface_mac;
        interfacePairInfo.link_metric_info.intfType = link_interface.media_type;
        // TODO
        //Indicates whether or not the 1905.1 link includes one or more IEEE 802.1 bridges
        interfacePairInfo.link_metric_info.IEEE802_1BridgeFlag =
            ieee1905_1::tlvTransmitterLinkMetric::LINK_DOES_NOT_INCLUDE_BRIDGE;
        interfacePairInfo.link_metric_info.packet_errors = link_metrics.transmitter.packet_errors;
        interfacePairInfo.link_metric_info.transmitted_packets =
            link_metrics.transmitter.transmitted_packets;
        interfacePairInfo.link_metric_info.mac_throughput_capacity =
            std::min(link_metrics.transmitter.mac_throughput_capacity_mbps,
                     static_cast<uint32_t>(UINT16_MAX));
        interfacePairInfo.link_metric_info.link_availability =
            link_metrics.transmitter.link_availability;
        interfacePairInfo.link_metric_info.phy_rate =
            std::min(link_metrics.transmitter.phy_rate_mbps, static_cast<uint32_t>(UINT16_MAX));
    }

    /**
     * Add Receiver Link Metric TLV if specifically requested or both requested
     */
    if ((ieee1905_1::eLinkMetricsType::RX_LINK_METRICS_ONLY == link_metrics_type) ||
        (ieee1905_1::eLinkMetricsType::BOTH_TX_AND_RX_LINK_METRICS == link_metrics_type)) {
        auto tlvReceiverLinkMetric = cmdu_tx.addClass<ieee1905_1::tlvReceiverLinkMetric>();
        if (!tlvReceiverLinkMetric) {
            LOG(ERROR) << "addClass ieee1905_1::tlvReceiverLinkMetric failed";
            return false;
        }

        tlvReceiverLinkMetric->reporter_al_mac() = reporter_al_mac;
        tlvReceiverLinkMetric->neighbor_al_mac() = link_neighbor.al_mac;

        if (!tlvReceiverLinkMetric->alloc_interface_pair_info()) {
            LOG(ERROR) << "alloc_interface_pair_info failed";
            return false;
        }
        auto interface_pair_info = tlvReceiverLinkMetric->interface_pair_info(0);
        if (!std::get<0>(interface_pair_info)) {
            LOG(ERROR) << "Failed accessing interface_pair_info";
            return false;
        }
        auto interfacePairInfo                           = std::get<1>(interface_pair_info);
        interfacePairInfo.rc_interface_mac               = link_interface.iface_mac;
        interfacePairInfo.neighbor_interface_mac         = link_neighbor.iface_mac;
        interfacePairInfo.link_metric_info.intfType      = link_interface.media_type;
        interfacePairInfo.link_metric_info.packet_errors = link_metrics.receiver.packet_errors;
        interfacePairInfo.link_metric_info.packets_received =
            link_metrics.receiver.packets_received;
        interfacePairInfo.link_metric_info.rssi_db = link_metrics.receiver.rssi;
    }

    return true;
}

bool backhaul_manager::handle_channel_selection_request(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                        const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();

    LOG(DEBUG) << "Forwarding CHANNEL_SELECTION_REQUEST to son_slave, mid=" << std::hex << int(mid);

    // Clear previous request, if any
    m_expected_channel_selection.requests.clear();
    m_expected_channel_selection.responses.clear();

    m_expected_channel_selection.mid = mid;

    // Save radio mac for each connected radio
    for (auto &socket : slaves_sockets) {
        m_expected_channel_selection.requests.emplace_back(socket->radio_mac);
    }

    // According to the WFA documentation, each radio should send channel selection
    // response even if that radio was not marked in the request. After filling radio
    // mac vector need to do forwarding for the channel selection request to all slaves.
    // In this scope return false forwards the message to the son_slave.
    return false;
}

bool backhaul_manager::handle_slave_channel_selection_response(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                               const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CHANNEL_SELECTION_RESPONSE message, mid=" << std::hex << int(mid);

    if (mid != m_expected_channel_selection.mid) {
        return false;
    }

    auto channel_selection_response = cmdu_rx.getClass<wfa_map::tlvChannelSelectionResponse>();
    if (!channel_selection_response) {
        LOG(ERROR) << "Failed cmdu_rx.getClass<wfa_map::tlvChannelSelectionResponse>(), mid="
                   << std::hex << int(mid);
        return false;
    }

    auto db = AgentDB::get();

    m_expected_channel_selection.responses.push_back(
        {channel_selection_response->radio_uid(), channel_selection_response->response_code()});

    // Remove an entry from the processed query
    m_expected_channel_selection.requests.erase(
        std::remove_if(m_expected_channel_selection.requests.begin(),
                       m_expected_channel_selection.requests.end(),
                       [&](sMacAddr const &query) {
                           return channel_selection_response->radio_uid() == query;
                       }),
        m_expected_channel_selection.requests.end());

    if (!m_expected_channel_selection.requests.empty()) {
        return true;
    }

    // We received all responses - prepare and send response message to the controller
    auto cmdu_header =
        cmdu_tx.create(mid, ieee1905_1::eMessageType::CHANNEL_SELECTION_RESPONSE_MESSAGE);

    if (!cmdu_header) {
        LOG(ERROR) << "Failed building IEEE1905 CHANNEL_SELECTION_RESPONSE_MESSAGE";
        return false;
    }

    for (const auto &response : m_expected_channel_selection.responses) {
        auto channel_selection_response_tlv =
            cmdu_tx.addClass<wfa_map::tlvChannelSelectionResponse>();

        if (!channel_selection_response_tlv) {
            LOG(ERROR) << "Failed addClass<wfa_map::tlvChannelSelectionResponse>";
            continue;
        }

        channel_selection_response_tlv->radio_uid()     = response.radio_mac;
        channel_selection_response_tlv->response_code() = response.response_code;
    }

    // Clear the m_expected_channel_selection.responses vector after preparing response to the controller
    m_expected_channel_selection.responses.clear();

    LOG(DEBUG) << "Sending CHANNEL_SELECTION_RESPONSE_MESSAGE, mid=" << std::hex << int(mid);
    return send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, tlvf::mac_to_string(db->bridge.mac));
}

bool backhaul_manager::handle_backhaul_steering_request(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                        const std::string &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received BACKHAUL_STA_STEERING message, mid=" << std::hex << mid;

    auto bh_sta_steering_req = cmdu_rx.getClass<wfa_map::tlvBackhaulSteeringRequest>();
    if (!bh_sta_steering_req) {
        LOG(WARNING) << "Failed cmdu_rx.getClass<wfa_map::tlvBackhaulSteeringRequest>(), mid="
                     << std::hex << mid;
        return false;
    }

    // build ACK message CMDU
    auto cmdu_tx_header = cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    LOG(DEBUG) << "Sending ACK message to the originator, mid=" << std::hex << mid;
    send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, bridge_info.mac);

    auto channel    = bh_sta_steering_req->target_channel_number();
    auto oper_class = bh_sta_steering_req->operating_class();

    auto is_valid_channel = son::wireless_utils::is_channel_in_operating_class(oper_class, channel);

    if (!is_valid_channel) {
        LOG(WARNING) << "Invalid channel number";

        auto response = create_backhaul_steering_response(
            wfa_map::tlvErrorCode::eReasonCode::
                BACKHAUL_STEERING_REQUEST_REJECTED_CANNOT_OPERATE_ON_CHANNEL_SPECIFIED);

        if (!response) {
            LOG(ERROR) << "Failed to build Backhaul STA Steering Response message.";
            return false;
        }

        send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, bridge_info.mac);

        return false;
    }

    /*
        TODO: BACKHAUL_STA_STEERING can be accepted in wired backhaul too.
              Code below is incorrect in that case.
    */
    auto active_hal = get_wireless_hal();
    if (!active_hal) {
        LOG(ERROR) << "Couldn't get active HAL";
        return false;
    }

    auto bssid = bh_sta_steering_req->target_bssid();

    auto associate = active_hal->roam(bssid, channel);
    if (!associate) {
        LOG(ERROR) << "Couldn't associate active HAL with bssid: " << bssid;

        LOG(DEBUG) << "Sending ACK message to the originator, mid=" << std::hex << mid;
        send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, bridge_info.mac);

        auto response = create_backhaul_steering_response(
            wfa_map::tlvErrorCode::eReasonCode::
                BACKHAUL_STEERING_REQUEST_REJECTED_TARGET_BSS_SIGNAL_NOT_SUITABLE);

        if (!response) {
            LOG(ERROR) << "Failed to build Backhaul STA Steering Response message.";
            return false;
        }

        send_cmdu_to_broker(cmdu_tx, controller_bridge_mac, bridge_info.mac);

        return false;
    }

    m_backhaul_sta_steering_enable = true;

    return true;
}

bool backhaul_manager::create_backhaul_steering_response(
    const wfa_map::tlvErrorCode::eReasonCode &error_code)
{
    auto cmdu_tx_header =
        cmdu_tx.create(0, ieee1905_1::eMessageType::BACKHAUL_STEERING_RESPONSE_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "Failed to create Backhoul STA Steering Response message";
        return false;
    }

    auto bh_steering_resp_tlv = cmdu_tx.addClass<wfa_map::tlvBackhaulSteeringResponse>();
    if (!bh_steering_resp_tlv) {
        LOG(ERROR) << "Couldn't addClass<wfa_map::tlvBackhaulSteeringResponse>";
        return false;
    }

    auto active_hal = get_wireless_hal();
    if (!active_hal) {
        LOG(ERROR) << "Couldn't get active HAL";
        return false;
    }

    sMacAddr sta_mac;
    auto interface = active_hal->get_iface_name();
    get_iface_mac(interface, sta_mac);

    LOG(DEBUG) << "Interface: " << interface << "MAC: " << sta_mac;

    bh_steering_resp_tlv->target_bssid()         = tlvf::mac_from_string(active_hal->get_bssid());
    bh_steering_resp_tlv->backhaul_station_mac() = sta_mac;

    if (!error_code) {
        bh_steering_resp_tlv->result_code() =
            wfa_map::tlvBackhaulSteeringResponse::eResultCode::SUCCESS;
    } else {
        bh_steering_resp_tlv->result_code() =
            wfa_map::tlvBackhaulSteeringResponse::eResultCode::FAILURE;

        auto error_code_tlv = cmdu_tx.addClass<wfa_map::tlvErrorCode>();
        if (!bh_steering_resp_tlv) {
            LOG(ERROR) << "Couldn't addClass<wfa_map::tlvErrorCode>";
            return false;
        }

        error_code_tlv->reason_code() = error_code;
    }

    return true;
}

const std::string backhaul_manager::freq_to_radio_mac(eFreqType freq) const
{
    auto db = AgentDB::get();
    for (const auto &radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }
        if (radio->front.freq_type == freq) {
            return tlvf::mac_to_string(radio->front.iface_mac);
        }
    }

    LOG(ERROR) << "Radio not found for freq " << int(freq);
    return std::string();
}

bool backhaul_manager::start_wps_pbc(const sMacAddr &radio_mac)
{
    if ((m_eFSMState == EState::OPERATIONAL)) {
        // WPS PBC registration on AP interface
        auto msg = message_com::create_vs_message<
            beerocks_message::cACTION_BACKHAUL_START_WPS_PBC_REQUEST>(cmdu_tx);
        if (!msg) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        auto it = std::find_if(
            slaves_sockets.begin(), slaves_sockets.end(),
            [&](std::shared_ptr<sRadioInfo> slave) { return slave->radio_mac == radio_mac; });
        if (it == slaves_sockets.end()) {
            LOG(ERROR) << "couldn't find slave for radio mac " << radio_mac;
            return false;
        }

        auto soc = *it;
        LOG(DEBUG) << "Start WPS PBC registration on interface " << soc->hostap_iface;
        return message_com::send_cmdu(soc->slave, cmdu_tx);
    } else {
        // WPS PBC registration on STA interface
        auto sta_wlan_hal = get_selected_backhaul_sta_wlan_hal();
        if (!sta_wlan_hal) {
            LOG(ERROR) << "Failed to get backhaul STA hal";
            return false;
        }
        return sta_wlan_hal->start_wps_pbc();
    }
}

std::shared_ptr<bwl::sta_wlan_hal> backhaul_manager::get_selected_backhaul_sta_wlan_hal()
{
    std::string selected_backhaul = m_agent_ucc_listener->get_selected_backhaul();
    auto selected_backhaul_it =
        std::find_if(slaves_sockets.begin(), slaves_sockets.end(),
                     [&selected_backhaul](std::shared_ptr<sRadioInfo> soc) {
                         return tlvf::mac_from_string(selected_backhaul) == soc->radio_mac;
                     });
    if (selected_backhaul_it == slaves_sockets.end()) {
        LOG(ERROR) << "Invalid backhaul";
        return nullptr;
    }
    return (*selected_backhaul_it)->sta_wlan_hal;
}

} // namespace beerocks
