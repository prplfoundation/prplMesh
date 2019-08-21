/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _DB_H_
#define _DB_H_

#include "node.h"

#include <beerocks/bcl/beerocks_defines.h>
#include <beerocks/bcl/beerocks_logging.h>

#include <mutex>
#include <queue>

namespace son {
class db {

    /*
        * none of the functions are thread-safe
        * code that uses database should be wrapped with calls 
        * to lock() and unlock()
        */

    typedef struct {
        Socket *sd;
        bool map_updates;
        bool stats_updates;
        bool events_updates;
    } sBmlListener;

public:
    // VAPs info list type
    typedef std::list<std::shared_ptr<beerocks_message::sConfigVapInfo>> vaps_list_t;

    typedef struct {
        std::string gw_ip;
        std::string gw_netmask;
        std::string ire_ip_range_low;
        std::string ire_ip_range_high;
        std::vector<uint8_t> global_restricted_channels;
        int diagnostics_measurements_polling_rate_sec;
        int ire_rssi_report_rate_sec;
        bool load_dfs_reentry;
        bool load_rdkb_extensions;
        bool load_client_band_steering;
        bool load_client_optimal_path_roaming;
        bool load_client_11k_roaming;
        bool load_legacy_client_roaming;
        bool load_ire_roaming;
        bool load_load_balancing;
        bool load_service_fairness;
        bool load_diagnostics_measurements;
        bool load_backhaul_measurements;
        bool load_front_measurements;
        bool load_health_check;
        bool load_monitor_on_vaps;
        bool certification_mode;
        int roaming_5ghz_failed_attemps_threshold;
        int roaming_24ghz_failed_attemps_threshold;
        int roaming_11v_failed_attemps_threshold;
        int roaming_hysteresis_percent_bonus;
        int roaming_unconnected_client_rssi_compensation_db;
        int roaming_hop_percent_penalty;
        int roaming_band_pathloss_delta_db;
        int roaming_rssi_cutoff_db;
        int monitor_total_ch_load_notification_hi_th_percent;
        int monitor_total_ch_load_notification_lo_th_percent;
        int monitor_total_ch_load_notification_delta_th_percent;
        int monitor_min_active_clients;
        int monitor_active_client_th;
        int monitor_client_load_notification_delta_th_percent;
        int monitor_rx_rssi_notification_threshold_dbm;
        int monitor_rx_rssi_notification_delta_db;
        int monitor_ap_idle_threshold_B;
        int monitor_ap_active_threshold_B;
        int monitor_ap_idle_stable_time_sec;
        int monitor_disable_initiative_arp;
        int slave_keep_alive_retries;

        int idle_steer_activity_check_timeout;
        int channel_selection_random_delay;
        int fail_safe_5G_frequency;
        int fail_safe_5G_bw;
        int fail_safe_5G_vht_frequency;
        int channel_selection_long_delay;
        int credentials_change_timeout_sec;
        int blacklist_channel_remove_timeout;
        int failed_roaming_counter_threshold;
        int roaming_sticky_client_rssi_threshold;

    } sDbMasterConfig;

    typedef struct {
        // Features
        bool enable_dfs_reentry          = true;
        bool client_band_steering        = true;
        bool client_optimal_path_roaming = true;
        bool client_11k_roaming          = true;
        bool legacy_client_roaming       = true;

        bool ire_roaming = true;

        bool load_balancing = false;

        bool diagnostics_measurements = true;
        bool backhaul_measurements    = true;
        bool front_measurements       = true;
        bool monitor_on_vaps          = true;

        bool health_check = true;

        bool service_fairness = false;

        bool rdkb_extensions = false;

        // Params
        bool client_optimal_path_roaming_prefer_signal_strength = false;
    } sDbMasterSettings;

    db(sDbMasterConfig &config_, beerocks::logging &logger_) : config(config_), logger(logger_)
    {
        settings.enable_dfs_reentry &= config_.load_dfs_reentry;
        settings.client_band_steering &= config_.load_client_band_steering;
        settings.client_optimal_path_roaming &= config_.load_client_optimal_path_roaming;
        settings.client_11k_roaming &= config_.load_client_11k_roaming;
        settings.legacy_client_roaming &= config_.load_legacy_client_roaming;
        settings.ire_roaming &= config_.load_ire_roaming;
        settings.load_balancing &= config_.load_load_balancing;
        settings.diagnostics_measurements &= config_.load_diagnostics_measurements;
        settings.backhaul_measurements &= config_.load_backhaul_measurements;
        settings.front_measurements &= config_.load_front_measurements;
        settings.monitor_on_vaps &= config_.load_monitor_on_vaps;
        settings.health_check &= config_.load_health_check;
        settings.service_fairness &= config_.load_service_fairness;
        settings.rdkb_extensions &= config_.load_rdkb_extensions;
    }
    ~db(){};

    //logger
    void set_log_level_state(const beerocks::eLogLevel &log_level, const bool &new_state);

    // General set/get
    bool has_node(std::string mac);

    bool add_virtual_node(std::string mac, std::string real_node_mac);
    bool add_node(std::string mac, std::string parent_mac = std::string(),
                  beerocks::eType type         = beerocks::TYPE_CLIENT,
                  std::string radio_identifier = std::string());
    bool remove_node(std::string mac);

    bool set_node_type(std::string mac, beerocks::eType type);
    beerocks::eType get_node_type(std::string mac);

    bool set_local_slave_mac(std::string mac);
    std::string get_local_slave_mac();

    bool set_node_ipv4(std::string mac, std::string ipv4);
    std::string get_node_ipv4(std::string mac);

    bool set_node_manufacturer(std::string mac, std::string manufacturer);
    std::string get_node_manufacturer(std::string mac);

    int get_node_channel(std::string mac);

    bool set_node_vap_id(std::string mac, int8_t vap_id);
    int8_t get_node_vap_id(std::string mac);

    bool set_node_beacon_measurement_support_level(
        std::string mac, beerocks::eBeaconMeasurementSupportLevel support_beacon_measurement);
    beerocks::eBeaconMeasurementSupportLevel
    get_node_beacon_measurement_support_level(std::string mac);

    bool set_node_name(std::string mac, std::string name);
    std::string get_node_name(std::string mac);

    bool set_node_state(std::string mac, beerocks::eNodeState state);
    beerocks::eNodeState get_node_state(std::string mac);

    bool set_node_operational_state(std::string bridge_mac, bool operational);
    int8_t get_node_operational_state(std::string bridge_mac);

    std::chrono::steady_clock::time_point get_last_state_change(std::string mac);

    bool set_node_handoff_flag(std::string mac, bool handoff);
    bool get_node_handoff_flag(std::string mac);

    bool set_node_confined_flag(std::string mac, bool flag);
    bool get_node_confined_flag(std::string mac);

    bool update_node_last_seen(std::string mac);

    bool update_node_last_ping_sent(std::string mac);
    std::chrono::steady_clock::time_point get_node_last_seen(std::string mac);
    std::chrono::steady_clock::time_point get_node_last_ping_sent(std::string mac);

    bool update_node_last_ping_received(std::string mac, int seq);
    bool update_node_last_ping_received_avg(std::string mac, int total_seq);
    std::chrono::steady_clock::time_point get_node_last_ping_received(std::string mac);
    int get_node_last_ping_delta_ms(std::string mac);
    int get_node_last_ping_min_ms(std::string mac);
    int get_node_last_ping_max_ms(std::string mac);
    int get_node_last_ping_avg_ms(std::string mac);

    bool set_hostap_active(std::string mac, bool active);
    bool is_hostap_active(std::string mac);

    bool set_hostap_backhaul_manager(std::string mac, bool is_backhaul_manager);
    bool is_hostap_backhaul_manager(std::string mac);
    std::string get_hostap_backhaul_manager(std::string ire);

    bool is_ap_out_of_band(std::string mac, std::string sta_mac);

    bool is_node_wireless(std::string mac);

    Socket *get_node_socket(std::string mac);
    bool set_node_socket(std::string mac, Socket *socket);

    bool disconnected_slave_mac_queue_empty();
    std::string disconnected_slave_mac_queue_pop();
    void disconnected_slave_mac_queue_push(std::string node_mac);
    std::string node_to_string(std::string mac);

    //
    // DB node functions (get only)
    //
    int get_node_hierarchy(std::string mac);
    std::set<std::string> get_nodes(int type = -1);
    std::set<std::string> get_device_nodes();
    std::set<std::string> get_active_hostaps();
    std::set<std::string> get_all_connected_ires();
    std::set<std::string> get_all_backhaul_manager_slaves();
    std::set<std::string> get_nodes_from_hierarchy(int hierarchy, int type = -1);
    std::string get_gw_mac();
    std::set<std::string> get_node_subtree(std::string mac);
    std::string get_node_parent(std::string mac);

    std::string get_node_parent_hostap(std::string mac);
    std::string get_node_previous_parent(std::string mac);
    std::string get_node_parent_ire(std::string mac);
    std::string get_node_parent_backhaul(std::string mac);
    std::set<std::string> get_node_siblings(std::string mac, int type = beerocks::TYPE_ANY);
    std::set<std::string> get_node_children(std::string mac, int type = beerocks::TYPE_ANY,
                                            int state = beerocks::STATE_ANY);
    std::string get_node_key(const std::string &al_mac, const std::string &ruid);

    //
    // Capabilities
    //

    const beerocks::message::sRadioCapabilities *get_station_current_capabilities(std::string mac);

    const beerocks::message::sRadioCapabilities *
    get_station_capabilities(const std::string &client_mac, bool is_bandtype_5ghz);
    bool set_station_capabilities(const std::string &client_mac,
                                  beerocks::message::sRadioCapabilities &sta_cap);

    bool set_hostap_ant_num(std::string mac, beerocks::eWiFiAntNum ant_num);
    beerocks::eWiFiAntNum get_hostap_ant_num(std::string mac);

    bool set_hostap_ant_gain(std::string mac, int ant_gain);
    int get_hostap_ant_gain(std::string mac);

    bool set_hostap_conducted_power(std::string mac, int conducted_power);
    int get_hostap_conducted_power(std::string mac);

    bool set_hostap_supported_channels(std::string mac,
                                       beerocks_message::sWifiChannel *supported_channels,
                                       int length);
    const std::vector<beerocks_message::sWifiChannel>
    get_hostap_supported_channels(std::string mac);

    bool set_hostap_band_capability(std::string mac, beerocks::eRadioBandCapability capability);
    beerocks::eRadioBandCapability get_hostap_band_capability(std::string mac);

    bool capability_check(std::string mac, int channel);

    bool get_node_5ghz_support(
        std::string mac); // TODO: add a real learning algorithm for per-channel support
    bool get_node_24ghz_support(std::string mac);
    bool is_node_5ghz(std::string mac);
    bool is_node_24ghz(std::string mac);
    bool update_node_failed_5ghz_steer_attempt(std::string mac);
    bool update_node_failed_24ghz_steer_attempt(std::string mac);

    bool update_node_11v_responsiveness(std::string mac, bool success);
    bool get_node_11v_capability(std::string mac);

    bool set_hostap_iface_name(std::string mac, std::string iface_name);
    std::string get_hostap_iface_name(std::string mac);

    bool set_hostap_iface_type(std::string mac, beerocks::eIfaceType iface_type);
    beerocks::eIfaceType get_hostap_iface_type(std::string mac);

    bool set_hostap_driver_version(std::string mac, std::string version);
    std::string get_hostap_driver_version(std::string mac);

    bool set_hostap_iface_id(std::string mac, int8_t iface_id);
    int8_t get_hostap_iface_id(std::string mac);

    bool set_hostap_vap_list(const std::string &mac,
                             const std::unordered_map<int8_t, sVapElement> &vap_list);
    std::unordered_map<int8_t, sVapElement> &get_hostap_vap_list(const std::string &mac);
    std::set<std::string> get_hostap_vaps_bssids(const std::string &mac);
    bool remove_vap(const std::string &mac, int vap_id);
    bool add_vap(const std::string &radio_mac, int vap_id, std::string bssid, std::string ssid,
                 bool backhual);

    std::string get_hostap_ssid(const std::string &mac);
    std::string get_hostap_vap_with_ssid(const std::string &mac, const std::string &ssid);
    std::string get_hostap_vap_mac(const std::string &mac, const int vap_id);
    std::string get_node_parent_radio(const std::string &mac);
    int8_t get_hostap_vap_id(const std::string &mac);

    bool get_hostap_advertise_ssid_flag(std::string mac);
    bool set_hostap_advertise_ssid_flag(std::string mac, bool flag);

    bool set_hostap_repeater_mode_flag(std::string mac, bool flag);
    bool get_hostap_repeater_mode_flag(std::string mac);

    bool set_node_backhaul_iface_type(std::string mac, beerocks::eIfaceType iface_type);
    beerocks::eIfaceType get_node_backhaul_iface_type(std::string mac);

    bool set_node_platform(std::string mac, beerocks::ePlatform platform);
    beerocks::ePlatform get_node_platform(std::string mac);

    std::string get_5ghz_sibling_hostap(std::string mac);

    bool set_cs_op_flag(std::string mac, bool flag);
    bool get_cs_op_flag(std::string mac);
    bool set_cs_lb_flag(std::string mac, bool flag);
    bool get_cs_lb_flag(std::string mac);

    bool set_global_restricted_channels(uint8_t *restricted_channels);
    std::vector<uint8_t> get_global_restricted_channels();
    bool set_hostap_conf_restricted_channels(std::string hostap_mac, uint8_t *restricted_channels);
    std::vector<uint8_t> get_hostap_conf_restricted_channels(std::string hostap_mac);

    //
    // CS - DFS
    //
    bool set_hostap_activity_mode(std::string mac, beerocks::eApActiveMode ap_activity_mode);
    beerocks::eApActiveMode get_hostap_activity_mode(std::string mac);
    bool set_radar_hit_stats(std::string mac, uint8_t channel, uint8_t bw, bool is_csa_entry);
    bool set_supported_channel_radar_affected(std::string mac, std::vector<uint8_t> channels,
                                              bool affected);
    //bool get_supported_channel_all_availble(std::string mac );

    bool set_hostap_is_dfs(std::string mac, bool enable);
    bool get_hostap_is_dfs(std::string mac);

    bool set_hostap_cac_completed(std::string mac, bool enable);
    bool get_hostap_cac_completed(std::string mac);

    bool set_hostap_is_on_fail_safe(std::string mac, bool enable);
    bool get_hostap_is_on_fail_safe(std::string mac);

    bool set_hostap_is_on_sub_band(std::string mac, bool enable);
    bool get_hostap_is_on_sub_band(std::string mac);

    bool set_hostap_on_dfs_reentry(std::string mac, bool enable);
    bool get_hostap_on_dfs_reentry(std::string mac);

    bool set_hostap_dfs_reentry_clients(std::string mac, std::set<std::string> dfs_reentry_clients);
    std::set<std::string> get_hostap_dfs_reentry_clients(std::string mac);
    bool clear_hostap_dfs_reentry_clients(std::string mac);

    bool set_hostap_is_acs_enabled(std::string mac, bool enable);
    bool get_hostap_is_acs_enabled(std::string mac);
    //
    // CLI
    //
    void add_cli_socket(Socket *sd);
    void remove_cli_socket(Socket *sd);
    bool get_cli_debug_enable(Socket *sd);
    Socket *get_cli_socket_at(int idx);
    void set_slave_stop_on_failure_attempts(int attempts);
    int get_slave_stop_on_failure_attempts();

    //
    // BML
    //
    void add_bml_socket(Socket *sd);
    void remove_bml_socket(Socket *sd);
    bool get_bml_nw_map_update_enable(Socket *sd);
    bool set_bml_nw_map_update_enable(Socket *sd, bool update_enable);
    bool get_bml_stats_update_enable(Socket *sd);
    bool set_bml_stats_update_enable(Socket *sd, bool update_enable);
    bool get_bml_events_update_enable(Socket *sd);
    bool set_bml_events_update_enable(Socket *sd, bool update_enable);
    Socket *get_bml_socket_at(int idx);
    bool is_bml_listener_exist();

    void set_vap_list(std::shared_ptr<vaps_list_t> vaps_list);
    const std::shared_ptr<vaps_list_t> get_vap_list();
    void clear_vap_list();

    //
    // Measurements
    //
    bool set_node_beacon_measurement(std::string sta_mac, std::string ap_mac, int8_t rcpi,
                                     uint8_t rsni);
    bool get_node_beacon_measurement(std::string sta_mac, std::string ap_mac, int8_t &rcpi,
                                     uint8_t &rsni);

    bool set_node_cross_rx_rssi(std::string sta_mac, std::string ap_mac, int8_t rssi,
                                int8_t rx_packets);
    bool get_node_cross_rx_rssi(std::string sta_mac, std::string ap_mac, int8_t &rssi,
                                int8_t &rx_packets);

    bool set_node_cross_rx_phy_rate_100kb(std::string mac, uint16_t rx_phy_rate_100kb);
    bool set_node_cross_tx_phy_rate_100kb(std::string mac, uint16_t tx_phy_rate_100kb);

    uint16_t get_node_cross_rx_phy_rate_100kb(std::string mac);
    uint16_t get_node_cross_tx_phy_rate_100kb(std::string mac);

    bool clear_node_cross_rssi(std::string sta_mac);

    bool set_node_cross_estimated_tx_phy_rate(std::string mac, double phy_rate);
    double get_node_cross_estimated_tx_phy_rate(std::string mac);

    bool set_hostap_stats_info(std::string mac, beerocks_message::sApStatsParams *params);
    void clear_hostap_stats_info(std::string mac);
    bool set_node_stats_info(std::string mac, beerocks_message::sStaStatsParams *params);
    void clear_node_stats_info(std::string mac);

    int get_hostap_stats_measurement_duration(std::string mac);
    std::chrono::steady_clock::time_point get_node_stats_info_timestamp(std::string mac);
    std::chrono::steady_clock::time_point get_hostap_stats_info_timestamp(std::string mac);

    uint32_t get_node_rx_bytes(std::string mac);
    uint32_t get_node_tx_bytes(std::string mac);

    double get_node_rx_bitrate(std::string mac);
    double get_node_tx_bitrate(std::string mac);

    bool set_node_rx_phy_rate_100kb(std::string mac, uint16_t rx_phy_rate_100kb);
    bool set_node_tx_phy_rate_100kb(std::string mac, uint16_t tx_phy_rate_100kb);

    uint16_t get_node_rx_phy_rate_100kb(std::string mac);
    uint16_t get_node_tx_phy_rate_100kb(std::string mac);

    int get_hostap_channel_load_percent(std::string mac);

    uint32_t get_hostap_total_sta_rx_bytes(std::string mac);
    uint32_t get_hostap_total_sta_tx_bytes(std::string mac);

    int get_hostap_total_client_tx_load_percent(std::string mac);
    int get_hostap_total_client_rx_load_percent(std::string mac);

    int get_node_rx_load_percent(std::string mac);
    int get_node_tx_load_percent(std::string mac);

    int8_t get_load_rx_rssi(std::string sta_mac);
    uint16_t get_load_rx_phy_rate_100kb(std::string sta_mac);
    uint16_t get_load_tx_phy_rate_100kb(std::string sta_mac);

    bool set_measurement_delay(std::string mac, int measurement_delay);
    int get_measurement_delay(std::string mac);

    std::chrono::steady_clock::time_point get_measurement_sent_timestamp(std::string mac);
    bool set_measurement_sent_timestamp(std::string mac);

    int get_measurement_recv_delta(std::string mac);
    bool set_measurement_recv_delta(std::string mac);

    int get_measurement_window_size(std::string mac);
    bool set_measurement_window_size(std::string mac, int window_size);

    bool get_hostap_exclude_from_steering_flag(std::string mac);
    bool set_hostap_exclude_from_steering_flag(std::string mac, bool flag);

    bool set_node_channel_bw(const std::string &mac, int channel, beerocks::eWiFiBandwidth bw,
                             bool channel_ext_above_secondary, int8_t channel_ext_above_primary,
                             uint16_t vht_center_frequency);
    beerocks::eWiFiBandwidth get_node_bw(std::string mac);
    int get_node_bw_int(std::string mac);
    bool get_hostap_channel_ext_above_primary(std::string hostap_mac);
    bool get_node_channel_ext_above_secondary(std::string mac);
    uint16_t get_hostap_vht_center_frequency(std::string mac);

    //
    // certification
    //
    std::shared_ptr<uint8_t> &get_certification_tx_buffer() { return certification_tx_buffer; };
    std::shared_ptr<uint8_t> &allocate_certification_tx_buffer()
    {
        return (certification_tx_buffer =
                    std::shared_ptr<uint8_t>(new uint8_t[beerocks::message::MESSAGE_BUFFER_LENGTH],
                                             std::default_delete<uint8_t[]>()));
    };
    void remove_certification_tx_buffer() { certification_tx_buffer.reset(); };
    void fill_certification_tx_buffer(ieee1905_1::CmduMessageTx &cmdu_tx)
    {
        std::copy_n(cmdu_tx.getMessageBuff() - sizeof(beerocks::message::sUdsHeader),
                    cmdu_tx.getMessageBuffLength() + sizeof(beerocks::message::sUdsHeader),
                    certification_tx_buffer.get());
    };

    //
    // tasks
    //
    bool assign_association_handling_task_id(std::string mac, int new_task_id);
    int get_association_handling_task_id(std::string mac);

    bool assign_steering_task_id(std::string mac, int new_task_id);
    int get_steering_task_id(std::string mac);

    bool assign_roaming_task_id(std::string mac, int new_task_id);
    int get_roaming_task_id(std::string mac);

    bool assign_load_balancer_task_id(std::string mac, int new_task_id);
    int get_load_balancer_task_id(std::string mac);

    bool assign_client_locating_task_id(std::string mac, int new_task_id, bool new_connection);
    int get_client_locating_task_id(std::string mac, bool new_connection);

    bool assign_network_optimization_task_id(int new_task_id);
    int get_network_optimization_task_id();

    bool assign_bml_task_id(int new_task_id);
    int get_bml_task_id();

    bool assign_rdkb_wlan_task_id(int new_task_id);
    int get_rdkb_wlan_task_id();

    bool assign_ire_4addr_mode_transition_task_id(std::string mac, int new_task_id);
    int get_ire_4addr_mode_transition_task_id(std::string mac);

    bool assign_channel_selection_task_id(int new_task_id);
    int get_channel_selection_task_id();

    bool assign_bml_config_update_task_id(int new_task_id);
    int get_bml_config_update_task_id();

    void lock();
    void unlock();

    //
    // settings
    //
    // Features:
    void settings_dfs_reentry(bool en)
    {
        settings.enable_dfs_reentry = en && config.load_dfs_reentry;
    }
    bool settings_dfs_reentry() { return settings.enable_dfs_reentry; }
    void settings_client_band_steering(bool en)
    {
        settings.client_band_steering = en && config.load_client_band_steering;
    }
    bool settings_client_band_steering() { return settings.client_band_steering; }
    void settings_client_optimal_path_roaming(bool en)
    {
        settings.client_optimal_path_roaming = en && config.load_client_optimal_path_roaming;
    }
    bool settings_client_optimal_path_roaming() { return settings.client_optimal_path_roaming; }
    void settings_legacy_client_roaming(bool en)
    {
        settings.legacy_client_roaming = en && config.load_legacy_client_roaming;
    }
    bool settings_legacy_client_roaming() { return settings.legacy_client_roaming; }
    void settings_client_11k_roaming(bool en)
    {
        settings.client_11k_roaming = en && config.load_client_11k_roaming;
    }
    bool settings_client_11k_roaming() { return settings.client_11k_roaming; }

    void settings_ire_roaming(bool en) { settings.ire_roaming = en && config.load_ire_roaming; }
    bool settings_ire_roaming() { return settings.ire_roaming; }

    void settings_load_balancing(bool en)
    {
        settings.load_balancing = en && config.load_load_balancing;
    }
    bool settings_load_balancing() { return settings.load_balancing; }

    void settings_diagnostics_measurements(bool en)
    {
        settings.diagnostics_measurements = en && config.load_diagnostics_measurements;
    }
    bool settings_diagnostics_measurements() { return settings.diagnostics_measurements; }
    void settings_backhaul_measurements(bool en)
    {
        settings.backhaul_measurements = en && config.load_backhaul_measurements;
    }
    bool settings_backhaul_measurements() { return settings.backhaul_measurements; }
    void settings_front_measurements(bool en)
    {
        settings.front_measurements = en && config.load_front_measurements;
    }
    bool settings_front_measurements() { return settings.front_measurements; }
    void settings_monitor_on_vaps(bool en)
    {
        settings.monitor_on_vaps = en && config.load_monitor_on_vaps;
    }
    bool settings_monitor_on_vaps() { return settings.monitor_on_vaps; }

    void settings_health_check(bool en) { settings.health_check = en && config.load_health_check; }
    bool settings_health_check() { return settings.health_check; }

    void settings_service_fairness(bool en)
    {
        settings.service_fairness = en && config.load_service_fairness;
    }
    bool settings_service_fairness() { return settings.service_fairness; }
    void settings_rdkb_extensions(bool en)
    {
        settings.rdkb_extensions = en && config.load_rdkb_extensions;
    }
    bool settings_rdkb_extensions() { return settings.rdkb_extensions; }

    // Params
    void setting_certification_mode(bool en);

    bool setting_certification_mode() { return config.certification_mode; }

    void settings_client_optimal_path_roaming_prefer_signal_strength(bool en)
    {
        settings.client_optimal_path_roaming_prefer_signal_strength = en;
    }
    bool settings_client_optimal_path_roaming_prefer_signal_strength()
    {
        return settings.client_optimal_path_roaming_prefer_signal_strength;
    }

    //
    // vars
    //
    sDbMasterConfig &config;

private:
    std::string local_slave_mac;
    std::shared_ptr<node> get_node(std::string key); //key can be <mac> or <al_mac>_<ruid>
    int get_node_hierarchy(std::shared_ptr<node> n);
    std::set<std::shared_ptr<node>> get_node_subtree(std::shared_ptr<node> n);
    void adjust_subtree_hierarchy(std::shared_ptr<node> n);
    void adjust_subtree_hierarchy(std::set<std::shared_ptr<node>> subtree, int offset);
    std::set<std::shared_ptr<node>> get_node_children(std::shared_ptr<node> n,
                                                      int type               = beerocks::TYPE_ANY,
                                                      int state              = beerocks::STATE_ANY,
                                                      std::string parent_mac = std::string());
    int get_node_bw_int(std::shared_ptr<node> &n);

    void rewind();
    bool get_next_node(std::shared_ptr<node> &n, int &hierarchy);
    bool get_next_node(std::shared_ptr<node> &n);

    int network_optimization_task_id = -1;
    int channel_selection_task_id    = -1;
    int bml_task_id                  = -1;
    int rdkb_wlan_task_id            = -1;
    int config_update_task_id        = -1;

    std::shared_ptr<node> last_accessed_node;
    std::string last_accessed_node_mac;

    std::mutex db_mutex;

    std::unordered_map<std::string, std::shared_ptr<node>> nodes[beerocks::HIERARCHY_MAX];

    std::queue<std::string> disconnected_slave_mac_queue;

    int slaves_stop_on_failure_attempts = 0;

    /*
            * some operations on unordered_map can cause iterators to be invalidated
            * use the following with caution
            */
    int current_hierarchy = 0;
    std::unordered_map<std::string, std::shared_ptr<node>>::iterator db_it =
        std::unordered_map<std::string, std::shared_ptr<node>>::iterator();

    std::vector<Socket *> cli_debug_sockets;
    std::vector<sBmlListener> bml_listeners_sockets;

    beerocks::logging &logger;

    sDbMasterSettings settings;
    std::vector<uint8_t> global_restricted_channels;
    friend class network_map;

    std::shared_ptr<vaps_list_t> m_vap_list;

    // certification
    std::shared_ptr<uint8_t> certification_tx_buffer;
};

} // namespace son

#endif
