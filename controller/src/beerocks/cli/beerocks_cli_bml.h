/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_CLI_BML_H
#define _BEEROCKS_CLI_BML_H

#define PRINT_BUFFER_LENGTH 4096
#include "beerocks_cli.h"
#include "bml.h"
#ifdef BEEROCKS_RDKB
#include "rdkb/bml_rdkb.h"
#endif

#include <bcl/network/network_utils.h>

#include <list>
#include <unordered_map>

namespace beerocks {
class cli_bml : public cli {

public:
    struct conn_map_node_t {
        uint8_t type;
        uint8_t state;
        uint8_t channel;
        uint8_t bw;
        uint8_t channel_ext_above_secondary;
        int8_t rx_rssi;
        std::string mac;
        std::string ip_v4;
        std::string name;

        struct gw_ire_t {

            std::string backhaul_mac;

            struct radio_t {
                uint8_t channel;
                uint8_t cac_completed;
                uint8_t bw;
                uint8_t channel_ext_above_secondary;
                uint8_t ch_load;
                std::string radio_identifier;
                std::string radio_mac;
                std::string ifname;

                struct vap_t {
                    std::string ssid;
                    std::string bssid;
                    bool backhaul_vap;
                };

                std::list<std::shared_ptr<vap_t>> vap;
            };

            std::list<std::shared_ptr<radio_t>> radio;

        } gw_ire;
    };

    cli_bml(std::string beerocks_conf_path_);
    virtual ~cli_bml();

    bool connect();
    void disconnect();
    bool is_connected();
    bool start() { return true; };
    void stop(){};
    void print_help(bool print_header = true) { help(print_header); }
    int get_onboarding_status();
    bool is_pending_response();

    int analyzer_init(std::string remote_pc_ip);

private:
    // Help functions
    void setFunctionsMapAndArray();
    void printBmlReturnVals(const std::string &func_name, int ret_val);

    static void map_query_cb(const struct BML_NODE_ITER *node_iter, bool to_console);
    static void connection_map_cb(const struct BML_NODE_ITER *node_iter, bool to_console);
    static void map_query_to_console_cb(const struct BML_NODE_ITER *node_iter);
    static void connection_map_to_console_cb(const struct BML_NODE_ITER *node_iter);
    static void map_query_to_socket_cb(const struct BML_NODE_ITER *node_iter);
    static void map_update_cb(const struct BML_NODE_ITER *node_iter, bool to_console);
    static void map_update_to_console_cb(const struct BML_NODE_ITER *node_iter);
    static void map_update_to_socket_cb(const struct BML_NODE_ITER *node_iter);
    static void stats_update_cb(const struct BML_STATS_ITER *stats_iter, bool to_console);
    static void stats_update_to_console_cb(const struct BML_STATS_ITER *stats_iter);
    static void stats_update_to_socket_cb(const struct BML_STATS_ITER *stats_iter);
    static void events_update_cb(const struct BML_EVENT *event, bool to_console);
    static void events_update_to_console_cb(const struct BML_EVENT *event);
    static void events_update_to_socket_cb(const struct BML_EVENT *event);

    // Caller functions
    int connect_caller(int numOfArgs);
    int onboard_status_caller(int numOfArgs);
    int ping_caller(int numOfArgs);
    int nw_map_register_update_cb_caller(int numOfArgs);
    int nw_map_query_caller(int numOfArgs);
    int bml_connection_map_caller(int numOfArgs);
    int stat_register_cb_caller(int numOfArgs);
    int events_register_cb_caller(int numOfArgs);
    int set_wifi_credentials_caller(int numOfArgs);
    int get_wifi_credentials_caller(int numOfArgs);
    int set_onboarding_state_caller(int numOfArgs);
    int get_onboarding_state_caller(int numOfArgs);
    int wps_onboarding_caller(int numOfArgs);

    int get_device_info_caller(int numOfArgs);

    int get_bml_version_caller(int numOfArgs);
    int get_master_slave_versions_caller(int numOfArgs);

    int enable_legacy_client_roaming_caller(int numOfArgs);
    int enable_client_roaming_caller(int numOfArgs);
    int enable_client_roaming_prefer_signal_strength_caller(int numOfArgs);
    int enable_client_band_steering_caller(int numOfArgs);
    int enable_ire_roaming_caller(int numOfArgs);
    int enable_load_balancer_caller(int numOfArgs);
    int enable_service_fairness_caller(int numOfArgs);
    int enable_dfs_reentry_caller(int numOfArgs);
    int enable_certification_mode_caller(int numOfArgs);
    int set_log_level_caller(int numOfArgs);
    int set_global_restricted_channels_caller(int numOfArgs);
    int get_global_restricted_channels_caller(int numOfArgs);
    int set_slave_restricted_channels_caller(int numOfArgs);
    int get_slave_restricted_channels_caller(int numOfArgs);
    int bml_trigger_topology_discovery_caller(int numOfArgs);
    int bml_channel_selection_caller(int numOfArgs);
#ifdef BEEROCKS_RDKB
    int bml_rdkb_steering_set_group_caller(int numOfArgs);
    int bml_rdkb_steering_client_set_caller(int numOfArgs);
    int bml_rdkb_steering_event_register_caller(int numOfArgs);
    int bml_rdkb_steering_client_measure_caller(int numOfArgs);
#endif
    int set_dcs_continuous_scan_enable_caller(int numOfArgs);
    int get_dcs_continuous_scan_enable_caller(int numOfArgs);
    int set_dcs_continuous_scan_params_caller(int numOfArgs);
    int get_dcs_continuous_scan_params_caller(int numOfArgs);
    int start_dcs_single_scan_caller(int numOfArgs);
    int get_dcs_scan_results_caller(int numOfArgs);
    // Functions
    int onboard_status();
    int ping();
    int nw_map_register_update_cb(const std::string &optional = std::string());
    int nw_map_query();
    int connection_map();
    int stat_register_cb(const std::string &optional = std::string());
    int events_register_cb(const std::string &optional = std::string());
    int set_wifi_credentials(const std::string &ssid, const std::string &pass, int sec = 4,
                             int vap_id = 0, int force = 1);
    int get_wifi_credentials(int vap_id = 0);
    int set_onboarding_state(int enable);
    int get_onboarding_state();
    int wps_onboarding(const std::string iface = std::string());

    int get_device_info();
    int get_bml_version();
    int get_master_slave_versions();

    int client_allow(const std::string &client_mac, const std::string &hostap_mac);
    int client_disallow(const std::string &client_mac, const std::string &hostap_mac);

    int enable_legacy_client_roaming(int8_t isEnable = -1);
    int enable_client_roaming(int8_t isEnable = -1);
    int enable_client_roaming_prefer_signal_strength(int8_t isEnable = -1);
    int enable_client_band_steering(int8_t isEnable = -1);
    int enable_ire_roaming(int8_t isEnable = -1);
    int enable_load_balancer(int8_t isEnable = -1);
    int enable_service_fairness(int8_t isEnable = -1);
    int enable_dfs_reentry(int8_t isEnable = -1);
    int enable_certification_mode(int8_t isEnable = -1);
    int set_log_level(const std::string &module_name, const std::string &log_level, uint8_t on,
                      const std::string &mac = net::network_utils::WILD_MAC_STRING);
    int set_global_restricted_channels(const std::string &restricted_channels);
    int get_global_restricted_channels();
    int set_slave_restricted_channels(const std::string &restricted_channels,
                                      const std::string &hostap_mac);
    int get_slave_restricted_channels(const std::string &hostap_mac);
    int topology_discovery(const std::string &al_mac);
    int channel_selection(const std::string &al_mac, const std::string &ruid);
#ifdef BEEROCKS_RDKB
    int steering_set_group(uint32_t steeringGroupIndex, const std::string &str_cfg_2,
                           const std::string &str_cfg_5);
    int steering_client_set(uint32_t steeringGroupIndex, const std::string &str_bssid,
                            const std::string &str_client_mac,
                            const std::string &str_config = std::string());
    int steering_event_register(const std::string &optional = std::string());
    int steering_client_measure(uint32_t steeringGroupIndex, const std::string &str_bssid,
                                const std::string &str_client_mac);
#endif
    int set_dcs_continuous_scan_enable(const std::string &radio_mac, int8_t enable);
    int get_dcs_continuous_scan_enable(const std::string &radio_mac);
    int set_dcs_continuous_scan_params(const std::string &radio_mac, int32_t dwell_time,
                                       int32_t interval_time, const std::string &channel_pool);
    int get_dcs_continuous_scan_params(const std::string &radio_mac);
    int start_dcs_single_scan(const std::string &radio_mac, int32_t dwell_time,
                              const std::string &channel_pool);
    int get_dcs_scan_results(const std::string &radio_mac, uint32_t max_results_size,
                             bool is_single_scan = false);
    template <typename T>
    const std::string string_from_int_array(T *arr, size_t arr_max_size);
    // Variable
    std::string beerocks_conf_path;
    BML_CTX ctx = nullptr;
    char print_buffer[PRINT_BUFFER_LENGTH];
    int is_onboarding;
    bool pending_response = false;

    bool is_analyzer = false;
    static SocketClient *m_analyzer_socket;
    SocketSelect select;
    std::unordered_multimap<std::string, std::shared_ptr<conn_map_node_t>>
        conn_map_nodes; // key=parent_mac, val=conn_map_node_t

    uint8_t rx_buffer[message::MESSAGE_BUFFER_LENGTH];
};
} // namespace beerocks

#endif // _BEEROCKS_CLI_BML_H
