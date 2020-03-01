/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#include "son_slave_thread.h"

#include "../monitor/monitor_thread.h"
#include "tlvf_utils.h"

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

#include <tlvf/WSC/AttrList.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddressType.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
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

    if (config.hostap_iface_type == beerocks::IFACE_TYPE_UNSUPPORTED) {
        LOG(ERROR) << "hostap_iface_type '" << config.hostap_iface_type << "' UNSUPPORTED!";
        return false;
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
    is_backhaul_manager   = false;
    detach_on_conf_change = false;

    if (configuration_stop_on_failure_attempts && !stop_on_failure_attempts) {
        LOG(ERROR) << "Reached to max stop on failure attempts!";
        stopped = true;
    }

    if (stopped && slave_state != STATE_INIT) {
        platform_notify_error(beerocks::bpl::eErrorCode::SLAVE_STOPPED, "");
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

void slave_thread::platform_notify_error(beerocks::bpl::eErrorCode code,
                                         const std::string &error_data)
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

    error->code() = uint32_t(code);
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
        platform_notify_error(bpl::eErrorCode::SLAVE_SLAVE_BACKHAUL_MANAGER_DISCONNECTED, "");
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

    if (call_slave_select) {
        if (!socket_thread::work()) {
            return false;
        }
    }
    return true;
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

            platform_notify_error(bpl::eErrorCode::SLAVE_MASTER_KEEP_ALIVE_TIMEOUT,
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
            return handle_cmdu_control_message(sd, beerocks_header);
        } break;
        case beerocks_message::ACTION_BACKHAUL: {
            return handle_cmdu_backhaul_manager_message(sd, beerocks_header);
        } break;
        case beerocks_message::ACTION_PLATFORM: {
            return handle_cmdu_platform_manager_message(sd, beerocks_header);
        } break;
        case beerocks_message::ACTION_APMANAGER: {
            return handle_cmdu_ap_manager_message(sd, beerocks_header);
        } break;
        case beerocks_message::ACTION_MONITOR: {
            return handle_cmdu_monitor_message(sd, beerocks_header);
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
        return handle_autoconfiguration_wsc(sd, cmdu_rx);
    case ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_RENEW_MESSAGE:
        return handle_autoconfiguration_renew(sd, cmdu_rx);
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
    case ieee1905_1::eMessageType::MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE:
        return handle_multi_ap_policy_config_request(sd, cmdu_rx);
    default:
        LOG(ERROR) << "Unknown CMDU message type: " << std::hex << int(cmdu_message_type);
        return false;
    }

    return true;
}

bool slave_thread::handle_cmdu_control_message(Socket *sd,
                                               std::shared_ptr<beerocks_header> beerocks_header)
{
    // LOG(DEBUG) << "handle_cmdu_control_message(), INTEL_VS: action=" + std::to_string(beerocks_header->action()) + ", action_op=" + std::to_string(beerocks_header->action_op());
    // LOG(DEBUG) << "received radio_mac=" << beerocks_header->radio_mac() << ", local radio_mac=" << hostap_params.iface_mac;

    // to me or not to me, this is the question...
    if (beerocks_header->actionhdr()->radio_mac() != hostap_params.iface_mac) {
        return true;
    }

    if (beerocks_header->actionhdr()->direction() == beerocks::BEEROCKS_DIRECTION_CONTROLLER) {
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

        auto request_in = beerocks_header->addClass<
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
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST>();
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

        LOG(INFO) << "rx_rssi measurement request for client mac=" << request_in->params().mac
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
    case beerocks_message::ACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION: {
        LOG(DEBUG) << "received ACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION";
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass ACTION_CONTROL_CLIENT_NEW_IP_ADDRESS_NOTIFICATION failed";
            return false;
        }

        // Notify monitor
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_CLIENT_NEW_IP_ADDRESS_NOTIFICATION message!";
            return false;
        }

        notification_out->mac()  = notification_in->mac();
        notification_out->ipv4() = notification_in->ipv4();
        message_com::send_cmdu(monitor_socket, cmdu_tx);
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
            auto request_in = beerocks_header->addClass<
                beerocks_message::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST>();
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
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>();
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
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST>();
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
        auto request_in = beerocks_header->addClass<
            beerocks_message::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST>();
        if (request_in == nullptr) {
            LOG(ERROR)
                << "addClass cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST failed";
            return false;
        }
        configuration_stop_on_failure_attempts = request_in->attempts();
        stop_on_failure_attempts               = configuration_stop_on_failure_attempts;
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
                   << "bssid = " << update->params().cfg.bssid << std::endl
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
                   << "client_mac = " << notification_ap_out->params().client_mac << std::endl
                   << "bssid = " << update->params().bssid << std::endl
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
    case beerocks_message::ACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST: {
        LOG(TRACE) << "ACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST";
        auto request_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST>();
        if (!request_in) {
            LOG(ERROR) << "addClass cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST failed";
            return false;
        }

        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST>(cmdu_tx);
        if (!request_out) {
            LOG(ERROR)
                << "Failed building cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST message!";
            return false;
        }

        request_out->scan_params() = request_in->scan_params();

        LOG(DEBUG) << "send cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_REQUEST";
        message_com::send_cmdu(monitor_socket, cmdu_tx);
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
    Socket *sd, std::shared_ptr<beerocks_header> beerocks_header)
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
            beerocks_header->addClass<beerocks_message::cACTION_BACKHAUL_ENABLE_APS_REQUEST>();
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
                    LOG(DEBUG) << "mac = " << backhaul_params.backhaul_scan_measurement_list[i].mac
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

        is_backhaul_disconnected = true;
        slave_state_timer =
            std::chrono::steady_clock::now() +
            std::chrono::milliseconds(beerocks::IRE_MAX_WIRELESS_RECONNECTION_TIME_MSC);

        master_socket = nullptr;

        slave_reset();
        break;
    }
    case beerocks_message::ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE";

        auto response_in = beerocks_header->addClass<
            beerocks_message::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (!response_in) {
            LOG(ERROR)
                << "Failed building ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE message!";
            return false;
        }

        LOG(DEBUG) << "ACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE mac="
                   << response_in->params().result.mac
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
        auto response_in = beerocks_header->addClass<
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
    Socket *sd, std::shared_ptr<beerocks_header> beerocks_header)
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
                platform_notify_error(
                    bpl::eErrorCode::CONFIG_PLATFORM_REPORTED_INVALID_CONFIGURATION, "");
                stop_on_failure_attempts--;
                slave_reset();
                return true;
            }

            platform_settings = response->platform_settings();
            wlan_settings     = response->wlan_settings();

            configuration_stop_on_failure_attempts =
                response->platform_settings().stop_on_failure_attempts;
            stop_on_failure_attempts = configuration_stop_on_failure_attempts;

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
                       << " mac " << notification->mac()
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

bool slave_thread::handle_cmdu_ap_manager_message(Socket *sd,
                                                  std::shared_ptr<beerocks_header> beerocks_header)
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
        auto response_in = beerocks_header->addClass<
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

        auto response =
            beerocks_header->addClass<beerocks_message::cACTION_APMANAGER_ENABLE_APS_RESPONSE>();
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
        auto notification_in = beerocks_header->addClass<
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
        LOG(TRACE) << "send ACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION";
        send_cmdu_to_controller(cmdu_tx);

        auto notification_out2 = message_com::create_vs_message<
            beerocks_message::cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION>(cmdu_tx);
        if (notification_out2 == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out2->params() = notification_in->params();
        notification_out2->ruid()   = hostap_params.iface_mac;
        LOG(TRACE) << "send ACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION";
        message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);

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
        hostap_cs_params = notification_in->cs_params();

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
        send_operating_channel_report();
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
        hostap_cs_params = notification_in->cs_params();

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION>(cmdu_tx,
                                                                       beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }

        notification_out->cs_params() = notification_in->cs_params();
        send_cmdu_to_controller(cmdu_tx);
        send_operating_channel_report();
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
        hostap_cs_params = notification_in->cs_params();

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION>(cmdu_tx,
                                                                             beerocks_header->id());
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->cs_params() = notification_in->cs_params();
        send_cmdu_to_controller(cmdu_tx);
        send_operating_channel_report();
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        auto response_in = beerocks_header->addClass<
            beerocks_message::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE failed";
            return false;
        }
        LOG(INFO) << "APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE mac="
                  << response_in->params().result.mac
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
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION>();
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

        // Build VS CMDU message to send to backhaul manager
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR)
                << "Failed building ACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION message!";
            break;
        }

        notification_out->iface_mac()  = hostap_params.iface_mac;
        notification_out->client_mac() = notification_in->params().mac;
        notification_out->bssid()      = notification_in->params().bssid;

        // Send the message
        LOG(DEBUG) << "send ACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION for client "
                   << notification_out->client_mac();
        if (!message_com::send_cmdu(backhaul_manager_socket, cmdu_tx)) {
            slave_reset();
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

        if (config.no_vendor_specific) {
            LOG(DEBUG) << "non-Intel, not adding ClientAssociationEvent VS TLV";
        } else {
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
        }

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
        //TODO Add target BSSID
        steering_btm_report_tlv->sta_mac()         = response_in->params().mac;
        steering_btm_report_tlv->btm_status_code() = response_in->params().status_code;
        steering_btm_report_tlv->bssid()           = response_in->params().source_bssid;

        LOG(DEBUG) << "sending CLIENT_STEERING_BTM_REPORT_MESSAGE back to controller";
        LOG(DEBUG) << "BTM report source bssid: " << steering_btm_report_tlv->bssid();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE: {
        auto response_in = beerocks_header->addClass<
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
        auto notification_in = beerocks_header->addClass<
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
        break;
    }
    case beerocks_message::ACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION: {
        auto notification_in = beerocks_header->addClass<
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

        // Build VS CMDU message to send to backhaul manager
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR) << "Failed building ACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION message!";
            break;
        }

        notification_out->iface_mac()  = hostap_params.iface_mac;
        notification_out->client_mac() = notification_in->params().mac;
        notification_out->bssid()      = notification_in->params().bssid;

        // Send the message
        LOG(DEBUG) << "send ACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION for client "
                   << notification_out->client_mac();
        if (!message_com::send_cmdu(backhaul_manager_socket, cmdu_tx)) {
            slave_reset();
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

        if (config.no_vendor_specific) {
            LOG(DEBUG) << "non-Intel, not adding ClientAssociationEvent VS TLV";
        } else {
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
        }

        send_cmdu_to_controller(cmdu_tx);

        break;
    }
    case beerocks_message::ACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION: {
        auto notification_in = beerocks_header->addClass<
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
        auto notification_in = beerocks_header->addClass<
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
    case beerocks_message::ACTION_APMANAGER_READ_ACS_REPORT_RESPONSE: {
        LOG(TRACE) << "received ACTION_APMANAGER_READ_ACS_REPORT_RESPONSE";
        auto response =
            beerocks_header
                ->addClass<beerocks_message::cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE>();
        if (!response) {
            LOG(ERROR) << "addClass cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE failed";
            return false;
        }

        auto tuple_supported_channels = response->supported_channels_list(0);
        std::copy_n(&std::get<1>(tuple_supported_channels), message::SUPPORTED_CHANNELS_LENGTH,
                    hostap_params.supported_channels);

        // build channel preference report
        auto cmdu_tx_header = cmdu_tx.create(
            beerocks_header->id(), ieee1905_1::eMessageType::CHANNEL_PREFERENCE_REPORT_MESSAGE);

        if (!cmdu_tx_header) {
            LOG(ERROR) << "cmdu creation of type CHANNEL_PREFERENCE_REPORT_MESSAGE, has failed";
            return false;
        }

        auto preferences =
            wireless_utils::get_channel_preferences(hostap_params.supported_channels);

        auto channel_preference_tlv = cmdu_tx.addClass<wfa_map::tlvChannelPreference>();
        if (!channel_preference_tlv) {
            LOG(ERROR) << "addClass ieee1905_1::tlvChannelPreference has failed";
            return false;
        }

        channel_preference_tlv->radio_uid() = hostap_params.iface_mac;

        for (auto preference : preferences) {
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

            op_class_channels->operating_class() = preference.preference.oper_class;
            if (!op_class_channels->alloc_channel_list(preference.channels.size())) {
                LOG(ERROR) << "alloc_channel_list() has failed!";
                return false;
            }

            uint8_t idx = 0;
            for (auto wifi_channel : preference.channels) {
                *op_class_channels->channel_list(idx) = wifi_channel.channel;
                idx++;
            }

            // Update channel list flags
            op_class_channels->flags().preference = preference.preference.preference;
            op_class_channels->flags().reason_code =
                (wfa_map::cPreferenceOperatingClasses::eReasonCode)preference.preference.reason;

            // Push operating class object to the list of operating class objects
            if (!channel_preference_tlv->add_operating_classes_list(op_class_channels)) {
                LOG(ERROR) << "add_operating_classes_list() has failed!";
                return false;
            }
        }

        LOG(DEBUG) << "sending channel preference report for ruid=" << hostap_params.iface_mac;

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

bool slave_thread::handle_cmdu_monitor_message(Socket *sd,
                                               std::shared_ptr<beerocks_header> beerocks_header)
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
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION>();
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

        bool agent_operational = false;
        if (slave_state == STATE_OPERATIONAL && notification_in->new_tx_state() == 1 &&
            notification_in->new_hostap_enabled_state() == 1) {
            slave_resets_counter = 0;
            agent_operational    = true;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION>(cmdu_tx);
        if (notification_out == nullptr) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->operational() = agent_operational;
        notification_out->bridge_mac() = network_utils::mac_from_string(backhaul_params.bridge_mac);
        send_cmdu_to_controller(cmdu_tx);

        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE>();
        if (!response_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CLIENT_START_MONITORING_RESPONSE failed";
            break;
        }

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE>(
            cmdu_tx, beerocks_header->id());
        if (!response_out) {
            LOG(ERROR)
                << "Failed building cACTION_CONTROL_CLIENT_START_MONITORING_RESPONSE message!";
            break;
        }
        response_out->success() = response_in->success();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE>();
        if (response_in == nullptr) {
            LOG(ERROR) << "addClass ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE failed";
            break;
        }
        LOG(INFO) << "ACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE mac="
                  << response_in->params().result.mac
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
        auto notification_in = beerocks_header->addClass<
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
    case beerocks_message::ACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION: {
        auto notification_in = beerocks_header->addClass<
            beerocks_message::cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION failed";
            return false;
        }
        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION>(
            cmdu_tx, beerocks_header->id());
        if (!notification_out) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        notification_out->params() = notification_in->params();
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
        auto response_in = beerocks_header->addClass<
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
        auto response_in = beerocks_header->addClass<
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
            platform_notify_error(bpl::eErrorCode::MONITOR_HOSTAP_DISABLED, "");
        } else if (err_code == monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_ATTACH_FAIL) {
            platform_notify_error(bpl::eErrorCode::MONITOR_ATTACH_FAIL, "");
        } else if (err_code == monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_SUDDEN_DETACH) {
            platform_notify_error(bpl::eErrorCode::MONITOR_SUDDEN_DETACH, "");
        } else if (err_code ==
                   monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_HAL_DISCONNECTED) {
            platform_notify_error(bpl::eErrorCode::MONITOR_HAL_DISCONNECTED, "");
        } else if (err_code ==
                   monitor_thread::eThreadErrors::MONITOR_THREAD_ERROR_REPORT_PROCESS_FAIL) {
            platform_notify_error(bpl::eErrorCode::MONITOR_REPORT_PROCESS_FAIL, "");
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
        auto notification_in = beerocks_header->addClass<
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
        auto notification_in = beerocks_header->addClass<
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
        auto notification_in = beerocks_header->addClass<
            beerocks_message::cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION>();
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
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE>();
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
    case beerocks_message::ACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE: {
        auto response_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE>();
        if (!response_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE failed";
            return false;
        }

        auto response_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE>(cmdu_tx);
        if (!response_out) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_CHANNEL_SCAN_TRIGGER_SCAN_RESPONSE";
            return false;
        }

        response_out->success() = response_in->success();
        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CHANNEL_SCAN_TRIGGERED_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_CHANNEL_SCAN_TRIGGERED_NOTIFICATION !";
            return false;
        }

        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CHANNEL_SCAN_RESULTS_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_CHANNEL_SCAN_RESULTS_NOTIFICATION !";
            return false;
        }

        notification_out->scan_results() = notification_in->scan_results();
        notification_out->is_dump()      = notification_in->is_dump();

        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CHANNEL_SCAN_FINISHED_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_CHANNEL_SCAN_FINISHED_NOTIFICATION !";
            return false;
        }

        send_cmdu_to_controller(cmdu_tx);
        break;
    }
    case beerocks_message::ACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION: {
        auto notification_in =
            beerocks_header
                ->addClass<beerocks_message::cACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION>();
        if (!notification_in) {
            LOG(ERROR) << "addClass cACTION_MONITOR_CHANNEL_SCAN_ABORT_NOTIFICATION failed";
            return false;
        }

        auto notification_out = message_com::create_vs_message<
            beerocks_message::cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION>(cmdu_tx);
        if (!notification_out) {
            LOG(ERROR) << "Failed building cACTION_CONTROL_CHANNEL_SCAN_ABORT_NOTIFICATION !";
            return false;
        }

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
                platform_notify_error(bpl::eErrorCode::SLAVE_FAILED_CONNECT_TO_PLATFORM_MANAGER,
                                      "");
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
            platform_notify_error(bpl::eErrorCode::SLAVE_PLATFORM_MANAGER_REGISTER_TIMEOUT, "");
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
                platform_notify_error(bpl::eErrorCode::SLAVE_CONNECTING_TO_BACKHAUL_MANAGER,
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
        request->ruid()                 = hostap_params.iface_mac;
        request->certification_mode()   = platform_settings.certification_mode;

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
            is_backhaul_manager = false;
        }

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
            platform_notify_error(bpl::eErrorCode::AP_MANAGER_START, "");
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
                platform_notify_error(
                    bpl::eErrorCode::CONFIG_BACKHAUL_WIRED_INTERFACE_IS_UNSUPPORTED, "");
                error = true;
            }
        }
        if (!config.backhaul_wireless_iface.empty()) {
            if (config.backhaul_wireless_iface_type == beerocks::IFACE_TYPE_UNSUPPORTED) {
                LOG(DEBUG) << "backhaul_wireless_iface is UNSUPPORTED";
                platform_notify_error(
                    bpl::eErrorCode::CONFIG_BACKHAUL_WIRELESS_INTERFACE_IS_UNSUPPORTED, "");
                error = true;
            }
        }
        if (config.backhaul_wire_iface.empty() && config.backhaul_wireless_iface.empty()) {
            LOG(DEBUG) << "No valid backhaul iface!";
            platform_notify_error(bpl::eErrorCode::CONFIG_NO_VALID_BACKHAUL_INTERFACE, "");
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

        auto tuple_supported_channels = bh_enable->supported_channels_list(0);
        if (!std::get<0>(tuple_supported_channels)) {
            LOG(ERROR) << "getting supported channels has failed!";
            break;
        }

        std::copy_n(hostap_params.supported_channels, message::SUPPORTED_CHANNELS_LENGTH,
                    &std::get<1>(tuple_supported_channels));

        // Send the message
        LOG(DEBUG) << "send ACTION_BACKHAUL_ENABLE for mac " << bh_enable->iface_mac();
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
            platform_notify_error(bpl::eErrorCode::SLAVE_INVALID_MASTER_SOCKET,
                                  "Invalid master socket");
            stop_on_failure_attempts--;
            slave_reset();
            break;
        }

        if (!cmdu_tx.create(0, ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WSC_MESSAGE)) {
            LOG(ERROR) << "Failed creating AP_AUTOCONFIGURATION_WSC_MESSAGE";
            return false;
        }

        std::array<beerocks::message::sWifiChannel, beerocks::message::SUPPORTED_CHANNELS_LENGTH>
            supported_channels;
        std::copy_n(std::begin(hostap_params.supported_channels), supported_channels.size(),
                    supported_channels.begin());

        if (!tlvf_utils::add_ap_radio_basic_capabilities(cmdu_tx, hostap_params.iface_mac,
                                                         supported_channels)) {
            LOG(ERROR) << "Failed adding AP Radio Basic Capabilities TLV";
            return false;
        }

        if (!autoconfig_wsc_add_m1()) {
            LOG(ERROR) << "Failed adding WSC M1 TLV";
            return false;
        }

        if (config.no_vendor_specific) {
            LOG(INFO) << "Configured as non-Intel, not sending SLAVE_JOINED_NOTIFICATION";
        } else {
            auto notification = message_com::add_vs_tlv<
                beerocks_message::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION>(cmdu_tx);

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
            notification->radio_identifier()     = hostap_params.iface_mac;
            network_utils::mac_from_string(config.radio_identifier);

            // Backhaul Params
            notification->backhaul_params().gw_ipv4 =
                network_utils::ipv4_from_string(backhaul_params.gw_ipv4);
            notification->backhaul_params().gw_bridge_mac =
                network_utils::mac_from_string(backhaul_params.gw_bridge_mac);
            notification->backhaul_params().is_backhaul_manager = is_backhaul_manager;
            notification->backhaul_params().backhaul_iface_type =
                backhaul_params.backhaul_iface_type;
            notification->backhaul_params().backhaul_mac =
                network_utils::mac_from_string(backhaul_params.backhaul_mac);
            notification->backhaul_params().backhaul_channel = backhaul_params.backhaul_channel;
            notification->backhaul_params().backhaul_bssid =
                network_utils::mac_from_string(backhaul_params.backhaul_bssid);
            notification->backhaul_params().backhaul_is_wireless =
                backhaul_params.backhaul_is_wireless;

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
                        << notification->backhaul_params().backhaul_scan_measurement_list[i].mac.oct
                        << " channel = "
                        << int(notification->backhaul_params()
                                   .backhaul_scan_measurement_list[i]
                                   .channel)
                        << " rssi = "
                        << int(notification->backhaul_params()
                                   .backhaul_scan_measurement_list[i]
                                   .rssi);
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
            }
        }

        send_cmdu_to_controller(cmdu_tx);
        LOG(DEBUG) << "sending WSC M1 Size=" << int(cmdu_tx.getMessageLength());

        LOG(TRACE) << "goto STATE_WAIT_FOR_JOINED_RESPONSE";
        slave_state_timer = std::chrono::steady_clock::now() +
                            std::chrono::seconds(WAIT_FOR_JOINED_RESPONSE_TIMEOUT_SEC);

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
        LOG(INFO) << "time_elapsed_secs > AP_MANAGER_HEARTBEAT_TIMEOUT_SEC "
                     "ap_manager_retries_counter = "
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
        auto beerocks_header = message_com::get_beerocks_header(cmdu_tx);
        if (!beerocks_header) {
            LOG(ERROR) << "Failed getting beerocks_header!";
            return false;
        }

        beerocks_header->actionhdr()->radio_mac() = hostap_params.iface_mac;
        beerocks_header->actionhdr()->direction() = beerocks::BEEROCKS_DIRECTION_CONTROLLER;
    }
    return message_com::send_cmdu(master_socket, cmdu_tx, backhaul_params.controller_bridge_mac,
                                  backhaul_params.bridge_mac);
}

/**
 * @brief Diffie-Hellman public key exchange keys calculation
 *        class member params authkey and keywrapauth are computed
 *        on success.
 *
 * @param[in] m2 WSC M2 recived from the controller
 * @param[out] authkey 32 bytes calculated authentication key
 * @param[out] keywrapkey 16 bytes calculated key wrap key
 * @return true on success
 * @return false on failure
 */
bool slave_thread::autoconfig_wsc_calculate_keys(WSC::m2 &m2, uint8_t authkey[32],
                                                 uint8_t keywrapkey[16])
{
    if (!dh) {
        LOG(ERROR) << "diffie hellman member not initialized";
        return false;
    }
    auto mac = network_utils::mac_from_string(backhaul_params.bridge_mac);
    mapf::encryption::wps_calculate_keys(*dh, m2.public_key(),
                                         WSC::eWscLengths::WSC_PUBLIC_KEY_LENGTH, dh->nonce(),
                                         mac.oct, m2.registrar_nonce(), authkey, keywrapkey);

    return true;
}

/**
 * @brief autoconfig global authenticator attribute calculation
 * 
 * Calculate authentication on the Full M1 || M2* whereas M2* = M2 without the authenticator
 * attribute. M1 is a saved buffer of the swapped M1 sent in the WSC autoconfig sent by the agent.
 * 
 * @param m2 WSC M2 attribute list from the controller
 * @param authkey authentication key
 * @return true on success
 * @return false on failure
 */
bool slave_thread::autoconfig_wsc_authenticate(WSC::m2 &m2, uint8_t authkey[32])
{
    if (!m1_auth_buf) {
        LOG(ERROR) << "Invalid M1";
        return false;
    }

    // This is the content of M1 and M2, without the type and length.
    uint8_t buf[m1_auth_buf_len + m2.getMessageLength() -
                WSC::cWscAttrAuthenticator::get_initial_size()];
    auto next = std::copy_n(m1_auth_buf, m1_auth_buf_len, buf);
    m2.swap(); //swap to get network byte order
    std::copy_n(m2.getMessageBuff(),
                m2.getMessageLength() - WSC::cWscAttrAuthenticator::get_initial_size(), next);
    m2.swap(); //swap back

    uint8_t kwa[WSC::WSC_AUTHENTICATOR_LENGTH];
    // Add KWA which is the 1st 64 bits of HMAC of config_data using AuthKey
    if (!mapf::encryption::kwa_compute(authkey, buf, sizeof(buf), kwa)) {
        LOG(ERROR) << "kwa_compute failure";
        return false;
    }

    if (!std::equal(kwa, kwa + sizeof(kwa), reinterpret_cast<uint8_t *>(m2.authenticator()))) {
        LOG(ERROR) << "WSC Global authentication failed";
        LOG(DEBUG) << "authenticator: "
                   << utils::dump_buffer(reinterpret_cast<uint8_t *>(m2.authenticator()),
                                         WSC::WSC_AUTHENTICATOR_LENGTH);
        LOG(DEBUG) << "calculated:    " << utils::dump_buffer(kwa, WSC::WSC_AUTHENTICATOR_LENGTH);
        LOG(DEBUG) << "authenticator key: " << utils::dump_buffer(authkey, 32);
        LOG(DEBUG) << "authenticator buf:" << std::endl << utils::dump_buffer(buf, sizeof(buf));
        return false;
    }

    LOG(DEBUG) << "WSC Global authentication success";
    return true;
}

bool slave_thread::autoconfig_wsc_parse_m2_encrypted_settings(WSC::m2 &m2, uint8_t authkey[32],
                                                              uint8_t keywrapkey[16],
                                                              WSC::configData::config &config)
{
    auto encrypted_settings = m2.encrypted_settings();
    uint8_t *iv             = reinterpret_cast<uint8_t *>(encrypted_settings.iv());
    auto ciphertext         = reinterpret_cast<uint8_t *>(encrypted_settings.encrypted_settings());
    int cipherlen           = encrypted_settings.encrypted_settings_length();
    // leave room for up to 16 bytes internal padding length - see aes_decrypt()
    int datalen = cipherlen + 16;
    uint8_t decrypted[datalen];

    LOG(DEBUG) << "M2 Parse: aes decrypt";
    if (!mapf::encryption::aes_decrypt(keywrapkey, iv, ciphertext, cipherlen, decrypted, datalen)) {
        LOG(ERROR) << "aes decrypt failure";
        return false;
    }

    LOG(DEBUG) << "M2 Parse: parse config_data, len = " << datalen;

    // Parsing failure means that the config data is invalid,
    // in which case it is unclear what we should do.
    // In practice, some controllers simply send an empty config data
    // when the radio should be tore down, so let the caller handle this
    // by returning true with a warning for now.
    auto config_data = WSC::configData::parse(decrypted, datalen);
    if (!config_data) {
        LOG(WARNING) << "Invalid config data, skip it";
        return true;
    }

    // get length of config_data for KWA authentication
    size_t len = config_data->getMessageLength();
    // Protect against M2 buffer overflow attacks
    if (len + sizeof(WSC::sWscAttrKeyWrapAuthenticator) > size_t(datalen)) {
        LOG(ERROR) << "invalid config data length";
        return false;
    }
    // Update VAP configuration
    config.auth_type   = config_data->auth_type();
    config.encr_type   = config_data->encr_type();
    config.bssid       = config_data->bssid();
    config.network_key = config_data->network_key();
    config.ssid        = config_data->ssid();
    config.bss_type    = static_cast<WSC::eWscVendorExtSubelementBssType>(config_data->bss_type());
    // Swap to network byte order for KWA HMAC calculation
    // from this point config data is not readable!
    config_data->swap();
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

    auto tlvAlMac = cmdu_rx.getClass<ieee1905_1::tlvAlMacAddressType>();
    if (tlvAlMac) {
        LOG(DEBUG) << "tlvAlMac=" << tlvAlMac->mac();
        // TODO register/update mapping of AL-MAC to interface, cfr. #81
    } else {
        LOG(ERROR) << "tlvAlMac missing - ignoring autconfig renew message";
        return false;
    }

    auto tlvSupportedRole = cmdu_rx.getClass<ieee1905_1::tlvSupportedRole>();
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

    auto tlvSupportedFreqBand = cmdu_rx.getClass<ieee1905_1::tlvSupportedFreqBand>();
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

    std::list<WSC::m2> m2_list;
    for (auto tlv : cmdu_rx.getClassList<ieee1905_1::tlvWsc>()) {
        auto m2 = WSC::m2::parse(*tlv);
        if (!m2) {
            LOG(INFO) << "Not a valid M2 - Ignoring WSC CMDU";
            continue;
        }
        m2_list.push_back(*m2);
    }
    if (m2_list.empty()) {
        LOG(ERROR) << "No M2s present";
        return false;
    }

    auto ruid = cmdu_rx.getClass<wfa_map::tlvApRadioIdentifier>();
    if (!ruid) {
        LOG(ERROR) << "getClass<wfa_map::tlvApRadioIdentifier> failed";
        return false;
    }
    // Check if the message is for this radio agent by comparing the ruid
    if (hostap_params.iface_mac != ruid->radio_uid()) {
        LOG(DEBUG) << "not to me - ruid " << hostap_params.iface_mac << " != " << ruid->radio_uid();
        return true;
    }

    std::vector<WSC::configData::config> configs;
    for (auto m2 : m2_list) {
        LOG(DEBUG) << "M2 Parse " << m2.manufacturer()
                   << " Controller configuration (WSC M2 Encrypted Settings)";
        uint8_t authkey[32];
        uint8_t keywrapkey[16];
        LOG(DEBUG) << "M2 Parse: calculate keys";
        if (!autoconfig_wsc_calculate_keys(m2, authkey, keywrapkey))
            return false;

        if (!autoconfig_wsc_authenticate(m2, authkey))
            return false;

        WSC::configData::config config;
        if (!autoconfig_wsc_parse_m2_encrypted_settings(m2, authkey, keywrapkey, config)) {
            LOG(ERROR) << "Invalid config data, skip it";
            continue;
        }
        // TODO - revisit this in the future
        // In practice, some controllers simply send an empty config data when asked for tear down,
        // so tear down the radio if the SSID is empty.
        if (config.ssid.empty()) {
            LOG(INFO) << "Empty config data, tear down radio";
            configs.clear();
            break;
        }

        LOG(INFO) << "bss_type: " << std::hex << int(config.bss_type);
        if (config.bss_type & WSC::eWscVendorExtSubelementBssType::TEARDOWN) {
            LOG(INFO) << "Teardown bit set, tear down radio";
            configs.clear();
            break;
        }
        // BACKHAUL_STA bit is not expected to be set
        if (config.bss_type & WSC::eWscVendorExtSubelementBssType::BACKHAUL_STA) {
            LOG(WARNING) << "Unexpected backhaul STA bit";
        }
        LOG(DEBUG) << m2.manufacturer() << " config data:" << std::endl
                   << " ssid: " << config.ssid << ", bssid: " << config.bssid
                   << ", authentication_type: " << std::hex << int(config.auth_type)
                   << ", encryption_type: " << int(config.encr_type);
        configs.push_back(config);
    }

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST>(cmdu_tx);
    if (!request) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }
    for (auto config : configs) {
        auto c = request->create_wifi_credentials();
        if (!c) {
            LOG(ERROR) << "Failed building message!";
            return false;
        }
        c->set_ssid(config.ssid);
        c->set_network_key(config.network_key);
        c->bssid_attr().data               = config.bssid;
        c->authentication_type_attr().data = config.auth_type;
        c->encryption_type_attr().data     = config.encr_type;
        c->multiap_attr().subelement_value = config.bss_type;
        request->add_wifi_credentials(c);
    }

    ///////////////////////////////////////////////////////////////////
    // TODO https://github.com/prplfoundation/prplMesh/issues/797
    //
    // Short term solution
    // In non-EasyMesh mode, never modify hostapd configuration
    // and in this case VAPs credentials
    //
    // Long term solution
    // All EasyMesh VAPs will be stored in the platform DB.
    // All other VAPs are manual, AKA should not be modified by prplMesh
    ////////////////////////////////////////////////////////////////////
    if (platform_settings.management_mode != BPL_MGMT_MODE_NOT_MULTIAP) {
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
    } else {
        LOG(WARNING) << "non-EasyMesh mode - skip updating VAP credentials";
    }

    // Notify backhaul manager that onboarding has finished (certification flow)
    auto onboarding_finished_notification = message_com::create_vs_message<
        beerocks_message::cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION>(cmdu_tx);

    if (onboarding_finished_notification == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    message_com::send_cmdu(backhaul_manager_socket, cmdu_tx);

    if (slave_state != STATE_WAIT_FOR_JOINED_RESPONSE) {
        LOG(ERROR) << "slave_state != STATE_WAIT_FOR_JOINED_RESPONSE";
        return false;
    }

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header) {
        LOG(INFO) << "Intel controller join response";
        if (!parse_intel_join_response(sd, *beerocks_header)) {
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

bool slave_thread::parse_intel_join_response(Socket *sd, beerocks::beerocks_header &beerocks_header)
{
    LOG(DEBUG) << "ACTION_CONTROL_SLAVE_JOINED_RESPONSE sd=" << intptr_t(sd);
    if (slave_state != STATE_WAIT_FOR_JOINED_RESPONSE) {
        LOG(ERROR) << "slave_state != STATE_WAIT_FOR_JOINED_RESPONSE";
        return false;
    }

    if (beerocks_header.action_op() != beerocks_message::ACTION_CONTROL_SLAVE_JOINED_RESPONSE) {
        LOG(ERROR) << "Unexpected Intel action op " << beerocks_header.action_op();
        return false;
    }

    auto joined_response =
        beerocks_header.addClass<beerocks_message::cACTION_CONTROL_SLAVE_JOINED_RESPONSE>();
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
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE, mid=" << std::dec
               << int(mid);

    auto association_control_request_tlv =
        cmdu_rx.getClass<wfa_map::tlvClientAssociationControlRequest>();
    if (!association_control_request_tlv) {
        LOG(ERROR) << "addClass wfa_map::tlvClientAssociationControlRequest failed";
        return false;
    }

    const auto &bssid   = association_control_request_tlv->bssid_to_block_client();
    const auto &sta_mac = std::get<1>(association_control_request_tlv->sta_list(0));

    auto block = association_control_request_tlv->association_control();
    if (block == wfa_map::tlvClientAssociationControlRequest::UNBLOCK) {
        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST>(cmdu_tx, mid);
        if (!request_out) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_ALLOW_REQUEST message!";
            return false;
        }

        request_out->mac()   = sta_mac;
        request_out->bssid() = bssid;
    } else if (block == wfa_map::tlvClientAssociationControlRequest::BLOCK) {
        auto request_out = message_com::create_vs_message<
            beerocks_message::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST>(cmdu_tx, mid);
        if (!request_out) {
            LOG(ERROR) << "Failed building ACTION_APMANAGER_CLIENT_DISALLOW_REQUEST message!";
            return false;
        }

        request_out->mac()   = sta_mac;
        request_out->bssid() = bssid;
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

    auto steering_request_tlv = cmdu_rx.getClass<wfa_map::tlvSteeringRequest>();
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
    const auto mid            = cmdu_rx.getMessageId();
    auto ap_metrics_query_tlv = cmdu_rx.getClass<wfa_map::tlvApMetricQuery>();
    if (!ap_metrics_query_tlv) {
        LOG(ERROR) << "AP Metrics Query CMDU mid=" << mid << " does not have AP Metric Query TLV";
        return false;
    }
    for (int bssid_idx = 0; bssid_idx < ap_metrics_query_tlv->bssid_list_length(); bssid_idx++) {
        auto bssid = ap_metrics_query_tlv->bssid_list(bssid_idx);
        if (!std::get<0>(bssid)) {
            LOG(ERROR) << "Failed to get bssid " << bssid_idx << " from AP_METRICS_QUERY";
            return false;
        }
        LOG(DEBUG) << "Received AP_METRICS_QUERY_MESSAGE, mid=" << std::hex << int(mid)
                   << "  bssid " << std::get<1>(bssid);
    }
    return true;
}

bool slave_thread::handle_link_metrics_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received LINK_METRIC_QUERY_MESSAGE, mid=" << std::hex << int(mid);
    // TODO add handling for Link metric query response}
    return true;
}

bool slave_thread::handle_channel_preference_query(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CHANNEL_PREFERENCE_QUERY_MESSAGE, mid=" << std::dec << int(mid);

    auto request_out =
        message_com::create_vs_message<beerocks_message::cACTION_APMANAGER_READ_ACS_REPORT_REQUEST>(
            cmdu_tx, mid);

    if (!request_out) {
        LOG(ERROR) << "Failed building message ACTION_APMANAGER_READ_ACS_REPORT_REQUEST!";
        return false;
    }
    return message_com::send_cmdu(ap_manager_socket, cmdu_tx);
}

beerocks::message::sWifiChannel slave_thread::channel_selection_select_channel()
{
    for (const auto &preference : channel_preferences) {
        LOG(DEBUG) << "Preference operating class: " << int(preference.preference.oper_class);
        for (uint8_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++) {
            auto channel         = hostap_params.supported_channels[i];
            auto operating_class = wireless_utils::get_operating_class_by_channel(
                channel.channel, static_cast<beerocks::eWiFiBandwidth>(channel.channel_bandwidth));

            // Skip channels from other operating classes
            if (operating_class != preference.preference.oper_class) {
                continue;
            }
            // Skip DFS channels
            if (channel.is_dfs_channel) {
                LOG(DEBUG) << "Skip DFS channel " << int(channel.channel);
                continue;
            }
            auto is_restricted =
                std::find_if(preference.channels.begin(), preference.channels.end(),
                             [&](const beerocks::message::sWifiChannel &wifi_channel) {
                                 return (channel.channel == wifi_channel.channel);
                             });
            // Skip restricted channels
            if (is_restricted != preference.channels.end()) {
                LOG(DEBUG) << "Skip restricted channel " << int(channel.channel);
                continue;
            }
            // If we got this far, we found a candidate channel, so switch to it
            LOG(DEBUG) << "Selected channel " << int(channel.channel);
            return channel;
        }
    }

    LOG(ERROR) << "Could not find a suitable channel";
    return beerocks::message::sWifiChannel();
}

bool slave_thread::channel_selection_current_channel_restricted()
{
    for (auto preference : channel_preferences) {
        auto is_restricted =
            std::find_if(preference.channels.begin(), preference.channels.end(),
                         [&](const beerocks::message::sWifiChannel &wifi_channel) {
                             return hostap_cs_params.channel == wifi_channel.channel;
                         });

        if (is_restricted != preference.channels.end()) {
            LOG(INFO) << "Current channel " << int(hostap_cs_params.channel)
                      << " is restricted, channel switch required";
            return true;
        }
    }
    LOG(INFO) << "Current channel " << int(hostap_cs_params.channel)
              << " not restricted, channel switch not required";
    return false;
}

bool slave_thread::channel_selection_get_channel_preference(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    channel_preferences.clear();

    for (auto channel_preference_tlv : cmdu_rx.getClassList<wfa_map::tlvChannelPreference>()) {

        const auto &ruid = channel_preference_tlv->radio_uid();
        if (ruid != hostap_params.iface_mac) {
            LOG(DEBUG) << "ruid_rx=" << ruid << ", son_slave_ruid=" << hostap_params.iface_mac;
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

            std::vector<beerocks::message::sWifiChannel> channels_list;
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

                beerocks::message::sWifiChannel wifi_channel;
                wifi_channel.channel = *channel;
                channels_list.push_back(wifi_channel);
            }
            LOG(DEBUG) << ss.str();
            auto pref =
                beerocks::message::sPreference({operating_class, preference, uint8_t(reason_code)});
            channel_preferences.push_back({pref, channels_list});
        }
    }

    return true;
}

bool slave_thread::channel_selection_get_transmit_power_limit(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                              int &power_limit)
{
    for (const auto &tx_power_limit_tlv : cmdu_rx.getClassList<wfa_map::tlvTransmitPowerLimit>()) {

        const auto &ruid = tx_power_limit_tlv->radio_uid();
        if (ruid != hostap_params.iface_mac) {
            LOG(DEBUG) << "ruid_rx=" << ruid << ", son_slave_ruid=" << hostap_params.iface_mac;
            continue;
        }

        power_limit = tx_power_limit_tlv->transmit_power_limit_dbm();
        LOG(DEBUG) << std::dec << "received tlvTransmitPowerLimit " << (int)power_limit;
        // Only one limit per ruid
        return true;
    }
    return false;
}

bool slave_thread::handle_channel_selection_request(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received CHANNEL_SELECTION_REQUEST_MESSAGE, mid=" << std::dec << int(mid);

    if (!channel_selection_get_channel_preference(cmdu_rx)) {
        LOG(ERROR) << "Failed to update channel preference";
        return false;
    }

    int power_limit           = 0;
    bool power_limit_received = channel_selection_get_transmit_power_limit(cmdu_rx, power_limit);

    // No preferences or power limit for current ruid
    if (channel_preferences.empty() && !power_limit_received)
        return true;

    // According to design only Resticted channels should be included in channel selection request
    auto switch_required = channel_selection_current_channel_restricted();
    beerocks::message::sWifiChannel channel_to_switch;
    if (switch_required) {
        channel_to_switch = channel_selection_select_channel();
    }

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

    if (switch_required && channel_to_switch.channel == 0) {
        channel_selection_response_tlv->radio_uid()     = hostap_params.iface_mac;
        channel_selection_response_tlv->response_code() = wfa_map::tlvChannelSelectionResponse::
            eResponseCode::DECLINE_VIOLATES_MOST_RECENTLY_REPORTED_PREFERENCES;
        LOG(INFO) << "Decline channel selection request " << hostap_params.iface_mac;
        if (!send_cmdu_to_controller(cmdu_tx)) {
            LOG(ERROR) << "failed to send CHANNEL_SELECTION_RESPONSE_MESSAGE";
            return false;
        }
        return true;
    }

    channel_selection_response_tlv->radio_uid() = hostap_params.iface_mac;
    channel_selection_response_tlv->response_code() =
        wfa_map::tlvChannelSelectionResponse::eResponseCode::ACCEPT;

    if (!send_cmdu_to_controller(cmdu_tx)) {
        LOG(ERROR) << "failed to send CHANNEL_SELECTION_RESPONSE_MESSAGE";
        return false;
    }

    // Currently, setting and reading tx power limit is not implemented
    // in any or our BWL backends.
    // Therefore, updating only the tx power limit does not result with an event
    // which should trigger the agent to send the operating channel report
    // with the new tx power, thus failing certification onboarding test 4.2.1.
    // As a temporary workaround, always send the operating channel report regardless
    // of the tx power setting.
    // TODO - Fix as part of https://github.com/prplfoundation/prplMesh/issues/725
    if (!switch_required) {
        LOG(INFO) << "Channel switch not required, sending operating channel report";
        send_operating_channel_report();
        if (!power_limit_received) {
            LOG(INFO) << "No power limit received - nothing else to do";
            return true;
        }
    }

    auto request_out = message_com::create_vs_message<
        beerocks_message::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START>(cmdu_tx, mid);
    if (!request_out) {
        LOG(ERROR) << "Failed building message!";
        return false;
    }

    LOG(DEBUG) << "send ACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START";

    // If only tx power limit change is required, set channel to current
    request_out->cs_params().channel =
        switch_required ? channel_to_switch.channel : hostap_cs_params.channel;
    request_out->cs_params().bandwidth = channel_to_switch.channel_bandwidth;
    request_out->tx_limit()            = power_limit;
    request_out->tx_limit_valid()      = power_limit_received;

    ///////////////////////////////////////////////////////////////////
    // TODO https://github.com/prplfoundation/prplMesh/issues/797
    //
    // Short term solution
    // In non-EasyMesh mode, never modify hostapd configuration
    // and in this case don't switch channel
    //
    ////////////////////////////////////////////////////////////////////
    if (platform_settings.management_mode != BPL_MGMT_MODE_NOT_MULTIAP) {
        message_com::send_cmdu(ap_manager_socket, cmdu_tx);
    } else {
        LOG(WARNING) << "non-EasyMesh mode - skip channel switch";
    }

    return true;
}

bool slave_thread::send_operating_channel_report()
{
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

    operating_channel_report_tlv->radio_uid() = hostap_params.iface_mac;

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

    auto &operating_class_entry           = std::get<1>(operating_class_entry_tuple);
    operating_class_entry.operating_class = wireless_utils::get_operating_class_by_channel(
        hostap_cs_params.channel, (beerocks::eWiFiBandwidth)hostap_cs_params.bandwidth);
    operating_class_entry.channel_number = hostap_cs_params.channel;

    operating_channel_report_tlv->current_transmit_power() = hostap_cs_params.tx_power;

    return send_cmdu_to_controller(cmdu_tx);
}

bool slave_thread::autoconfig_wsc_add_m1()
{
    auto tlv = cmdu_tx.addClass<ieee1905_1::tlvWsc>();
    if (tlv == nullptr) {
        LOG(ERROR) << "Error creating tlvWsc";
        return false;
    }

    // Allocate maximum allowed length for the payload, so it can accommodate variable length
    // data inside the internal TLV list.
    // On finalize(), the buffer is shrunk back to its real size.
    size_t payload_length =
        tlv->getBuffRemainingBytes() - ieee1905_1::tlvEndOfMessage::get_initial_size();
    tlv->alloc_payload(payload_length);

    WSC::m1::config cfg;
    cfg.msg_type = WSC::eWscMessageType::WSC_MSG_TYPE_M1;
    cfg.mac      = network_utils::mac_from_string(backhaul_params.bridge_mac);
    dh           = std::make_unique<mapf::encryption::diffie_hellman>();
    std::copy(dh->nonce(), dh->nonce() + dh->nonce_length(), cfg.enrollee_nonce);
    copy_pubkey(*dh, cfg.pub_key);
    cfg.auth_type_flags =
        uint16_t(WSC::eWscAuth::WSC_AUTH_OPEN) | uint16_t(WSC::eWscAuth::WSC_AUTH_WPA2PSK);
    cfg.encr_type_flags     = uint16_t(WSC::eWscEncr::WSC_ENCR_AES);
    cfg.manufacturer        = "Intel";
    cfg.model_name          = "Ubuntu";
    cfg.model_number        = "18.04";
    cfg.serial_number       = "prpl12345";
    cfg.primary_dev_type_id = WSC::WSC_DEV_NETWORK_INFRA_AP;
    cfg.device_name         = "prplmesh-agent";
    cfg.bands       = hostap_params.iface_is_5ghz ? WSC::WSC_RF_BAND_5GHZ : WSC::WSC_RF_BAND_2GHZ;
    auto attributes = WSC::m1::create(*tlv, cfg);
    if (!attributes)
        return false;

    // Authentication support - store swapped M1 for later M1 || M2* authentication
    // This is the content of M1, without the type and length.
    if (m1_auth_buf)
        delete[] m1_auth_buf;
    m1_auth_buf_len = attributes->len();
    m1_auth_buf     = new uint8_t[m1_auth_buf_len];
    std::copy_n(attributes->buffer(), m1_auth_buf_len, m1_auth_buf);
    return true;
}
