/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_cli_socket.h"

#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_cli.h>

#define SELECT_TIMEOUT (500) // 500 milliseconds

using namespace beerocks::net;

namespace beerocks {

cli_socket::cli_socket(std::string temp_path_, std::string proxy_ip_)
{
    master_socket = nullptr;
    cli_socket::setFunctionsMapAndArray();
    wait_dump_node_info = false;
    wait_response       = false;
    proxy_ip            = proxy_ip_;
    temp_path           = temp_path_;
}

cli_socket::~cli_socket() { cli_socket::disconnect(); }

bool cli_socket::init()
{
    set_select_timeout(SELECT_TIMEOUT);
    return true;
}

void cli_socket::on_thread_stop() { disconnect(); }

bool cli_socket::socket_disconnected(Socket *sd) { return true; }

void cli_socket::disconnect()
{
    if (master_socket != nullptr) { //close cli socket
        remove_socket(master_socket);
        master_socket->closeSocket();
        delete master_socket;
        master_socket = nullptr;
    }
}

bool cli_socket::connect()
{
    disconnect(); // Disconnect if already connected

    if (!proxy_ip.empty()) {
        // connect proxy sock
        LOG(INFO) << "connecting to proxy socket at " << proxy_ip;
        master_socket = new SocketClient(proxy_ip, beerocks::CLI_PROXY_TCP_PORT, 2000);
    } else {
        std::string master_uds = temp_path + std::string(BEEROCKS_MASTER_UDS);
        master_socket          = new SocketClient(master_uds);
    }
    err = master_socket->getError();
    if (!err.empty()) {
        std::cout << "ERROR: " << err << std::endl;
        delete master_socket;
        master_socket = nullptr;
        return false;
    }

    add_socket(master_socket);
    return true;
}

//
// Help functions
//

void cli_socket::setFunctionsMapAndArray()
{
    helpLineSize = 0;

    //Setting up functions map : <function_name>,
    //                           <help_args>,<help>,
    //                           <funcPtr>,
    //                           <minNumOfArgs>,<maxNumOfArgs>,
    //                           <type1>,<type2>,...,<type10>
    insertCommandToMap("help", "", "get supported commands", &cli::help_caller, 0, 0);
    insertCommandToMap("-c", "<command1 command1_parameters command2 command2_parameters... >",
                       "executes given commands", &cli::multiple_commands_caller, 1, 30);
    insertCommandToMap("q", "", "exit command line", &cli::exit_caller, 0, 0);
    insertCommandToMap("exit", "", "exit command line", &cli::exit_caller, 0, 0);
    insertCommandToMap("enable_debug", "", "enable debug messages on cli",
                       static_cast<pFunction>(&cli_socket::enable_debug_caller), 0, 1);
    insertCommandToMap("set_stop_on_failure_attempts", "",
                       "if value is given,sets stop on failure attempts, else read value",
                       static_cast<pFunction>(&cli_socket::set_stop_on_failure_attempts_caller), 0,
                       1);
    insertCommandToMap(
        "enable_diagnostics_measurements", "[<1 or 0>]",
        "if input was given - enable/disable diagnostics measurements, prints current value",
        static_cast<pFunction>(&cli_socket::enable_diagnostics_measurements_caller), 0, 1, INT_ARG);
    insertCommandToMap("dump_node_info", "<mac_address>", "print given node info",
                       static_cast<pFunction>(&cli_socket::dump_node_info_caller), 1, 1,
                       STRING_ARG);
    insertCommandToMap(
        "cross_rx_rssi_measurement", "<client_mac> <hostap_mac> <center_frequency>",
        "initiate a cross rx_rssi measurement of given 'client mac' and  'hostap_mac' ",
        static_cast<pFunction>(&cli_socket::cross_rx_rssi_measurement_caller), 2, 3, STRING_ARG,
        STRING_ARG, INT_ARG);
    insertCommandToMap("steer_client", "<client_mac> <bssid> [<disassoc_timer>]",
                       "steer given 'client mac' to 'bssid'  using 11v",
                       static_cast<pFunction>(&cli_socket::steer_client_caller), 2, 3, STRING_ARG,
                       STRING_ARG, INT_ARG);
    insertCommandToMap(
        "steer_ire", "<client_mac> <bssid> ", "steer given 'slave mac' to 'bssid' using roam",
        static_cast<pFunction>(&cli_socket::steer_ire_caller), 2, 2, STRING_ARG, STRING_ARG);
    insertCommandToMap("optimal_path_task", "<client_mac>",
                       "initiate an optimal path task for the given 'client mac'",
                       static_cast<pFunction>(&cli_socket::optimal_path_caller), 1, 1, STRING_ARG);

    insertCommandToMap(
        "ap_channel_switch",
        "<hostap_mac> <channel> <bw> <channel_ext_above_secondary> <vht_center_frequency>",
        "sends channel switch request to ap (when channel=0, bw is not needed and acs procedure "
        "with acs report will be triggered)",
        static_cast<pFunction>(&cli_socket::ap_channel_switch_caller), 2, 5, STRING_ARG, INT_ARG,
        INT_ARG, INT_ARG, INT_ARG);
    insertCommandToMap("client_allow", "<client_mac> <hostap_mac>",
                       "sends to 'hostap_mac' client allow request for 'client_mac'",
                       static_cast<pFunction>(&cli_socket::client_allow_caller), 2, 2, STRING_ARG,
                       STRING_ARG);
    insertCommandToMap("client_disallow", "<client_mac> <hostap_mac>",
                       "sends to 'hostap_mac' client disallow request for 'client_mac'",
                       static_cast<pFunction>(&cli_socket::client_disallow_caller), 2, 2,
                       STRING_ARG, STRING_ARG);
    insertCommandToMap("client_disconnect", "<client_mac> [<type> <reason>]",
                       "sends client disconnect request for 'client_mac'. type=2 is "
                       "deauthenticate, all other options are disassociate",
                       static_cast<pFunction>(&cli_socket::client_disconnect_caller), 1, 3,
                       STRING_ARG, INT_ARG, INT_ARG);
    insertCommandToMap("hostap_stats_measurement", "[<ap_mac>]",
                       "initiate a load measurement request for 'hostap_mac' if given as input, "
                       "otherwise for all hostap's",
                       static_cast<pFunction>(&cli_socket::hostap_stats_measurement_caller), 0, 1,
                       STRING_ARG);
    insertCommandToMap(
        "load_balancer_task", "<ap_mac>", "initiate a load balancing task for the given 'ap mac'",
        static_cast<pFunction>(&cli_socket::load_balancer_task_caller), 1, 1, STRING_ARG);
    insertCommandToMap("ire_network_optimization_task", "",
                       "starts an IRE network optimization task",
                       static_cast<pFunction>(&cli_socket::ire_network_optimization_task_caller), 0,
                       0, STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "client_channel_load_11k_req", "<hostap_mac> <client_mac> <channel>",
        "sends to 'hostap_mac' 11k channel load request for 'client_mac' on 'channel' number",
        static_cast<pFunction>(&cli_socket::client_channel_load_11k_req_caller), 2, 3, STRING_ARG,
        STRING_ARG, INT_ARG);
    insertCommandToMap("client_beacon_11k_req", "<sta_mac> <[params]>",
                       "sends beacon request to 'sta_mac' with 'params' = (bssid, ch, ssid, "
                       "duration, rand_ival, repeats, op_class, mode)=value",
                       static_cast<pFunction>(&cli_socket::client_beacon_11k_req_caller), 1, 9,
                       STRING_ARG, STRING_ARG, STRING_ARG, STRING_ARG, STRING_ARG, STRING_ARG,
                       STRING_ARG, STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "client_statistics_11k_req", "<hostap_mac> <client_mac> <group_identity> <peer_mac>",
        "sends statistics 11k request to 'client_mac', with 'group_identity', about 'peer_mac'",
        static_cast<pFunction>(&cli_socket::client_statistics_11k_req_caller), 3, 4, STRING_ARG,
        STRING_ARG, INT_ARG, STRING_ARG);
    insertCommandToMap("client_link_measurement_11k_req", "<hostap_mac> <client_mac>",
                       "sends link measurement 11k request for 'client_mac'",
                       static_cast<pFunction>(&cli_socket::client_link_measurement_11k_req_caller),
                       2, 2, STRING_ARG, STRING_ARG);
    insertCommandToMap(
        "ap_neighbor_11k_set", "<hostap_mac> <bssid> <channel> <vap_id>",
        "add 'bssid' with 'channel' to 11k neighbor list of 'hostap_mac' with 'vap_id'",
        static_cast<pFunction>(&cli_socket::set_neighbor_11k_caller), 4, 4, STRING_ARG, STRING_ARG,
        INT_ARG);
    insertCommandToMap("ap_neighbor_11k_rm", "<hostap_mac> <bssid> <vap_id>",
                       "remove 'bssid' from  11k neighbor list of 'hostap_mac' 'vap_id'",
                       static_cast<pFunction>(&cli_socket::rm_neighbor_11k_caller), 3, 3,
                       STRING_ARG, STRING_ARG);
    insertCommandToMap("ping_slave", "<ire_mac> [<num_of_req>] [<packet_size>]",
                       "send ping request to slave mac, num_of_req times (1 time by default), with "
                       "size = 'packet_size' (0 by default) ",
                       static_cast<pFunction>(&cli_socket::ping_slave_caller), 1, 3, STRING_ARG,
                       INT_ARG, INT_ARG);
    insertCommandToMap("ping_all_slaves", "[<num_of_req>] [<packet_size>]",
                       "send ping request to all slaves, num_of_req times each (1 time by "
                       "default), with size = 'packet_size' (0 by default) ",
                       static_cast<pFunction>(&cli_socket::ping_all_slaves_caller), 0, 2, INT_ARG,
                       INT_ARG);
}

bool cli_socket::waitResponseReady()
{
    int t = 0, timeout_msec = 5000;
    while (wait_response && (t < timeout_msec)) {
        UTILS_SLEEP_MSEC(100);
        t += 100;
    }
    if (wait_response) {
        wait_response = false;
        std::cout << "did not get response, timeout reached." << std::endl;
        return false;
    }
    return true;
}

bool cli_socket::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header == nullptr) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    bool success = true;
    // LOG(DEBUG) << "CLI handleMessage: action=" << int(beerocks_header->action()) << " action_op=" << int(beerocks_header->action_op());

    if (beerocks_header->action() != beerocks_message::ACTION_CLI) {
        LOG(ERROR) << "bad action: " << int(beerocks_header->action());
        return false;
    }

    switch (beerocks_header->action_op()) {

    case beerocks_message::ACTION_CLI_RESPONSE_STR: {
        auto response = beerocks_header->addClass<beerocks_message::cACTION_CLI_RESPONSE_STR>();
        if (!response) {
            LOG(ERROR) << "addClass cACTION_CLI_RESPONSE_STR failed";
            return false;
        }

        auto buffer = response->buffer(0);
        if (!buffer) {
            LOG(ERROR) << "get buffer has failed";
            return false;
        }
        std::cout << buffer << std::endl;
        std::cout.flush();
        wait_response = false;
        break;
    }
    case beerocks_message::ACTION_CLI_RESPONSE_INT: {
        auto notification = beerocks_header->addClass<beerocks_message::cACTION_CLI_RESPONSE_INT>();
        if (!notification) {
            LOG(ERROR) << "addClass cACTION_CLI_RESPONSE_INT failed";
            return false;
        }
        if (!notification->isOK()) {
            LOG(ERROR) << "bad response";
            success = false;
        } else {
            if (notification->currentValue() != -1) {
                std::cout << "Current value is: " << (int)(notification->currentValue())
                          << std::endl;
                ;
            }
        }
        wait_response = false;
        break;
    }
    default: {
        LOG(ERROR) << "ACTION_CLI, action_op " << int(beerocks_header->action_op())
                   << " not supported.";
        success = false;
        break;
    }
    }
    return success;
}

//
// Caller functions
//

int cli_socket::enable_debug_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_debug();
    return enable_debug(args.intArgs[0]);
}

int cli_socket::set_stop_on_failure_attempts_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return set_stop_on_failure_attempts();
    return set_stop_on_failure_attempts(args.intArgs[0]);
}

int cli_socket::enable_diagnostics_measurements_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return enable_diagnostics_measurements();
    return enable_diagnostics_measurements(args.intArgs[0]);
}

int cli_socket::dump_node_info_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return dump_node_info(args.stringArgs[0]);
}

int cli_socket::cross_rx_rssi_measurement_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return cross_rx_rssi_measurement(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
}

int cli_socket::steer_client_caller(int numOfArgs)
{
    if (numOfArgs == 2)
        return steer_client(args.stringArgs[0], args.stringArgs[1]);
    else if (numOfArgs == 3)
        return steer_client(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    else
        return -1;
}

int cli_socket::steer_ire_caller(int numOfArgs)
{
    if (numOfArgs < 2)
        return -1;
    return steer_ire(args.stringArgs[0], args.stringArgs[1]);
}

int cli_socket::optimal_path_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return optimal_path(args.stringArgs[0]);
}

int cli_socket::ap_channel_switch_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return ap_channel_switch(args.stringArgs[0], args.intArgs[1]);
    } else if (numOfArgs == 5) {
        return ap_channel_switch(args.stringArgs[0], args.intArgs[1], args.intArgs[2],
                                 args.intArgs[3], args.intArgs[4]);
    } else
        return -1;
}

int cli_socket::client_allow_caller(int numOfArgs)
{
    if (numOfArgs != 2)
        return -1;
    return client_allow(args.stringArgs[0], args.stringArgs[1]);
}

int cli_socket::client_disallow_caller(int numOfArgs)
{
    if (numOfArgs != 2)
        return -1;
    return client_disallow(args.stringArgs[0], args.stringArgs[1]);
}

int cli_socket::client_disconnect_caller(int numOfArgs)
{
    if (numOfArgs == 1) {
        return client_disconnect(args.stringArgs[0]);
    } else if (numOfArgs == 3) {
        return client_disconnect(args.stringArgs[0], args.intArgs[1], args.intArgs[2]);
    } else
        return -1;
}

int cli_socket::hostap_stats_measurement_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 0)
        return hostap_stats_measurement();
    return hostap_stats_measurement(args.stringArgs[0]);
}

int cli_socket::load_balancer_task_caller(int numOfArgs)
{
    if (numOfArgs < 1)
        return -1;
    return load_balancer_task(args.stringArgs[0]);
}

int cli_socket::ire_network_optimization_task_caller(int numOfArgs)
{
    return ire_network_optimization_task();
}

int cli_socket::client_channel_load_11k_req_caller(int numOfArgs)
{
    if (numOfArgs == 3) {
        return client_channel_load_11k_req(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    } else if (numOfArgs == 2) {
        return client_channel_load_11k_req(args.stringArgs[0], args.stringArgs[1]);
    } else
        return -1;
}

int cli_socket::client_beacon_11k_req_caller(int numOfArgs)
{

    std::string bssid(network_utils::WILD_MAC_STRING);
    std::string ssid;
    uint8_t ch         = beerocks::BEACON_MEASURE_DEFAULT_CHANNEL_ALL_CHANNELS;
    uint16_t duration  = beerocks::BEACON_MEASURE_DEFAULT_ACTIVE_DURATION;
    uint16_t rand_ival = beerocks::BEACON_MEASURE_DEFAULT_RANDOMIZATION_INTERVAL;
    uint16_t repeats   = beerocks::BEACON_MEASURE_DEFAULT_REPEATS;
    int16_t op_class   = beerocks::BEACON_MEASURE_DEFAULT_AUTO_OPERATION_CODE;
    std::string mode("active");
    std::string::size_type pos;
    //[bssid=<bssid> ch=<channel> ssid='<ssid>']"
    for (int i = 1; i < numOfArgs; i++) { //first optional arg
        if ((pos = args.stringArgs[i].find("bssid=")) != std::string::npos) {
            bssid = args.stringArgs[i].substr(pos + sizeof("bssid"));
        } else if ((pos = args.stringArgs[i].find("ch=")) != std::string::npos) {
            ch = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("ch")));
        } else if ((pos = args.stringArgs[i].find("ssid=")) != std::string::npos) {
            ssid = args.stringArgs[i].substr(pos + sizeof("ssid"));
        } else if ((pos = args.stringArgs[i].find("duration=")) != std::string::npos) {
            duration = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("duration")));
        } else if ((pos = args.stringArgs[i].find("rand_ival=")) != std::string::npos) {
            rand_ival = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("rand_ival")));
        } else if ((pos = args.stringArgs[i].find("repeats=")) != std::string::npos) {
            repeats = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("repeats")));
        } else if ((pos = args.stringArgs[i].find("op_class=")) != std::string::npos) {
            op_class = string_utils::stoi(args.stringArgs[i].substr(pos + sizeof("op_class")));
        } else if ((pos = args.stringArgs[i].find("mode=")) != std::string::npos) {
            mode = args.stringArgs[i].substr(pos + sizeof("mode"));
        }
    }

    if (numOfArgs > 1)
        return client_beacon_11k_req(args.stringArgs[0], bssid, ch, ssid, duration, rand_ival,
                                     repeats, op_class, mode);
    else
        return -1;
}

int cli_socket::client_statistics_11k_req_caller(int numOfArgs)
{
    if (numOfArgs == 4) {
        return client_statistics_11k_req(args.stringArgs[0], args.stringArgs[1], args.intArgs[2],
                                         args.stringArgs[3]);
    } else if (numOfArgs == 3) {
        return client_statistics_11k_req(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    } else
        return -1;
}

int cli_socket::client_link_measurement_11k_req_caller(int numOfArgs)
{
    if (numOfArgs != 2)
        return -1;
    return client_link_measurement_11k_req(args.stringArgs[0], args.stringArgs[1]);
}

int cli_socket::set_neighbor_11k_caller(int numOfArgs)
{
    if (numOfArgs == 4) {
        return set_neighbor_11k(args.stringArgs[0], args.stringArgs[1], args.intArgs[2],
                                args.intArgs[3]);
    }
    return -1;
}

int cli_socket::rm_neighbor_11k_caller(int numOfArgs)
{
    if (numOfArgs == 2) {
        return rm_neighbor_11k(args.stringArgs[0], args.stringArgs[1], args.intArgs[2]);
    }
    return -1;
}

int cli_socket::ping_slave_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 3)
        return ping_slave(args.stringArgs[0], args.intArgs[1], args.intArgs[2]);
    else if (numOfArgs == 2)
        return ping_slave(args.stringArgs[0], args.intArgs[1]);
    return ping_slave(args.stringArgs[0]);
}

int cli_socket::ping_all_slaves_caller(int numOfArgs)
{
    if (numOfArgs < 0)
        return -1;
    else if (numOfArgs == 2)
        return ping_all_slaves(args.intArgs[0], args.intArgs[1]);
    else if (numOfArgs == 1)
        return ping_all_slaves(args.intArgs[0]);
    return ping_all_slaves();
}

//
// Functions
//

int cli_socket::enable_debug(int8_t isEnable)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_ENABLE_DEBUG>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_ENABLE_DEBUG message!";
        return -1;
    }
    request->isEnable() = isEnable;

    wait_response = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::set_stop_on_failure_attempts(int32_t attempts)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS message!";
        return -1;
    }
    request->attempts() = attempts;

    wait_response = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::enable_diagnostics_measurements(int8_t isEnable)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS message!";
        return -1;
    }
    request->isEnable() = isEnable;
    wait_response       = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::dump_node_info(std::string mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_DUMP_NODE_INFO>(cmdu_tx);

    request->mac() = network_utils::mac_from_string(mac);
    wait_response  = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::cross_rx_rssi_measurement(std::string client_mac, std::string hostap_mac,
                                          uint16_t center_frequency)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT message!";
        return -1;
    }
    request->client_mac()       = network_utils::mac_from_string(client_mac);
    request->hostap_mac()       = network_utils::mac_from_string(hostap_mac);
    request->center_frequency() = center_frequency;
    wait_response               = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::steer_client(std::string client_mac, std::string bssid, int disassoc_timer_ms)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CLIENT_BSS_STEER_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_BSS_STEER_REQUEST message!";
        return -1;
    }
    request->client_mac()        = network_utils::mac_from_string(client_mac);
    request->bssid()             = network_utils::mac_from_string(bssid);
    request->disassoc_timer_ms() = disassoc_timer_ms;
    wait_response                = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::steer_ire(std::string client_mac, std::string bssid)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_BACKHAUL_ROAM_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_BACKHAUL_ROAM_REQUEST message!";
        return -1;
    }
    request->slave_mac() = network_utils::mac_from_string(client_mac);
    request->bssid()     = network_utils::mac_from_string(bssid);
    wait_response        = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::optimal_path(std::string client_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_OPTIMAL_PATH_TASK>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_OPTIMAL_PATH_TASK message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    wait_response         = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::ap_channel_switch(std::string ap_mac, uint8_t channel, uint8_t bw,
                                  uint8_t channel_ext_above_secondary,
                                  uint16_t vht_center_frequency)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST message!";
        return -1;
    }
    request->mac()                 = network_utils::mac_from_string(ap_mac);
    request->cs_params().channel   = channel;
    request->cs_params().bandwidth = utils::convert_bandwidth_to_enum(bw);
    request->cs_params().vht_center_frequency =
        vht_center_frequency ? vht_center_frequency
                             : son::wireless_utils::channel_to_vht_center_freq(
                                   channel, bw, channel_ext_above_secondary);
    wait_response = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_allow(std::string client_mac, std::string hostap_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CLIENT_ALLOW_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_ALLOW_REQUEST message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    request->hostap_mac() = network_utils::mac_from_string(hostap_mac);
    wait_response         = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_disallow(std::string client_mac, std::string hostap_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CLIENT_DISALLOW_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_DISALLOW_REQUEST message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    request->hostap_mac() = network_utils::mac_from_string(hostap_mac);
    wait_response         = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_disconnect(std::string client_mac, uint32_t type, uint32_t reason)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CLIENT_DISCONNECT_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_DISCONNECT_REQUEST message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    request->type()       = beerocks_message::eDisconnectType(type);
    request->reason()     = reason;
    wait_response         = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::hostap_stats_measurement(std::string ap_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_HOSTAP_STATS_MEASUREMENT>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_HOSTAP_STATS_MEASUREMENT message!";
        return -1;
    }
    request->ap_mac() = network_utils::mac_from_string(ap_mac);
    wait_response     = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::load_balancer_task(std::string ap_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_LOAD_BALANCER_TASK>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_LOAD_BALANCER_TASK message!";
        return -1;
    }
    request->ap_mac() = network_utils::mac_from_string(ap_mac);
    wait_response     = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::ire_network_optimization_task()
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK message!";
        return -1;
    }
    wait_response = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_channel_load_11k_req(std::string hostap_mac, std::string client_mac,
                                            int channel)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    request->hostap_mac() = network_utils::mac_from_string(hostap_mac);
    request->channel()    = channel;
    wait_response         = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_beacon_11k_req(std::string client_mac, std::string bssid, uint8_t channel,
                                      std::string ssid, uint16_t duration, uint16_t rand_ival,
                                      uint16_t repeats, int16_t op_class, std::string mode)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CLIENT_BEACON_11K_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_BEACON_11K_REQUEST message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    request->bssid()      = network_utils::mac_from_string(bssid);
    request->channel()    = channel;
    request->duration()   = duration;
    request->rand_ival()  = rand_ival;
    request->repeats()    = repeats;
    request->op_class()   = op_class;
    if (mode == "passive")
        request->measurement_mode() = beerocks::MEASURE_MODE_PASSIVE;
    else if (mode == "table")
        request->measurement_mode() = beerocks::MEASURE_MODE_TABLE;
    else //deafult "active"
        request->measurement_mode() = beerocks::MEASURE_MODE_ACTIVE;

    if (!ssid.empty()) {
        request->use_optional_ssid() = true;
        string_utils::copy_string((char *)request->ssid(), ssid.c_str(),
                                  message::WIFI_SSID_MAX_LENGTH);
    }
    wait_response = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_statistics_11k_req(std::string hostap_mac, std::string client_mac,
                                          uint8_t group_identity, std::string peer_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST message!";
        return -1;
    }
    request->client_mac()     = network_utils::mac_from_string(client_mac);
    request->hostap_mac()     = network_utils::mac_from_string(hostap_mac);
    request->peer_mac()       = network_utils::mac_from_string(peer_mac);
    request->group_identity() = group_identity;
    wait_response             = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::client_link_measurement_11k_req(std::string hostap_mac, std::string client_mac)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST message!";
        return -1;
    }
    request->client_mac() = network_utils::mac_from_string(client_mac);
    request->hostap_mac() = network_utils::mac_from_string(hostap_mac);

    wait_response = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::set_neighbor_11k(std::string ap_mac, std::string bssid, uint8_t channel,
                                 int8_t vap_id)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST message!";
        return -1;
    }
    request->ap_mac()  = network_utils::mac_from_string(ap_mac);
    request->bssid()   = network_utils::mac_from_string(bssid);
    request->channel() = channel;
    request->vap_id()  = vap_id;
    wait_response      = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::rm_neighbor_11k(std::string ap_mac, std::string bssid, int8_t vap_id)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST message!";
        return -1;
    }
    request->ap_mac() = network_utils::mac_from_string(ap_mac);
    request->bssid()  = network_utils::mac_from_string(bssid);
    request->vap_id() = vap_id;
    wait_response     = true;
    message_com::send_cmdu(master_socket, cmdu_tx);
    waitResponseReady();
    return 0;
}

int cli_socket::ping_slave(std::string ire_mac, int num_of_req, int ping_size)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_PING_SLAVE_REQUEST>(cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_PING_SLAVE_REQUEST message!";
        return -1;
    }
    request->mac()        = network_utils::mac_from_string(ire_mac);
    request->num_of_req() = (uint16_t)num_of_req;
    request->size()       = (uint16_t)ping_size;

    message_com::send_cmdu(master_socket, cmdu_tx);

    return 0;
}

int cli_socket::ping_all_slaves(int num_of_req, int ping_size)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_CLI_PING_ALL_SLAVES_REQUEST>(
            cmdu_tx);
    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_CLI_PING_ALL_SLAVES_REQUEST message!";
        return -1;
    }
    request->num_of_req() = (uint16_t)num_of_req;
    request->size()       = (uint16_t)ping_size;

    message_com::send_cmdu(master_socket, cmdu_tx);

    return 0;
}

} // namespace beerocks
