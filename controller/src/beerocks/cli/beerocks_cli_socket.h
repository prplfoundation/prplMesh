/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_CLI_SOCKET_H
#define _BEEROCKS_CLI_SOCKET_H

#include "beerocks_cli.h"

#include <bcl/beerocks_socket_thread.h>
#include <bcl/network/network_utils.h>

namespace beerocks {

class cli_socket : public socket_thread, public cli {
public:
    cli_socket(std::string temp_path_, std::string proxy_ip_ = std::string());
    ~cli_socket();

    bool connect() override;
    void disconnect() override;
    bool is_connected() override { return (master_socket != nullptr); }
    bool start() override { return socket_thread::start(); }
    void stop() override { socket_thread::stop(); }
    void print_help(bool print_header = true) { help(print_header); }

protected:
    virtual bool init() override;
    virtual void on_thread_stop() override;
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual bool socket_disconnected(Socket *sd) override;

private:
    void setFunctionsMapAndArray() override;

    bool waitResponseReady();

    int getResponse();

    // Caller functions

    int enable_debug_caller(int numOfArgs);

    int set_stop_on_failure_attempts_caller(int numOfArgs);

    int enable_diagnostics_measurements_caller(int numOfArgs);

    int dump_node_info_caller(int numOfArgs);

    int cross_rx_rssi_measurement_caller(int numOfArgs);

    int steer_client_caller(int numOfArgs);

    int steer_ire_caller(int numOfArgs);

    int optimal_path_caller(int numOfArgs);

    int ap_channel_switch_caller(int numOfArgs);

    int client_allow_caller(int numOfArgs);

    int client_disallow_caller(int numOfArgs);

    int client_disconnect_caller(int numOfArgs);

    int hostap_stats_measurement_caller(int numOfArgs);

    int load_balancer_task_caller(int numOfArgs);

    int ire_network_optimization_task_caller(int numOfArgs);

    int ping_slave_caller(int numOfArgs);

    int ping_all_slaves_caller(int numOfArgs);

    int rm_neighbor_11k_caller(int numOfArgs);

    int set_neighbor_11k_caller(int numOfArgs);

    int client_beacon_11k_req_caller(int numOfArgs);

    int client_channel_load_11k_req_caller(int numOfArgs);

    int client_link_measurement_11k_req_caller(int numOfArgs);

    int client_statistics_11k_req_caller(int numOfArgs);

    // Functions

    int enable_debug(int8_t isEnable = -1);

    int set_stop_on_failure_attempts(int32_t attempts = -1);

    int enable_diagnostics_measurements(int8_t isEnable = -1);

    int dump_node_info(std::string mac);

    int cross_rx_rssi_measurement(std::string client_mac, std::string hostap_mac,
                                  uint16_t center_frequency = 0);

    int steer_client(std::string client_mac, std::string bssid,
                     int disassoc_timer_ms = beerocks::BSS_STEER_DISASSOC_TIMER_MS);

    int steer_ire(std::string client_mac, std::string bssid);

    int hostap_stats_measurement(std::string ap_mac = std::string());

    int load_balancer_task(std::string ap_mac);

    int optimal_path(std::string client_mac);

    int ap_channel_switch(std::string ap_mac, uint8_t channel, uint8_t bw = 0,
                          uint8_t channel_ext_above = 1, uint16_t vht_center_frequency = 0);

    int client_allow(std::string client_mac, std::string hostap_mac);

    int client_disallow(std::string client_mac, std::string hostap_mac);

    int client_disconnect(std::string client_mac, uint32_t type = 0, uint32_t reason = 0);

    int ire_network_optimization_task();

    int ping_slave(std::string ire_mac, int num_of_req = 1, int ping_size = 0);

    int ping_all_slaves(int num_of_req = 1, int ping_size = 0);

    int rm_neighbor_11k(std::string ap_mac, std::string bssid, int8_t vap_id);

    int set_neighbor_11k(std::string ap_mac, std::string bssid, uint8_t channel, int8_t vap_id);

    int client_beacon_11k_req(std::string client_mac, std::string bssid, uint8_t channel,
                              std::string ssid, uint16_t duration, uint16_t rand_ival,
                              uint16_t repeats, int16_t op_class, std::string mode);

    int client_channel_load_11k_req(std::string hostap_mac, std::string client_mac,
                                    int channel = 0);

    int client_link_measurement_11k_req(std::string hostap_mac, std::string client_mac);

    int client_statistics_11k_req(std::string hostap_mac, std::string client_mac,
                                  uint8_t group_identity,
                                  std::string peer_mac = net::network_utils::WILD_MAC_STRING);

    // Variables
    std::string temp_path;
    SocketClient *master_socket = nullptr;

    bool wait_dump_node_info;
    bool wait_response;
    std::string wait_dump_node_info_address;
    std::string proxy_ip;
};

} // namespace beerocks

#endif // _BEEROCKS_CLI_SOCKET_H
