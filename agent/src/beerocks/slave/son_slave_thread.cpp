/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#include "son_slave_thread.h"

#include "../monitor/monitor_thread.h"

#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_1905_vs.h>
#include <beerocks/tlvf/beerocks_message_apmanager.h>
#include <beerocks/tlvf/beerocks_message_backhaul.h>
#include <beerocks/tlvf/beerocks_message_control.h>
#include <beerocks/tlvf/beerocks_message_monitor.h>
#include <beerocks/tlvf/beerocks_message_platform.h>

#include <tlvf/ieee_1905_1/tlvAlMacAddressType.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/ieee_1905_1/tlvWscM1.h>
#include <tlvf/ieee_1905_1/tlvWscM2.h>
#include <tlvf/wfa_map/tlvApMetricQuery.h>
#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>
#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>
#include <tlvf/wfa_map/tlvClientAssociationEvent.h>
#include <tlvf/wfa_map/tlvOperatingChannelReport.h>
#include <tlvf/wfa_map/tlvSteeringBTMReport.h>
#include <tlvf/wfa_map/tlvSteeringRequest.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>

// BPL Error Codes
#include <bpl/bpl_cfg.h>
#include <bpl/bpl_err.h>

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

// TODO: Should be moved somewhere else?
static bwl::WiFiSec platform_to_bwl_security(const std::string &sec)
{
    if (!sec.compare("None")) {
        return bwl::WiFiSec::None;
    } else if (!sec.compare("WEP-64")) {
        return bwl::WiFiSec::WEP_64;
    } else if (!sec.compare("WEP-128")) {
        return bwl::WiFiSec::WEP_128;
    } else if (!sec.compare("WPA-Personal")) {
        return bwl::WiFiSec::WPA_PSK;
    } else if (!sec.compare("WPA2-Personal")) {
        return bwl::WiFiSec::WPA2_PSK;
    } else if (!sec.compare("WPA-WPA2-Personal")) {
        return bwl::WiFiSec::WPA_WPA2_PSK;
    } else {
        return bwl::WiFiSec::Invalid;
    }
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define SLAVE_STATE_CONTINUE() call_slave_select = false

using namespace beerocks;
using namespace net;
using namespace son;

slave_thread::slave_thread(sSlaveConfig conf, beerocks::logging &logger_)
    : socket_thread(conf.temp_path + std::string(BEEROCKS_SLAVE_UDS) + +"_" + conf.hostap_iface),
      config(conf), logger(logger_)
{
    thread_name = "son_slave_" + conf.hostap_iface;
    slave_uds   = conf.temp_path + std::string(BEEROCKS_SLAVE_UDS) + "_" + conf.hostap_iface;
    backhaul_manager_uds    = conf.temp_path + std::string(BEEROCKS_BACKHAUL_MGR_UDS);
    platform_manager_uds    = conf.temp_path + std::string(BEEROCKS_PLAT_MGR_UDS);
    ap_manager              = nullptr;
    backhaul_manager_socket = nullptr;
    master_socket           = nullptr;
    monitor_socket          = nullptr;
    ap_manager_socket       = nullptr;
    platform_manager_socket = nullptr;
    configuration_stop_on_failure_attempts = conf.stop_on_failure_attempts;
    stop_on_failure_attempts               = configuration_stop_on_failure_attempts;

    slave_state = STATE_INIT;
    set_select_timeout(SELECT_TIMEOUT_MSEC);
}

slave_thread::~slave_thread()
{
    LOG(DEBUG) << "destructor - slave_reset()";
    stop_slave_thread();
}

bool slave_thread::init()
{
    LOG(INFO) << "Slave Info:";
    LOG(INFO) << "hostap_iface=" << config.hostap_iface;
    LOG(INFO) << "hostap_iface_type=" << config.hostap_iface_type;
    LOG(INFO) << "ruid=" << config.radio_identifier;

    if (config.hostap_iface_type == beerocks::IFACE_TYPE_UNSUPPORTED) {
        LOG(ERROR) << "hostap_iface_type '" << config.hostap_iface_type << "' UNSUPPORTED!";
        return false;
    }

    if (config.ucc_listener_slave_hostap_iface == config.hostap_iface &&
        config.ucc_listener_port != 0) {
        m_agent_ucc_listener =
            std::make_unique<agent_ucc_listener>(config.ucc_listener_port, config.vendor,
                                                 config.model, config.bridge_iface, &master_socket);
        if (m_agent_ucc_listener && !m_agent_ucc_listener->start("ucc_listener")) {
            LOG(ERROR) << "failed start agent_ucc_listener";
            return false;
        }
    }

    return socket_thread::init();
}

void slave_thread::stop_slave_thread()
{
    LOG(DEBUG) << "stop_slave_thread()";
    slave_reset();
    should_stop = true;
}

void slave_thread::slave_reset()
{
    slave_resets_counter++;
    LOG(DEBUG) << "slave_reset() #" << slave_resets_counter << " - start";
    if (!detach_on_conf_change) {
        backhaul_manager_stop();
    }
    platform_manager_stop();
    hostap_services_off();
    ap_manager_stop();
    monitor_stop();
    is_backhaul_manager            = false;
    iface_status_operational_state = false;
    detach_on_conf_change          = false;

    if (configuration_stop_on_failure_attempts && !stop_on_failure_attempts) {
        LOG(ERROR) << "Reached to max stop on failure attempts!";
        stopped = true;
    }

    if (stopped && slave_state != STATE_INIT) {
        platform_notify_error(BPL_ERR_SLAVE_STOPPED, "");
        LOG(DEBUG) << "goto STATE_STOPPED";
        slave_state = STATE_STOPPED;
    } else if (is_backhaul_disconnected) {
        slave_state_timer =
            std::chrono::steady_clock::now() + std::chrono::seconds(SLAVE_INIT_DELAY_SEC);
        LOG(DEBUG) << "goto STATE_WAIT_BEFORE_INIT";
        slave_state = STATE_WAIT_BERFORE_INIT;
    } else {
        LOG(DEBUG) << "goto STATE_INIT";
        slave_state = STATE_INIT;
    }

    is_slave_reset = true;
    LOG(DEBUG) << "slave_reset() #" << slave_resets_counter << " - done";
}

void slave_thread::platform_notify_error(int code, const std::string &error_data)
{
    if (platform_manager_socket == nullptr) {
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

    // Send the message
    message_com::send_cmdu(platform_manager_socket, cmdu_tx);
}

void slave_thread::on_thread_stop() { stop_slave_thread(); }

bool slave_thread::socket_disconnected(Socket *sd)
{
    if (configuration_in_progress) {
        LOG(DEBUG) << "configuration is in progress, ignoring";
        detach_on_conf_change = true;
        if (sd == ap_manager_socket || sd == monitor_socket) {
            ap_manager_stop();
            monitor_stop();
            return false;
        }
        return true;
    }

    if (sd == backhaul_manager_socket) {
        LOG(DEBUG) << "backhaul manager & master socket disconnected! - slave_reset()";
        platform_notify_error(BPL_ERR_SLAVE_SLAVE_BACKHAUL_MANAGER_DISCONNECTED, "");
        stop_slave_thread();
        return false;
    } else if (sd == platform_manager_socket) {
        LOG(DEBUG) << "platform_manager disconnected! - slave_reset()";
        stop_slave_thread();
        return false;
    } else if (sd == ap_manager_socket || sd == monitor_socket) { //TODO WLANRTSYS-9119
        // if both ap_manager and monitor disconnected, but monitor disconnection got first
        auto err_code = ap_manager->get_thread_last_error_code();
        if (sd == ap_manager_socket ||
            err_code != ap_manager_thread::eThreadErrors::APMANAGER_THREAD_ERROR_NO_ERROR) {
            LOG(DEBUG) << "ap_manager socket disconnected, last error code " << int(err_code)
                       << "  - slave_reset()";
            slave_reset();
        } else {
            // only monitor disconnected
            LOG(DEBUG) << "monitor socket disconnected! - slave_reset()";
            slave_reset();
        }

        return false;
    }

    return true;
}

std::string slave_thread::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

bool slave_thread::work()
{
    bool call_slave_select = true;

    if (!monitor_heartbeat_check() || !ap_manager_heartbeat_check()) {
        slave_reset();
    }

    if (!slave_fsm(call_slave_select)) {
        return false;
    }

    if (config.enable_bpl_iface_status_notifications && platform_manager_socket &&
        !platform_settings.onboarding) {
        send_iface_status();
    }

    if (call_slave_select) {
        if (!socket_thread::work()) {
            return false;
        }
    }
    return true;
}

void slave_thread::before_select()
{
    if (m_agent_ucc_listener) {
        m_agent_ucc_listener->unlock();
    }
}

void slave_thread::after_select(bool timeout)
{
    if (m_agent_ucc_listener) {
        m_agent_ucc_listener->lock();
    }
}

void slave_thread::process_keep_alive()
{
    if (!config.enable_keep_alive || !son_config.slave_keep_alive_retries) {
        return;
    }

    if (master_socket == nullptr) {
        LOG(ERROR) << "process_keep_alive(): master_socket is nullptr!";
        return;
    }

    auto now = std::chrono::steady_clock::now();
    int keep_alive_time_elapsed_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - master_last_seen).count();
    if (keep_alive_time_elapsed_ms >= beerocks::KEEP_ALIVE_INTERVAL_MSC) {
        if (keep_alive_retries >= son_config.slave_keep_alive_retries) {
            LOG(DEBUG) << "exceeded keep_alive_retries " << keep_alive_retries
                       << " - slave_reset()";

            platform_notify_error(BPL_ERR_SLAVE_MASTER_KEEP_ALIVE_TIMEOUT,
                                  "Reached master keep-alive retries limit: " +
                                      std::to_string(keep_alive_retries));

            stop_on_failure_attempts--;
            slave_reset();
        } else {
            LOG(DEBUG) << "time elapsed since last master message: " << keep_alive_time_elapsed_ms
                       << "ms, sending PING_MSG_REQUEST, tries=" << keep_alive_retries;
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_AGENT_PING_REQUEST>(cmdu_tx);
            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return;
            }

            request->total() = 1;
            request->seq()   = 0;
            request->size()  = 0;

            send_cmdu_to_controller(cmdu_tx);
            keep_alive_retries++;
            master_last_seen = now;
        }
    }
}

void slave_thread::update_iface_status(bool is_ap, int8_t iface_status)
{
    if (iface_status == 1) {
        if (is_ap) {
            iface_status_ap = beerocks::eRadioStatus::AP_OK;
        } else { //BH
            iface_status_bh = beerocks::eRadioStatus::BH_SCAN;
        }
    } else if (iface_status == 0) {
        iface_status_bh = beerocks::eRadioStatus::OFF;
        iface_status_ap = beerocks::eRadioStatus::OFF;
    }
}

void slave_thread::send_iface_status()
{
    // LOG(DEBUG) << std::endl
    //            << "iface_status_ap_prev=" << int(iface_status_ap_prev) << ", iface_status_ap=" << int(iface_status_ap) << std::endl
    //            << "iface_status_bh_prev=" << int(iface_status_bh_prev) << ", iface_status_bh=" << int(iface_status_bh) << std::endl
    //            << "iface_status_bh_wired_prev=" << int(iface_status_bh_wired_prev) << ", iface_status_bh_wired_prev=" << int(iface_status_bh_wired_prev);

    if (iface_status_ap_prev != iface_status_ap || iface_status_bh_prev != iface_status_bh ||
        iface_status_bh_wired_prev != iface_status_bh_wired ||
        iface_status_operational_state_prev != iface_status_operational_state) {

        send_platform_iface_status_notif(iface_status_ap, iface_status_operational_state);
    }
}

bool slave_thread::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (cmdu_rx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {

        auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);

        if (!beerocks_header) {
            LOG(ERROR) << "Not a vendor specific message";
            return false;
        }

        switch (beerocks_header->action()) {
        case beerocks_message::ACTION_CONTROL: {
            return handle_cmdu_control_message(sd, beerocks_header, cmdu_rx);
        } break;
        case beerocks_message::ACTION_BACKHAUL: {
            return handle_cmdu_backhaul_manager_message(sd, beerocks_header, cmdu_rx);
        } break;
        case beerocks_message::ACTION_PLATFORM: {
            return handle_cmdu_platform_manager_message(sd, beerocks_header, cmdu_rx);
        } break;
        case beerocks_message::ACTION_APMANAGER: {
            return handle_cmdu_ap_manager_message(sd, beerocks_header, cmdu_rx);
        } break;
        case beerocks_message::ACTION_MONITOR: {
            return handle_cmdu_monitor_message(sd, beerocks_header, cmdu_rx);
        } break;
        default: {
            LOG(ERROR) << "Unknown message, action: " << int(beerocks_header->action());
        }
        }
    } else { // IEEE 1905.1 message
        return handle_cmdu_control_ieee1905_1_message(sd, cmdu_rx);
    }
    return true;
}

////////////////////////////////////////////////////////////////////////
////////////////////////// HANDLE CMDU ACTIONS /////////////////////////
////////////////////////////////////////////////////////////////////////

bool slave_thread::handle_cmdu_control_ieee1905_1_message(Socket *sd,
                                                          ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto cmdu_message_type = cmdu_rx.getMessageType();

    if (master_socket == nullptr) {
        LOG(WARNING) << "master_socket == nullptr";
        return true;
    } else if (master_socket != sd) {
        LOG(WARNING) << "Unknown socket, cmdu message type: " << int(cmdu_message_type);
        return true;
    }

    if (slave_state == STATE_STOPPED) {
        LOG(WARNING) << "slave_state == STATE_STOPPED";
        return true;
    }

    master_last_seen   = std::chrono::steady_clock::now();
    keep_alive_retries = 0;

    switch (cmdu_message_type) {
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE:
        if (!handle_autoconfiguration_wsc(sd, cmdu_rx)) {
            LOG(ERROR) << "Autoconfiguration WSC failure, slave reset";
            stop_on_failure_attempts--;
            slave_reset();
            return false;
        }
        return true;
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE:
        return handle_autoconfiguration_renew(sd, cmdu_rx);
    case ieee1905_1::eMessageType::AP_CAPABILITY_QUERY_MESSAGE:
        return handle_ap_capability_query(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE:
        return handle_client_association_request(sd, cmdu_rx);
    case ieee1905_1::eMessageType::AP_METRICS_QUERY_MESSAGE:
        return handle_ap_metrics_query(sd, cmdu_rx);
    case ieee1905_1::eMessageType::LINK_METRIC_QUERY_MESSAGE:
        return handle_link_metrics_query(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CHANNEL_PREFERENCE_QUERY_MESSAGE:
        return handle_channel_preference_query(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CHANNEL_SELECTION_REQUEST_MESSAGE:
        return handle_channel_selection_request(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CLIENT_STEERING_REQUEST_MESSAGE:
        return handle_client_steering_request(sd, cmdu_rx);
    case ieee1905_1::eMessageType::ACK_MESSAGE:
        return handle_ack_message(sd, cmdu_rx);
    case ieee1905_1::eMessageType::CLIENT_CAPABILITY_QUERY_MESSAGE:
        return handle_client_capability_query(sd, cmdu_rx);
    case ieee1905_1::eMessageType::MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE:
        return handle_multi_ap_policy_config_request(sd, cmdu_rx);
    default:
        LOG(ERROR) << "Unknown CMDU message type: " << std::hex << int(cmdu_message_type);
        return false;
    }

    return true;
}

bool slave_thread::handle_cmdu_control_message(
    Socket *sd, std::shared_ptr<beerocks::message_com::beerocks_header> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    // LOG(DEBUG) << "handle_cmdu_control_message(), INTEL_VS: action=" + std::to_string(beerocks_header->action()) + ", action_op=" + std::to_string(beerocks_header->action_op());
    // LOG(DEBUG) << "received radio_mac=" << network_utils::mac_to_string(beerocks_header->radio_mac()) << ", local radio_mac=" << network_utils::mac_to_string(hostap_params.iface_mac);

    // to me or not to me, this is the question...
    if (beerocks_header->m_header->radio_mac() != hostap_params.iface_mac) {
        return true;
    }

    if (beerocks_header->m_header->direction() == beerocks::BEEROCKS_DIRECTION_CONTROLLER) {
        return true;
    }

    if (master_socket == nullptr) {
        // LOG(WARNING) << "master_socket == nullptr";
        return true;
    } else if (master_socket != sd) {
        LOG(WARNING) << "Unknown socket, ACTION_CONTROL action_op: "
                     << int(beerocks_header->action_op());
        return true;
    }

    if (slave_state == STATE_STOPPED) {
        return true;
    }

    master_last_seen   = std::chrono::steady_clock::now();
    keep_alive_retries = 0;

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_CONTROL_ARP_QUERY_REQUEST: {
        LOG(TRACE) << "ACTION_CONTROL_ARP_QUERY_REQUEST";
        auto request_in =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_ARP_QUERY_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_ARP_QUERY_REQUEST failed";
            return false;
        }
        auto request_out =
            message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ARP_QUERY_REQUEST>(
                cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        // notify platform manager
        request_out->params() = request_in->params();
        message_com::send_cmdu(platform_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_SON_CONFIG_UPDATE: {
        LOG(DEBUG) << "received ACTION_CONTROL_SON_CONFIG_UPDATE";
        auto update =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_SON_CONFIG_UPDATE>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_SON_CONFIG_UPDATE failed";
            return false;
        }
        son_config = update->config();
        log_son_config();
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST: {
        LOG(DEBUG) << "received ACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST";

        auto request_in = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST>(
            cmdu_tx);
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        LOG(DEBUG) << "send ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST";
        request_out->params() = request_in->params();
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START: {
        LOG(DEBUG) << "received ACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START";
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        LOG(DEBUG) << "send cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START";
        request_out->cs_params() = request_in->cs_params();
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_START_MONITORING_REQUEST: {
        LOG(DEBUG) << "received ACTION_CONTROL_CLIENT_START_MONITORING_REQUEST";
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_START_MONITORING_REQUEST failed";
            return false;
        }

        std::string client_mac = network_utils::mac_to_string(request_in->params().mac);
        std::string client_bridge_4addr_mac =
            network_utils::mac_to_string(request_in->params().bridge_4addr_mac);
        std::string client_ip = network_utils::ipv4_to_string(request_in->params().ipv4);

        LOG(DEBUG) << "START_MONITORING_REQUEST: mac=" << client_mac << " ip=" << client_ip
                   << " bridge_4addr_mac=" << client_bridge_4addr_mac;

        if (request_in->params().is_ire) {
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION>(cmdu_tx);
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION "
                              "message!";
                return false;
            }
            request_out->mac() = request_in->params().mac;
            message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        }

        //notify monitor
        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_MONITOR_CLIENT_START_MONITORING_REQUEST message!";
            return false;
        }
        request_out->params() = request_in->params();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST: {
        LOG(DEBUG) << "received ACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST";
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST failed";
            return false;
        }
        std::string client_mac = network_utils::mac_to_string(request_in->mac());

        LOG(DEBUG) << "STOP_MONITORING_REQUEST: mac=" << client_mac;

        //notify monitor
        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST message!";
            return false;
        }

        request_out->mac() = request_in->mac();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST: {
        LOG(DEBUG) << "received ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST";

        auto request_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST failed";
            return false;
        }
        auto hostap_mac = network_utils::mac_to_string(request_in->params().mac);
        bool forbackhaul =
            (is_backhaul_manager && backhaul_params.backhaul_is_wireless) ? true : false;

        if (request_in->params().cross && (request_in->params().ipv4.oct[0] == 0) &&
            forbackhaul) { //if backhaul manager and wireless send to backhaul else front.
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(
                cmdu_tx, beerocks_header->id());
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST "
                              "message!";
                return false;
            }

            request_out->params() = request_in->params();
            message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);
        } else if (request_in->params().cross &&
                   (request_in->params().ipv4.oct[0] ==
                    0)) { // unconnected client cross --> send to ap_manager
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(
                cmdu_tx, beerocks_header->id());
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST "
                              "message!";
                return false;
            }
            request_out->params() = request_in->params();
            message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        } else {
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>(
                cmdu_tx, beerocks_header->id());
            if (request_out == nullptr) {
                LOG(ERROR)
                    << "Failed building ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST message!";
                return false;
            }
            request_out->params() = request_in->params();
            message_com::send_cmdu(monitor_socket, cmdu_tx);
        }

        LOG(INFO) << "rx_rssi measurement request for client mac="
                  << network_utils::mac_to_string(request_in->params().mac)
                  << " ip=" << network_utils::ipv4_to_string(request_in->params().ipv4)
                  << " channel=" << int(request_in->params().channel) << " bandwidth="
                  << utils::convert_bandwidth_to_int(
                         (beerocks::eWiFiBandwidth)request_in->params().bandwidth)
                  << " cross=" << int(request_in->params().cross)
                  << " id=" << int(beerocks_header->id());
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST: {
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_DISCONNECT_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST>(cmdu_tx,
                                                                           beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST message!";
            return false;
        }

        request_out->mac()    = request_in->mac();
        request_out->vap_id() = request_in->vap_id();
        request_out->type()   = request_in->type();
        request_out->reason() = request_in->reason();

        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CONTROLLER_PING_REQUEST: {
        LOG(DEBUG) << "received ACTION_CONTROL_CONTROLLER_PING_REQUEST";
        auto request =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_CONTROLLER_PING_REQUEST>();
        if (request == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_CONTROLLER_PING_REQUEST failed";
            return false;
        }

        auto response = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CONTROLLER_PING_RESPONSE>(cmdu_tx);
        if (response == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        response->total() = request->total();
        response->seq()   = request->seq();
        response->size()  = request->size();

        if (response->size()) {
            if (!response->alloc_data(request->size())) {
                LOG(ERROR) << "Failed buffer allocation to size=" << int(request->size());
                break;
            }
            memset(request->data(), 0, request->data_length());
        }
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_AGENT_PING_RESPONSE: {
        LOG(DEBUG) << "received ACTION_CONTROL_AGENT_PING_RESPONSE";
        auto response =
            beerocks_header->addClass<beerocks_message::cACTION_CONTROL_AGENT_PING_RESPONSE>();
        if (response == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_AGENT_PING_RESPONSE failed";
            return false;
        }
        if (response->seq() < (response->total() - 1)) { //send next ping request
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_AGENT_PING_REQUEST>(cmdu_tx);
            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            request->total() = response->total();
            request->seq()   = response->seq() + 1;
            request->size()  = response->size();
            if (request->size()) {
                if (!request->alloc_data(request->size())) {
                    LOG(ERROR) << "Failed buffer allocation to size=" << int(request->size());
                    break;
                }
                memset(request->data(), 0, request->data_length());
            }
            send_cmdu_to_controller(cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL: {
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL failed";
            return false;
        }
        bool all = false;
        if (request_in->params().module_name == beerocks::BEEROCKS_PROCESS_ALL) {
            all = true;
        }
        if (all || request_in->params().module_name == beerocks::BEEROCKS_PROCESS_SLAVE) {
            logger.set_log_level_state((eLogLevel)request_in->params().log_level,
                                       request_in->params().enable);
        }
        if (all || request_in->params().module_name == beerocks::BEEROCKS_PROCESS_MONITOR) {
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL>(cmdu_tx);
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            request_out->params() = request_in->params();
            message_com::send_cmdu(monitor_socket, cmdu_tx);
        }
        if (all || request_in->params().module_name == beerocks::BEEROCKS_PROCESS_PLATFORM) {
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL>(cmdu_tx);
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            request_out->params() = request_in->params();
            message_com::send_cmdu(platform_manager_socket, cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_BACKHAUL_ROAM_REQUEST: {
        LOG(TRACE) << "received ACTION_CONTROL_BACKHAUL_ROAM_REQUEST";
        if (is_backhaul_manager && backhaul_params.backhaul_is_wireless) {
            auto request_in =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_CONTROL_BACKHAUL_ROAM_REQUEST>();
            if (request_in == nullptr) {
                LOG(ERROR) << "addClass cACTION_CONTROL_BACKHAUL_ROAM_REQUEST failed";
                return false;
            }
            auto bssid = network_utils::mac_to_string(request_in->params().bssid);
            LOG(DEBUG) << "reconfigure wpa_supplicant to bssid " << bssid
                       << " channel=" << int(request_in->params().channel);

            auto request_out =
                message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_ROAM_REQUEST>(
                    cmdu_tx, beerocks_header->id());
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            request_out->params() = request_in->params();
            message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_BACKHAUL_RESET: {
        LOG(TRACE) << "received ACTION_CONTROL_BACKHAUL_RESET";
        auto request =
            message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_RESET>(cmdu_tx);
        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST: {
        if (monitor_socket) {
            // LOG(TRACE) << "received ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST"; // floods the log
            auto request_in =
                cmdu_rx
                    .addClass<beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST>();
            if (request_in == nullptr) {
                LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST failed";
                return false;
            }

            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST>(
                cmdu_tx, beerocks_header->id());
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            request_out->sync() = request_in->sync();
            message_com::send_cmdu(monitor_socket, cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST: {
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        request_out->params() = request_in->params();
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST: {
        auto request_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        request_out->params() = request_in->params();
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST: {
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST failed";
            return false;
        }
        //LOG(DEBUG) << "ACTION_CONTROL_CLIENT_BEACON_11K_REQUEST";
        // override ssid in case of:
        if (request_in->params().use_optional_ssid &&
            std::string((char *)request_in->params().ssid).empty()) {
            //LOG(DEBUG) << "ssid field is empty! using slave ssid -> " << config.ssid;
            string_utils::copy_string((char *)request_in->params().ssid,
                                      platform_settings.front_ssid, message::WIFI_SSID_MAX_LENGTH);
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>(cmdu_tx,
                                                                         beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_MONITOR_CLIENT_BEACON_11K_REQUEST message!";
            return false;
        }
        request_out->params() = request_in->params();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST: {
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST message!";
            return false;
        }

        request_out->params() = request_in->params();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST: {
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST>(cmdu_tx,
                                                                             beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST message!";
            return false;
        }

        request_out->params() = request_in->params();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST: {
        auto request_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST>(
            cmdu_tx, beerocks_header->id());
        if (request_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST message!";
            return false;
        }

        request_out->mac() = request_in->mac();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST: {
        auto request_in = cmdu_rx.addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST failed";
            return false;
        }
        configuration_stop_on_failure_attempts = request_in->attempts();
        LOG(DEBUG) << "stop_on_failure_attempts new value: "
                   << configuration_stop_on_failure_attempts;

        if (is_backhaul_manager) {
            auto request_out = message_com::create_vs_message<
                beerocks_message::cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST>(
                cmdu_tx);
            if (request_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            request_out->attempts() = request_in->attempts();
            message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER: {
        LOG(DEBUG) << "ACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER, marking slave as operational!";
        iface_status_operational_state = true;
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST: {
        LOG(TRACE) << "ACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST";
        auto update =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST>(
            cmdu_tx, beerocks_header->id());

        if (notification_out == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST message!";
            break;
        }
        notification_out->params() = update->params();

        LOG(DEBUG) << std::endl
                   << "remove = " << int(update->params().remove) << std::endl
                   << "steeringGroupIndex = " << update->params().steeringGroupIndex << std::endl
                   << "bssid = " << network_utils::mac_to_string(update->params().cfg.bssid)
                   << std::endl
                   << "utilCheckIntervalSec = " << update->params().cfg.utilCheckIntervalSec
                   << std::endl
                   << "utilAvgCount = " << update->params().cfg.utilAvgCount << std::endl
                   << "inactCheckIntervalSec = " << update->params().cfg.inactCheckIntervalSec
                   << std::endl
                   << "inactCheckThresholdSec = " << update->params().cfg.inactCheckThresholdSec
                   << std::endl;

        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_CONTROL_STEERING_CLIENT_SET_REQUEST: {
        LOG(TRACE) << "ACTION_CONTROL_STEERING_CLIENT_SET_REQUEST";
        auto update =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST>();
        if (update == nullptr) {
            LOG(ERROR) << "addClass failed";
            return false;
        }

        // send to Monitor
        auto notification_mon_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST>(cmdu_tx,
                                                                           beerocks_header->id());

        if (notification_mon_out == nullptr) {
            LOG(ERROR) << "Failed building cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST message!";
            break;
        }

        notification_mon_out->params() = update->params();

        message_com::send_cmdu(monitor_socket, cmdu_tx);

        // send to AP MANAGER
        auto notification_ap_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST>(cmdu_tx,
                                                                             beerocks_header->id());

        if (notification_ap_out == nullptr) {
            LOG(ERROR) << "Failed building cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST message!";
            break;
        }

        notification_ap_out->params() = update->params();

        message_com::send_cmdu(ap_manager_socket, cmdu_tx);

        LOG(DEBUG) << std::endl
                   << "remove = " << notification_ap_out->params().remove << std::endl
                   << "steeringGroupIndex = " << notification_ap_out->params().steeringGroupIndex
                   << std::endl
                   << "client_mac = "
                   << network_utils::mac_to_string(notification_ap_out->params().client_mac)
                   << std::endl
                   << "bssid = " << network_utils::mac_to_string(update->params().bssid)
                   << std::endl
                   << "config.snrProbeHWM = " << notification_ap_out->params().config.snrProbeHWM
                   << std::endl
                   << "config.snrProbeLWM = " << notification_ap_out->params().config.snrProbeLWM
                   << std::endl
                   << "config.snrAuthHWM = " << notification_ap_out->params().config.snrAuthHWM
                   << std::endl
                   << "config.snrAuthLWM = " << notification_ap_out->params().config.snrAuthLWM
                   << std::endl
                   << "config.snrInactXing = " << notification_ap_out->params().config.snrInactXing
                   << std::endl
                   << "config.snrHighXing = " << notification_ap_out->params().config.snrHighXing
                   << std::endl
                   << "config.snrLowXing = " << notification_ap_out->params().config.snrLowXing
                   << std::endl
                   << "config.authRejectReason = "
                   << notification_ap_out->params().config.authRejectReason << std::endl;

        break;
    }
    default: {
        LOG(ERROR) << "Unknown CONTROL message, action_op: " << int(beerocks_header->action_op());
        return false;
    }
    }

    return true;
}

bool slave_thread::handle_cmdu_backhaul_manager_message(
    Socket *sd, std::shared_ptr<beerocks::message_com::beerocks_header> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (backhaul_manager_socket == nullptr) {
        LOG(ERROR) << "backhaul_socket == nullptr";
        return true;
    } else if (backhaul_manager_socket != sd) {
        LOG(ERROR) << "Unknown socket, ACTION_BACKHAUL action_op: "
                   << int(beerocks_header->action_op());
        return true;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_BACKHAUL_REGISTER_RESPONSE: {
        LOG(DEBUG) << "ACTION_BACKHAUL_REGISTER_RESPONSE";
        if (slave_state == STATE_WAIT_FOR_BACKHAUL_MANAGER_REGISTER_RESPONSE) {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BACKHAUL_REGISTER_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }
            LOG(DEBUG) << "goto STATE_JOIN_INIT";
            slave_state = STATE_JOIN_INIT;
        } else {
            LOG(ERROR) << "slave_state != STATE_WAIT_FOR_BACKHAUL_MANAGER_REGISTER_RESPONSE";
        }
        break;
    }

    case beerocks_message::ACTION_BACKHAUL_ENABLE_APS_REQUEST: {
        auto notification_in =
            cmdu_rx.addClass<beerocks_message::cACTION_BACKHAUL_ENABLE_APS_REQUEST>();
        if (!notification_in) {
            LOG(ERROR) << "Failed building cACTION_BACKHAUL_ENABLE_APS_REQUEST message!";
            return false;
        }

        auto notification_out =
            message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_ENABLE_APS_REQUEST>(
                cmdu_tx);
        if (!notification_out) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_ENABLE_APS_REQUEST message!";
            return false;
        }

        notification_out->channel()        = notification_in->channel();
        notification_out->bandwidth()      = notification_in->bandwidth();
        notification_out->center_channel() = notification_in->center_channel();
        LOG(DEBUG) << "Sending ACTION_APMANAGER_ENABLE_APS_REQUEST";
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);

        configuration_in_progress = true;

        break;
    }

    case beerocks_message::ACTION_BACKHAUL_CONNECTED_NOTIFICATION: {

        auto notification =
            beerocks_header->addClass<beerocks_message::cACTION_BACKHAUL_CONNECTED_NOTIFICATION>();
        if (!notification) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        LOG(DEBUG) << "ACTION_BACKHAUL_CONNECTED_NOTIFICATION";

        if (slave_state >= STATE_WAIT_FOR_BACKHAUL_MANAGER_CONNECTED_NOTIFICATION &&
            slave_state <= STATE_OPERATIONAL) {

            // Already sent join_master request, mark as reconfiguration
            if (slave_state >= STATE_WAIT_FOR_JOINED_RESPONSE && slave_state <= STATE_OPERATIONAL)
                is_backhual_reconf = true;

            is_backhaul_manager = (bool)notification->params().is_backhaul_manager;
            LOG_IF(is_backhaul_manager, DEBUG) << "Selected as backhaul manager";

            backhaul_params.gw_ipv4 = network_utils::ipv4_to_string(notification->params().gw_ipv4);
            backhaul_params.gw_bridge_mac =
                network_utils::mac_to_string(notification->params().gw_bridge_mac);
            backhaul_params.controller_bridge_mac =
                network_utils::mac_to_string(notification->params().controller_bridge_mac);
            backhaul_params.bridge_mac =
                network_utils::mac_to_string(notification->params().bridge_mac);
            backhaul_params.bridge_ipv4 =
                network_utils::ipv4_to_string(notification->params().bridge_ipv4);
            backhaul_params.backhaul_mac =
                network_utils::mac_to_string(notification->params().backhaul_mac);
            backhaul_params.backhaul_ipv4 =
                network_utils::ipv4_to_string(notification->params().backhaul_ipv4);
            backhaul_params.backhaul_bssid =
                network_utils::mac_to_string(notification->params().backhaul_bssid);
            // backhaul_params.backhaul_freq        = notification->params.backhaul_freq; // HACK temp disabled because of a bug on endian converter
            backhaul_params.backhaul_channel     = notification->params().backhaul_channel;
            backhaul_params.backhaul_is_wireless = notification->params().backhaul_is_wireless;
            backhaul_params.backhaul_iface_type  = notification->params().backhaul_iface_type;

            std::copy_n(notification->params().backhaul_scan_measurement_list,
                        beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH,
                        backhaul_params.backhaul_scan_measurement_list);

            for (unsigned int i = 0; i < message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH; i++) {
                if (backhaul_params.backhaul_scan_measurement_list[i].channel > 0) {
                    LOG(DEBUG) << "mac = "
                               << network_utils::mac_to_string(
                                      backhaul_params.backhaul_scan_measurement_list[i].mac)
                               << " channel = "
                               << int(backhaul_params.backhaul_scan_measurement_list[i].channel)
                               << " rssi = "
                               << int(backhaul_params.backhaul_scan_measurement_list[i].rssi);
                }
            }

            if (notification->params().backhaul_is_wireless) {
                backhaul_params.backhaul_iface = config.backhaul_wireless_iface;
            } else {
                backhaul_params.backhaul_iface = config.backhaul_wire_iface;
            }
            //Radio status
            if (is_backhaul_manager) {
                if (notification->params().backhaul_is_wireless) {
                    iface_status_bh =
                        eRadioStatus::BH_SIGNAL_OK; //TODO - send according to the RSSI
                    iface_status_bh_wired = eRadioStatus::OFF;
                } else {
                    iface_status_bh       = eRadioStatus::OFF;
                    iface_status_bh_wired = eRadioStatus::BH_WIRED;
                }
            } else {
                iface_status_bh       = eRadioStatus::OFF;
                iface_status_bh_wired = eRadioStatus::OFF;
            }
            LOG(DEBUG) << "goto STATE_BACKHAUL_MANAGER_CONNECTED";
            slave_state = STATE_BACKHAUL_MANAGER_CONNECTED;

        } else {
            LOG(WARNING) << "slave_state != STATE_WAIT_FOR_BACKHAUL_CONNECTED_NOTIFICATION";
        }
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_BUSY_NOTIFICATION: {
        if (slave_state != STATE_WAIT_FOR_BACKHAUL_MANAGER_CONNECTED_NOTIFICATION) {
            LOG(WARNING) << "slave_state != STATE_WAIT_FOR_BACKHAUL_CONNECTED_NOTIFICATION";
            break;
        }

        slave_state_timer = std::chrono::steady_clock::now() +
                            std::chrono::seconds(WAIT_BEFORE_SEND_BH_ENABLE_NOTIFICATION_SEC);

        LOG(DEBUG) << "goto STATE_WAIT_BACKHAUL_MANAGER_BUSY";
        slave_state = STATE_WAIT_BACKHAUL_MANAGER_BUSY;

        break;
    }
    case beerocks_message::ACTION_BACKHAUL_DISCONNECTED_NOTIFICATION: {

        if (is_slave_reset)
            break;

        LOG(DEBUG) << "ACTION_BACKHAUL_DISCONNECTED_NOTIFICATION";

        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION>();
        if (!notification) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        stopped |= bool(notification->stopped());

        is_backhaul_disconnected       = true;
        iface_status_operational_state = false;
        update_iface_status(false, false);

        slave_state_timer =
            std::chrono::steady_clock::now() +
            std::chrono::milliseconds(beerocks::IRE_MAX_WIRELESS_RECONNECTION_TIME_MSC);

        master_socket = nullptr;

        slave_reset();
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE";

        auto response_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (!response_in) {
            LOG(ERROR)
                << "Failed building ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
            return false;
        }

        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE mac="
                   << network_utils::mac_to_string(response_in->params().result.mac)
                   << " rx_rssi=" << int(response_in->params().rx_rssi)
                   << " id=" << int(beerocks_header->id());

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
            cmdu_tx, beerocks_header->id());

        if (response_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
            break;
        }

        response_out->params()            = response_in->params();
        response_out->params().src_module = beerocks::BEEROCKS_ENTITY_BACKHAUL_MANAGER;
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: {
        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE";
        auto response_in = cmdu_rx.addClass<
            beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>();
        if (!response_in) {
            LOG(ERROR) << "Failed building ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE "
                          "message!";
            return false;
        }

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
            cmdu_tx, beerocks_header->id());

        if (!response_out) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE "
                          "message!";
            break;
        }
        response_out->mac() = response_in->mac();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION: {
        LOG(DEBUG) << "ACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION";
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "Failed building ACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION message!";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());

        if (!notification_out) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION message!";
            break;
        }

        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);

        int rssi = notification_in->params().rssi;

        if (abs(last_reported_backhaul_rssi - rssi) >= BH_SIGNAL_RSSI_THRESHOLD_HYSTERESIS) {
            last_reported_backhaul_rssi = rssi;

            if (rssi < BH_SIGNAL_RSSI_THRESHOLD_LOW) {
                iface_status_bh = eRadioStatus::BH_SIGNAL_TOO_LOW;
            } else if (rssi >= BH_SIGNAL_RSSI_THRESHOLD_LOW &&
                       rssi < BH_SIGNAL_RSSI_THRESHOLD_HIGH) {
                iface_status_bh = eRadioStatus::BH_SIGNAL_OK;
            } else {
                iface_status_bh = eRadioStatus::BH_SIGNAL_TOO_HIGH;
            }
        }

        break;
    }
    default: {
        LOG(ERROR) << "Unknown BACKHAUL_MANAGER message, action_op: "
                   << int(beerocks_header->action_op());
        return false;
    }
    }

    return true;
}

bool slave_thread::handle_cmdu_platform_manager_message(
    Socket *sd, std::shared_ptr<beerocks::message_com::beerocks_header> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (platform_manager_socket != sd) {
        LOG(ERROR) << "Unknown socket, ACTION_PLATFORM_MANAGER action_op: "
                   << int(beerocks_header->action_op());
        return true;
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE: {
        LOG(TRACE) << "ACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE";
        if (slave_state == STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE) {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE failed";
                return false;
            }
            // Configuration is invalid
            if (response->valid() == 0) {
                LOG(ERROR) << "response->valid == 0";
                platform_notify_error(BPL_ERR_CONFIG_PLATFORM_REPORTED_INVALID_CONFIGURATION, "");
                stop_on_failure_attempts--;
                slave_reset();
                return true;
            }

            platform_settings = response->platform_settings();
            wlan_settings     = response->wlan_settings();

            LOG(INFO) << "local_master=" << (int)platform_settings.local_master;
            LOG(INFO) << "local_gw=" << (int)platform_settings.local_gw;

            LOG(TRACE) << "goto STATE_CONNECT_TO_BACKHAUL_MANAGER";
            slave_state = STATE_CONNECT_TO_BACKHAUL_MANAGER;
        } else {
            LOG(ERROR) << "slave_state != STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE";
        }
        break;
    }
    case beerocks_message::ACTION_PLATFORM_ARP_MONITOR_NOTIFICATION: {
        // LOG(TRACE) << "ACTION_PLATFORM_ARP_MONITOR_NOTIFICATION";
        if (master_socket) {
            auto notification_in =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION>();
            if (notification_in == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION failed";
                return false;
            }

            auto notification_out = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION>(cmdu_tx);
            if (notification_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            notification_out->params() = notification_in->params();
            send_cmdu_to_controller(cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION: {
        LOG(TRACE) << "ACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION";

        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION failed";
            return false;
        }

        // slave only reacts to band_enabled change
        if (wlan_settings.band_enabled != notification->wlan_settings().band_enabled) {
            LOG(DEBUG) << "band_enabled changed - performing slave_reset()";
            slave_reset();
        }
        break;
    }
    case beerocks_message::ACTION_PLATFORM_OPERATIONAL_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_OPERATIONAL_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_PLATFORM_OPERATIONAL_NOTIFICATION failed";
            return false;
        }

        LOG(DEBUG) << "sending master operational notification, new_oper_state="
                   << int(notification_in->operational())
                   << " bridge_mac=" << backhaul_params.bridge_mac;

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->operational() = notification_in->operational();
        notification_out->bridge_mac() = network_utils::mac_from_string(backhaul_params.bridge_mac);
        if (master_socket) {
            send_cmdu_to_controller(cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION: {
        auto notification =
            beerocks_header
                ->addClass<beerocks_message::cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass ACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION failed";
            return false;
        }

        if (notification->op() == beerocks_message::eDHCPOp_Add ||
            notification->op() == beerocks_message::eDHCPOp_Old) {
            std::string client_mac = network_utils::mac_to_string(notification->mac());
            std::string client_ip  = network_utils::ipv4_to_string(notification->ipv4());

            LOG(DEBUG) << "ACTION_DHCP_LEASE_ADDED_NOTIFICATION mac " << client_mac
                       << " ip = " << client_ip << " name="
                       << std::string(notification->hostname(message::NODE_NAME_LENGTH));

            // notify master
            if (master_socket) {
                auto master_notification = message_com::create_vs_message<
                    beerocks_message::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION>(cmdu_tx);
                if (master_notification == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return false;
                }

                master_notification->mac()  = notification->mac();
                master_notification->ipv4() = notification->ipv4();
                string_utils::copy_string(master_notification->name(message::NODE_NAME_LENGTH),
                                          notification->hostname(message::NODE_NAME_LENGTH),
                                          message::NODE_NAME_LENGTH);
                send_cmdu_to_controller(cmdu_tx);
            }

        } else {
            LOG(DEBUG) << "ACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION op " << notification->op()
                       << " mac " << network_utils::mac_to_string(notification->mac())
                       << " ip = " << network_utils::ipv4_to_string(notification->ipv4());
        }
        break;
    }
    case beerocks_message::ACTION_PLATFORM_ARP_QUERY_RESPONSE: {
        LOG(TRACE) << "ACTION_PLATFORM_ARP_QUERY_RESPONSE";
        if (master_socket) {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_PLATFORM_ARP_QUERY_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_ARP_QUERY_RESPONSE failed";
                return false;
            }

            auto response_out = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_ARP_QUERY_RESPONSE>(cmdu_tx,
                                                                      beerocks_header->id());
            if (response_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            response_out->params() = response->params();
            send_cmdu_to_controller(cmdu_tx);
        }
        break;
    }

    default: {
        LOG(ERROR) << "Unknown PLATFORM_MANAGER message, action_op: "
                   << int(beerocks_header->action_op());
        return false;
    }
    }

    return true;
}

bool slave_thread::handle_cmdu_ap_manager_message(
    Socket *sd, std::shared_ptr<beerocks::message_com::beerocks_header> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (ap_manager_socket == nullptr) {
        if (beerocks_header->action_op() !=
            beerocks_message::ACTION_APMANAGER_INIT_DONE_NOTIFICATION) {
            LOG(ERROR) << "Not ACTION_APMANAGER_INIT_DONE_NOTIFICATION, action_op: "
                       << int(beerocks_header->action_op());
            return true;
        }
    } else if (ap_manager_socket != sd) {
        LOG(ERROR) << "Unknown socket, ACTION_APMANAGER action_op: "
                   << int(beerocks_header->action_op())
                   << ", ap_manager_socket=" << intptr_t(ap_manager_socket)
                   << ", incoming sd=" << intptr_t(sd);
        return true;
    } else if (beerocks_header->action_op() ==
               beerocks_message::ACTION_APMANAGER_HEARTBEAT_NOTIFICATION) {
        ap_manager_last_seen       = std::chrono::steady_clock::now();
        ap_manager_retries_counter = 0;
        return true;
    } else if (slave_state > STATE_BACKHAUL_MANAGER_CONNECTED && master_socket == nullptr) {
        LOG(ERROR) << "master_socket == nullptr ACTION_APMANAGER action_op: "
                   << int(beerocks_header->action_op());
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_APMANAGER_INIT_DONE_NOTIFICATION: {
        LOG(INFO) << "received ACTION_APMANAGER_INIT_DONE_NOTIFICATION from sd=" << intptr_t(sd);
        ap_manager_socket = sd;
        slave_state       = STATE_WAIT_FOR_AP_MANAGER_JOINED;
        break;
    }
    case beerocks_message::ACTION_APMANAGER_JOINED_NOTIFICATION: {
        LOG(INFO) << "received ACTION_APMANAGER_JOINED_NOTIFICATION";
        auto notification =
            beerocks_header->addClass<beerocks_message::cACTION_APMANAGER_JOINED_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_JOINED_NOTIFICATION failed";
            return false;
        }
        hostap_params    = notification->params();
        hostap_cs_params = notification->cs_params();
        if (slave_state == STATE_WAIT_FOR_AP_MANAGER_JOINED) {
            slave_state = STATE_AP_MANAGER_JOINED;
        } else {
            LOG(ERROR) << "ACTION_APMANAGER_JOINED_NOTIFICATION, slave_state != "
                          "STATE_WAIT_FOR_AP_MANAGER_JOINED";
        }
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE: {
        auto response_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass "
                          "cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE "
                          "failed";
            return false;
        }
        LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE";

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE>(
            cmdu_tx);
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        response_out->success() = response_in->success();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION failed";
            return false;
        }
        LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION on vap_id="
                  << int(response_in->vap_id());
        if (response_in->vap_id() == beerocks::IFACE_RADIO_ID) {
            LOG(WARNING) << __FUNCTION__ << "AP_Disabled on radio, slave reset";
            if (configuration_in_progress) {
                LOG(INFO) << "configuration in progress, ignoring";
                detach_on_conf_change = true;
                break;
            }
            slave_reset();
        } else {
            auto response_out = message_com::create_vs_message<
                beerocks_message::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION>(cmdu_tx);
            if (response_out == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            response_out->vap_id() = response_in->vap_id();
            send_cmdu_to_controller(cmdu_tx);
        }
        break;
    }
    case beerocks_message::ACTION_APMANAGER_ENABLE_APS_RESPONSE: {
        configuration_in_progress = false;
        LOG(INFO) << "received ACTION_APMANAGER_ENABLE_APS_RESPONSE";

        auto response = cmdu_rx.addClass<beerocks_message::cACTION_APMANAGER_ENABLE_APS_RESPONSE>();
        if (!response) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_ENABLE_APS_RESPONSE failed";
            return false;
        }

        if (!response->success()) {
            LOG(ERROR) << "failed to enable APs";
            slave_reset();
        }

        if (detach_on_conf_change) {
            LOG(DEBUG) << "detach occurred on wifi conf change, slave reset!";
            slave_reset();
        }

        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION failed";
            return false;
        }
        LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION vap_id="
                  << int(notification_in->vap_id());

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->vap_id()   = notification_in->vap_id();
        notification_out->vap_info() = notification_in->vap_info();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION failed";
            return false;
        }

        LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION";

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION: {
        LOG(INFO) << "ACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION";
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION failed";
            return false;
        }
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION>(cmdu_tx,
                                                                       beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->cs_params()     = notification_in->cs_params();
        auto tuple_in_supported_channels  = notification_in->supported_channels_list(0);
        auto tuple_out_supported_channels = notification_out->supported_channels(0);
        std::copy_n(&std::get<1>(tuple_in_supported_channels), message::SUPPORTED_CHANNELS_LENGTH,
                    &std::get<1>(tuple_out_supported_channels));
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION: {
        LOG(INFO) << "ACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION";

        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION failed";
            return false;
        }
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION>(cmdu_tx,
                                                                       beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->cs_params() = notification_in->cs_params();
        send_cmdu_to_controller(cmdu_tx);

        if (wireless_utils::is_dfs_channel(hostap_cs_params.channel)) {
            LOG(INFO) << "AP is in DFS channel: " << (int)hostap_cs_params.channel;
            iface_status_ap = beerocks::eRadioStatus::AP_DFS_CAC;
        } else {
            iface_status_ap = beerocks::eRadioStatus::AP_OK;
        }

        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION: {
        LOG(INFO) << "received ACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION";
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION failed";
            return false;
        }
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION>(cmdu_tx,
                                                                             beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->cs_params() = notification_in->cs_params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        auto response_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE failed";
            return false;
        }
        LOG(INFO) << "APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE mac="
                  << network_utils::mac_to_string(response_in->params().result.mac)
                  << " rx_rssi=" << int(response_in->params().rx_rssi)
                  << " id=" << int(beerocks_header->id());

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
            cmdu_tx, beerocks_header->id());

        if (response_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
            break;
        }

        response_out->params()            = response_in->params();
        response_out->params().src_module = beerocks::BEEROCKS_ENTITY_AP_MANAGER;
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION: {
        auto notification_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION failed";
            return false;
        }

        std::string client_mac = network_utils::mac_to_string(notification_in->params().mac);
        LOG(INFO) << "client disconnected sta_mac=" << client_mac;

        //notify monitor
        {
            auto notification_out = message_com::create_vs_message<
                beerocks_message::cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST>(
                cmdu_tx, beerocks_header->id());

            if (notification_out == nullptr) {
                LOG(ERROR)
                    << "Failed building cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST message!";
                break;
            }
            notification_out->mac() = notification_in->params().mac;
            message_com::send_cmdu(monitor_socket, cmdu_tx);
        }

        //notify master
        if (!master_socket) {
            LOG(DEBUG) << "Controller is not connected";
            return true;
        }

        // build 1905.1 message CMDU to send to the controller
        if (!cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE)) {
            LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
            return false;
        }

        auto client_association_event_tlv = cmdu_tx.addClass<wfa_map::tlvClientAssociationEvent>();
        if (!client_association_event_tlv) {
            LOG(ERROR) << "addClass tlvClientAssociationEvent failed";
            return false;
        }
        client_association_event_tlv->client_mac() = notification_in->params().mac;
        client_association_event_tlv->bssid()      = notification_in->params().bssid;
        client_association_event_tlv->association_event() =
            wfa_map::tlvClientAssociationEvent::CLIENT_HAS_LEFT_THE_BSS;

        // Add vendor specific tlv
        auto vs_tlv =
            message_com::add_vs_tlv<beerocks_message::tlvVsClientAssociationEvent>(cmdu_tx);

        if (!vs_tlv) {
            LOG(ERROR) << "add_vs_tlv tlvVsClientAssociationEvent failed";
            return false;
        }

        vs_tlv->mac()               = notification_in->params().mac;
        vs_tlv->bssid()             = notification_in->params().bssid;
        vs_tlv->vap_id()            = notification_in->params().vap_id;
        vs_tlv->disconnect_reason() = notification_in->params().reason;
        vs_tlv->disconnect_source() = notification_in->params().source;
        vs_tlv->disconnect_type()   = notification_in->params().type;

        send_cmdu_to_controller(cmdu_tx);

        break;
    }
    case beerocks_message::ACTION_APMANAGER_ACK: {
        auto response_in = beerocks_header->addClass<beerocks_message::cACTION_APMANAGER_ACK>();
        if (!response_in) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE failed";
            return false;
        }

        auto cmdu_tx_header =
            cmdu_tx.create(beerocks_header->id(), ieee1905_1::eMessageType::ACK_MESSAGE);

        if (!cmdu_tx_header) {
            LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
            return false;
        }

        LOG(DEBUG) << "sending ACK message back to controller";
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE failed";
            return false;
        }
        LOG(INFO) << "ACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE, rep_mode="
                  << int(response_in->params().status_code);

        if (!cmdu_tx.create(0, ieee1905_1::eMessageType::CLIENT_STEERING_BTM_REPORT_MESSAGE)) {
            LOG(ERROR) << "cmdu creation of type CLIENT_STEERING_BTM_REPORT_MESSAGE, has failed";
            return false;
        }
        auto steering_btm_report_tlv = cmdu_tx.addClass<wfa_map::tlvSteeringBTMReport>();
        if (!steering_btm_report_tlv) {
            LOG(ERROR) << "addClass wfa_map::tlvSteeringBTMReport failed";
            return false;
        }
        //TODO Add target/source BSSID
        steering_btm_report_tlv->sta_mac()         = response_in->params().mac;
        steering_btm_report_tlv->btm_status_code() = response_in->params().status_code;

        LOG(DEBUG) << "sending CLIENT_STEERING_BTM_REPORT_MESSAGE back to controller";
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: {
        auto response_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR)
                << "addClass ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE failed";
            return false;
        }

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE "
                          "message!";
            break;
        }
        LOG(INFO) << "ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE";
        response_out->mac() = response_in->mac();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION failed";
            return false;
        }
        LOG(TRACE) << "received ACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION";

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        iface_status_ap = beerocks::eRadioStatus::AP_OK;
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION failed";
            return false;
        }
        LOG(TRACE) << "received ACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION";

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION failed";
            return false;
        }
        LOG(TRACE) << "received ACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION";
        std::string client_mac = network_utils::mac_to_string(notification_in->params().mac);
        LOG(INFO) << "client associated sta_mac=" << client_mac;

        if (!master_socket) {
            LOG(DEBUG) << "Controller is not connected";
            return true;
        }
        // build 1905.1 message CMDU to send to the controller
        if (!cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE)) {
            LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
            return false;
        }

        auto client_association_event_tlv = cmdu_tx.addClass<wfa_map::tlvClientAssociationEvent>();
        if (!client_association_event_tlv) {
            LOG(ERROR) << "addClass tlvClientAssociationEvent failed";
            return false;
        }
        client_association_event_tlv->client_mac() = notification_in->params().mac;
        client_association_event_tlv->bssid()      = notification_in->params().bssid;
        client_association_event_tlv->association_event() =
            wfa_map::tlvClientAssociationEvent::CLIENT_HAS_JOINED_THE_BSS;

        // Add vendor specific tlv
        auto vs_tlv =
            message_com::add_vs_tlv<beerocks_message::tlvVsClientAssociationEvent>(cmdu_tx);

        if (!vs_tlv) {
            LOG(ERROR) << "add_vs_tlv tlvVsClientAssociationEvent failed";
            return false;
        }

        vs_tlv->mac()          = notification_in->params().mac;
        vs_tlv->bssid()        = notification_in->params().bssid;
        vs_tlv->vap_id()       = notification_in->params().vap_id;
        vs_tlv->capabilities() = notification_in->params().capabilities;

        send_cmdu_to_controller(cmdu_tx);

        break;
    }
    case beerocks_message::ACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }

    case beerocks_message::ACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass "
                          "cACTION_APMANAGER_CLIENT_ScACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_"
                          "NOTIFICATIONOFTBLOCK_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    default: {
        LOG(ERROR) << "Unknown AP_MANAGER message, action_op: "
                   << int(beerocks_header->action_op());
        return false;
    }
    }

    return true;
}

bool slave_thread::handle_cmdu_monitor_message(
    Socket *sd, std::shared_ptr<beerocks::message_com::beerocks_header> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx)
{
    if (monitor_socket == nullptr) {
        if (beerocks_header->action_op() != beerocks_message::ACTION_MONITOR_JOINED_NOTIFICATION) {
            LOG(ERROR) << "Not MONITOR_JOINED_NOTIFICATION, action_op: "
                       << int(beerocks_header->action_op());
            return true;
        }
    } else if (monitor_socket != sd) {
        LOG(WARNING) << "Unknown socket, ACTION_MONITOR action_op: "
                     << int(beerocks_header->action_op());
        return true;
    } else if (beerocks_header->action_op() ==
               beerocks_message::ACTION_MONITOR_HEARTBEAT_NOTIFICATION) {
        monitor_last_seen       = std::chrono::steady_clock::now();
        monitor_retries_counter = 0;
        return true;
    } else if (master_socket == nullptr) {
        LOG(WARNING) << "master_socket == nullptr, MONITOR action_op: "
                     << int(beerocks_header->action_op());
    }

    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_MONITOR_JOINED_NOTIFICATION: {
        if (slave_state == STATE_WAIT_FOR_MONITOR_JOINED) {
            LOG(INFO) << "ACTION_MONITOR_JOINED_NOTIFICATION";
            monitor_socket = sd;
            LOG(INFO) << "goto STATE_BACKHAUL_ENABLE ";
            slave_state = STATE_BACKHAUL_ENABLE;
        } else {
            LOG(ERROR) << "ACTION_MONITOR_JOINED_NOTIFICATION, but slave_state != "
                          "STATE_WAIT_FOR_MONITOR_JOINED";
        }
        break;
    }
    case beerocks_message::ACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION failed";
            return false;
        }
        LOG(INFO) << "received ACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION";
        if (response_in->vap_id() == beerocks::IFACE_RADIO_ID) {
            LOG(WARNING) << __FUNCTION__ << "AP_Disabled on radio, slave reset";
            if (configuration_in_progress) {
                LOG(INFO) << "configuration is in progress, ignoring";
                detach_on_conf_change = true;
                break;
            }
            slave_reset();
        }
        break;
    }
    case beerocks_message::ACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION: {
        auto notification_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_HOSTAP_STATUS_CHANGED_NOTIFICATION failed";
            return false;
        }
        std::stringstream print_str;
        if (notification_in->new_tx_state() != -1) {
            print_str << " new tx state: "
                      << std::string(notification_in->new_tx_state() ? "on" : "off");
        }
        if (notification_in->new_hostap_enabled_state() != -1) {
            print_str << " | new hostap_enabled state: "
                      << std::string(notification_in->new_hostap_enabled_state() ? "on" : "off");
        }
        LOG(INFO) << "ACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION" << print_str.str();

        if (slave_state == STATE_OPERATIONAL && notification_in->new_tx_state() == 1 &&
            notification_in->new_hostap_enabled_state() == 1) {

            // marking slave as operational if it is on operational state with tx on and hostap is enabled
            iface_status_operational_state = true;
            slave_resets_counter           = 0;
        } else {
            iface_status_operational_state = false;
        }

        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        auto response_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE failed";
            break;
        }
        LOG(INFO) << "ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE mac="
                  << network_utils::mac_to_string(response_in->params().result.mac)
                  << " rx_rssi=" << int(response_in->params().rx_rssi)
                  << " id=" << int(beerocks_header->id());

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
            break;
        }

        response_out->params()            = response_in->params();
        response_out->params().src_module = beerocks::BEEROCKS_ENTITY_MONITOR;
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR)
                << "addClass ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION failed";
            break;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
            break;
        }
        notification_out->mac() = notification_in->mac();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE: {
        /*
             * the following code will break if the structure of
             * message::sACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE
             * will be different from
             * message::sACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE
             */

        // LOG(DEBUG) << "Received ACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE"; // the print is flooding the log

        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE failed";
            return false;
        }

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        response_out->ap_stats() = response_in->ap_stats();
        auto sta_stats_size      = response_in->sta_stats_size();
        if (sta_stats_size > 0) {
            if (!response_out->alloc_sta_stats(sta_stats_size)) {
                LOG(ERROR) << "Failed buffer allocation to size=" << int(sta_stats_size);
                break;
            }
            auto sta_stats_tuple_in  = response_in->sta_stats(0);
            auto sta_stats_tuple_out = response_out->sta_stats(0);
            std::copy_n(&std::get<1>(sta_stats_tuple_in), sta_stats_size,
                        &std::get<1>(sta_stats_tuple_out));
        }

        // LOG(DEBUG) << "send ACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE"; // the print is flooding the log

        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION failed";
            break;
        }
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION message!";
            break;
        }
        notification_out->mac() = notification_in->mac();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE: {
        LOG(TRACE) << "ACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE id=" << int(beerocks_header->id());
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE failed";
            break;
        }
        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE>(cmdu_tx,
                                                                          beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE message!";
            break;
        }
        response_out->params() = response_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE failed";
            break;
        }
        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE message!";
            break;
        }
        response_out->params() = response_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE failed";
            break;
        }
        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE message!";
            break;
        }
        response_out->params() = response_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE: {
        auto response_in = cmdu_rx.addClass<
            beerocks_message::cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE failed";
            break;
        }
        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE message!";
            break;
        }
        response_out->params() = response_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: {
        LOG(INFO) << "ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: action_op: "
                  << int(beerocks_header->action_op());
        auto response_in = cmdu_rx.addClass<
            beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE failed";
            break;
        }
        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE "
                          "message!";
            break;
        }
        response_out->mac() = response_in->mac();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION failed";
            break;
        }
        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building ACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION message!";
            break;
        }
        // Only mac id is the part of notification now, if this changes in future this message will break
        response_out->mac() = response_in->mac();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_ERROR_NOTIFICATION: {
        auto notification =
            beerocks_header->addClass<beerocks_message::cACTION_MONITOR_ERROR_NOTIFICATION>();
        if (notification == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_ERROR_NOTIFICATION failed";
            return false;
        }
        LOG(INFO) << "ACTION_MONITOR_ERROR_NOTIFICATION, error_code="
                  << int(notification->error_code());

        if (configuration_in_progress) {
            LOG(INFO) << "configuration is in progress, ignoring";
            detach_on_conf_change = true;
            break;
        }

        monitor_thread::eThreadErrors err_code =
            monitor_thread::eThreadErrors(notification->error_code());
        if (err_code == monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_HOSTAP_DISABLED) {
            platform_notify_error(BPL_ERR_MONITOR_HOSTAP_DISABLED, "");
        } else if (err_code == monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_ATTACH_FAIL) {
            platform_notify_error(BPL_ERR_MONITOR_ATTACH_FAIL, "");
        } else if (err_code == monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_SUDDEN_DETACH) {
            platform_notify_error(BPL_ERR_MONITOR_SUDDEN_DETACH, "");
        } else if (err_code ==
                   monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_HAL_DISCONNECTED) {
            platform_notify_error(BPL_ERR_MONITOR_HAL_DISCONNECTED, "");
        } else if (err_code ==
                   monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_REPORT_PROCESS_FAIL) {
            platform_notify_error(BPL_ERR_MONITOR_REPORT_PROCESS_FAIL, "");
        }

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_ERROR_NOTIFICATION_ACK>(cmdu_tx);
        if (response_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        message_com::send_cmdu(monitor_socket, cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION: {
        auto notification_in = cmdu_rx.addClass<
            beerocks_message::cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building  "
                          "cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION: {
        auto notification_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE: {
        auto notification_in =
            cmdu_rx
                .addClass<beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR)
                << "Failed building cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE>();
        if (notification_in == nullptr) {
            LOG(ERROR) << "addClass cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE message!";
            return false;
        }
        notification_out->params() = notification_in->params();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    default: {
        LOG(ERROR) << "Unknown MONITOR message, action_op: " << int(beerocks_header->action_op());
        return false;
    }
    }

    return true;
}

bool slave_thread::slave_fsm(bool &call_slave_select)
{
    bool slave_ok = true;

    switch (slave_state) {
    case STATE_WAIT_BERFORE_INIT: {
        if (std::chrono::steady_clock::now() > slave_state_timer) {
            is_backhaul_disconnected = false;
            LOG(TRACE) << "goto STATE_INIT";
            slave_state = STATE_INIT;
        }
        break;
    }
    case STATE_INIT: {
        LOG(INFO) << "STATE_INIT";
        slave_state = STATE_CONNECT_TO_PLATFORM_MANAGER;
        break;
    }
    case STATE_CONNECT_TO_PLATFORM_MANAGER: {
        platform_manager_socket = new SocketClient(platform_manager_uds);
        std::string err         = platform_manager_socket->getError();
        if (!err.empty()) {
            delete platform_manager_socket;
            platform_manager_socket = nullptr;

            LOG(WARNING) << "Unable to connect to Platform Manager: " << err;
            if (++connect_platform_retry_counter >= CONNECT_PLATFORM_RETRY_COUNT_MAX) {
                LOG(ERROR) << "Failed connecting to Platform Manager! Resetting...";
                platform_notify_error(BPL_ERR_SLAVE_FAILED_CONNECT_TO_PLATFORM_MANAGER, "");
                stop_on_failure_attempts--;
                slave_reset();
                connect_platform_retry_counter = 0;
            } else {
                LOG(INFO) << "Retrying in " << CONNECT_PLATFORM_RETRY_SLEEP << " milliseconds...";
                UTILS_SLEEP_MSEC(CONNECT_PLATFORM_RETRY_SLEEP);
                break;
            }

        } else {
            add_socket(platform_manager_socket);

            // CMDU Message
            auto request = message_com::create_vs_message<
                beerocks_message::cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST>(cmdu_tx);

            if (request == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            string_utils::copy_string(request->iface_name(message::IFACE_NAME_LENGTH),
                                      config.hostap_iface.c_str(), message::IFACE_NAME_LENGTH);
            message_com::send_cmdu(platform_manager_socket, cmdu_tx);

            LOG(TRACE) << "send ACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST";
            LOG(TRACE) << "goto STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE";
            slave_state_timer =
                std::chrono::steady_clock::now() +
                std::chrono::seconds(WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE_TIMEOUT_SEC);
            slave_state = STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE;
        }
        break;
    }
    case STATE_WAIT_FOR_PLATFORM_MANAGER_CREDENTIALS_UPDATE_RESPONSE: {
        break;
    }
    case STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE: {
        if (std::chrono::steady_clock::now() > slave_state_timer) {
            LOG(ERROR) << "STATE_WAIT_FOR_PLATFORM_MANAGER_REGISTER_RESPONSE timeout!";
            platform_notify_error(BPL_ERR_SLAVE_PLATFORM_MANAGER_REGISTER_TIMEOUT, "");
            stop_on_failure_attempts--;
            slave_reset();
        }
        break;
    }
    case STATE_CONNECT_TO_BACKHAUL_MANAGER: {
        if (backhaul_manager_socket == nullptr) {
            LOG(DEBUG) << "create backhaul_manager_socket";
            backhaul_manager_socket = new SocketClient(backhaul_manager_uds);
            std::string err         = backhaul_manager_socket->getError();
            if (!err.empty()) {
                LOG(ERROR) << "backhaul_manager_socket: " << err;
                backhaul_manager_stop();
                platform_notify_error(BPL_ERR_SLAVE_CONNECTING_TO_BACKHAUL_MANAGER,
                                      "iface=" + config.backhaul_wireless_iface);
                stop_on_failure_attempts--;
                slave_reset();
                break;
            } else {
                add_socket(backhaul_manager_socket);
            }
        } else {
            LOG(DEBUG) << "using existing backhaul_manager_socket=0x"
                       << intptr_t(backhaul_manager_socket);
        }

        // CMDU Message
        auto request =
            message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_REGISTER_REQUEST>(
                cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        if (platform_settings.local_gw || config.backhaul_wireless_iface.empty()) {
            memset(request->sta_iface(message::IFACE_NAME_LENGTH), 0, message::IFACE_NAME_LENGTH);
        } else {
            string_utils::copy_string(request->sta_iface(message::IFACE_NAME_LENGTH),
                                      config.backhaul_wireless_iface.c_str(),
                                      message::IFACE_NAME_LENGTH);
        }
        string_utils::copy_string(request->hostap_iface(message::IFACE_NAME_LENGTH),
                                  config.hostap_iface.c_str(), message::IFACE_NAME_LENGTH);

        request->local_master()         = platform_settings.local_master;
        request->local_gw()             = platform_settings.local_gw;
        request->sta_iface_filter_low() = config.backhaul_wireless_iface_filter_low;
        request->onboarding()           = platform_settings.onboarding;
        LOG(INFO) << "ACTION_BACKHAUL_REGISTER_REQUEST local_master="
                  << int(platform_settings.local_master)
                  << " local_gw=" << int(platform_settings.local_gw)
                  << " hostap_iface=" << request->hostap_iface(message::IFACE_NAME_LENGTH)
                  << " sta_iface=" << request->sta_iface(message::IFACE_NAME_LENGTH)
                  << " onboarding=" << int(request->onboarding());

        message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);
        LOG(TRACE) << "send ACTION_BACKHAUL_REGISTER_REQUEST";
        LOG(TRACE) << "goto STATE_WAIT_FOR_BACKHAUL_MANAGER_REGISTER_RESPONSE";
        slave_state = STATE_WAIT_FOR_BACKHAUL_MANAGER_REGISTER_RESPONSE;

        break;
    }
    case STATE_WAIT_RETRY_CONNECT_TO_BACKHAUL_MANAGER: {
        if (std::chrono::steady_clock::now() > slave_state_timer) {
            LOG(DEBUG) << "retrying to connect connecting to backhaul manager";
            LOG(TRACE) << "goto STATE_CONNECT_TO_BACKHAUL_MANAGER";
            slave_state = STATE_CONNECT_TO_BACKHAUL_MANAGER;
        }
        break;
    }
    case STATE_WAIT_FOR_BACKHAUL_MANAGER_REGISTER_RESPONSE: {
        break;
    }
    case STATE_JOIN_INIT: {

        LOG(DEBUG) << "onboarding: " << int(platform_settings.onboarding);
        if (platform_settings.onboarding) {
            LOG(TRACE) << "goto STATE_ONBOARDING";
            slave_state = STATE_ONBOARDING;
            break;
        } else if (!wlan_settings.band_enabled) {
            LOG(DEBUG) << "wlan_settings.band_enabled=false";
            LOG(TRACE) << "goto STATE_BACKHAUL_ENABLE";
            slave_state = STATE_BACKHAUL_ENABLE;
            break;
        }

        if (!platform_settings.local_gw) {
            is_backhaul_manager   = false;
            iface_status_bh_wired = eRadioStatus::OFF;
        }

        // mark slave as in non operational state
        iface_status_operational_state = false;

        LOG(TRACE) << "goto STATE_START_AP_MANAGER";
        is_slave_reset = false;
        slave_state    = STATE_START_AP_MANAGER;
        break;
    }
    case STATE_START_AP_MANAGER: {

        LOG(INFO) << "STATE_START_AP_MANAGER";
        if (ap_manager_start()) {
            LOG(TRACE) << "goto STATE_WAIT_FOR_AP_MANAGER_JOINED";
            slave_state = STATE_WAIT_FOR_AP_MANAGER_JOINED;
        } else {
            LOG(ERROR) << "ap_manager_start() failed!";
            platform_notify_error(BPL_ERR_AP_MANAGER_START, "");
            stop_on_failure_attempts--;
            slave_reset();
        }
        break;
    }
    case STATE_WAIT_FOR_AP_MANAGER_JOINED: {
        break;
    }
    case STATE_AP_MANAGER_JOINED: {
        LOG(TRACE) << "goto STATE_START_MONITOR";
        slave_state = STATE_START_MONITOR;
        break;
    }
    case STATE_START_MONITOR: {
        monitor_start();
        LOG(TRACE) << "goto STATE_WAIT_FOR_MONITOR_JOINED";
        slave_state = STATE_WAIT_FOR_MONITOR_JOINED;
        break;
    }
    case STATE_WAIT_FOR_MONITOR_JOINED: {
        break;
    }
    case STATE_BACKHAUL_ENABLE: {
        bool error = false;
        if (!config.backhaul_wire_iface.empty()) {
            if (config.backhaul_wire_iface_type == beerocks::IFACE_TYPE_UNSUPPORTED) {
                LOG(DEBUG) << "backhaul_wire_iface_type is UNSUPPORTED";
                platform_notify_error(BPL_ERR_CONFIG_BACKHAUL_WIRED_INTERFACE_IS_UNSUPPORTED, "");
                error = true;
            }
        }
        if (!config.backhaul_wireless_iface.empty()) {
            if (config.backhaul_wireless_iface_type == beerocks::IFACE_TYPE_UNSUPPORTED) {
                LOG(DEBUG) << "backhaul_wireless_iface is UNSUPPORTED";
                platform_notify_error(BPL_ERR_CONFIG_BACKHAUL_WIRELESS_INTERFACE_IS_UNSUPPORTED,
                                      "");
                error = true;
            }
        }
        if (config.backhaul_wire_iface.empty() && config.backhaul_wireless_iface.empty()) {
            LOG(DEBUG) << "No valid backhaul iface!";
            platform_notify_error(BPL_ERR_CONFIG_NO_VALID_BACKHAUL_INTERFACE, "");
            error = true;
        }

        if (error) {
            stop_on_failure_attempts--;
            slave_reset();
        } else {
            // backhaul manager will request for backhaul iface and tx enable after receiving ACTION_BACKHAUL_ENABLE,
            // when wireless connection is required
            LOG(TRACE) << "goto STATE_SEND_BACKHAUL_MANAGER_ENABLE";
            slave_state = STATE_SEND_BACKHAUL_MANAGER_ENABLE;
        }
        break;
    }
    case STATE_SEND_BACKHAUL_MANAGER_ENABLE: {

        // CMDU Message
        auto bh_enable =
            message_com::create_vs_message<beerocks_message::cACTION_BACKHAUL_ENABLE>(cmdu_tx);
        if (bh_enable == nullptr) {
            LOG(ERROR) << "Failed building message!";
            break;
        }

        if (!platform_settings.local_gw) {
            // Wireless config

            // TODO: On passive mode, mem_only_psk is always be set, so supplying the credentials
            // to the backhaul manager will no longer be necessary, and therefore should be be
            // removed completely from beerocks including the BPL.
            string_utils::copy_string(bh_enable->ssid(message::WIFI_SSID_MAX_LENGTH),
                                      platform_settings.back_ssid, message::WIFI_SSID_MAX_LENGTH);
            string_utils::copy_string(bh_enable->pass(message::WIFI_PASS_MAX_LENGTH),
                                      platform_settings.back_pass, message::WIFI_PASS_MAX_LENGTH);
            bh_enable->security_type() = static_cast<uint32_t>(
                platform_to_bwl_security(platform_settings.back_security_type));
            bh_enable->mem_only_psk() = platform_settings.mem_only_psk;
            bh_enable->backhaul_preferred_radio_band() =
                platform_settings.backhaul_preferred_radio_band;

            string_utils::copy_string(bh_enable->wire_iface(message::IFACE_NAME_LENGTH),
                                      config.backhaul_wire_iface.c_str(),
                                      message::IFACE_NAME_LENGTH);

            bh_enable->wire_iface_type()     = config.backhaul_wire_iface_type;
            bh_enable->wireless_iface_type() = config.backhaul_wireless_iface_type;
        }

        bh_enable->iface_mac()     = hostap_params.iface_mac;
        bh_enable->iface_is_5ghz() = hostap_params.iface_is_5ghz;
        bh_enable->preferred_bssid() =
            network_utils::mac_from_string(config.backhaul_preferred_bssid);

        // necessary to erase all pending enable slaves on backhaul manager
        string_utils::copy_string(bh_enable->ap_iface(message::IFACE_NAME_LENGTH),
                                  config.hostap_iface.c_str(), message::IFACE_NAME_LENGTH);
        string_utils::copy_string(bh_enable->sta_iface(message::IFACE_NAME_LENGTH),
                                  config.backhaul_wireless_iface.c_str(),
                                  message::IFACE_NAME_LENGTH);
        string_utils::copy_string(bh_enable->bridge_iface(message::IFACE_NAME_LENGTH),
                                  config.bridge_iface.c_str(), message::IFACE_NAME_LENGTH);

        // Send the message
        LOG(DEBUG) << "send ACTION_BACKHAUL_ENABLE for mac "
                   << network_utils::mac_to_string(bh_enable->iface_mac());
        if (!message_com::send_cmdu(backhaul_manager_socket, cmdu_tx)) {
            slave_reset();
        }

        // Next state
        LOG(TRACE) << "goto STATE_WAIT_FOR_BACKHAUL_MANAGER_CONNECTED_NOTIFICATION";
        slave_state = STATE_WAIT_FOR_BACKHAUL_MANAGER_CONNECTED_NOTIFICATION;
        break;
    }
    case STATE_WAIT_FOR_BACKHAUL_MANAGER_CONNECTED_NOTIFICATION: {
        break;
    }
    case STATE_WAIT_BACKHAUL_MANAGER_BUSY: {
        if (std::chrono::steady_clock::now() > slave_state_timer) {
            LOG(TRACE) << "goto STATE_SEND_BACKHAUL_MANAGER_ENABLE";
            slave_state = STATE_SEND_BACKHAUL_MANAGER_ENABLE;
        }
        break;
    }
    case STATE_BACKHAUL_MANAGER_CONNECTED: {
        LOG(TRACE) << "MASTER_CONNECTED";

        master_socket = backhaul_manager_socket;
        master_socket->setPeerMac(backhaul_params.controller_bridge_mac);

        if (!wlan_settings.band_enabled) {

            iface_status_operational_state = true;
            iface_status_ap                = eRadioStatus::OFF;
            LOG(TRACE) << "goto STATE_OPERATIONAL";
            slave_state = STATE_OPERATIONAL;
            break;
        }

        if (platform_settings.local_gw) {
            //TODO get bridge_iface from platform manager
            network_utils::iface_info bridge_info;
            network_utils::get_iface_info(bridge_info, config.bridge_iface);
            backhaul_params.bridge_iface = config.bridge_iface;
            //

            backhaul_params.gw_ipv4        = bridge_info.ip;
            backhaul_params.gw_bridge_mac  = bridge_info.mac;
            backhaul_params.bridge_mac     = bridge_info.mac;
            backhaul_params.bridge_ipv4    = bridge_info.ip;
            backhaul_params.backhaul_iface = backhaul_params.bridge_iface;
            backhaul_params.backhaul_mac   = bridge_info.mac;
            backhaul_params.backhaul_ipv4  = bridge_info.ip;
            backhaul_params.backhaul_bssid = network_utils::ZERO_MAC_STRING;
            // backhaul_params.backhaul_freq           = 0; // HACK temp disabled because of a bug on endian converter
            backhaul_params.backhaul_channel     = 0;
            backhaul_params.backhaul_is_wireless = 0;
            backhaul_params.backhaul_iface_type  = beerocks::IFACE_TYPE_GW_BRIDGE;
            if (is_backhaul_manager) {
                backhaul_params.backhaul_iface = config.backhaul_wire_iface;
            }
        }

        LOG(INFO) << "Backhaul Params Info:";
        LOG(INFO) << "gw_ipv4=" << backhaul_params.gw_ipv4;
        LOG(INFO) << "gw_bridge_mac=" << backhaul_params.gw_bridge_mac;
        LOG(INFO) << "controller_bridge_mac=" << backhaul_params.controller_bridge_mac;
        LOG(INFO) << "bridge_mac=" << backhaul_params.bridge_mac;
        LOG(INFO) << "bridge_ipv4=" << backhaul_params.bridge_ipv4;
        LOG(INFO) << "backhaul_iface=" << backhaul_params.backhaul_iface;
        LOG(INFO) << "backhaul_mac=" << backhaul_params.backhaul_mac;
        LOG(INFO) << "backhaul_ipv4=" << backhaul_params.backhaul_ipv4;
        LOG(INFO) << "backhaul_bssid=" << backhaul_params.backhaul_bssid;
        LOG(INFO) << "backhaul_channel=" << int(backhaul_params.backhaul_channel);
        LOG(INFO) << "backhaul_is_wireless=" << int(backhaul_params.backhaul_is_wireless);
        LOG(INFO) << "backhaul_iface_type=" << int(backhaul_params.backhaul_iface_type);
        LOG(INFO) << "is_backhaul_manager=" << int(is_backhaul_manager);

        if (is_backhaul_manager) {
            LOG(DEBUG) << "sending "
                          "ACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION to "
                          "platform manager";
            auto notification = message_com::create_vs_message<
                beerocks_message::
                    cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION>(cmdu_tx);

            if (notification == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return false;
            }

            notification->is_backhaul_manager() =
                is_backhaul_manager; //redundant for now but might be needed in the future
            message_com::send_cmdu(platform_manager_socket, cmdu_tx);
        }

        LOG(TRACE) << "goto STATE_JOIN_MASTER";
        slave_state = STATE_JOIN_MASTER;

        SLAVE_STATE_CONTINUE();
        break;
    }
    case STATE_WAIT_BEFORE_JOIN_MASTER: {

        if (std::chrono::steady_clock::now() > slave_state_timer) {
            LOG(TRACE) << "goto STATE_JOIN_MASTER";
            slave_state = STATE_JOIN_MASTER;
        }

        break;
    }
    case STATE_JOIN_MASTER: {

        if (master_socket == nullptr) {
            LOG(ERROR) << "master_socket == nullptr";
            platform_notify_error(BPL_ERR_SLAVE_INVALID_MASTER_SOCKET, "Invalid master socket");
            stop_on_failure_attempts--;
            slave_reset();
            break;
        }

        if (!cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
            LOG(ERROR) << "Failed creating AP_AUTOCONFIGURATION_WSC_MESSAGE";
            return false;
        }

        if (!add_radio_basic_capabilities()) {
            LOG(ERROR) << "Failed adding WSC M1 TLV";
            return false;
        }

        if (!autoconfig_wsc_add_m1()) {
            LOG(ERROR) << "Failed adding WSC M1 TLV";
            return false;
        }

        auto notification =
            message_com::add_vs_tlv<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION>(
                cmdu_tx);

        if (!notification) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION!";
            return false;
        }

        notification->is_slave_reconf() = is_backhual_reconf;
        is_backhual_reconf              = false;

        // Version
        string_utils::copy_string(notification->slave_version(message::VERSION_LENGTH),
                                  BEEROCKS_VERSION, message::VERSION_LENGTH);

        // Platform Configuration
        notification->low_pass_filter_on()   = config.backhaul_wireless_iface_filter_low;
        notification->enable_repeater_mode() = config.enable_repeater_mode;
        notification->radio_identifier() = network_utils::mac_from_string(config.radio_identifier);

        // Backhaul Params
        notification->backhaul_params().gw_ipv4 =
            network_utils::ipv4_from_string(backhaul_params.gw_ipv4);
        notification->backhaul_params().gw_bridge_mac =
            network_utils::mac_from_string(backhaul_params.gw_bridge_mac);
        notification->backhaul_params().is_backhaul_manager = is_backhaul_manager;
        notification->backhaul_params().backhaul_iface_type = backhaul_params.backhaul_iface_type;
        notification->backhaul_params().backhaul_mac =
            network_utils::mac_from_string(backhaul_params.backhaul_mac);
        notification->backhaul_params().backhaul_channel = backhaul_params.backhaul_channel;
        notification->backhaul_params().backhaul_bssid =
            network_utils::mac_from_string(backhaul_params.backhaul_bssid);
        notification->backhaul_params().backhaul_is_wireless = backhaul_params.backhaul_is_wireless;

        if (!config.bridge_iface.empty()) {
            notification->backhaul_params().bridge_mac =
                network_utils::mac_from_string(backhaul_params.bridge_mac);
            notification->backhaul_params().bridge_ipv4 =
                network_utils::ipv4_from_string(backhaul_params.bridge_ipv4);
            notification->backhaul_params().backhaul_ipv4 =
                network_utils::ipv4_from_string(backhaul_params.bridge_ipv4);
        } else {
            notification->backhaul_params().backhaul_ipv4 =
                network_utils::ipv4_from_string(backhaul_params.backhaul_ipv4);
        }

        std::copy_n(backhaul_params.backhaul_scan_measurement_list,
                    beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH,
                    notification->backhaul_params().backhaul_scan_measurement_list);

        for (unsigned int i = 0; i < message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH; i++) {
            if (notification->backhaul_params().backhaul_scan_measurement_list[i].channel > 0) {
                LOG(DEBUG)
                    << "mac = "
                    << network_utils::mac_to_string(notification->backhaul_params()
                                                        .backhaul_scan_measurement_list[i]
                                                        .mac.oct)
                    << " channel = "
                    << int(notification->backhaul_params()
                               .backhaul_scan_measurement_list[i]
                               .channel)
                    << " rssi = "
                    << int(notification->backhaul_params().backhaul_scan_measurement_list[i].rssi);
            }
        }

        //Platform Settings
        notification->platform_settings() = platform_settings;

        //Wlan Settings
        notification->wlan_settings() = wlan_settings;
        // Hostap Params
        notification->hostap()          = hostap_params;
        notification->hostap().ant_gain = config.hostap_ant_gain;

        // Channel Selection Params
        notification->cs_params() = hostap_cs_params;

        send_cmdu_to_controller(cmdu_tx);
        LOG(DEBUG) << "send SLAVE_JOINED_NOTIFICATION Size=" << int(cmdu_tx.getMessageLength());

        LOG(DEBUG) << "sending ACTION_CONTROL_SLAVE_JOINED_NOTIFICATION";
        LOG(TRACE) << "goto STATE_WAIT_FOR_JOINED_RESPONSE";
        slave_state_timer = std::chrono::steady_clock::now() +
                            std::chrono::seconds(WAIT_FOR_JOINED_RESPONSE_TIMEOUT_SEC);

        if (wlan_settings.channel != 0 /* NOT ACS */) {
            send_platform_iface_status_notif(eRadioStatus::AP_OK, true);
        }

        slave_state = STATE_WAIT_FOR_JOINED_RESPONSE;
        break;
    }
    case STATE_WAIT_FOR_JOINED_RESPONSE: {
        if (std::chrono::steady_clock::now() > slave_state_timer) {
            LOG(INFO) << "STATE_WAIT_FOR_JOINED_RESPONSE timeout!";
            LOG(TRACE) << "goto STATE_JOIN_MASTER";
            slave_state = STATE_JOIN_MASTER;
        }
        break;
    }
    case STATE_UPDATE_MONITOR_SON_CONFIG: {
        LOG(INFO) << "sending ACTION_MONITOR_SON_CONFIG_UPDATE";

        auto update =
            message_com::create_vs_message<beerocks_message::cACTION_MONITOR_SON_CONFIG_UPDATE>(
                cmdu_tx);
        if (update == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        update->config() = son_config;
        message_com::send_cmdu(monitor_socket, cmdu_tx);
        LOG(TRACE) << "goto STATE_OPERATIONAL";
        slave_state = STATE_OPERATIONAL;
        break;
    }
    case STATE_OPERATIONAL: {
        stop_on_failure_attempts = configuration_stop_on_failure_attempts;
        process_keep_alive();
        break;
    }
    case STATE_ONBOARDING: {
        break;
    }
    case STATE_VERSION_MISMATCH: {
        break;
    }
    case STATE_SSID_MISMATCH: {
        break;
    }
    case STATE_STOPPED: {
        break;
    }
    default: {
        LOG(ERROR) << "Unknown state!";
        break;
    }
    }

    return slave_ok;
}

bool slave_thread::ap_manager_start()
{
    ap_manager = new ap_manager_thread(slave_uds);

    ap_manager_thread::ap_manager_conf_t ap_manager_conf;
    ap_manager_conf.hostap_iface        = config.hostap_iface;
    ap_manager_conf.hostap_iface_type   = config.hostap_iface_type;
    ap_manager_conf.channel             = wlan_settings.channel;
    ap_manager_conf.iface_filter_low    = config.backhaul_wireless_iface_filter_low;
    ap_manager_conf.backhaul_vaps_bssid = platform_settings.backhaul_vaps_bssid;

    ap_manager->ap_manager_config(ap_manager_conf);

    if (!ap_manager->start()) {
        delete ap_manager;
        ap_manager = nullptr;
        LOG(ERROR) << "ap_manager.start()";
        return false;
    }

    return true;
}

void slave_thread::ap_manager_stop()
{
    bool did_stop = false;
    if (ap_manager_socket) {
        remove_socket(ap_manager_socket);
        delete ap_manager_socket;
        ap_manager_socket = nullptr;
        did_stop          = true;
    }
    if (ap_manager) {
        LOG(DEBUG) << "ap_manager->stop();";
        ap_manager->stop();
        delete ap_manager;
        ap_manager = nullptr;
        did_stop   = true;
    }
    if (did_stop)
        LOG(DEBUG) << "ap_manager_stop() - done";

    iface_status_ap = eRadioStatus::OFF;
}

void slave_thread::backhaul_manager_stop()
{
    if (backhaul_manager_socket) {
        LOG(DEBUG) << "removing backhaul_manager_socket";
        remove_socket(backhaul_manager_socket);
        delete backhaul_manager_socket;
    }
    backhaul_manager_socket = nullptr;
    master_socket           = nullptr;

    iface_status_bh       = eRadioStatus::OFF;
    iface_status_bh_wired = eRadioStatus::OFF;
}

void slave_thread::platform_manager_stop()
{
    if (platform_manager_socket) {
        LOG(DEBUG) << "removing platform_manager_socket";
        remove_socket(platform_manager_socket);
        delete platform_manager_socket;
        platform_manager_socket = nullptr;
    }
}

void slave_thread::hostap_services_off() { LOG(DEBUG) << "hostap_services_off() - done"; }

bool slave_thread::hostap_services_on()
{
    bool success = true;
    LOG(DEBUG) << "hostap_services_on() - done";
    return success;
}

void slave_thread::monitor_stop()
{
    bool did_stop = false;
    if (monitor_socket) {
        remove_socket(monitor_socket);
        delete monitor_socket;
        monitor_socket = nullptr;
        did_stop       = true;
    }

    // kill monitor slave pid
    os_utils::kill_pid(config.temp_path, std::string(BEEROCKS_MONITOR) + "_" + config.hostap_iface);

    if (did_stop)
        LOG(DEBUG) << "monitor_stop() - done";
}

void slave_thread::monitor_start()
{
    monitor_stop();

    LOG(DEBUG) << "monitor_start()";

    //start new monitor process
    std::string file_name = "./" + std::string(BEEROCKS_MONITOR);
    if (access(file_name.c_str(), F_OK) == -1) //file does not exist in current location
    {
        file_name = BEEROCKS_BIN_PATH + std::string(BEEROCKS_MONITOR);
    }
    std::string cmd = file_name + " -i " + config.hostap_iface;
    SYSTEM_CALL(cmd, 2, true);
}

void slave_thread::log_son_config()
{
    LOG(DEBUG) << "SON_CONFIG_UPDATE: " << std::endl
               << "monitor_total_ch_load_notification_th_hi_percent="
               << int(son_config.monitor_total_ch_load_notification_lo_th_percent) << std::endl
               << "monitor_total_ch_load_notification_th_lo_percent="
               << int(son_config.monitor_total_ch_load_notification_hi_th_percent) << std::endl
               << "monitor_total_ch_load_notification_delta_th_percent="
               << int(son_config.monitor_total_ch_load_notification_delta_th_percent) << std::endl
               << "monitor_min_active_clients=" << int(son_config.monitor_min_active_clients)
               << std::endl
               << "monitor_active_client_th=" << int(son_config.monitor_active_client_th)
               << std::endl
               << "monitor_client_load_notification_delta_th_percent="
               << int(son_config.monitor_client_load_notification_delta_th_percent) << std::endl
               << "monitor_rx_rssi_notification_threshold_dbm="
               << int(son_config.monitor_rx_rssi_notification_threshold_dbm) << std::endl
               << "monitor_rx_rssi_notification_delta_db="
               << int(son_config.monitor_rx_rssi_notification_delta_db) << std::endl
               << "monitor_ap_idle_threshold_B=" << int(son_config.monitor_ap_idle_threshold_B)
               << std::endl
               << "monitor_ap_active_threshold_B=" << int(son_config.monitor_ap_active_threshold_B)
               << std::endl
               << "monitor_ap_idle_stable_time_sec="
               << int(son_config.monitor_ap_idle_stable_time_sec) << std::endl
               << "monitor_disable_initiative_arp="
               << int(son_config.monitor_disable_initiative_arp) << std::endl
               << "slave_keep_alive_retries=" << int(son_config.slave_keep_alive_retries);
}

void slave_thread::send_platform_iface_status_notif(eRadioStatus radio_status,
                                                    bool status_operational)
{
    // CMDU Message
    auto platform_notification = message_com::create_vs_message<
        beerocks_message::cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION>(cmdu_tx);

    if (platform_notification == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return;
    }
    string_utils::copy_string(platform_notification->iface_name_ap(message::IFACE_NAME_LENGTH),
                              config.hostap_iface.c_str(), message::IFACE_NAME_LENGTH);
    string_utils::copy_string(platform_notification->iface_name_bh(message::IFACE_NAME_LENGTH),
                              config.backhaul_wireless_iface.c_str(), message::IFACE_NAME_LENGTH);

    platform_notification->status_ap()          = (uint8_t)radio_status;
    platform_notification->status_bh()          = (uint8_t)iface_status_bh;
    platform_notification->status_bh_wired()    = (uint8_t)iface_status_bh_wired;
    platform_notification->is_bh_manager()      = (uint8_t)is_backhaul_manager;
    platform_notification->status_operational() = (uint8_t)status_operational;

    iface_status_ap_prev                = iface_status_ap;
    iface_status_bh_prev                = iface_status_bh;
    iface_status_bh_wired_prev          = iface_status_bh_wired;
    iface_status_operational_state_prev = iface_status_operational_state;
    LOG(INFO) << "***** send_iface_status:"
              << " iface_name_ap: "
              << platform_notification->iface_name_ap(message::IFACE_NAME_LENGTH)
              << " iface_name_bh: "
              << platform_notification->iface_name_bh(message::IFACE_NAME_LENGTH)
              << " status_ap: " << (int)platform_notification->status_ap()
              << " status_bh: " << (int)platform_notification->status_bh()
              << " status_bh_wired: " << (int)platform_notification->status_bh_wired()
              << " is_bh_manager: " << (int)platform_notification->is_bh_manager()
              << " operational: " << (int)platform_notification->status_operational();

    message_com::send_cmdu(platform_manager_socket, cmdu_tx);
}

bool slave_thread::monitor_heartbeat_check()
{
    if (monitor_socket == nullptr) {
        return true;
    }
    auto now = std::chrono::steady_clock::now();
    int time_elapsed_secs =
        std::chrono::duration_cast<std::chrono::seconds>(now - monitor_last_seen).count();
    if (time_elapsed_secs > MONITOR_HEARTBEAT_TIMEOUT_SEC) {
        monitor_retries_counter++;
        monitor_last_seen = now;
        LOG(INFO) << "time_elapsed_secs > MONITOR_HEARTBEAT_TIMEOUT_SEC monitor_retries_counter = "
                  << int(monitor_retries_counter);
    }
    if (monitor_retries_counter >= MONITOR_HEARTBEAT_RETRIES) {
        LOG(INFO)
            << "monitor_retries_counter >= MONITOR_HEARTBEAT_RETRIES monitor_retries_counter = "
            << int(monitor_retries_counter) << " slave_reset!!";
        monitor_retries_counter = 0;
        return false;
    }
    return true;
}

bool slave_thread::ap_manager_heartbeat_check()
{
    if (ap_manager_socket == nullptr) {
        return true;
    }
    auto now = std::chrono::steady_clock::now();
    int time_elapsed_secs =
        std::chrono::duration_cast<std::chrono::seconds>(now - ap_manager_last_seen).count();
    if (time_elapsed_secs > AP_MANAGER_HEARTBEAT_TIMEOUT_SEC) {
        ap_manager_retries_counter++;
        ap_manager_last_seen = now;
        LOG(INFO)
            << "time_elapsed_secs > AP_MANAGER_HEARTBEAT_TIMEOUT_SEC ap_manager_retries_counter = "
            << int(ap_manager_retries_counter);
    }
    if (ap_manager_retries_counter >= AP_MANAGER_HEARTBEAT_RETRIES) {
        LOG(INFO) << "ap_manager_retries_counter >= AP_MANAGER_HEARTBEAT_RETRIES "
                     "ap_manager_retries_counter = "
                  << int(ap_manager_retries_counter) << " slave_reset!!";
        ap_manager_retries_counter = 0;
        return false;
    }
    return true;
}

bool slave_thread::send_cmdu_to_controller(ieee1905_1::CmduMessageTx &cmdu_tx)
{
    if (!master_socket) {
        LOG(ERROR) << "socket to master is nullptr";
        return false;
    }

    if (cmdu_tx.getMessageType() == ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE) {
        auto beerocks_header = message_com::get_vs_class_header(cmdu_tx);
        if (!beerocks_header) {
            LOG(ERROR) << "Failed getting beerocks_header!";
            return false;
        }

        beerocks_header->radio_mac() = hostap_params.iface_mac;
        beerocks_header->direction() = beerocks::BEEROCKS_DIRECTION_CONTROLLER;
    }
    return message_com::send_cmdu(master_socket, cmdu_tx, backhaul_params.controller_bridge_mac,
                                  backhaul_params.bridge_mac);
}

/**
 * @brief Diffie-Hellman public key exchange keys calculation
 *        class member params authkey and keywrapauth are computed
 *        on success.
 *
 * @param[in] m1 WSC M1 TLV received from the radio agent
 * @param[out] authkey 32 bytes calculated authentication key
 * @param[out] keywrapkey 16 bytes calculated key wrap key
 * @return true on success
 * @return false on failure
 */
bool slave_thread::autoconfig_wsc_calculate_keys(std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                                 uint8_t authkey[32], uint8_t keywrapkey[16])
{
    auto mac = network_utils::mac_from_string(backhaul_params.bridge_mac);
    if (!dh) {
        LOG(ERROR) << "diffie hellman member not initialized";
        return false;
    }
    if (!mapf::encryption::wps_calculate_keys(
            *dh, m2->public_key_attr().data, m2->public_key_attr().data_length, dh->nonce(),
            mac.oct, m2->registrar_nonce_attr().data, authkey, keywrapkey)) {
        LOG(ERROR) << "Failed to calculate WPS keys";
        return false;
    }

    return true;
}

/**
 * @brief autoconfig global authenticator attribute calculation
 * 
 * Calculate authentication on the Full M1 || M2* whereas M2* = M2 without the authenticator
 * attribute. M1 is a saved buffer of the swapped M1 sent in the WSC autoconfig sent by the agent.
 * 
 * @param m2 WSC M2 TLV
 * @param authkey authentication key
 * @return true on success
 * @return false on failure
 */
bool slave_thread::autoconfig_wsc_authenticate(std::shared_ptr<ieee1905_1::tlvWscM2> m2,
                                               uint8_t authkey[32])
{
    if (!m1_auth_buf) {
        LOG(ERROR) << "Invalid M1";
        return false;
    }

    // This is the content of M1 and M2, without the type and length.
    uint8_t buf[m1_auth_buf_len + m2->getLen() - 3 - sizeof(WSC::sWscAttrAuthenticator)];
    auto next = std::copy_n(m1_auth_buf, m1_auth_buf_len, buf);
    m2->class_swap(); //swap to get network byte order
    std::copy_n(m2->getStartBuffPtr() + 3, m2->getLen() - 3 - sizeof(WSC::sWscAttrAuthenticator),
                next);
    m2->class_swap(); //swap back

    uint8_t kwa[WSC::WSC_AUTHENTICATOR_LENGTH];
    // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
    if (!mapf::encryption::kwa_compute(authkey, buf, sizeof(buf), kwa)) {
        LOG(ERROR) << "kwa_compute failure";
        return false;
    }

    if (!std::equal(kwa, kwa + sizeof(kwa),
                    reinterpret_cast<uint8_t *>(m2->authenticator().data))) {
        return false;
    }

    LOG(DEBUG) << "WSC Global authentication success";
    return true;
}

bool slave_thread::autoconfig_wsc_parse_m2_encrypted_settings(
    std::shared_ptr<ieee1905_1::tlvWscM2> m2, uint8_t authkey[32], uint8_t keywrapkey[16],
    std::string &ssid, sMacAddr &bssid, WSC::eWscAuth &auth_type, WSC::eWscEncr &encr_type,
    bool &backhaul, bool &fronthaul, bool &teardown)
{
    auto encrypted_settings = m2->encrypted_settings();
    uint8_t *iv             = reinterpret_cast<uint8_t *>(encrypted_settings->iv());
    auto ciphertext         = reinterpret_cast<uint8_t *>(encrypted_settings->encrypted_settings());
    int cipherlen           = encrypted_settings->encrypted_settings_length();
    // leave room for up to 16 bytes internal padding length - see aes_decrypt()
    int datalen = cipherlen + 16;
    uint8_t decrypted[datalen];

    LOG(DEBUG) << "M2 Parse: aes decrypt";
    if (!mapf::encryption::aes_decrypt(keywrapkey, iv, ciphertext, cipherlen, decrypted, datalen)) {
        LOG(ERROR) << "aes decrypt failure";
        return false;
    }

    LOG(DEBUG) << "M2 Parse: parse config_data, len = " << datalen;

    // Need to convert to host byte order to get config data length,
    // which is needed for computing the KWA (1st 64 bits of HMAC)
    // However, the KWA is computed on the finalized version of the config
    // data, hence the class_swap.
    // Finally, once authentication succeeds, swap back to host byte order.
    // Theoretically, this can be avoided by not passing swap_needed=True to
    // cConfigData constructor, but then parsing will fail since the length
    // will be calculated wrong. TLVF does not support parsing network byte order
    // without full swap, so keep this workaround for now (another future TLVF V2 feature)
    WSC::cConfigData config_data(decrypted, datalen, true, true);

    // get length of config_data for KWA authentication
    size_t len = config_data.getLen();
    // Protect against M2 buffer overflow attacks
    if (len + sizeof(WSC::sWscAttrKeyWrapAuthenticator) > size_t(datalen)) {
        LOG(ERROR) << "invalid config data length";
        return false;
    }
    // Swap to network byte order for KWA HMAC calculation
    config_data.class_swap();

    uint8_t kwa[WSC::WSC_AUTHENTICATOR_LENGTH];
    // Compute KWA based on decrypted settings
    if (!mapf::encryption::kwa_compute(authkey, decrypted, len, kwa)) {
        LOG(ERROR) << "kwa compute";
        return false;
    }

    // Basically, the keywrap authenticator is part of the config_data (last member of the
    // config_data to be precise).
    // However, since we need to calculate it over the part of config_data without the keywrap
    // authenticator, and since it is anyway going to be encrypted so config_data is not a
    // substructure of encrypted_settings, it is easier to define it separately and just append to
    // config_data.
    WSC::sWscAttrKeyWrapAuthenticator *keywrapauth =
        reinterpret_cast<WSC::sWscAttrKeyWrapAuthenticator *>(&decrypted[len]);
    keywrapauth->struct_swap();
    if ((keywrapauth->attribute_type != WSC::ATTR_KEY_WRAP_AUTH) ||
        (keywrapauth->data_length != WSC::WSC_KEY_WRAP_AUTH_LENGTH) ||
        !std::equal(kwa, kwa + sizeof(kwa), reinterpret_cast<uint8_t *>(keywrapauth->data))) {
        LOG(ERROR) << "WSC KWA (Key Wrap Auth) failure" << std::endl
                   << "type: " << std::hex << int(keywrapauth->attribute_type)
                   << "length: " << std::hex << int(keywrapauth->data_length);
        return false;
    }

    // Swap back to host byte order to read and use config_data
    config_data.class_swap();

    // TODO tlvf should check this
    if (config_data.ssid_length() > WSC::WSC_MAX_SSID_LENGTH) {
        LOG(INFO) << "SSID too long: " << config_data.ssid_length();
        return false;
    }
    // TODO tlvf should do conversion to std::string
    if (config_data.ssid_length() == 0)
        ssid = "";
    else
        ssid = std::string(config_data.ssid(), config_data.ssid_length());
    bssid            = config_data.bssid_attr().data;
    auth_type        = config_data.authentication_type_attr().data;
    encr_type        = config_data.encryption_type_attr().data;
    uint8_t bss_type = config_data.multiap_attr().subelement_value;
    LOG(INFO) << "bss_type: " << std::hex << int(bss_type);
    fronthaul = bss_type & WSC::eWscVendorExtSubelementBssType::FRONTHAUL_BSS;
    backhaul  = bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_BSS;
    teardown  = bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN;
    // BACKHAUL_STA bit is not expected to be set
    if (bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA) {
        LOG(WARNING) << "Unexpected backhaul STA bit";
    }
    LOG(DEBUG) << "KWA (Key Wrap Auth) success";

    return true;
}

/**
 * @brief Parse AP-Autoconfiguration Renew message
 *
 * This function checks the TLVs in the AP-Autoconfiguration Renew message. If OK, it triggers
 * autoconfiguration.
 *
 * @param sd socket descriptor
 * @param cmdu_rx received CMDU containing AP-Autoconfiguration Renew
 * @return true on success
 * @return false on failure
 */
bool slave_thread::handle_autoconfiguration_renew(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(INFO) << "received autoconfig renew message";

    auto tlvAlMac = cmdu_rx.addClass<ieee1905_1::tlvAlMacAddressType>();
    if (tlvAlMac) {
        LOG(DEBUG) << "tlvAlMac=" << network_utils::mac_to_string(tlvAlMac->mac());
        // TODO register/update mapping of AL-MAC to interface, cfr. #81
    } else {
        LOG(ERROR) << "tlvAlMac missing - ignoring autconfig renew message";
        return false;
    }

    auto tlvSupportedRole = cmdu_rx.addClass<ieee1905_1::tlvSupportedRole>();
    if (tlvSupportedRole) {
        LOG(DEBUG) << "tlvSupportedRole->value()=" << int(tlvSupportedRole->value());
        if (tlvSupportedRole->value() != ieee1905_1::tlvSupportedRole::REGISTRAR) {
            LOG(ERROR) << "invalid tlvSupportedRole value";
            return false;
        }
    } else {
        LOG(ERROR) << "tlvSupportedRole missing - ignoring autconfig renew message";
        return false;
    }

    auto tlvSupportedFreqBand = cmdu_rx.addClass<ieee1905_1::tlvSupportedFreqBand>();
    if (tlvSupportedFreqBand) {
        LOG(DEBUG) << "tlvSupportedFreqBand->value()=" << int(tlvSupportedFreqBand->value());
    } else {
        LOG(ERROR) << "tlvSupportedFreqBand missing - ignoring autoconfig renew message";
        return false;
    }

    LOG(TRACE) << "goto STATE_JOIN_MASTER";
    slave_state = STATE_JOIN_MASTER;
    return true;
}

/**
 * @brief Parse AP-Autoconfiguration WSC which should include one AP Radio Identifier
 *        TLV and one or more WSC TLV containing M2
 * 
 * @param sd socket descriptor
 * @param cmdu_rx received CMDU containing M2
 * @return true on success
 * @return false on failure
 */
bool slave_thread::handle_autoconfiguration_wsc(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(DEBUG) << "Received AP_AUTOCONFIGURATION_WSC_MESSAGE";

    std::shared_ptr<wfa_map::tlvApRadioIdentifier> ruid = nullptr;
    std::vector<std::shared_ptr<ieee1905_1::tlvWscM2>> m2_list;
    bool intel_agent = false;
    int type         = cmdu_rx.getNextTlvType();

    while ((type = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        if (type == int(wfa_map::eTlvTypeMap::TLV_AP_RADIO_IDENTIFIER)) {
            ruid = cmdu_rx.addClass<wfa_map::tlvApRadioIdentifier>();
            if (!ruid)
                return false;
            LOG(DEBUG) << "Found TLV_AP_RADIO_IDENTIFIER TLV";
        } else if (type == int(ieee1905_1::eTlvType::TLV_WSC)) {
            // parse all M2 TLVs
            LOG(DEBUG) << "Found TLV_WSC TLV (assuming M2)";
            auto tlvWscM2 = cmdu_rx.addClass<ieee1905_1::tlvWscM2>();
            if (!tlvWscM2) {
                LOG(ERROR) << "Not an WSC M2 TLV!";
                return false;
            }
            m2_list.push_back(tlvWscM2);
        } else if (type == int(ieee1905_1::eTlvType::TLV_VENDOR_SPECIFIC)) {
            // If this is an Intel Agent, it will have VS TLV as the last TLV.
            // Currently, we don't support skipping TLVs, so if we see a VS TLV, we assume
            // It is an Intel agent, and will add the class in the Intel agent handling below.
            intel_agent = true;
            break;
        } else if (type == int(wfa_map::eTlvTypeMap::TLV_AP_RADIO_BASIC_CAPABILITIES)) {
            // Check if this is a M1 message that we sent to the controller, which was just looped back.
            // The M1 and M2 messages are both of CMDU type AP_Autoconfiguration_WSC. Thus,
            // when we send the M2 to the local agent, it will be published back on the local bus because
            // the destination is our AL-MAC, and the controller does listen to this CMDU.
            // Ideally, we should use the message type attribute from the WSC payload to distinguish.
            // Unfortunately, that is a bit complicated with the current tlv parser. However, there is another
            // way to distinguish them: the M1 message has the AP_Radio_Basic_Capabilities TLV,
            // while the M2 has the AP_Radio_Identifier TLV.
            // If this is a looped back M1 CMDU, we can treat is as handled successfully.
            LOG(DEBUG) << "Loopbed back M1 CMDU";
            return true;
        } else {
            LOG(ERROR) << "Uknown TLV, type " << std::hex << type;
            return false;
        }
        type = cmdu_rx.getNextTlvType();
    }

    if (!ruid) {
        LOG(ERROR) << "Failed to get tlvApRadioIdentifier TLV";
        return false;
    }

    // Check if the message is for this radio agent by comparing the ruid
    if (network_utils::mac_from_string(config.radio_identifier) != ruid->radio_uid()) {
        LOG(DEBUG) << "not to me - ruid " << config.radio_identifier
                   << " != " << network_utils::mac_to_string(ruid->radio_uid());
        return true;
    }

    if (m2_list.empty()) {
        LOG(ERROR) << "No M2 TLVs present";
        return false;
    }

    for (auto tlvWscM2 : m2_list) {
        uint8_t authkey[32];
        uint8_t keywrapkey[16];
        LOG(DEBUG) << "M2 Parse: calculate keys";
        if (!autoconfig_wsc_calculate_keys(tlvWscM2, authkey, keywrapkey))
            return false;

        if (!autoconfig_wsc_authenticate(tlvWscM2, authkey))
            return false;

        std::string ssid;
        sMacAddr bssid;
        WSC::eWscAuth authtype;
        WSC::eWscEncr enctype;
        bool fronthaul, backhaul, teardown;
        if (!autoconfig_wsc_parse_m2_encrypted_settings(tlvWscM2, authkey, keywrapkey, ssid, bssid,
                                                        authtype, enctype, backhaul, fronthaul,
                                                        teardown))
            return false;

        std::string manufacturer = tlvWscM2->manufacturer_str();

        LOG(DEBUG) << "Controller configuration (WSC M2 Encrypted Settings)";
        LOG(DEBUG) << "     Manufacturer: " << manufacturer << ", ssid: " << ssid
                   << ", bssid: " << network_utils::mac_to_string(bssid)
                   << ", authentication_type: " << std::hex << int(authtype)
                   << ", encryption_type: " << int(enctype) << (fronthaul ? " fronthaul" : "")
                   << (backhaul ? " backhaul" : "") << (teardown ? " teardown" : "");
    }

    if (slave_state != STATE_WAIT_FOR_JOINED_RESPONSE) {
        LOG(ERROR) << "slave_state != STATE_WAIT_FOR_JOINED_RESPONSE";
        return false;
    }

    if (intel_agent) {
        LOG(INFO) << "Intel controller join response";
        if (!parse_intel_join_response(sd, cmdu_rx)) {
            LOG(ERROR) << "Parse join response failed";
            return false;
        }
    } else {
        LOG(INFO) << "Non-Intel controller join response";
        if (!parse_non_intel_join_response(sd)) {
            LOG(ERROR) << "Parse join response failed";
            return false;
        }
    }

    return true;
}

bool slave_thread::parse_intel_join_response(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    LOG(DEBUG) << "ACTION_CONTROL_SLAVE_JOINED_RESPONSE sd=" << intptr_t(sd);
    if (slave_state != STATE_WAIT_FOR_JOINED_RESPONSE) {
        LOG(ERROR) << "slave_state != STATE_WAIT_FOR_JOINED_RESPONSE";
        return false;
    }

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(ERROR) << "Failed to parse intel vs message (not Intel?)";
        return false;
    }

    if (beerocks_header->action_op() != beerocks_message::ACTION_CONTROL_SLAVE_JOINED_RESPONSE) {
        LOG(ERROR) << "Unexpected Intel action op " << beerocks_header->action_op();
        return false;
    }

    auto joined_response =
        beerocks_header->addClass<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_RESPONSE>();
    if (joined_response == nullptr) {
        LOG(ERROR) << "addClass cACTION_CONTROL_SLAVE_JOINED_RESPONSE failed";
        return false;
    }

    // check master rejection
    if (joined_response->err_code() == beerocks::JOIN_RESP_REJECT) {
        slave_state_timer = std::chrono::steady_clock::now() +
                            std::chrono::seconds(WAIT_BEFORE_SEND_SLAVE_JOINED_NOTIFICATION_SEC);
        LOG(DEBUG) << "STATE_WAIT_FOR_JOINED_RESPONSE: join rejected!";
        LOG(DEBUG) << "goto STATE_WAIT_BEFORE_JOIN_MASTER";
        slave_state = STATE_WAIT_BEFORE_JOIN_MASTER;
        return true;
    }

    // request the current vap list from ap_manager
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST message!";
        return false;
    }
    message_com::send_cmdu(ap_manager_socket, cmdu_tx);

    auto notification = message_com::create_vs_message<
        beerocks_message::
            cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST>(cmdu_tx);
    if (!notification) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }
    message_com::send_cmdu(ap_manager_socket, cmdu_tx);

    master_version.assign(joined_response->master_version(message::VERSION_LENGTH));

    LOG(DEBUG) << "Version (Master/Slave): " << master_version << "/" << BEEROCKS_VERSION;
    auto slave_version_s  = version::version_from_string(BEEROCKS_VERSION);
    auto master_version_s = version::version_from_string(master_version);

    // check if mismatch notification is needed
    if ((master_version_s.major > slave_version_s.major) ||
        ((master_version_s.major == slave_version_s.major) &&
         (master_version_s.minor > slave_version_s.minor)) ||
        ((master_version_s.major == slave_version_s.major) &&
         (master_version_s.minor == slave_version_s.minor) &&
         (master_version_s.build_number > slave_version_s.build_number))) {
        LOG(INFO) << "master_version > slave_version, sending "
                     "ACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION";
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        string_utils::copy_string(notification->versions().master_version, master_version.c_str(),
                                  sizeof(beerocks_message::sVersions::master_version));
        string_utils::copy_string(notification->versions().slave_version, BEEROCKS_VERSION,
                                  sizeof(beerocks_message::sVersions::slave_version));
        message_com::send_cmdu(platform_manager_socket, cmdu_tx);
    }

    // check if fatal mismatch
    if (joined_response->err_code() == beerocks::JOIN_RESP_VERSION_MISMATCH) {
        LOG(ERROR) << "Mismatch version! slave_version=" << std::string(BEEROCKS_VERSION)
                   << " master_version=" << master_version;
        LOG(DEBUG) << "goto STATE_VERSION_MISMATCH";
        slave_state = STATE_VERSION_MISMATCH;
    } else if (joined_response->err_code() == beerocks::JOIN_RESP_SSID_MISMATCH) {
        LOG(ERROR) << "Mismatch SSID!";
        LOG(DEBUG) << "goto STATE_SSID_MISMATCH";
        slave_state = STATE_SSID_MISMATCH;
    } else {
        //Send master version + slave version to platform manager
        auto notification = message_com::create_vs_message<
            beerocks_message::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION>(cmdu_tx);
        if (notification == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        string_utils::copy_string(notification->versions().master_version, master_version.c_str(),
                                  sizeof(beerocks_message::sVersions::master_version));
        string_utils::copy_string(notification->versions().slave_version, BEEROCKS_VERSION,
                                  sizeof(beerocks_message::sVersions::slave_version));
        message_com::send_cmdu(platform_manager_socket, cmdu_tx);
        LOG(DEBUG) << "send ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION";

        son_config = joined_response->config();
        log_son_config();

        slave_state = STATE_UPDATE_MONITOR_SON_CONFIG;
    }

    return true;
}

bool slave_thread::parse_non_intel_join_response(Socket *sd)
{
    // request the current vap list from ap_manager
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST message!";
        return false;
    }
    message_com::send_cmdu(ap_manager_socket, cmdu_tx);

    // No version checking for non-Intel controller

    // TODO
    //        auto notification = message_com::create_vs_message<
    //            beerocks_message::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION>(cmdu_tx);
    //        if (notification == nullptr) {
    //            LOG(ERROR) << "Failed building message!";
    //            return false;
    //        }
    //        string_utils::copy_string(notification->versions().master_version, master_version.c_str(),
    //                                  sizeof(beerocks_message::sVersions::master_version));
    //        string_utils::copy_string(notification->versions().slave_version, BEEROCKS_VERSION,
    //                                  sizeof(beerocks_message::sVersions::slave_version));
    //        message_com::send_cmdu(platform_manager_socket, cmdu_tx);
    //        LOG(DEBUG) << "send ACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION";

    // TODO set son_config
    log_son_config();

    slave_state = STATE_UPDATE_MONITOR_SON_CONFIG;

    return true;
}

bool slave_thread::handle_client_capability_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CLIENT_CAPABILITY_QUERY_MESSAGE , mid=" << std::dec << int(mid);
    return true;
}

bool slave_thread::handle_ap_capability_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    //TODO - this is a stub handler for the purpose of controller certification testing,
    //       will be implemented later on agent certification
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received AP_CAPABILITY_QUERY_MESSAGE, mid=" << std::dec << int(mid);
    return true;
}

bool slave_thread::handle_multi_ap_policy_config_request(Socket *sd,
                                                         ieee1905_1::CmduMessageRx &cmdu_rx)
{
    // TODO: This is a stub handler for the purpose of controller certification testing, will be
    // implemented later on agent certification.
    auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE, mid=" << std::hex << int(mid);

    // send ACK_MESSAGE back to the controller
    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }

    return send_cmdu_to_controller(cmdu_tx);
}

bool slave_thread::handle_client_association_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    //TODO - this is a stub handler for the purpose of controller certification testing,
    //       will be implemented later on agent certification
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE, mid=" << std::dec
               << int(mid);

    auto association_control_request_tlv =
        cmdu_rx.addClass<wfa_map::tlvClientAssociationControlRequest>();
    if (!association_control_request_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvClientAssociationControlRequest failed";
        return false;
    }
    auto sta_mac = std::get<1>(association_control_request_tlv->sta_list(0));

    //Add VS tlv
    auto vs_tlv =
        message_com::add_vs_tlv<beerocks_message::tlvVsClientAssociationControlRequest>(cmdu_tx);
    if (!vs_tlv) {
        LOG(ERROR) << "add_vs_tlv tlvVsClientAssociationControlRequest failed";
    }

    auto block = association_control_request_tlv->association_control();
    if (block == wfa_map::tlvClientAssociationControlRequest::UNBLOCK) {
        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST>(cmdu_tx, mid);
        if (!request_out) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_ALLOW_REQUEST message!";
            return false;
        }
        if (vs_tlv) {
            request_out->ipv4() = vs_tlv->ipv4();
        }
        request_out->mac() = sta_mac;
    } else if (block == wfa_map::tlvClientAssociationControlRequest::BLOCK) {
        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST>(cmdu_tx, mid);
        if (!request_out) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_DISALLOW_REQUEST message!";
            return false;
        }
        request_out->mac() = sta_mac;
    }
    message_com::send_cmdu(ap_manager_socket, cmdu_tx);

    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::ACK_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type ACK_MESSAGE, has failed";
        return false;
    }
    LOG(DEBUG) << "sending ACK message back to controller";
    return send_cmdu_to_controller(cmdu_tx);
}

bool slave_thread::handle_ack_message(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    //TODO - this is a stub handler for the purpose of controller certification testing,
    //       will be implemented later on agent certification
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received ACK_MESSAGE, mid=" << std::dec << int(mid);
    return true;
}

bool slave_thread::handle_client_steering_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();

    auto steering_request_tlv = cmdu_rx.addClass<wfa_map::tlvSteeringRequest>();
    if (!steering_request_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvSteeringRequest failed";
        return false;
    }

    LOG(DEBUG) << "Received CLIENT_STEERING_REQUEST_MESSAGE , mid=" << std::hex << int(mid);

    auto request_mode = steering_request_tlv->request_flags().request_mode;
    LOG(DEBUG) << "request_mode: " << std::hex << int(request_mode);

    if (request_mode ==
        wfa_map::tlvSteeringRequest::REQUEST_IS_A_STEERING_MANDATE_TO_TRIGGER_STEERING) {
        //TODO Handle 0 or more then 1 sta in list, currenlty cli steers only 1 client
        LOG(DEBUG) << "Request Mode bit is set - Steering Mandate";

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST>(cmdu_tx, mid);
        if (!request_out) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST message!";
            return false;
        }

        auto bssid_list                 = steering_request_tlv->target_bssid_list(0);
        request_out->params().cur_bssid = steering_request_tlv->bssid();
        request_out->params().mac       = std::get<1>(steering_request_tlv->sta_list(0));
        request_out->params().disassoc_timer_ms =
            steering_request_tlv->btm_disassociation_timer_ms();
        request_out->params().target.bssid = std::get<1>(bssid_list).target_bssid;
        request_out->params().target.operating_class =
            std::get<1>(bssid_list).target_bss_operating_class;
        request_out->params().target.channel = std::get<1>(bssid_list).target_bss_channel_number;
        request_out->params().disassoc_imminent =
            steering_request_tlv->request_flags().btm_disassociation_imminent_bit;

        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
        return true;
    } else {
        //TODO Add handling of steering opportunity
        LOG(DEBUG) << "Request Mode bit is not set - Steering Opportunity";
        // build and send steering completed message
        auto cmdu_tx_header =
            cmdu_tx.create(0, ieee1905_1::eMessageType::STEERING_COMPLETED_MESSAGE);

        if (!cmdu_tx_header) {
            LOG(ERROR) << "cmdu creation of type STEERING_COMPLETED_MESSAGE, has failed";
            return false;
        }
        LOG(DEBUG) << "sending STEERING_COMPLETED_MESSAGE back to controller";
        return send_cmdu_to_controller(cmdu_tx);
    }
}

bool slave_thread::handle_ap_metrics_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    auto tlv_type  = cmdu_rx.getNextTlvType();
    while (tlv_type != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        if (tlv_type == int(wfa_map::eTlvTypeMap::TLV_AP_METRIC_QUERY)) {
            auto ap_metrics_query_tlv = cmdu_tx.addClass<wfa_map::tlvApMetricQuery>();
            if (!ap_metrics_query_tlv) {
                LOG(ERROR) << "addClass tlvApMetricQuery has failed";
                return false;
            }
            LOG(DEBUG) << "Received AP_METRICS_QUERY_MESSAGE, mid=" << std::hex << int(mid)
                       << ", with TLV type=" << std::hex << int(ap_metrics_query_tlv->type());
        }
        tlv_type = cmdu_tx.getNextTlvType();
    }
    return true;
}

bool slave_thread::handle_link_metrics_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    auto tlv_type  = cmdu_rx.getNextTlvType();
    while (tlv_type != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        if (tlv_type == int(ieee1905_1::eTlvType::TLV_LINK_METRIC_QUERY)) {
            auto link_metrics_query_tlv = cmdu_tx.addClass<ieee1905_1::tlvLinkMetricQuery>();
            if (!link_metrics_query_tlv) {
                LOG(ERROR) << "addClass tlvLinkMetricQuery has failed";
                return false;
            }
            LOG(DEBUG) << "Received LINK_METRIC_QUERY_MESSAGE , mid=" << std::hex << int(mid)
                       << ", with TLV type=" << std::hex << int(link_metrics_query_tlv->type());
        }
        tlv_type = cmdu_tx.getNextTlvType();
    }
    return true;
}

bool slave_thread::handle_channel_preference_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CHANNEL_PREFERENCE_QUERY_MESSAGE, mid=" << std::dec << int(mid);

    // build channel preference report
    auto cmdu_tx_header =
        cmdu_tx.create(mid, ieee1905_1::eMessageType::CHANNEL_PREFERENCE_REPORT_MESSAGE);

    if (!cmdu_tx_header) {
        LOG(ERROR) << "cmdu creation of type CHANNEL_PREFERENCE_REPORT_MESSAGE, has failed";
        return false;
    }

    auto channel_preference_tlv = cmdu_tx.addClass<wfa_map::tlvChannelPreference>();
    if (!channel_preference_tlv) {
        LOG(ERROR) << "addClass ieee1905_1::tlvChannelPreference has failed";
        return false;
    }

    channel_preference_tlv->radio_uid() = network_utils::mac_from_string(config.radio_identifier);

    // Create operating class object
    auto op_class_channels = channel_preference_tlv->create_operating_classes_list();
    if (!op_class_channels) {
        LOG(ERROR) << "create_operating_classes_list() has failed!";
        return false;
    }

    // TODO: check that the data is parsed properly after fixing the following bug:
    // Since sFlags is defined after dynamic list cPreferenceOperatingClasses it cause data override
    // on the first channel on the list and sFlags itself.
    // See: https://github.com/prplfoundation/prplMesh/issues/8

    // Fill operating class object
    op_class_channels->operating_class() = 80; // random operating class for test purpose

    // Fill up channels list in operating class object, with test values
    for (uint8_t ch = 36; ch < 50; ch += 2) {
        // allocate 1 channel
        if (!op_class_channels->alloc_channel_list()) {
            LOG(ERROR) << "alloc_channel_list() has failed!";
            return false;
        }
        auto channel_idx = op_class_channels->channel_list_length();
        auto channel     = op_class_channels->channel_list(channel_idx - 1);
        if (!channel) {
            LOG(ERROR) << "getting channel entry has failed!";
            return false;
        }
        *channel = ch;
    }

    // Update channel list flags
    op_class_channels->flags().preference = 15; // channels preference
    op_class_channels->flags().reason_code =
        wfa_map::cPreferenceOperatingClasses::eReasonCode::UNSPECIFIED;

    // Push operating class object to the list of operating class objects
    if (!channel_preference_tlv->add_operating_classes_list(op_class_channels)) {
        LOG(ERROR) << "add_operating_classes_list() has failed!";
        return false;
    }

    LOG(DEBUG) << "sending channel preference report for ruid=" << config.radio_identifier;

    return send_cmdu_to_controller(cmdu_tx);
}

bool slave_thread::handle_channel_selection_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CHANNEL_SELECTION_REQUEST_MESSAGE, mid=" << std::dec << int(mid);

    uint8_t selected_channel         = 0;
    uint8_t selected_operating_class = 0;
    // parse all tlvs in cmdu
    int tlvType;
    while ((tlvType = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {

        if (tlvType < 0) {
            LOG(ERROR) << "getNextTlvType has failed";
            return false;
        }

        if (tlvType == int(wfa_map::eTlvTypeMap::TLV_CHANNEL_PREFERENCE)) {

            // parse channel preference report message
            auto channel_preference_tlv = cmdu_rx.addClass<wfa_map::tlvChannelPreference>();
            if (!channel_preference_tlv) {
                LOG(ERROR) << "addClass wfa_map::tlvChannelPreference has failed";
                return false;
            }

            const auto &ruid = channel_preference_tlv->radio_uid();
            if (network_utils::mac_to_string(ruid) != config.radio_identifier) {
                LOG(DEBUG) << "ruid_rx=" << network_utils::mac_to_string(ruid)
                           << ", son_slave_ruid=" << config.radio_identifier;
                continue;
            }

            // read all operating class list
            auto operating_classes_list_length =
                channel_preference_tlv->operating_classes_list_length();

            for (int oc_idx = 0; oc_idx < operating_classes_list_length; oc_idx++) {
                std::stringstream ss;
                auto operating_class_tuple = channel_preference_tlv->operating_classes_list(oc_idx);
                if (!std::get<0>(operating_class_tuple)) {
                    LOG(ERROR) << "getting operating class entry has failed!";
                    return false;
                }
                auto &op_class_channels = std::get<1>(operating_class_tuple);
                auto operating_class    = op_class_channels.operating_class();
                ss << "operating class=" << int(operating_class);

                const auto &flags = op_class_channels.flags();
                auto preference   = flags.preference;
                auto reason_code  = flags.reason_code;
                ss << ", preference=" << int(preference) << ", reason=" << int(reason_code);
                ss << ", channel_list={";

                auto channel_list_length = op_class_channels.channel_list_length();
                for (int ch_idx = 0; ch_idx < channel_list_length; ch_idx++) {
                    auto channel = op_class_channels.channel_list(ch_idx);
                    if (!channel) {
                        LOG(ERROR) << "getting channel entry has failed!";
                        return false;
                    }

                    ss << int(*channel);

                    // add comma if not last channel in the list, else close list by add curl brackets
                    ss << (((ch_idx + 1) != channel_list_length) ? "," : "}");

                    // mark first supported non-dfs channel as selected channel
                    // TODO: need to check that selected channel does not violate radio restriction
                    if (preference == 0 || wireless_utils::is_dfs_channel(*channel)) {
                        continue;
                    }

                    LOG(INFO) << "selected_operating_class=" << int(operating_class);
                    LOG(INFO) << "selected_channel=" << int(*channel);
                    selected_channel         = *channel;
                    selected_operating_class = operating_class;

                    // TODO: check that the data is parsed properly after fixing the following bug:
                    // Since sFlags is defined after dynamic list cPreferenceOperatingClasses it cause data override
                    // on the first channel on the list and sFlags itself.
                    // See: https://github.com/prplfoundation/prplMesh/issues/8
                }
                LOG(DEBUG) << ss.str();
            }
        } else if (tlvType == int(wfa_map::eTlvTypeMap::TLV_TRANSMIT_POWER_LIMIT)) {
            // parse radio operation restriction tlv
            auto tlvTransmitPowerLimit = cmdu_rx.addClass<wfa_map::tlvTransmitPowerLimit>();
            if (!tlvTransmitPowerLimit) {
                LOG(ERROR) << "addClass wfa_map::tlvTransmitPowerLimit has failed";
                return false;
            }

            // TODO: continute to parse this tlv
            // This TLV contains power Tx power limit that must be considered in channel selection
            // request message. Since this is a dummy message, this TLV is ignored.
            // Full implemtation will be as part of channel selection agent certification task.
            // See: https://github.com/prplfoundation/prplMesh/issues/62

        } else {
            LOG(ERROR) << "Unexpected tlv type in CHANNEL_SELECTION_REQUEST_MESSAGE, type="
                       << int(tlvType);
            // TODO: replace return statement with function that skip s unexpected tlv
            // See: https://github.com/prplfoundation/prplMesh/issues/107

            return false;
        } // close if (tlvType == some_tlv_type)

    } //close while (cmdu_rx.getNextTlvType(tlvType))

    // build and send channel response message
    if (!cmdu_tx.create(mid, ieee1905_1::eMessageType::CHANNEL_SELECTION_RESPONSE_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type CHANNEL_SELECTION_RESPONSE_MESSAGE, has failed";
        return false;
    }

    auto channel_selection_response_tlv = cmdu_tx.addClass<wfa_map::tlvChannelSelectionResponse>();
    if (!channel_selection_response_tlv) {
        LOG(ERROR) << "addClass ieee1905_1::tlvChannelSelectionResponse has failed";
        return false;
    }

    channel_selection_response_tlv->radio_uid() =
        network_utils::mac_from_string(config.radio_identifier);
    channel_selection_response_tlv->response_code() =
        wfa_map::tlvChannelSelectionResponse::eResponseCode::ACCEPT;

    if (!send_cmdu_to_controller(cmdu_tx)) {
        LOG(ERROR) << "failed to send CHANNEL_SELECTION_RESPONSE_MESSAGE";
        return false;
    }

    // build and send operating channel report message
    if (!cmdu_tx.create(0, ieee1905_1::eMessageType::OPERATING_CHANNEL_REPORT_MESSAGE)) {
        LOG(ERROR) << "cmdu creation of type OPERATING_CHANNEL_REPORT_MESSAGE, has failed";
        return false;
    }

    auto operating_channel_report_tlv = cmdu_tx.addClass<wfa_map::tlvOperatingChannelReport>();
    if (!operating_channel_report_tlv) {
        LOG(ERROR) << "addClass ieee1905_1::operating_channel_report_tlv has failed";
        return false;
    }

    operating_channel_report_tlv->radio_uid() =
        network_utils::mac_from_string(config.radio_identifier);

    auto op_classes_list = operating_channel_report_tlv->alloc_operating_classes_list();
    if (!op_classes_list) {
        LOG(ERROR) << "alloc_operating_classes_list() has failed!";
        return false;
    }

    auto operating_class_entry_tuple = operating_channel_report_tlv->operating_classes_list(0);
    if (!std::get<0>(operating_class_entry_tuple)) {
        LOG(ERROR) << "getting operating class entry has failed!";
        return false;
    }

    auto &operating_class_entry = std::get<1>(operating_class_entry_tuple);
    if (selected_channel) {
        operating_class_entry.operating_class = selected_operating_class;
        operating_class_entry.channel_number  = selected_channel;
    } else {
        operating_class_entry.operating_class = 80;
        operating_class_entry.channel_number  = 36;
    }

    operating_channel_report_tlv->current_transmit_power() = -50;

    return send_cmdu_to_controller(cmdu_tx);
}

bool slave_thread::add_radio_basic_capabilities()
{
    auto radio_basic_caps = cmdu_tx.addClass<wfa_map::tlvApRadioBasicCapabilities>();
    if (!radio_basic_caps) {
        LOG(ERROR) << "Error creating TLV_AP_RADIO_BASIC_CAPABILITIES";
        return false;
    }
    radio_basic_caps->radio_uid() = network_utils::mac_from_string(config.radio_identifier);
    //TODO get maximum supported VAPs from DWPAL
    radio_basic_caps->maximum_number_of_bsss_supported() = 2;

    auto operationClassesInfo = radio_basic_caps->create_operating_classes_info_list();
    if (!operationClassesInfo) {
        LOG(ERROR) << "Failed creating operating classes info list";
        return false;
    }

    // TODO: Currently sending dummy values, just one operating class based on the band. Need
    // to read them from DWPAL and use the correct WiFi Parameters based on the regulatory
    // domain
    uint8_t operating_class                            = hostap_params.iface_is_5ghz ? 110 : 81;
    operationClassesInfo->operating_class()            = operating_class;
    operationClassesInfo->maximum_transmit_power_dbm() = 0;

    if (!radio_basic_caps->add_operating_classes_info_list(operationClassesInfo)) {
        LOG(ERROR) << "add_operating_classes_info_list failed";
        return false;
    }

    return true;
}

bool slave_thread::autoconfig_wsc_add_m1()
{
    auto tlvWscM1 = cmdu_tx.addClass<ieee1905_1::tlvWscM1>();
    if (tlvWscM1 == nullptr) {
        LOG(ERROR) << "Error creating tlvWscM1";
        return false;
    }

    // All attributes which are not explicitely set below are set to default by the TLV factory,
    // see WSC_Attributes.yml
    tlvWscM1->mac_attr().data = network_utils::mac_from_string(backhaul_params.bridge_mac);
    // TODO: read manufactured, name, model and device name from BPL
    if (!tlvWscM1->set_manufacturer("Intel"))
        return false;
    if (!tlvWscM1->set_model_name("Ubuntu"))
        return false;
    if (!tlvWscM1->set_model_number("18.04"))
        return false;
    if (!tlvWscM1->set_serial_number("prpl12345"))
        return false;
    if (!tlvWscM1->set_device_name("prplMesh-agent"))
        return false;
    std::memset(tlvWscM1->uuid_e_attr().data, 0xff, tlvWscM1->uuid_e_attr().data_length);
    tlvWscM1->rf_bands_attr().data =
        hostap_params.iface_is_5ghz ? WSC::WSC_RF_BAND_5GHZ : WSC::WSC_RF_BAND_2GHZ;
    tlvWscM1->primary_device_type_attr().sub_category_id = WSC::WSC_DEV_NETWORK_INFRA_AP;

    auto vendor_ext = tlvWscM1->create_vendor_ext();
    if (!vendor_ext) {
        LOG(ERROR) << "Failed to create the vendor_ext attribute";
        return false;
    }

    if (!vendor_ext->alloc_vs_data(sizeof(WSC::sWscAttrVersion2))) {
        LOG(ERROR) << "buffer allocation failed for version2 attribute";
        return false;
    }

    WSC::sWscAttrVersion2 version2;
    version2.struct_swap();
    uint8_t *version2_buf = reinterpret_cast<uint8_t *>(&version2);
    std::copy(version2_buf, version2_buf + sizeof(version2), vendor_ext->vs_data());
    tlvWscM1->add_vendor_ext(vendor_ext);

    // encryption support - diffie-helman key-exchange
    dh = std::make_unique<mapf::encryption::diffie_hellman>();
    std::copy(dh->pubkey(), dh->pubkey() + dh->pubkey_length(), tlvWscM1->public_key_attr().data);
    std::copy(dh->nonce(), dh->nonce() + dh->nonce_length(), tlvWscM1->enrolee_nonce_attr().data);
    tlvWscM1->authentication_type_flags_attr().data =
        uint16_t(WSC::eWscAuth::WSC_AUTH_OPEN) | uint16_t(WSC::eWscAuth::WSC_AUTH_WPA2PSK);
    tlvWscM1->encryption_type_flags_attr().data = uint16_t(WSC::eWscEncr::WSC_ENCR_AES);

    // Authentication support - store swapped M1 for later M1 || M2* authentication
    // This is the content of M1, without the type and length.
    if (m1_auth_buf)
        delete[] m1_auth_buf;
    m1_auth_buf_len = tlvWscM1->getLen() - 3;
    m1_auth_buf     = new uint8_t[m1_auth_buf_len];
    tlvWscM1->class_swap(); //swap before store
    std::copy_n(tlvWscM1->getStartBuffPtr() + 3, m1_auth_buf_len, m1_auth_buf);
    tlvWscM1->class_swap(); //swap after store (will be swapped in Finalize)

    return true;
}
