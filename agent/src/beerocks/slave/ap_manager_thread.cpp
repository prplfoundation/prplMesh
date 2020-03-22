/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_manager_thread.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_apmanager.h>

using namespace beerocks::net;

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define SELECT_TIMEOUT_MSC 200
#define ACS_READ_SLEEP_USC 1000
#define READ_ACS_ATTEMPT_MAX 5
#define DISABLE_BACKHAUL_VAP_TIMEOUT_SEC 30
#define OPERATION_SUCCESS 0
#define OPERATION_FAIL -1

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void copy_radio_supported_channels(std::shared_ptr<bwl::ap_wlan_hal> &ap_wlan_hal,
                                          beerocks::message::sWifiChannel supported_channels[])
{
    auto radio_channels = ap_wlan_hal->get_radio_info().supported_channels;

    // Copy the channels
    for (uint i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH && i < radio_channels.size();
         i++) {

        supported_channels[i].channel        = radio_channels[i].channel;
        supported_channels[i].noise          = radio_channels[i].noise;
        supported_channels[i].tx_pow         = radio_channels[i].tx_pow;
        supported_channels[i].bss_overlap    = radio_channels[i].bss_overlap;
        supported_channels[i].is_dfs_channel = radio_channels[i].is_dfs;
        supported_channels[i].channel_bandwidth =
            uint8_t(beerocks::utils::convert_bandwidth_to_enum(radio_channels[i].bandwidth));
    }
}

static std::string
get_radio_supported_channels_string(std::shared_ptr<bwl::ap_wlan_hal> &ap_wlan_hal)
{
    std::ostringstream os;
    for (auto val : ap_wlan_hal->get_radio_info().supported_channels) {
        if (val.channel > 0) {
            os << " ch = " << int(val.channel) << " | dfs = " << int(val.tx_pow)
               << " | tx_pow = " << int(val.is_dfs) << " | noise = " << int(val.noise)
               << " [dbm] | bss_overlap = " << int(val.bss_overlap) << std::endl;
        }
    }

    return os.str();
}

static void
copy_vaps_info(std::shared_ptr<bwl::ap_wlan_hal> &ap_wlan_hal,
               std::list<son::ap_manager_thread::backhaul_vap_list_element_t> &backhaul_vaps_list,
               beerocks_message::sVapInfo vaps[])
{
    if (!ap_wlan_hal->refresh_vaps_info()) {
        LOG(ERROR) << "Failed to refresh vaps info!";
        return;
    }

    const auto &radio_vaps = ap_wlan_hal->get_radio_info().available_vaps;

    // Copy the VAPs
    for (int vap_id = beerocks::IFACE_VAP_ID_MIN, i = 0; vap_id < beerocks::IFACE_VAP_ID_MAX;
         vap_id++, i++) {

        // Clear the memory
        vaps[i] = {};

        // If the VAP ID exists
        if (radio_vaps.find(vap_id) != radio_vaps.end()) {
            const auto &curr_vap = radio_vaps.at(vap_id);

            // mark backhaul vaps
            auto it =
                std::find_if(backhaul_vaps_list.begin(), backhaul_vaps_list.end(),
                             [&](const son::ap_manager_thread::backhaul_vap_list_element_t &vap) {
                                 return (vap.bssid == curr_vap.mac);
                             });

            bool bVAP = (it != backhaul_vaps_list.end());

            LOG(DEBUG) << "vap_id = " << int(vap_id) << ", mac = " << curr_vap.mac
                       << ", ssid = " << curr_vap.ssid
                       << ", backhaul_vap = " << beerocks::string_utils::bool_str(bVAP);

            // Copy the VAP MAC and SSID
            vaps[i].mac = network_utils::mac_from_string(curr_vap.mac);
            beerocks::string_utils::copy_string((char *)vaps[i].ssid, curr_vap.ssid.c_str(),
                                                beerocks::message::WIFI_SSID_MAX_LENGTH);
            vaps[i].backhaul_vap = bVAP;
        }
    }
}

static bool enable_backhaul_vap(
    std::shared_ptr<bwl::ap_wlan_hal> ap_wlan_hal,
    std::list<son::ap_manager_thread::backhaul_vap_list_element_t> &backhaul_vaps_list,
    std::set<std::string> &connected_ires, bool enable, uint8_t vap_id = beerocks::IFACE_ID_INVALID)
{

    if (!enable && (connected_ires.size() == backhaul_vaps_list.size() - 1))
        return false;

    uint8_t vap_idx = 0;
    for (auto &vap : backhaul_vaps_list) {
        if (enable && !vap.enabled) {
            // always enable the first disabled
            vap.enabled         = true;
            auto vap_iface_name = beerocks::utils::get_iface_string_from_iface_vap_ids(
                ap_wlan_hal->get_radio_info().iface_name, vap_idx);
            return ap_wlan_hal->set_vap_enable(vap_iface_name, true);
        } else if (!enable && vap_id == vap_idx) {
            // always disable a specific vap_id
            vap.enabled         = false;
            auto vap_iface_name = beerocks::utils::get_iface_string_from_iface_vap_ids(
                ap_wlan_hal->get_radio_info().iface_name, vap_idx);
            return ap_wlan_hal->set_vap_enable(vap_iface_name, false);
        }
        vap_idx++;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

using namespace beerocks;
using namespace son;

ap_manager_thread::ap_manager_thread(std::string slave_uds_)
    : socket_thread(), bss_steer_valid_int(BSS_STEER_VALID_INT),
      bss_steer_imminent_valid_int(BSS_STEER_IMMINENT_VALID_INT)
{
    thread_name = "ap_manager";
    slave_uds   = slave_uds_;
    set_select_timeout(SELECT_TIMEOUT_MSC);
}

ap_manager_thread::~ap_manager_thread() { stop_ap_manager_thread(); }

void ap_manager_thread::ap_manager_config(ap_manager_conf_t &conf)
{
    acs_enabled  = (conf.channel == 0);
    wifi_channel = conf.channel;
    low_filter   = conf.iface_filter_low;

    // initialize backhaul_vaps_list
    int num_of_elements = sizeof(beerocks_message::sPlatformSettings::backhaul_vaps_bssid) /
                          sizeof(beerocks_message::sPlatformSettings::backhaul_vaps_bssid[0]);
    for (int i = 0; i < num_of_elements; i++) {
        backhaul_vap_list_element_t element = {};
        element.bssid = network_utils::mac_to_string(conf.backhaul_vaps_bssid[i]);
        element.type  = eBackhaulVapType(i % 3);
        backhaul_vaps_list.push_back(std::move(element));
    }

    using namespace std::placeholders; // for `_1`

    bwl::hal_conf_t hal_conf;
    hal_conf.ap_acs_enabled = acs_enabled;

    // Create a new AP HAL instance
    ap_wlan_hal = bwl::ap_wlan_hal_create(
        conf.hostap_iface, hal_conf, std::bind(&ap_manager_thread::hal_event_handler, this, _1));

    LOG_IF(!ap_wlan_hal, FATAL) << "Failed creating HAL instance!";
}

void ap_manager_thread::on_thread_stop() { stop_ap_manager_thread(); }

bool ap_manager_thread::init()
{
    stop_ap_manager_thread();

    if (!ap_wlan_hal->wds_set_mode(bwl::ap_wlan_hal::WDSMode::Dynamic)) {
        LOG(ERROR) << "Failed to enabling WDS Dynamic mode!";
        return false;
    }

    //connect to slave //
    slave_socket    = new SocketClient(slave_uds);
    std::string err = slave_socket->getError();
    if (!err.empty()) {
        LOG(ERROR) << "slave_socket: " << err;
        delete slave_socket;
        slave_socket = nullptr;
        return false;
    } else {
        add_socket(slave_socket);
    }

    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_INIT_DONE_NOTIFICATION>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    message_com::send_cmdu(slave_socket, cmdu_tx);

    return true;
}

void ap_manager_thread::after_select(bool timeout)
{
    // continue only if slave is connected
    if (slave_socket == nullptr) {
        return;
    }

    auto now = std::chrono::steady_clock::now();

    for (auto it = pending_4addr_stas.begin(); it != pending_4addr_stas.end();) {
        int time_elapsed_4addr_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.joined_timestamp)
                .count();
        if (time_elapsed_4addr_ms > beerocks::IRE_MAX_WIRELESS_RECONNECTION_TIME_MSC) {
            std::string mac = it->first;
            LOG(DEBUG) << "4addr rejoin timed out for sta mac " << mac
                       << " deleting from 4addr list";

            if (it->second.pending_reconnect == true) {
                if (connected_ires.find(mac) != connected_ires.end()) {
                    connected_ires.erase(mac);
                }
                auto notification = message_com::create_vs_message<
                    beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION>(cmdu_tx);

                if (notification == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return;
                }
                notification->params().mac = network_utils::mac_from_string(mac);
                message_com::send_cmdu(slave_socket, cmdu_tx);
            }
            it = pending_4addr_stas.erase(it);
        } else {
            ++it;
        }
    }
    /////////

    pending_disable_vaps.remove_if([&](pending_disable_vap_t &pending_vap) {
        if (std::chrono::steady_clock::now() > pending_vap.timeout) {
            enable_backhaul_vap(ap_wlan_hal, backhaul_vaps_list, connected_ires, false,
                                pending_vap.vap_id);
            return true;
        }
        return false;
    });

    if (ap_hal_int_events == nullptr) { // ap not attached
        auto attach_state = ap_wlan_hal->attach();

        if (attach_state == bwl::HALState::Operational) {

            // External events
            int ext_events_fd = ap_wlan_hal->get_ext_events_fd();
            if (ext_events_fd > 0) {
                ap_hal_ext_events = new Socket(ext_events_fd);
                add_socket(ap_hal_ext_events);
                // No external event FD is available, we will trigger the process method periodically
            } else if (ext_events_fd == 0) {
                ap_hal_ext_events = nullptr;
            } else {
                LOG(ERROR) << "Invalid external event file descriptor: " << ext_events_fd;
                thread_last_error_code = APMANAGER_THREAD_ERROR_ATTACH_FAIL;
                stop_ap_manager_thread();
                return;
            }

            // Internal events
            int int_events_fd = ap_wlan_hal->get_int_events_fd();
            if (int_events_fd > 0) {

                // NOTE: Eventhough the internal events are not socket based, at this
                //       point we have to use the Socket class wrapper to add the
                //       file descriptor into the select()
                ap_hal_int_events = new Socket(int_events_fd);
                add_socket(ap_hal_int_events);
            } else {
                LOG(ERROR) << "Invalid internal event file descriptor: " << int_events_fd;
                thread_last_error_code = APMANAGER_THREAD_ERROR_ATTACH_FAIL;
                stop_ap_manager_thread();
                return;
            }

            // remove irrelevant bssids from backhaul vap list
            eBackhaulVapType current_type;
            if (!ap_wlan_hal->get_radio_info().is_5ghz) {
                current_type = BH_VAP_TYPE_2G;
            } else if (low_filter) {
                current_type = BH_VAP_TYPE_5G_SECONDARY;
            } else {
                current_type = BH_VAP_TYPE_5G;
            }

            backhaul_vaps_list.remove_if([&](const backhaul_vap_list_element_t &element) {
                return (element.type != current_type);
            });

            // enable first vap
            if (enable_backhaul_vap(ap_wlan_hal, backhaul_vaps_list, connected_ires, true)) {
                // update master about the updated vap list
                // Note: this could be removed if bwl vap list will contain disabled backhaul vaps
                ap_wlan_hal->refresh_vaps_info();
            }

            // Set the time for the first radio info polling
            next_heartbeat_notification_timestamp =
                std::chrono::steady_clock::now() +
                std::chrono::seconds(HEARTBEAT_NOTIFICATION_DELAY_SEC);
        } else if (attach_state == bwl::HALState::Failed) {
            LOG(ERROR) << "Failed attaching to WLAN HAL, call stop_ap_manager_thread()";
            thread_last_error_code = APMANAGER_THREAD_ERROR_ATTACH_FAIL;
            stop_ap_manager_thread();
            return;
        } else {
            LOG(INFO) << "waiting to attach to " << ap_wlan_hal->get_radio_info().iface_name;
            UTILS_SLEEP_MSEC(200);
        }

        // AP is attached
    } else {
        // Process external events
        if (ap_hal_ext_events) {
            if (read_ready(ap_hal_ext_events)) {
                clear_ready(ap_hal_ext_events);
                if (!ap_wlan_hal->process_ext_events()) {
                    LOG(ERROR) << "process_ext_events() failed!";
                    thread_last_error_code = APMANAGER_THREAD_ERROR_REPORT_PROCESS_FAIL;
                    stop_ap_manager_thread();
                    return;
                }
            }
        } else {
            // There is no socket for external events, so we simply try
            // to process any available periodically
            if (!ap_wlan_hal->process_ext_events()) {
                LOG(ERROR) << "process_ext_events() failed!";
                thread_last_error_code = APMANAGER_THREAD_ERROR_REPORT_PROCESS_FAIL;
                stop_ap_manager_thread();
                return;
            }
        }

        // Process internal events
        if (read_ready(ap_hal_int_events)) {
            // A callback (hal_event_handler()) will invoked for pending events
            clear_ready(ap_hal_int_events);
            if (!ap_wlan_hal->process_int_events()) {
                LOG(ERROR) << "process_int_events() failed!";
                thread_last_error_code = APMANAGER_THREAD_ERROR_REPORT_PROCESS_FAIL;
                stop_ap_manager_thread();
                return;
            }
        }

        now = std::chrono::steady_clock::now();
        if (now > next_heartbeat_notification_timestamp) {
            send_heartbeat();
            next_heartbeat_notification_timestamp =
                now + std::chrono::seconds(HEARTBEAT_NOTIFICATION_DELAY_SEC);
        }
    }
}

bool ap_manager_thread::socket_disconnected(Socket *sd)
{
    if (slave_socket && (sd == slave_socket)) {
        LOG(ERROR) << "slave socket disconnected!";
        stop_ap_manager_thread();
        return false;
    } else if (ap_hal_ext_events && (sd == ap_hal_ext_events)) {
        LOG(ERROR) << "ap_hal_ext_events disconnected!";
        thread_last_error_code = APMANAGER_THREAD_ERROR_HAL_DISCONNECTED;
        stop_ap_manager_thread();
        return false;
    } else if (ap_hal_int_events && (sd == ap_hal_int_events)) {
        LOG(ERROR) << "ap_hal_int_events socket disconnected!";
        thread_last_error_code = APMANAGER_THREAD_ERROR_HAL_DISCONNECTED;
        stop_ap_manager_thread();
        return false;
    }
    return true;
}

std::string ap_manager_thread::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

bool ap_manager_thread::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header == nullptr) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    if (beerocks_header->action() != beerocks_message::ACTION_APMANAGER) {
        LOG(ERROR) << "Unsupported action: " << int(beerocks_header->action())
                   << " op=" << int(beerocks_header->action_op());
        return false;
    } else if (slave_socket != sd) {
        LOG(ERROR) << "slave_socket != sd";
        return false;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_APMANAGER_ENABLE_APS_REQUEST: {
        LOG(TRACE) << "ACTION_APMANAGER_ENABLE_APS_REQUEST";

        auto notification =
            beerocks_header->addClass<beerocks_message::cACTION_APMANAGER_ENABLE_APS_REQUEST>();
        if (!notification) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_ENABLE_APS_REQUEST failed";
            return false;
        }

        auto response =
            message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_ENABLE_APS_RESPONSE>(
                cmdu_tx);

        if (!response) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        response->success() = true;

        // Enable beaconing by setting start_disabled flag to false.
        if (!ap_wlan_hal->set_start_disabled(false)) {
            LOG(ERROR) << "Failed setting start_disabled";
            response->success() = false;
            message_com::send_cmdu(slave_socket, cmdu_tx);
            break;
        }

        // Disable the radio interface to make hostapd to consider the new configuration.
        if (!ap_wlan_hal->disable()) {
            LOG(ERROR) << "Failed disable";
            response->success() = false;
            message_com::send_cmdu(slave_socket, cmdu_tx);
            break;
        }

        // If it is not the radio of the BH, then channel, bandwidth and center channel paramenters
        // will be all set to 0.
        LOG(DEBUG) << "Setting AP channel: "
                   << ", channel=" << int(notification->channel())
                   << ", bandwidth=" << int(notification->bandwidth())
                   << ", center_channel=" << int(notification->center_channel());

        // Set original channel or BH channel
        if (!ap_wlan_hal->set_channel(notification->channel(), notification->bandwidth(),
                                      notification->center_channel())) {
            LOG(ERROR) << "Failed setting set_channel";
            response->success() = false;
            message_com::send_cmdu(slave_socket, cmdu_tx);
            break;
        }

        // Enable the radio interface to apply the new configuration.
        if (!ap_wlan_hal->enable()) {
            LOG(ERROR) << "Failed enable";
            response->success() = false;
        }

        LOG(INFO) << "send ACTION_APMANAGER_ENABLE_APS_RESPONSE, success="
                  << int(response->success());
        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST: {

        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass "
                          "cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST failed";
            return false;
        }

        bool success = true;

        uint32_t failsafe_channel = request->params().failsafe_channel;

        if (request->params().restricted_channels[0] == 0) {
            LOG(INFO) << "Clearing restricted channels...";
        } else {
            LOG(INFO) << "Setting radio restricted channels";
        }

        if (!ap_wlan_hal->restricted_channels_set((char *)request->params().restricted_channels)) {
            LOG(ERROR) << "Failed setting restricted channels!";
            success = false;
        }

        if (failsafe_channel != 0) {
            // Send channel_switch_request to bwl
            LOG(INFO) << " Calling failsafe_channel_set - "
                      << "failsafe_channel: " << failsafe_channel << ", channel_bandwidth: "
                      << beerocks::utils::convert_bandwidth_to_int(
                             (beerocks::eWiFiBandwidth)request->params().failsafe_channel_bandwidth)
                      << ", vht_center_frequency: " << request->params().vht_center_frequency;

            if (!ap_wlan_hal->failsafe_channel_set(failsafe_channel,
                                                   request->params().failsafe_channel_bandwidth,
                                                   request->params().vht_center_frequency)) {

                LOG(ERROR) << "Failed setting failsafe channel!";
                success = false;
            }

        } else {
            LOG(INFO) << "failsafe channel is zero";
        }

        LOG(INFO)
            << "send ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE success = "
            << int(success);
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE>(
            cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        response->success() = success;
        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START failed";
            return false;
        }
        LOG(DEBUG) << "ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START: requested channel="
                   << int(request->cs_params().channel) << " bandwidth="
                   << beerocks::utils::convert_bandwidth_to_int(
                          (beerocks::eWiFiBandwidth)request->cs_params().bandwidth);

        LOG_IF(request->cs_params().channel == 0, DEBUG) << "Start ACS";

        // Set transmit power
        if (request->tx_limit_valid()) {
            ap_wlan_hal->set_tx_power_limit(request->tx_limit());
            if (ap_wlan_hal->get_radio_info().channel == request->cs_params().channel) {
                LOG(DEBUG) << "Setting tx power without channel switch, send CSA notification";
                auto notification = message_com::create_vs_message<
                    beerocks_message::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION>(cmdu_tx);
                if (!notification) {
                    LOG(ERROR) << "Failed building message!";
                    return false;
                }
                ap_wlan_hal->refresh_radio_info();
                fill_cs_params(notification->cs_params());
                message_com::send_cmdu(slave_socket, cmdu_tx);
            }
        }

        // Set AP channel
        if (ap_wlan_hal->get_radio_info().channel != request->cs_params().channel &&
            !ap_wlan_hal->switch_channel(request->cs_params().channel,
                                         request->cs_params().bandwidth,
                                         request->cs_params().vht_center_frequency)) { //error
            std::string error("Failed to set AP channel!");
            LOG(ERROR) << error;

            ap_wlan_hal->refresh_radio_info();

            // Send the error reponse
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION>(
                cmdu_tx, beerocks_header->id());
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            fill_cs_params(notification->cs_params());
            message_com::send_cmdu(slave_socket, cmdu_tx);
            return false;
        }
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST: {
        LOG(WARNING) << "UNIMPLEMENTED - ACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST";
        // auto request = (message::sACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST*)rx_buffer;
        // if (!ap_man_hal.neighbor_set(request->params)){
        //     LOG(ERROR) << "Failed to set neighbor!";
        // }
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST: {
        LOG(WARNING) << "UNIMPLEMENTED - ACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST";
        // auto request = (message::sACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST*)rx_buffer;
        // if (!ap_man_hal.neighbor_remove(request->params)){
        //     LOG(ERROR) << "Failed to set neighbor!";
        // }
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE: {
        auto update =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE failed";
            return false;
        }
        std::string mac = network_utils::mac_to_string(update->mac());
        LOG(DEBUG) << "add 4addr sta update for mac=" << mac;
        ap_wlan_hal->wds_add_sta(mac);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE: {
        auto update =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE failed";
            return false;
        }
        std::string mac = network_utils::mac_to_string(update->mac());
        LOG(DEBUG) << "del 4addr sta update for mac=" << mac;
        ap_wlan_hal->wds_del_sta(mac);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST failed";
            send_steering_return_status(
                beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE, OPERATION_FAIL);
            return false;
        }
        std::string sta_mac = network_utils::mac_to_string(request->mac());
        auto vap_id         = request->vap_id();
        auto type           = request->type();
        auto reason         = request->reason();

        LOG(DEBUG) << "CLIENT_DISCONNECT, type "
                   << ((type == beerocks_message::eDisconnect_Type_Deauth) ? "DEAUTH" : "DISASSOC")
                   << " vap_id = " << int(vap_id) << " mac = " << sta_mac
                   << " reason = " << std::to_string(reason);
        bool res;
        if (type == beerocks_message::eDisconnect_Type_Deauth) {
            res = ap_wlan_hal->sta_deauth(vap_id, sta_mac, reason);
        } else {
            res = ap_wlan_hal->sta_disassoc(vap_id, sta_mac, reason);
        }
        send_steering_return_status(beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE,
                                    res ? OPERATION_SUCCESS : OPERATION_FAIL);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_DISALLOW_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST failed";
            return false;
        }

        std::string sta_mac = network_utils::mac_to_string(request->mac());
        std::string bssid   = network_utils::mac_to_string(request->bssid());
        LOG(DEBUG) << "CLIENT_DISALLOW: mac = " << sta_mac << ", bssid = " << bssid;

        ap_wlan_hal->sta_deny(sta_mac, bssid);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_ALLOW_REQUEST: {
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_ALLOW_REQUEST failed";
            return false;
        }

        std::string sta_mac = network_utils::mac_to_string(request->mac());
        std::string bssid   = network_utils::mac_to_string(request->bssid());
        LOG(DEBUG) << "CLIENT_ALLOW: mac = " << sta_mac << ", bssid = " << bssid;

        ap_wlan_hal->sta_allow(sta_mac, bssid);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_READ_ACS_REPORT_REQUEST: {
        if (!ap_wlan_hal->read_acs_report()) {
            LOG(ERROR) << "Failed to read acs report";
            return false;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE>(cmdu_tx,
                                                                          beerocks_header->id());
        if (!response) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        auto tuple_supported_channels = response->supported_channels_list(0);
        copy_radio_supported_channels(ap_wlan_hal, &std::get<1>(tuple_supported_channels));

        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST: {
        auto request = beerocks_header->addClass<
            beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST failed";
            return false;
        }
        std::string sta_mac = network_utils::mac_to_string(request->params().mac);
        LOG(DEBUG) << "APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST cross, curr id="
                   << sta_unassociated_rssi_measurement_header_id
                   << " request id=" << int(beerocks_header->id());
        bool ap_busy   = false;
        bool bwl_error = false;
        if (sta_unassociated_rssi_measurement_header_id == -1) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            response->mac() = network_utils::mac_from_string(sta_mac);
            message_com::send_cmdu(slave_socket, cmdu_tx);
            LOG(DEBUG)
                << "send sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE, sta_mac = "
                << sta_mac;

            int bandwidth = beerocks::utils::convert_bandwidth_to_int(
                (beerocks::eWiFiBandwidth)request->params().bandwidth);
            if (ap_wlan_hal->sta_unassoc_rssi_measurement(
                    sta_mac, request->params().channel, bandwidth,
                    request->params().vht_center_frequency, request->params().measurement_delay,
                    request->params().mon_ping_burst_pkt_num)) {
            } else {
                bwl_error = true;
                LOG(ERROR) << "sta_unassociated_rssi_measurement failed!";
            }

            sta_unassociated_rssi_measurement_header_id = beerocks_header->id();
            LOG(DEBUG) << "CLIENT_RX_RSSI_MEASUREMENT_REQUEST, mac = " << sta_mac
                       << " channel = " << int(request->params().channel)
                       << " bandwidth = " << bandwidth
                       << " vht_center_frequency = " << int(request->params().vht_center_frequency)
                       << " id = " << sta_unassociated_rssi_measurement_header_id;
        } else {
            ap_busy = true;
            LOG(WARNING)
                << "busy!, send response to retry CLIENT_RX_RSSI_MEASUREMENT_REQUEST, mac = "
                << sta_mac;
        }

        if (ap_busy || bwl_error) {
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
                cmdu_tx, beerocks_header->id());
            if (response == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            response->params().result.mac = request->params().mac;
            response->params().rx_rssi    = beerocks::RSSI_INVALID;
            response->params().rx_snr     = beerocks::SNR_INVALID;
            response->params().rx_packets = -1;
            message_com::send_cmdu(slave_socket, cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST failed";
            return false;
        }

        auto bssid                    = network_utils::mac_to_string(request->params().cur_bssid);
        const auto &vap_unordered_map = ap_wlan_hal->get_radio_info().available_vaps;
        auto it = std::find_if(vap_unordered_map.begin(), vap_unordered_map.end(),
                               [&](const std::pair<int, bwl::VAPElement> &element) {
                                   return element.second.mac == bssid;
                               });

        if (it == vap_unordered_map.end()) {
            //AP does not have the requested vap, probably will be handled on the other AP
            return true;
        }
        //TODO Check for STA errors, if error ACK with ErrorCodeTLV
        auto response = message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_ACK>(
            cmdu_tx, beerocks_header->id());

        if (!response) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        message_com::send_cmdu(slave_socket, cmdu_tx);

        std::string sta_mac       = network_utils::mac_to_string(request->params().mac);
        std::string target_bssid  = network_utils::mac_to_string(request->params().target.bssid);
        uint8_t disassoc_imminent = request->params().disassoc_imminent;

        LOG(DEBUG) << "CLIENT_BSS_STEER (802.11v) for sta_mac = " << sta_mac
                   << " to bssid = " << target_bssid
                   << " channel = " << int(request->params().target.channel);
        ap_wlan_hal->sta_bss_steer(sta_mac, target_bssid, request->params().target.channel,
                                   (disassoc_imminent) ? request->params().disassoc_timer_ms : 0,
                                   (disassoc_imminent) ? bss_steer_imminent_valid_int
                                                       : bss_steer_valid_int);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST>();
        if (!request) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST failed";
            return false;
        }

        std::list<son::wireless_utils::sBssInfoConf> bss_info_conf_list;
        auto wifi_credentials_size = request->wifi_credentials_size();

        for (auto i = 0; i < wifi_credentials_size; i++) {
            son::wireless_utils::sBssInfoConf bss_info_conf;
            auto config_data_tuple = request->wifi_credentials(i);
            if (!std::get<0>(config_data_tuple)) {
                LOG(ERROR) << "getting config data entry has failed!";
                return false;
            }
            auto &config_data = std::get<1>(config_data_tuple);

            // If a Multi-AP Agent receives an AP-Autoconfiguration WSC message containing an M2
            // with a Multi-AP Extension subelement with bit 4 (Tear Down bit) of the subelement
            // value set to one (see Table 4), it shall tear down all currently operating BSS(s)
            // on the radio indicated by the Radio Unique Identifier, and shall ignore the other
            // attributes in the M2.
            auto bss_type = static_cast<WSC::eWscVendorExtSubelementBssType>(
                config_data.multiap_attr().subelement_value);
            if ((bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN) != 0) {
                LOG(DEBUG) << "received teardown";
                bss_info_conf_list.clear();
                break;
            }

            bss_info_conf.ssid                = config_data.ssid_str();
            bss_info_conf.authentication_type = config_data.authentication_type_attr().data;
            bss_info_conf.encryption_type     = config_data.encryption_type_attr().data;
            bss_info_conf.network_key         = config_data.network_key_str();
            bss_info_conf.bss_type            = bss_type;

            bss_info_conf_list.push_back(bss_info_conf);
        }

        ap_wlan_hal->update_vap_credentials(bss_info_conf_list);

        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION: {
        auto update =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION failed";
            return false;
        }
        auto ire_mac = network_utils::mac_to_string(update->mac());
        LOG(DEBUG) << "ire connected in 4addr, mac=" << ire_mac;
        connected_ires.insert(ire_mac);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST: {
        auto request =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST failed";
            send_steering_return_status(
                beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE, OPERATION_FAIL);
            return false;
        }

        auto bssid                    = network_utils::mac_to_string(request->params().bssid);
        const auto &vap_unordered_map = ap_wlan_hal->get_radio_info().available_vaps;
        auto it = std::find_if(vap_unordered_map.begin(), vap_unordered_map.end(),
                               [&](const std::pair<int, bwl::VAPElement> &element) {
                                   return element.second.mac == bssid;
                               });

        auto vap_id   = it->first;
        auto vap_name = utils::get_iface_string_from_iface_vap_ids(
            ap_wlan_hal->get_radio_info().iface_name, vap_id);

        if (!request->params().remove) {
            if (!ap_wlan_hal->sta_softblock_add(
                    vap_name, network_utils::mac_to_string(request->params().client_mac),
                    request->params().config.authRejectReason, request->params().config.snrProbeHWM,
                    request->params().config.snrProbeLWM, request->params().config.snrAuthHWM,
                    request->params().config.snrAuthLWM)) {
                LOG(ERROR) << "sta_softblock_add failed!";
                send_steering_return_status(
                    beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE,
                    OPERATION_FAIL);
            }
        } else {
            if (!ap_wlan_hal->sta_softblock_remove(
                    vap_name, network_utils::mac_to_string(request->params().client_mac))) {
                LOG(ERROR) << "sta_softblock_remove failed!";
                send_steering_return_status(
                    beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE,
                    OPERATION_FAIL);
            }
        }
        send_steering_return_status(beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE,
                                    OPERATION_SUCCESS);

        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST: {
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        copy_vaps_info(ap_wlan_hal, backhaul_vaps_list, notification->params().vaps);
        LOG(DEBUG) << "Sending Vap List update to controller";
        if (!message_com::send_cmdu(slave_socket, cmdu_tx)) {
            LOG(ERROR) << "Failed sending cmdu!";
            return false;
        }

        break;
    }
    case beerocks_message::
        ACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST: {
        ap_wlan_hal->generate_connected_clients_events();
        break;
    }
    default: {
        LOG(ERROR) << "Unsupported header action_op: " << int(beerocks_header->action_op());
        break;
    }
    }

    return true;
}

void ap_manager_thread::fill_cs_params(beerocks_message::sApChannelSwitch &params)
{
    params.tx_power  = ap_wlan_hal->get_radio_info().conducted_power;
    params.channel   = ap_wlan_hal->get_radio_info().channel;
    params.bandwidth = uint8_t(
        beerocks::utils::convert_bandwidth_to_enum(ap_wlan_hal->get_radio_info().bandwidth));
    params.channel_ext_above_primary = ap_wlan_hal->get_radio_info().channel_ext_above;
    params.vht_center_frequency      = ap_wlan_hal->get_radio_info().vht_center_freq;
    params.switch_reason             = uint8_t(ap_wlan_hal->get_radio_info().last_csa_sw_reason);
    params.is_dfs_channel            = ap_wlan_hal->get_radio_info().is_dfs_channel;
}

bool ap_manager_thread::hal_event_handler(bwl::base_wlan_hal::hal_event_ptr_t event_ptr)
{
    if (!event_ptr) {
        LOG(ERROR) << "Invalid event!";
        return false;
    }

    if (!slave_socket) {
        LOG(ERROR) << "slave_socket == nullptr";
        return false;
    }

    // AP Event & Data
    typedef bwl::ap_wlan_hal::Event Event;
    auto event = (Event)(event_ptr->first);
    auto data  = event_ptr->second.get();

    switch (event) {

    case Event::AP_Attached: {
        handle_hostapd_attached();
    } break;

    case Event::AP_Enabled: {
        if (!data) {
            LOG(ERROR) << "AP_Enabled without data!";
            return false;
        }

        auto msg = static_cast<bwl::sHOSTAP_ENABLED_NOTIFICATION *>(data);
        handle_ap_enabled(msg->vap_id);

    } break;

    // ACS/CSA Completed
    case Event::ACS_Completed:
    case Event::CSA_Finished: {

        ap_wlan_hal->read_acs_report();
        ap_wlan_hal->refresh_radio_info();

        LOG(INFO) << ((event == Event::ACS_Completed) ? "ACS_Completed" : "CSA_Finished:")
                  << " channel = " << int(ap_wlan_hal->get_radio_info().channel)
                  << " bandwidth = " << ap_wlan_hal->get_radio_info().bandwidth
                  << " channel_ext_above_primary = "
                  << int(ap_wlan_hal->get_radio_info().channel_ext_above)
                  << " vht_center_frequency = "
                  << int(ap_wlan_hal->get_radio_info().vht_center_freq)
                  << " last_csa_switch_reason enum = "
                  << int(ap_wlan_hal->get_radio_info().last_csa_sw_reason);

        if (event == Event::ACS_Completed) {
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            auto tuple_supported_channels = notification->supported_channels_list(0);
            copy_radio_supported_channels(ap_wlan_hal, &std::get<1>(tuple_supported_channels));
            fill_cs_params(notification->cs_params());
            acs_completed_vap_update = true;
        } else {
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION>(cmdu_tx);
            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            fill_cs_params(notification->cs_params());
        }

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    // STA Connected
    case Event::STA_Connected: {

        if (!data) {
            LOG(ERROR) << "STA_Connected without data!";
            return false;
        }

        auto msg = static_cast<bwl::sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(data);
        std::string client_mac = network_utils::mac_to_string(msg->params.mac);

        LOG(INFO) << "STA_Connected mac = " << client_mac;

        auto it = std::find_if(
            pending_disable_vaps.begin(), pending_disable_vaps.end(),
            [&](pending_disable_vap_t vap) { return (vap.vap_id == msg->params.vap_id); });

        if (it != pending_disable_vaps.end()) {
            pending_disable_vaps.erase(it);
            if (enable_backhaul_vap(ap_wlan_hal, backhaul_vaps_list, connected_ires, true)) {
                // update master about the updated vap list
                // Note: this could be removed if bwl vap list will contain disabled backhaul vaps
            }
        }

        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        auto vap_node = ap_wlan_hal->get_radio_info().available_vaps.find(msg->params.vap_id);
        if (vap_node == ap_wlan_hal->get_radio_info().available_vaps.end()) {
            LOG(ERROR) << "Can't find vap with id " << int(msg->params.vap_id);
            return false;
        }

        notification->params().mac          = msg->params.mac;
        notification->params().vap_id       = msg->params.vap_id;
        notification->params().bssid        = network_utils::mac_from_string(vap_node->second.mac);
        notification->params().capabilities = msg->params.capabilities;

        message_com::send_cmdu(slave_socket, cmdu_tx);

        if (pending_4addr_stas.find(client_mac) != pending_4addr_stas.end()) {
            LOG(INFO) << "sta mac " << client_mac << " rejoined in 4addr mode, removing from list";
            pending_4addr_stas.erase(client_mac);
        }

    } break;

    // STA Disconnected
    case Event::STA_Disconnected: {

        if (!data) {
            LOG(ERROR) << "STA_Disconnected without data!";
            return false;
        }

        auto msg = static_cast<bwl::sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION *>(data);
        std::string mac = network_utils::mac_to_string(msg->params.mac);
        LOG(INFO) << "STA_Disconnected client " << mac;

        auto it = std::find_if(
            pending_disable_vaps.begin(), pending_disable_vaps.end(),
            [&](pending_disable_vap_t vap) { return (vap.vap_id == msg->params.vap_id); });

        if (it == pending_disable_vaps.end()) {
            pending_disable_vaps.push_back(std::move((pending_disable_vap_t){
                .vap_id  = msg->params.vap_id,
                .timeout = std::chrono::steady_clock::now() +
                           std::chrono::seconds(DISABLE_BACKHAUL_VAP_TIMEOUT_SEC),

            }));
        }

        if (pending_4addr_stas.find(mac) != pending_4addr_stas.end()) {
            LOG(INFO) << "sta mac " << mac << " is pending 4addr rejoin, ignoring";
            return true;
            //disconnect in 4Addr mode trigger disconnect timer , enter to pending_4addr_stas. (beckaul will get a chance to reconnect in 4addr )
        } else if (connected_ires.find(mac) != connected_ires.end()) {
            LOG(INFO) << "sta mac " << mac << " 4addr disconnect , will try to reconnect";
            auto now = std::chrono::steady_clock::now();
            pending_4addr_sta_t sta;
            sta.mac               = mac;
            sta.joined_timestamp  = now;
            sta.pending_reconnect = true;
            pending_4addr_stas.insert({mac, sta});
            return true;
        } else {
            auto notification = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION>(cmdu_tx);

            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            auto vap_node = ap_wlan_hal->get_radio_info().available_vaps.find(msg->params.vap_id);
            if (vap_node == ap_wlan_hal->get_radio_info().available_vaps.end()) {
                LOG(ERROR) << "Can't find vap with id " << int(msg->params.vap_id);
                return false;
            }

            notification->params().mac    = msg->params.mac;
            notification->params().bssid  = network_utils::mac_from_string(vap_node->second.mac);
            notification->params().vap_id = msg->params.vap_id;
            notification->params().reason = msg->params.reason;
            notification->params().source = msg->params.source;
            notification->params().type   = msg->params.type;

            message_com::send_cmdu(slave_socket, cmdu_tx);
        }

    } break;

    // BSS Transition (802.11v)
    case Event::BSS_TM_Response: {

        if (!data) {
            LOG(ERROR) << "BSS_TM_Response without data!";
            return false;
        }

        //TODO EasyMesh SteeringBTMReport should contain source BSSID and target BSSID
        auto msg = static_cast<bwl::sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE *>(data);
        LOG(INFO) << "BSS_STEER_RESPONSE client " << msg->params.mac
                  << " status_code=" << int(msg->params.status_code);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE>(cmdu_tx);

        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        response->params().mac          = msg->params.mac;
        response->params().status_code  = msg->params.status_code;
        response->params().source_bssid = msg->params.source_bssid;
        // TODO: add the optional target BSSID

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    // Unassociated STA Measurement
    case Event::STA_Unassoc_RSSI: {

        if (!data) {
            LOG(ERROR) << "STA_Unassoc_RSSI without data!";
            return false;
        }

        auto msg = static_cast<bwl::sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE *>(data);

        LOG(INFO) << "CLIENT_RX_RSSI_MEASUREMENT_RESPONSE for mac " << msg->params.result.mac
                  << " id=" << sta_unassociated_rssi_measurement_header_id;

        if (sta_unassociated_rssi_measurement_header_id > -1) {
            // Build the response message
            auto response = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
                cmdu_tx, sta_unassociated_rssi_measurement_header_id);
            if (response == nullptr) {
                LOG(ERROR) << "Failed building "
                              "cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
                break;
            }

            response->params().result.mac        = msg->params.result.mac;
            response->params().rx_phy_rate_100kb = msg->params.rx_phy_rate_100kb;
            response->params().tx_phy_rate_100kb = msg->params.tx_phy_rate_100kb;
            response->params().rx_rssi           = msg->params.rx_rssi;
            response->params().rx_packets        = msg->params.rx_packets;
            response->params().src_module        = msg->params.src_module;

            message_com::send_cmdu(slave_socket, cmdu_tx);
        } else {
            LOG(ERROR) << "sta_unassociated_rssi_measurement_header_id == -1";
            return false;
        }

        // Clear the ID
        sta_unassociated_rssi_measurement_header_id = -1;

    } break;

    // STA Softblock Message Dropped
    case Event::STA_Steering_Probe_Req: {

        if (!data) {
            LOG(ERROR) << "STA_Steering_Probe_Req without data!";
            return false;
        }

        auto msg =
            static_cast<bwl::sACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION *>(data);

        LOG(INFO) << "CLIENT_SOFTBLOCK_NOTIFICATION for client mac " << msg->params.client_mac;

        // Build the response message
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION "
                          "message!";
            break;
        }

        notification->params().client_mac = msg->params.client_mac;
        notification->params().bssid      = msg->params.bssid;
        notification->params().rx_snr     = msg->params.rx_snr;
        notification->params().blocked    = msg->params.blocked;
        notification->params().broadcast  = msg->params.broadcast;
        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    case Event::STA_Steering_Auth_Fail: {

        if (!data) {
            LOG(ERROR) << "STA_Steering_Auth_Fail without data!";
            return false;
        }

        auto msg =
            static_cast<bwl::sACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION *>(data);

        LOG(INFO) << "CLIENT_SOFTBLOCK_NOTIFICATION for client mac " << msg->params.client_mac;

        // Build the response message
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION "
                          "message!";
            break;
        }

        notification->params().client_mac = msg->params.client_mac;
        notification->params().bssid      = msg->params.bssid;
        notification->params().rx_snr     = msg->params.rx_snr;
        notification->params().blocked    = msg->params.blocked;
        notification->params().reject     = msg->params.reject;
        notification->params().reason     = msg->params.reason;
        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    // DFS CAC Completed
    case Event::DFS_CAC_Completed: {

        if (!data) {
            LOG(ERROR) << "DFS_CAC_Completed without data!";
            return false;
        }

        auto msg =
            static_cast<bwl::sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION *>(data);
        LOG(INFO) << "DFS_EVENT_CAC_COMPLETED succsess = " << int(msg->params.success);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION "
                          "message!";
            break;
        }

        response->params().timeout   = msg->params.timeout;
        response->params().frequency = msg->params.frequency;
        response->params().success   = msg->params.success;
        response->params().channel   = msg->params.channel;
        response->params().bandwidth = msg->params.bandwidth;

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    // DFS NOP Finished
    case Event::DFS_NOP_Finished: {

        if (!data) {
            LOG(ERROR) << "DFS_CAC_Completed without data!";
            return false;
        }

        auto msg =
            static_cast<bwl::sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION *>(data);
        LOG(INFO) << "DFS_EVENT_NOP_FINISHED "
                  << " channel = " << int(msg->params.channel) << " bw = "
                  << beerocks::utils::convert_bandwidth_to_int(
                         (beerocks::eWiFiBandwidth)msg->params.bandwidth)
                  << " vht_center_frequency = " << int(msg->params.vht_center_frequency);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building "
                          "ACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION message!";
            break;
        }

        response->params().frequency            = msg->params.frequency;
        response->params().channel              = msg->params.channel;
        response->params().bandwidth            = msg->params.bandwidth;
        response->params().vht_center_frequency = msg->params.vht_center_frequency;
        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    // AP/Interface Disabled
    case Event::AP_Disabled: {
        if (!data) {
            LOG(ERROR) << "AP_Disabled without data!";
            return false;
        }

        auto msg = static_cast<bwl::sHOSTAP_DISABLED_NOTIFICATION *>(data);
        LOG(INFO) << "AP_Disabled on vap_id = " << int(msg->vap_id);

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION message!";
            break;
        }

        response->vap_id() = msg->vap_id;
        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;
    case Event::Interface_Disabled: {

        LOG(ERROR) << " event Interface_Disabled call stop_ap_manager_thread()";
        thread_last_error_code = APMANAGER_THREAD_ERROR_HOSTAP_DISABLED;
        stop_ap_manager_thread();

    } break;

    case Event::ACS_Failed: {
        LOG(INFO) << "ACS_Failed event!";
        // notify slave //
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION>(cmdu_tx);

        if (notification == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION message!";
            break;
        }

        notification->cs_params().channel   = ap_wlan_hal->get_radio_info().channel;
        notification->cs_params().bandwidth = uint8_t(
            beerocks::utils::convert_bandwidth_to_enum(ap_wlan_hal->get_radio_info().bandwidth));
        notification->cs_params().channel_ext_above_primary =
            ap_wlan_hal->get_radio_info().channel_ext_above;
        notification->cs_params().vht_center_frequency =
            ap_wlan_hal->get_radio_info().vht_center_freq;
        notification->cs_params().switch_reason =
            uint8_t(ap_wlan_hal->get_radio_info().last_csa_sw_reason);

        message_com::send_cmdu(slave_socket, cmdu_tx);

    } break;

    // Unhandled events
    default:
        LOG(ERROR) << "Unhandled event: " << int(event);
        break;
    }

    return true;
}

void ap_manager_thread::handle_hostapd_attached()
{
    LOG(DEBUG) << "handling enabled hostapd";

    if (acs_enabled) {
        LOG(DEBUG) << "retrieving ACS report";
        int read_acs_attempt = 0;
        while (!ap_wlan_hal->read_acs_report()) {
            read_acs_attempt++;
            if (read_acs_attempt >= READ_ACS_ATTEMPT_MAX) {
                LOG(ERROR) << "retrieving ACS report fails " << int(READ_ACS_ATTEMPT_MAX)
                           << " times - stop ap_manager_thread";
                stop_ap_manager_thread();
                break;
            }

            usleep(ACS_READ_SLEEP_USC);
        }
    } else {
        ap_wlan_hal->read_supported_channels();
    }

    auto notification =
        message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_JOINED_NOTIFICATION>(
            cmdu_tx);

    if (notification == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return;
    }

    string_utils::copy_string(notification->params().iface_name,
                              ap_wlan_hal->get_iface_name().c_str(), message::IFACE_NAME_LENGTH);
    string_utils::copy_string(notification->params().driver_version,
                              ap_wlan_hal->get_radio_driver_version().c_str(),
                              message::WIFI_DRIVER_VER_LENGTH);

    notification->params().iface_type = uint8_t(ap_wlan_hal->get_iface_type());
    notification->params().iface_mac = network_utils::mac_from_string(ap_wlan_hal->get_radio_mac());
    notification->params().iface_is_5ghz   = ap_wlan_hal->get_radio_info().is_5ghz;
    notification->params().ant_num         = ap_wlan_hal->get_radio_info().ant_num;
    notification->params().conducted_power = ap_wlan_hal->get_radio_info().conducted_power;
    notification->cs_params().channel      = ap_wlan_hal->get_radio_info().channel;
    notification->cs_params().channel_ext_above_primary =
        ap_wlan_hal->get_radio_info().channel_ext_above;
    notification->cs_params().vht_center_frequency = ap_wlan_hal->get_radio_info().vht_center_freq;
    notification->cs_params().bandwidth            = uint8_t(
        beerocks::utils::convert_bandwidth_to_enum(ap_wlan_hal->get_radio_info().bandwidth));

    // Copy the channels supported by the AP
    copy_radio_supported_channels(ap_wlan_hal, notification->params().supported_channels);

    LOG(INFO) << "send ACTION_APMANAGER_JOINED_NOTIFICATION";
    LOG(INFO) << " mac = " << ap_wlan_hal->get_radio_mac();
    LOG(INFO) << " ant_num = " << ap_wlan_hal->get_radio_info().ant_num;
    LOG(INFO) << " conducted_power = " << ap_wlan_hal->get_radio_info().conducted_power;
    LOG(INFO) << " current channel = " << ap_wlan_hal->get_radio_info().channel;
    LOG(INFO) << " vht_center_frequency = " << ap_wlan_hal->get_radio_info().vht_center_freq;
    LOG(INFO) << " current bw = " << ap_wlan_hal->get_radio_info().bandwidth;
    LOG(INFO) << " supported_channels = " << std::endl
              << get_radio_supported_channels_string(ap_wlan_hal);

    // Send CMDU
    message_com::send_cmdu(slave_socket, cmdu_tx);
}

void ap_manager_thread::stop_ap_manager_thread()
{
    LOG(TRACE) << __func__;

    // if(ap_hal_ext_events || slave_socket) LOG(DEBUG) << "stop_ap_manager_thread()";

    if (ap_hal_ext_events) {
        ap_wlan_hal->detach();
        remove_socket(ap_hal_ext_events);
        delete ap_hal_ext_events;
        ap_hal_ext_events = nullptr;
    }

    if (ap_hal_int_events) {
        remove_socket(ap_hal_int_events);
        delete ap_hal_int_events;
        ap_hal_int_events = nullptr;
    }

    if (slave_socket) {
        remove_socket(slave_socket);
        slave_socket->closeSocket();

        delete slave_socket;
        slave_socket = nullptr;
    }

    should_stop = true;
}

void ap_manager_thread::send_heartbeat()
{
    if (slave_socket == nullptr) {
        LOG(ERROR) << "process_keep_alive(): slave_socket is nullptr!";
        return;
    }

    //LOG(DEBUG) << "sending HEARTBEAT notification";
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_APMANAGER_HEARTBEAT_NOTIFICATION message!";
        return;
    }

    message_com::send_cmdu(slave_socket, cmdu_tx);
}

bool ap_manager_thread::handle_ap_enabled(int vap_id)
{
    LOG(INFO) << "AP_Enabled on vap_id = " << int(vap_id);

    if (!ap_wlan_hal->refresh_vaps_info(vap_id)) {
        LOG(ERROR) << "Failed updating vap info!!!";
    }

    auto vap_iter = ap_wlan_hal->get_radio_info().available_vaps.find(vap_id);
    if (vap_iter == ap_wlan_hal->get_radio_info().available_vaps.end()) {
        LOG(ERROR) << "Recieved AP-ENABLED but can't get vap info";
        return false;
    }

    const auto vap_info = vap_iter->second;

    // mark backhaul vaps
    auto it = std::find_if(backhaul_vaps_list.begin(), backhaul_vaps_list.end(),
                           [&](const backhaul_vap_list_element_t &backhaul_elem) {
                               return (backhaul_elem.bssid == vap_info.mac);
                           });

    bool bVAP = (it != backhaul_vaps_list.end());

    LOG(INFO) << "vap_id = " << int(vap_id) << ", bssid = " << vap_info.mac
              << ", ssid = " << vap_info.ssid
              << ", backhaul_vap = " << beerocks::string_utils::bool_str(bVAP);

    auto notification = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION>(cmdu_tx);
    if (notification == nullptr) {
        LOG(ERROR) << "Failed building cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION message!";
        return false;
    }

    notification->vap_id() = vap_id;

    // Copy the VAP MAC and SSID
    notification->vap_info().mac = network_utils::mac_from_string(vap_info.mac);
    beerocks::string_utils::copy_string((char *)notification->vap_info().ssid,
                                        vap_info.ssid.c_str(),
                                        beerocks::message::WIFI_SSID_MAX_LENGTH);
    notification->vap_info().backhaul_vap = bVAP;

    message_com::send_cmdu(slave_socket, cmdu_tx);

    return true;
}

void ap_manager_thread::send_steering_return_status(beerocks_message::eActionOp_APMANAGER ActionOp,
                                                    int32_t status)
{
    switch (ActionOp) {
    case beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }
        response->params().error_code = status;
        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE: {
        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }
        response->params().error_code = status;
        message_com::send_cmdu(slave_socket, cmdu_tx);
        break;
    }
    default: {
        LOG(ERROR) << "UNKNOWN ActionOp was received, ActionOp = " << int(ActionOp);
        break;
    }
    }
    return;
}
