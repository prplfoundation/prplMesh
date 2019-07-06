/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "backhaul_manager_thread.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>
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
#include <tlvf/ieee_1905_1/eMessageType.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddressType.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvSupportedService.h>

// BPL Error Codes
#include <bpl/bpl_err.h>

using namespace beerocks::net;

namespace beerocks {
namespace backhaul_manager {

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

#define UDHCPC_SCRIPT "./udhcpc.script.alt"

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void run_dhcp_client(std::future<int> &future, std::string strIface, bool fApplyLease)
{
    // Trigger the DHCP process on the interface
    LOG(DEBUG) << "Running DHCP client on '" << strIface << "'...";
    future = std::async(std::launch::async, [=]() -> int {

        std::string strCmd("/sbin/udhcpc -i " + strIface + " -f -n -q");

        if (fApplyLease)
            strCmd += " -s " + std::string(UDHCPC_SCRIPT);

        FILE *fp;
        LOG(DEBUG) << "DHCP Command: " << strCmd;
        if ((fp = popen(strCmd.c_str(), "r")) == nullptr) {
            LOG(ERROR) << "failed to run DHCP";
            return -1;
        }

        int ret = pclose(fp);
        LOG(DEBUG) << "DHCP Result: " << ret;

        return ret;
    });
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Static Members ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

const char *main_thread::s_arrStates[] = {FOREACH_STATE(GENERATE_STRING)};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

main_thread::main_thread(std::string temp_path_, std::set<std::string> slave_ap_ifaces_,
                         std::set<std::string> slave_sta_ifaces_, int stop_on_failure_attempts_,
                         std::string const_bh_slave_)
    : transport_socket_thread(temp_path_ + std::string(BEEROCKS_BACKHAUL_MGR_UDS)),
      beerocks_temp_path(temp_path_), slave_ap_ifaces(slave_ap_ifaces_),
      slave_sta_ifaces(slave_sta_ifaces_), pending_slave_ifaces(slave_ap_ifaces_),
      config_const_bh_slave(const_bh_slave_), stop_on_failure_attempts(stop_on_failure_attempts_),
      m_eFSMState(EState::INIT)
{
    thread_name = "backhaul_manager";
    LOG(DEBUG) << "stop_on_failure_attempts=" << stop_on_failure_attempts;
    configuration_stop_on_failure_attempts = stop_on_failure_attempts;
    set_select_timeout(SELECT_TIMEOUT_MSC);
}

main_thread::~main_thread() { on_thread_stop(); }

bool main_thread::init()
{
    on_thread_stop();
    if (!transport_socket_thread::init()) {
        LOG(ERROR) << "Failed init!";
        stop();
        return false;
    }

    if (!bus_subscribe(std::vector<ieee1905_1::eMessageType>{
            ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE,
            ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE,
            ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE})) {
        LOG(ERROR) << "Failed to init mapf_bus";
        return false;
    }

    return true;
}

bool main_thread::work()
{
    bool skip_select = false;
    if (!backhaul_fsm_main(skip_select))
        return false;

    if (!skip_select)
        if (!transport_socket_thread::work())
            return false;

    return true;
}

void main_thread::on_thread_stop()
{
    // Close the socket with the platform manager
    if (!m_scPlatform) {
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

void main_thread::socket_connected(Socket *sd)
{
    LOG(DEBUG) << "new slave_socket, sd=" << intptr_t(sd);
    auto soc   = std::make_shared<SSlaveSockets>();
    soc->slave = sd;
    slaves_sockets.push_back(soc);
    add_socket(soc->slave);
}

bool main_thread::socket_disconnected(Socket *sd)
{
    if (from_bus(sd)) {
        LOG(ERROR) << "bus socket to mrouter disconnected " << intptr_t(sd)
                   << " restarting backhaul manager";
        FSM_MOVE_STATE(RESTART);
        return true;
    }

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

            LOG(INFO) << "sending platform_notify: slave socket disconnected " << iface;
            platform_notify_error(BPL_ERR_BH_SLAVE_SOCKET_DISCONNECTED,
                                  "slave socket disconnected " + iface);

            it = slaves_sockets.erase(it);
            if ((m_eFSMState > EState::_WIRELESS_START_ && m_eFSMState < EState::_WIRELESS_END_) ||
                (m_eFSMState > EState::_WIRED_START_ && m_eFSMState < EState::_WIRED_END_) ||
                (soc->slave_is_backhaul_manager &&
                 m_sConfig.eType == SBackhaulConfig::EType::Wireless)) {
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
            return false;
        } else {
            ++it;
        }
    }

    return true;
}

std::string main_thread::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

void main_thread::platform_notify_error(int code, const std::string &error_data)
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

    error->code() = code;

    string_utils::copy_string(error->data(0), error_data.c_str(),
                              message::PLATFORM_ERROR_DATA_SIZE);

    LOG(ERROR) << "platform_notify_error: " << error_data;

    // Send the message
    message_com::send_cmdu(m_scPlatform.get(), cmdu_tx);
}

void main_thread::after_select(bool timeout)
{
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
}

bool main_thread::finalize_slaves_connect_state(bool fConnected,
                                                std::shared_ptr<SSlaveSockets> pSocket)
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

        std::string strIface;
        network_utils::iface_info iface_info;
        bool backhaul_manager_exist = false;

        notification->params().controller_bridge_mac =
            network_utils::mac_from_string(controller_bridge_mac);

        if (!local_gw) {

            if (m_sConfig.eType == SBackhaulConfig::EType::Wired) {
                strIface = m_sConfig.wire_iface;
            } else {
                strIface = m_sConfig.wireless_iface;
            }
            // Read the IP addresses of the bridge interface
            if (network_utils::get_iface_info(iface_info, strIface) != 0) {
                LOG(ERROR) << "Failed reading addresses for: " << strIface;
                return (false);
            }

            notification->params().gw_ipv4 = network_utils::ipv4_from_string(bridge_info.ip_gw);
            notification->params().gw_bridge_mac = network_utils::mac_from_string(bssid_bridge_mac);
            notification->params().bridge_mac    = network_utils::mac_from_string(bridge_info.mac);
            notification->params().bridge_ipv4   = network_utils::ipv4_from_string(bridge_info.ip);
            notification->params().backhaul_mac  = network_utils::mac_from_string(iface_info.mac);
            notification->params().backhaul_ipv4 = network_utils::ipv4_from_string(iface_info.ip);

            if (m_sConfig.eType == SBackhaulConfig::EType::Wired) {
                notification->params().backhaul_bssid =
                    network_utils::mac_from_string(network_utils::ZERO_MAC_STRING);
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
                    if (soc->sta_iface == m_sConfig.wireless_iface) {

                        // Mark the slave as the backhaul manager
                        soc->slave_is_backhaul_manager = true;
                        backhaul_manager_exist         = true;

                        notification->params().backhaul_bssid =
                            network_utils::mac_from_string(soc->sta_wlan_hal->get_bssid());
                        // notification->params().backhaul_freq          = utils::wifi_channel_to_freq(soc->sta_wlan_hal->get_channel()); // HACK temp disabled because of a bug on endian converter
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

            //if no controller was discovered for that slave's band, skip
            if (!sc->controller_discovered) {
                continue;
            }

            // If the notification should be sent to a specific socket, skip all other
            if (pSocket != nullptr && pSocket != sc)
                continue;
            if (sc->slave == nullptr) {
                LOG(ERROR) << "slave " << sc->hostap_iface << " socket is nullptr!";
                continue;
            }

            // note: On wired connections ore GW, the first connected slave is selected as the backhaul manager
            notification->params().is_backhaul_manager = sc->slave_is_backhaul_manager;

            if (local_gw) {
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

bool main_thread::backhaul_fsm_main(bool &skip_select)
{
    // Process internal FSMs before the main one, to prevent
    // falling into the "default" case...

    // Wired FSM
    if (m_eFSMState > EState::_WIRED_START_ && m_eFSMState < EState::_WIRED_END_) {
        return backhaul_fsm_wired(skip_select);
    }
    // Wireless FSM
    else if (m_eFSMState > EState::_WIRELESS_START_ && m_eFSMState < EState::_WIRELESS_END_) {
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
        if (!onboarding && !local_gw &&
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

        LOG(TRACE) << "backhaul manager state=ENABLED";

        // Connect to the platform manager
        if (!m_scPlatform) {
            m_scPlatform.reset();
        }

        m_scPlatform = std::make_shared<SocketClient>(
            SocketClient(beerocks_temp_path + std::string(BEEROCKS_PLAT_MGR_UDS)));
        std::string err = m_scPlatform->getError();
        if (!err.empty()) {
            m_scPlatform.reset();
            LOG(ERROR) << "Failed connecting to Platform Manager: " << err;
        }

        if (local_master && local_gw) {
            LOG(DEBUG) << "local master && local gw";
            FSM_MOVE_STATE(MASTER_DISCOVERY);
        } else { // link establish

            // Remove the sta interfaces from the bridge
            for (auto soc : slaves_sockets) {
                if (!soc->sta_iface.empty()) {
                    network_utils::linux_remove_iface_from_bridge(m_sConfig.bridge_iface,
                                                                  soc->sta_iface);
                }
            }

            // If a wired (WAN) interface was provided, try it first, check if the interface is UP
            if ((!m_sConfig.wire_iface.empty()) &&
                (wan_mon.initialize(m_sConfig.wire_iface) == wan_monitor::ELinkState::eUp) &&
                (m_sConfig.wired_backhaul)) {

                // Mark the connection as WIRED
                m_sConfig.eType = SBackhaulConfig::EType::Wired;

                LOG(DEBUG) << "add wired_backhaul to bridge " << m_sConfig.wire_iface;
                if (!network_utils::linux_add_iface_to_bridge(m_sConfig.bridge_iface,
                                                              m_sConfig.wire_iface)) {
                    LOG(ERROR) << "linux_add_iface_to_bridge failed!";
                }

            } else {
                // Mark the connection as WIRELESS
                m_sConfig.eType = SBackhaulConfig::EType::Wireless;

                if (m_sConfig.wired_backhaul) {
                    LOG(DEBUG) << "remove wired_backhaul to bridge " << m_sConfig.wire_iface;
                    if (!network_utils::linux_remove_iface_from_bridge(m_sConfig.bridge_iface,
                                                                       m_sConfig.wire_iface)) {
                        LOG(ERROR) << "linux_remove_iface_to_bridge failed!";
                    }
                }
            }

            // Move to the next state immediately
            if (m_sConfig.eType == SBackhaulConfig::EType::Wireless) {
                FSM_MOVE_STATE(INIT_HAL);
            } else { // EType::Wired
                FSM_MOVE_STATE(MASTER_DISCOVERY);
            }

            skip_select = true;
        }
        break;
    }
    case EState::MASTER_DISCOVERY: {
        if (network_utils::get_iface_info(bridge_info, m_sConfig.bridge_iface) != 0) {
            LOG(ERROR) << "Failed reading addresses from the bridge!";
            platform_notify_error(BPL_ERR_BH_READING_DATA_FROM_THE_BRIDGE, "");
            stop_on_failure_attempts--;
            FSM_MOVE_STATE(RESTART);
            break;
        }

        auto ifaces = network_utils::linux_get_iface_list_from_bridge(m_sConfig.bridge_iface);
        if (!configure_ieee1905_transport_interfaces(m_sConfig.bridge_iface, ifaces)) {
            LOG(ERROR) << "configure_ieee1905_transport_interfaces() failed!";
            FSM_MOVE_STATE(RESTART);
            break;
        }

        if (!bus_connect(beerocks_temp_path, local_master)) {
            platform_notify_error(BPL_ERR_BH_CONNECTING_TO_MASTER, "");
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
        /*
             * TODO
             * this is a workaround, need to find a better way to know each slave's band
             */
        for (auto soc : slaves_sockets) {
            if (soc->hostap_iface == "wlan0") {
                if (sent_for_2g)
                    continue;
                if (send_autoconfig_search_message(soc)) {
                    sent_for_2g = true;
                }
            } else if (soc->hostap_iface == "wlan2" || soc->hostap_iface == "wlan4") {
                if (sent_for_5g)
                    continue;
                if (send_autoconfig_search_message(soc)) {
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
        LOG(DEBUG) << "Sending notifications...";

        LOG(DEBUG) << "clearing blacklist";
        ap_blacklist.clear();

        finalize_slaves_connect_state(true);
        LOG(DEBUG) << "Sent notifications, goto OPERATIONAL state";

        eth_link_poll_timer = std::chrono::steady_clock::now();
        m_eth_link_up       = network_utils::linux_iface_is_up_and_running(m_sConfig.wire_iface);
        FSM_MOVE_STATE(OPERATIONAL);
        break;
    }
    // Backhaul manager is OPERATIONAL!
    case EState::OPERATIONAL: {
        auto now = std::chrono::steady_clock::now();
        int time_elapsed_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - eth_link_poll_timer)
                .count();
        //pooling eth link status every seconed to notice if there been a change.
        if (time_elapsed_ms > POLL_TIMER_TIMEOUT_MS) {

            eth_link_poll_timer = now;
            bool eth_link_up = network_utils::linux_iface_is_up_and_running(m_sConfig.wire_iface);
            if (eth_link_up != m_eth_link_up) {
                m_eth_link_up = network_utils::linux_iface_is_up_and_running(m_sConfig.wire_iface);
                FSM_MOVE_STATE(RESTART);
            }
        } else if (pending_enable && m_sConfig.eType != SBackhaulConfig::EType::Invalid) {
            pending_enable = false;
            finalize_slaves_connect_state(true);
        }

        break;
    }
    case EState::RESTART: {

        LOG(DEBUG) << "Restarting ...";

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

        m_sConfig.eType = SBackhaulConfig::EType::Invalid;

        controller_bridge_mac.clear();

        if (configuration_stop_on_failure_attempts && !stop_on_failure_attempts) {
            LOG(ERROR) << "Reached to max stop on failure attempts!";
            platform_notify_error(BPL_ERR_BH_STOPPED, "backhaul manager stopped");
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
        return (false);
    }
    }

    return (true);
}

bool main_thread::send_autoconfig_search_message(std::shared_ptr<SSlaveSockets> soc)
{
    ieee1905_1::tlvAutoconfigFreqBand::eValue freq_band =
        ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    /*
     * TODO
     * this is a workaround, need to find a better way to know each slave's band
     */
    if (soc->freq_type == beerocks::eFreqType::FREQ_24G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_2_4_GHZ;
    } else if (soc->freq_type == beerocks::eFreqType::FREQ_5G) {
        freq_band = ieee1905_1::tlvAutoconfigFreqBand::IEEE_802_11_5_GHZ;
    } else {
        LOG(ERROR) << "unsupported freq_type=" << int(soc->freq_type)
                   << ", iface=" << soc->hostap_iface;
        return false;
    }
    auto p_cmdu_header =
        cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE);

    auto tlvAlMacAddressType = cmdu_tx.addClass<ieee1905_1::tlvAlMacAddressType>();
    if (!tlvAlMacAddressType) {
        LOG(ERROR) << "addClass ieee1905_1::tlvAlMacAddressType failed";
        return false;
    }
    network_utils::mac_from_string(tlvAlMacAddressType->mac().mac, bridge_info.mac);

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

    LOG(DEBUG) << "sending autoconfig search message, bridge_mac=" << bridge_info.mac;
    return send_cmdu_to_bus(cmdu_tx, MULTICAST_MAC_ADDR, bridge_info.mac);
}

bool main_thread::backhaul_fsm_wired(bool &skip_select)
{
    return (true);
}

bool main_thread::backhaul_fsm_wireless(bool &skip_select)
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

        for (auto soc : slaves_sockets) {
            std::string iface = soc->sta_iface;
            if (iface.empty())
                continue;

            LOG(DEBUG) << FSM_CURR_STATE_STR << " iface: " << iface;

            // Create a HAL instance if doesn't exists
            if (!soc->sta_wlan_hal) {
                using namespace std::placeholders; // for `_1`
                soc->sta_wlan_hal = std::shared_ptr<bwl::sta_wlan_hal>(
                    sta_wlan_hal_create(
                        iface, std::bind(&main_thread::hal_event_handler, this, _1, iface)),
                    [](bwl::sta_wlan_hal *obj) {
                        if (obj)
                            sta_wlan_hal_destroy(obj);
                    });

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

            } else if (attach_state == bwl::HALState::Failed) {
                // Delete the HAL instance
                soc->sta_wlan_hal.reset();
                success = false;
                break;
            }
        }

        if (success) {
            FSM_MOVE_STATE(INITIATE_SCAN);
            state_attempts = 0; // for next state
        } else {
            if (std::chrono::steady_clock::now() > state_time_stamp_timeout) {
                LOG(ERROR) << "attach wpa timeout";
                platform_notify_error(BPL_ERR_BH_TIMEOUT_ATTACHING_TO_WPA_SUPPLICANT, "");
                stop_on_failure_attempts--;
                FSM_MOVE_STATE(RESTART);
            } else {
                UTILS_SLEEP_MSEC(1000);
            }
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

        bool success = true;
        for (auto soc : slaves_sockets) {
            if (soc->sta_iface.empty())
                continue;

            std::string iface = soc->sta_iface;
            pending_slave_sta_ifaces.insert(iface);

            if (!soc->sta_wlan_hal->initiate_scan()) {
                LOG(ERROR) << "initiate_scan for iface " << iface << " failed!";
                platform_notify_error(BPL_ERR_BH_SCAN_FAILED_TO_INITIATE_SCAN,
                                      "iface='" + iface + "'");
                ;
                success = false;
                break;
            }
            LOG(INFO) << "wait for scan results on iface " << iface;
        }

        if (!success) {
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
            platform_notify_error(BPL_ERR_BH_SCAN_TIMEOUT, "SSID='" + m_sConfig.ssid + "'");

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
                if (soc->sta_iface.empty())
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
        }

        if (wifi_reconnect_flag) {
            if (!active_hal->roam(selected_bssid, selected_bssid_channel)) {
                platform_notify_error(BPL_ERR_BH_ROAMING, "BSSID='" + selected_bssid + "'");
                wifi_reconnect_flag = false;
                stop_on_failure_attempts--;
                FSM_MOVE_STATE(RESTART);
                break;
            }
            wifi_reconnect_flag = false;

        } else {
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

                m_sConfig.wireless_iface = iface;
                active_hal               = get_wireless_hal();
            }

            if (active_hal->connect(m_sConfig.ssid, m_sConfig.pass, m_sConfig.security_type,
                                    selected_bssid, selected_bssid_channel, hidden_ssid)) {
                LOG(DEBUG) << "successful call to active_hal->connect(), bssid=" << selected_bssid
                           << ", channel=" << selected_bssid_channel
                           << ", iface=" << m_sConfig.wireless_iface;
            } else {
                LOG(ERROR) << "connect command failed for iface " << m_sConfig.wireless_iface;
                FSM_MOVE_STATE(INITIATE_SCAN);
                break;
            }
        }

        FSM_MOVE_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT);
        state_attempts           = 0;
        skip_select              = true;
        state_time_stamp_timeout = std::chrono::steady_clock::now() +
                                   std::chrono::seconds(MAX_WIRELESS_ASSOCIATE_TIMEOUT_SECONDS);
        break;
    }
    case EState::WIRELESS_ASSOCIATE_4ADDR_WAIT: {

        auto now = std::chrono::steady_clock::now();
        if (now > state_time_stamp_timeout) {
            LOG(ERROR) << "associate wait timeout";
            if (hidden_ssid) {
                if (pending_slave_sta_ifaces.empty()) {
                    LOG(ERROR) << "hidden ssid association failed for all ifaces";
                    platform_notify_error(BPL_ERR_BH_SCAN_EXCEEDED_MAXIMUM_FAILED_SCAN_ATTEMPTS,
                                          "attempts=" + std::to_string(MAX_FAILED_SCAN_ATTEMPTS) +
                                              ", SSID='" + m_sConfig.ssid + "'");
                } else {
                    FSM_MOVE_STATE(WIRELESS_ASSOCIATE_4ADDR);
                    break;
                }
            } else {
                if (!wifi_reconnect_flag && !roam_flag) {
                    stop_on_failure_attempts--;
                    platform_notify_error(BPL_ERR_BH_ASSOCIATE_4ADDR_TIMEOUT,
                                          "SSID='" + m_sConfig.ssid + "', iface='" +
                                              m_sConfig.wireless_iface + "'");
                } else {
                    FSM_MOVE_STATE(RESTART);
                    wifi_reconnect_flag = false;
                    roam_flag           = false;
                    break;
                }
                if (!selected_bssid.empty()) {
                    ap_blacklist_entry &entry = ap_blacklist[selected_bssid];
                    entry.timestamp           = now;
                    entry.attempts++;
                    LOG(DEBUG) << "updating bssid " << selected_bssid
                               << " blacklist entry, attempts=" << entry.attempts;
                }
                wifi_reconnect_flag = false;
                roam_flag           = false;
            }
            FSM_MOVE_STATE(INITIATE_SCAN);
        }
        break;
    }
    case EState::WIRELESS_BRIDGE_DHCP: {

        // Clear the wireless backhaul interface IP
        if (network_utils::linux_iface_is_up(m_sConfig.wireless_iface)) {
            network_utils::linux_iface_ctrl(m_sConfig.wireless_iface, true);
        }

        //add interface to bridge
        network_utils::linux_add_iface_to_bridge(m_sConfig.bridge_iface, m_sConfig.wireless_iface);

        // Trigger the DHCP process on the bridge and apply the leased address
        run_dhcp_client(m_ftDHCPRetCode, m_sConfig.bridge_iface, true);
        FSM_MOVE_STATE(WIRELESS_BRIDGE_DHCP_WAIT);
        break;
    }
    case EState::WIRELESS_BRIDGE_DHCP_WAIT: {
        // As long as the future is NOT valid, the command DHCP
        // command hasn't finished
        if (!m_ftDHCPRetCode.valid()) {
            state_attempts++;
            break;
        }

        // DHCP Succeeded
        if (m_ftDHCPRetCode.get() == 0) {
            LOG(DEBUG) << "DHCP Succeeded";

            // Attempt a connection to the master
            FSM_MOVE_STATE(MASTER_DISCOVERY);

            // DHCP Failed
        } else {
            LOG(ERROR) << "Failed obtaining DHCP on interface '" << m_sConfig.bridge_iface << "'";
            state_attempts++;
            FSM_MOVE_STATE(WIRELESS_BRIDGE_DHCP);
        }
        if (state_attempts == MAX_FAILED_DHCP_ATTEMPTS) {
            stop_on_failure_attempts--;
            platform_notify_error(BPL_ERR_BH_OBTAINING_DHCP_BRIDGE_INTERFACE_MAX_ATTEMPTS,
                                  "iface=" + m_sConfig.bridge_iface + ", attempts=" +
                                      std::to_string(MAX_FAILED_DHCP_ATTEMPTS));
            stop_on_failure_attempts--;
            FSM_MOVE_STATE(RESTART);
        }
        break;
    }
    case EState::WIRELESS_WAIT_FOR_RECONNECT: {
        if (std::chrono::steady_clock::now() > state_time_stamp_timeout) {
            LOG(DEBUG) << "reconnect wait timed out";

            FSM_MOVE_STATE(WIRELESS_ASSOCIATE_4ADDR);
            wifi_reconnect_flag = true;
        }
        break;
    }
    default: {
        LOG(ERROR) << "backhaul_fsm_wireless() Invalid state: " << int(m_eFSMState);
        return (false);
    }
    }
    return (true);
}

bool main_thread::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto uds_header = message_com::get_uds_header(cmdu_rx);

    if (!uds_header) {
        LOG(ERROR) << "uds_header=nullptr";
        return false;
    }

    uint16_t length     = uds_header->length;
    std::string src_mac = network_utils::mac_to_string(uds_header->src_bridge_mac);
    std::string dst_mac = network_utils::mac_to_string(uds_header->dst_bridge_mac);

    // LOG(DEBUG) << "handle_cmdu() - received msg from " << std::string(from_bus(sd) ? "bus" : "uds") << ", src=" << src_mac
    //            << ", dst=" << dst_mac << ", " << int(cmdu_rx.getMessageType()); // floods the log

    if (from_bus(sd)) {

        // Filter messages which are not destined to this agent
        if (dst_mac != MULTICAST_MAC_ADDR && dst_mac != bridge_info.mac) {
            LOG(DEBUG) << "handle_cmdu() - dropping msg, dst_mac=" << dst_mac
                       << ", local_bridge_mac=" << bridge_info.mac;
            return true;
        }

        // TODO: Add optimization of PID filtering for cases like the following:
        // 1. If VS message was sent by Controllers local agent to the controller, it is looped back.
        // 2. If IRE is sending message to the mrouter of the Controller, it will be received in Controllers backhaul manager as well,
        //    and should ignored.

        // Handle the CMDU message. If the message was processed locally
        // (by the Backhaul Manager), this function will return 'true'.
        // Otherwise, it should be forwarded to the slaves.
        if (handle_1905_1_message(cmdu_rx, src_mac)) {
            //function returns true if message doesn't need to be forwarded
            return true;
        }

        ////////// If got here, message needs to be forwarded //////////

        // Message from controller (bus) to agent (uds)
        // Send the data (uds_header + cmdu) how it is on UDS, without changing it

        // Forward cmdu to all slaves how it is on UDS, without changing it
        for (auto soc_iter : slaves_sockets) {
            if (!message_com::forward_cmdu_to_uds(soc_iter->slave, cmdu_rx, length)) {
                LOG(ERROR) << "forward_cmdu_to_uds() failed - " << print_cmdu_types(uds_header);
            }
        }

    } else { // from uds to bus or local handling (ACTION_BACKHAUL)

        // Check for local handling
        if (dst_mac == network_utils::ZERO_MAC_STRING) {
            std::shared_ptr<SSlaveSockets> soc;
            for (auto soc_iter : slaves_sockets) {
                if (soc_iter->slave == sd) {
                    soc = soc_iter;
                    break;
                }
            }

            if (soc && soc->slave == sd) {
                return handle_slave_backhaul_message(soc, cmdu_rx);
            } else {
                LOG(ERROR) << "ACTION_BACKHAUL from none slave socket!";
                return false;
            }

        } else { // Forward the data (cmdu) to bus
            // LOG(DEBUG) << "forwarding slave->master message, controller_bridge_mac=" << controller_bridge_mac << " radio_mac=" << soc->radio_mac;
            send_cmdu_to_bus(cmdu_rx, controller_bridge_mac, bridge_info.mac, length);
        }
    }

    return true;
}

bool main_thread::handle_slave_backhaul_message(std::shared_ptr<SSlaveSockets> soc,
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
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BACKHAUL_REGISTER_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BACKHAUL_REGISTER_REQUEST failed";
            return false;
        }

        soc->sta_iface.assign(request->sta_iface(message::IFACE_NAME_LENGTH));
        soc->hostap_iface.assign(request->hostap_iface(message::IFACE_NAME_LENGTH));
        soc->sta_iface_filter_low = request->sta_iface_filter_low();
        local_master              = request->local_master();
        local_gw                  = request->local_gw();
        onboarding                = request->onboarding();

        // Add the slave socket to the backhaul configuration
        m_sConfig.slave_iface_socket[soc->sta_iface] = soc;

        LOG(DEBUG) << "ACTION_BACKHAUL_REGISTER_REQUEST sta_iface=" << soc->sta_iface
                   << " local_master=" << int(local_master) << " local_gw=" << int(local_gw)
                   << " hostap_iface=" << soc->hostap_iface;

        auto register_response =
            message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_REGISTER_RESPONSE>(
                cmdu_tx);
        message_com::send_cmdu(soc->slave, cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_BACKHAUL_ENABLE: {

        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BACKHAUL_ENABLE>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BACKHAUL_ENABLE failed";
            return false;
        }

        std::string mac = network_utils::mac_to_string(request->iface_mac());
        soc->radio_mac  = mac;
        soc->freq_type  = (request->iface_is_5ghz() ? beerocks::eFreqType::FREQ_5G
                                                   : beerocks::eFreqType::FREQ_24G);
        soc->controller_discovered = false;

        LOG(DEBUG) << "ACTION_BACKHAUL_ENABLE hostap_iface=" << soc->hostap_iface
                   << " sta_iface=" << soc->sta_iface << " mac=" << mac
                   << " is_5ghz=" << int(request->iface_is_5ghz());

        // If we're already connected, send a notification to the slave
        if (FSM_IS_IN_STATE(OPERATIONAL)) {
            send_autoconfig_search_message(soc);
        } else if (pending_enable) {
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_BUSY_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building cACTION_BACKHAUL_BUSY_NOTIFICATION message!";
                break;
            }
            message_com::send_cmdu(soc->slave, cmdu_tx);
        } else {
            pending_slave_ifaces.erase(request->ap_iface(message::IFACE_NAME_LENGTH));

            if (pending_slave_ifaces.empty()) {

                LOG(DEBUG) << "All pending slaves have sent us backhaul enable!";

                /* All pending slaves have sent us backhaul enable
                     * which means we can proceed to the scan->connect->operational flow
                     */
                pending_enable = true;

                m_sConfig.bridge_iface.assign(request->bridge_iface(message::IFACE_NAME_LENGTH));

                if (local_gw) {
                    LOG(DEBUG) << "All slaves ready, proceeding, local GW, Bridge: "
                               << m_sConfig.bridge_iface;
                } else {

                    m_sConfig.preferred_bssid =
                        network_utils::mac_to_string(request->preferred_bssid());
                    m_sConfig.ssid.assign(request->ssid(message::WIFI_SSID_MAX_LENGTH));
                    m_sConfig.pass.assign(request->pass(message::WIFI_PASS_MAX_LENGTH));
                    m_sConfig.security_type = static_cast<bwl::WiFiSec>(request->security_type());
                    // Change mixed state to WPA2
                    if (m_sConfig.security_type == bwl::WiFiSec::WPA_WPA2_PSK) {
                        m_sConfig.security_type = bwl::WiFiSec::WPA2_PSK;
                    }
                    m_sConfig.wire_iface.assign(request->wire_iface(message::IFACE_NAME_LENGTH));
                    m_sConfig.wire_iface_type = (beerocks::eIfaceType)request->wire_iface_type();
                    m_sConfig.wired_backhaul  = request->wired_backhaul();
                    LOG(DEBUG) << "All slaves ready, proceeding" << std::endl
                               << "SSID: " << m_sConfig.ssid << ", Pass: " << m_sConfig.pass
                               << ", Security: " << int(m_sConfig.security_type)
                               << ", Bridge: " << m_sConfig.bridge_iface
                               << ", Wired: " << m_sConfig.wire_iface;
                }
            }

            soc->slave_iface_type = (beerocks::eIfaceType)request->wireless_iface_type();
        }
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_ROAM_REQUEST: {
        auto request = cmdu_rx.addClass<beerocks_message::cACTION_BACKHAUL_ROAM_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass ACTION_BACKHAUL_ROAM_REQUEST failed";
            return false;
        }
        roam_selected_bssid_channel = request->params().channel;
        roam_selected_bssid         = network_utils::mac_to_string(request->params().bssid);
        roam_flag                   = true;
        LOG(DEBUG) << "ACTION_BACKHAUL_ROAM_REQUEST to bssid=" << roam_selected_bssid
                   << " on channel=" << int(roam_selected_bssid_channel);
        FSM_MOVE_STATE(RESTART);
        break;
    }

    case beerocks_message::ACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST: {
        LOG(DEBUG) << "ACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST received from iface "
                   << soc->sta_iface;
        auto request_in = cmdu_rx.addClass<
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
            cmdu_rx
                .addClass<beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST failed";
            return false;
        }
        std::string sta_mac = network_utils::mac_to_string(request->params().mac);
        bool ap_busy        = false;
        bool fapi_error     = false;
        if (unassociated_rssi_measurement_header_id == -1) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building "
                              "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE message!";
                break;
            }
            response->mac() = network_utils::mac_from_string(sta_mac);
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
                fapi_error = true;
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

        if (ap_busy || fapi_error) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE "
                              "message!";
                break;
            }
            response->params().result.mac        = request->params().mac;
            response->params().rx_rssi    = beerocks::RSSI_INVALID;
            response->params().rx_snr     = beerocks::SNR_INVALID;
            response->params().rx_packets = -1;
            message_com::send_cmdu(soc->slave, cmdu_tx);
        }
        break;
    }
    default: {
        LOG(ERROR) << "Unhandled message received from master: "
                   << int(beerocks_header->action_op());
        return false;
    }
    }

    return true;
}

bool main_thread::handle_1905_1_message(ieee1905_1::CmduMessageRx &cmdu_rx,
                                        const std::string &src_mac)
{
    /*
     * return values:
     * true if the message was handled by the backhaul manager
     * false if the message needs to be forwarded by the calling function
     */
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RESPONSE_MESSAGE: {
        return handle_1905_autoconfiguration_response(cmdu_rx, src_mac);
    }
    default: {
        // TODO add a warning once all vendor specific flows are replaced with EasyMesh
        // flows, since we won't expect a 1905 message not handled in this function
        return false;
    }
    }
}

bool main_thread::handle_1905_autoconfiguration_response(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                         const std::string &src_mac)
{
    LOG(DEBUG) << "received autoconfig response message";

    if (!controller_bridge_mac.empty() && src_mac != controller_bridge_mac) {
        LOG(INFO) << "current controller_bridge_mac=" << controller_bridge_mac
                  << " but response came from src_mac=" << src_mac << ", ignoring";
        return true;
    }

    auto tlvSupportedRole = cmdu_rx.addClass<ieee1905_1::tlvSupportedRole>();
    if (tlvSupportedRole) {
        LOG(DEBUG) << "tlvSupportedRole->value()=" << int(tlvSupportedRole->value());
        if (tlvSupportedRole->value() != ieee1905_1::tlvSupportedRole::REGISTRAR) {
            LOG(ERROR) << "invalid tlvSupportedRole value";
            return true;
        }
    } else {
        LOG(ERROR) << "tlvSupportedRole missing - ignoring autconfig search message";
        return true;
    }

    controller_bridge_mac = src_mac;
    LOG(INFO) << "update controller_bridge_mac=" << controller_bridge_mac;

    auto tlvSupportedFreqBand = cmdu_rx.addClass<ieee1905_1::tlvSupportedFreqBand>();
    if (tlvSupportedFreqBand) {
        LOG(DEBUG) << "tlvSupportedFreqBand->value()=" << int(tlvSupportedFreqBand->value());

        if (tlvSupportedFreqBand->value() == ieee1905_1::tlvSupportedFreqBand::BAND_2_4G) {
            LOG(DEBUG) << "received auto-config reply for 2.4GHz band";
            for (auto soc : slaves_sockets) {
                if (soc->freq_type == beerocks::eFreqType::FREQ_24G) {
                    LOG(DEBUG) << "2.4GHz band socket found, iface=" << soc->hostap_iface;
                    if (!soc->controller_discovered) {
                        LOG(DEBUG) << "set controller_discovered to true for 2.4GHz band, iface="
                                   << soc->hostap_iface;
                        LOG(DEBUG) << FSM_CURR_STATE_STR;
                        soc->controller_discovered = true;
                        controller_bridge_mac      = src_mac;
                        if (FSM_IS_IN_STATE(OPERATIONAL) || FSM_IS_IN_STATE(CONNECTED)) {
                            finalize_slaves_connect_state(true, soc);
                        }
                    }
                }
            }
        } else if (tlvSupportedFreqBand->value() == ieee1905_1::tlvSupportedFreqBand::BAND_5G) {
            LOG(DEBUG) << "received auto-config reply for 5GHz band";
            for (auto soc : slaves_sockets) {
                if (soc->freq_type == beerocks::eFreqType::FREQ_5G) {
                    LOG(DEBUG) << "5GHz band socket found, iface=" << soc->hostap_iface;
                    if (!soc->controller_discovered) {
                        LOG(DEBUG) << "set controller_discovered to true for 5GHz band, iface="
                                   << soc->hostap_iface;
                        LOG(DEBUG) << FSM_CURR_STATE_STR;
                        soc->controller_discovered = true;
                        controller_bridge_mac      = src_mac;
                        if (FSM_IS_IN_STATE(OPERATIONAL) || FSM_IS_IN_STATE(CONNECTED)) {
                            finalize_slaves_connect_state(true, soc);
                        }
                    }
                }
            }
        } else if (tlvSupportedFreqBand->value() == ieee1905_1::tlvSupportedFreqBand::BAND_60G) {
            LOG(DEBUG) << "received auto-config reply for 60GHz band, unsupported";
            return true;
        } else {
            LOG(ERROR) << "invalid tlvSupportedFreqBand value";
            return true;
        }
    } else {
        LOG(ERROR) << "tlvSupportedFreqBand missing - ignoring autconfig search message";
        return true;
    }

    auto tlvSupportedService = cmdu_rx.addClass<wfa_map::tlvSupportedService>();
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
        LOG(ERROR) << "tlvSupportedService missing - ignoring autconfig search message";
        return true;
    }

    if (FSM_IS_IN_STATE(WAIT_FOR_AUTOCONFIG_RESPONSE_MESSAGE)) {
        FSM_MOVE_STATE(CONNECT_TO_MASTER);
    } else {
        LOG(TRACE) << "no state change";
    }
    return true;
}

bool main_thread::hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr,
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

        if (iface == m_sConfig.wireless_iface && !hidden_ssid) {
            //this is generally not supposed to happen
            LOG(WARNING) << "event iface != wireless iface!";
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
                            if (FSM_IS_IN_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)) {
                                FSM_MOVE_STATE(WIRELESS_CONFIG_4ADDR_MODE);
                            }
                            break;
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
                            if (FSM_IS_IN_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)) {
                                FSM_MOVE_STATE(WIRELESS_CONFIG_4ADDR_MODE);
                            }
                            break;
                        }
                    }
                }
            }
            roam_flag           = false;
            wifi_reconnect_flag = false;
            state_attempts      = 0;
            if (FSM_IS_IN_STATE(WIRELESS_ASSOCIATE_4ADDR_WAIT)) {
                FSM_MOVE_STATE(WIRELESS_BRIDGE_DHCP);
            }
        } else if (FSM_IS_IN_STATE(WIRELESS_WAIT_FOR_RECONNECT)) {
            LOG(DEBUG) << "reconnected successfully, continuing";
            if (local_master && !local_gw) { // ire_master
                FSM_MOVE_STATE(CONNECT_TO_MASTER);
            } else {
                FSM_MOVE_STATE(OPERATIONAL);
            }
        }

    } break;

    case Event::Disconnected: {

        if (iface == m_sConfig.wireless_iface) {
            if (FSM_IS_IN_STATE(OPERATIONAL) || FSM_IS_IN_STATE(CONNECTED)) {
                platform_notify_error(BPL_ERR_BH_DISCONNECTED,
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
                wifi_reconnect_flag = false;
                roam_flag           = false;
                auto msg =
                    static_cast<bwl::sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION *>(data);
                if (msg->disconnect_reason == uint32_t(DEAUTH_REASON_PASSPHRASE_MISMACH)) {
                    //enter bssid to black_list trigger timer
                    auto local_time_stamp = std::chrono::steady_clock::now();
                    auto local_bssid      = network_utils::mac_to_string(msg->bssid);
                    LOG(DEBUG) << "insert bssid = " << local_bssid << " to backhaul blacklist";
                    ap_blacklist_entry entry;
                    entry.timestamp           = local_time_stamp;
                    entry.attempts            = AP_BLACK_LIST_FAILED_ATTEMPTS_THRESHOLD;
                    ap_blacklist[local_bssid] = entry;
                    platform_notify_error(BPL_ERR_BH_ASSOCIATE_4ADDR_FAILURE,
                                          "SSID='" + m_sConfig.ssid + "', BSSID='" + local_bssid +
                                              "', DEAUTH_REASON='" +
                                              std::to_string(msg->disconnect_reason));
                    stop_on_failure_attempts--;
                    FSM_MOVE_STATE(INITIATE_SCAN);
                }

            } else {
                platform_notify_error(BPL_ERR_BH_DISCONNECTED,
                                      "Backhaul disconnected non operational state");
                stop_on_failure_attempts--;
                FSM_MOVE_STATE(RESTART);
            }
        }

    } break;

    case Event::Terminating: {

        LOG(DEBUG) << "wpa_supplicant terminated, restarting";
        platform_notify_error(BPL_ERR_BH_WPA_SUPPLICANT_TERMINATED, "wpa_supplicant terminated");
        stop_on_failure_attempts--;
        FSM_MOVE_STATE(RESTART);

    } break;

    case Event::ScanResults: {

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
                   << network_utils::mac_to_string(msg->params.result.mac)
                   << " id = " << unassociated_rssi_measurement_header_id;

        if (unassociated_rssi_measurement_header_id > -1) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(cmdu_tx);

            std::copy_n(msg->params.result.mac.oct, sizeof(msg->params.result.mac.oct),
                        response->params().result.mac.oct);
            response->params().result.channel       = msg->params.result.channel;
            response->params().result.rssi          = msg->params.result.rssi;
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
}

bool main_thread::select_bssid()
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
    net::sScanResult scan_results[256];

    LOG(DEBUG) << "select_bssid: SSID = " << m_sConfig.ssid;

    for (auto soc : slaves_sockets) {

        if (soc->sta_iface.empty()) {
            LOG(DEBUG) << "skipping empty iface";
            continue;
        }

        std::string iface = soc->sta_iface;

        LOG(DEBUG) << "select_bssid: iface  = " << iface;
        int num_of_results = soc->sta_wlan_hal->get_scan_results(m_sConfig.ssid, scan_results, 256);
        LOG(DEBUG) << "Scan Results: " << num_of_results;

        for (int i = 0; i < num_of_results; i++) {
            auto &scan_result = scan_results[i];

            auto bssid = network_utils::mac_to_string(scan_result.mac);
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
                    m_sConfig.wireless_iface = iface;
                    return true;
                }
            } else if (!m_sConfig.preferred_bssid.empty() && bssid == m_sConfig.preferred_bssid) {
                LOG(DEBUG) << "preferred bssid - found bssid match = " << bssid;
                selected_bssid_channel   = scan_result.channel;
                selected_bssid           = bssid;
                m_sConfig.wireless_iface = iface;
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
        selected_bssid           = best_bssid_5;
        selected_bssid_channel   = best_bssid_channel_5;
        m_sConfig.wireless_iface = best_5_sta_iface;
    } else if (max_rssi_5_best == beerocks::RSSI_INVALID) {
        selected_bssid           = best_bssid_24;
        selected_bssid_channel   = best_bssid_channel_24;
        m_sConfig.wireless_iface = best_24_sta_iface;
    } else if ((max_rssi_5_best > RSSI_THRESHOLD_5GHZ)) {
        selected_bssid           = best_bssid_5;
        selected_bssid_channel   = best_bssid_channel_5;
        m_sConfig.wireless_iface = best_5_sta_iface;
    } else if (max_rssi_24 < max_rssi_5_best + RSSI_BAND_DELTA_THRESHOLD) {
        selected_bssid           = best_bssid_5;
        selected_bssid_channel   = best_bssid_channel_5;
        m_sConfig.wireless_iface = best_5_sta_iface;
    } else {
        selected_bssid           = best_bssid_24;
        selected_bssid_channel   = best_bssid_channel_24;
        m_sConfig.wireless_iface = best_24_sta_iface;
    }

    if (!get_wireless_hal()) {
        LOG(ERROR) << "Slave for interface " << m_sConfig.wireless_iface << " NOT found!";
        return false;
    }

    return true;
}

void main_thread::get_scan_measurement()
{

    // Support up to 256 scan results
    net::sScanResult scan_results[256];

    LOG(DEBUG) << "get_scan_measurement: SSID = " << m_sConfig.ssid;
    scan_measurement_list.clear();
    for (auto &soc : slaves_sockets) {

        if (soc->sta_iface.empty()) {
            LOG(DEBUG) << "skipping empty iface";
            continue;
        }

        std::string iface = soc->sta_iface;
        LOG(DEBUG) << "get_scan_measurement: iface  = " << iface;
        int num_of_results = soc->sta_wlan_hal->get_scan_results(m_sConfig.ssid, scan_results, 256);
        LOG(DEBUG) << "Scan Results: " << int(num_of_results);
        if (num_of_results < 0) {
            LOG(ERROR) << "get_scan_results failed!";
            return;
        } else if (num_of_results == 0) {
            continue;
        }

        for (int i = 0; i < num_of_results; i++) {
            auto &scan_result = scan_results[i];

            auto bssid = network_utils::mac_to_string(scan_result.mac);
            LOG(DEBUG) << "get_scan_measurement: bssid = " << bssid
                       << ", channel = " << int(scan_result.channel) << " iface = " << iface;

            auto it = std::find_if(scan_measurement_list.begin(), scan_measurement_list.end(),
                        [bssid](const std::pair<std::string, net::sScanResult> & t) -> bool
                        { return t.second.mac == network_utils::mac_from_string(bssid); });
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

                std::copy_n(scan_result.mac.oct, MAC_ADDR_LEN, scan_measurement.mac.oct);
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

std::shared_ptr<bwl::sta_wlan_hal> main_thread::get_wireless_hal(std::string iface)
{
    // If the iface argument is empty, use the default wireless interface
    if (iface.empty()) {
        iface = m_sConfig.wireless_iface;
    }

    auto slave_sk = m_sConfig.slave_iface_socket.find(iface);
    if (slave_sk == m_sConfig.slave_iface_socket.end()) {
        return {};
    }

    return slave_sk->second->sta_wlan_hal;
}

} // namespace beerocks
} //namespace backhaul_manager
