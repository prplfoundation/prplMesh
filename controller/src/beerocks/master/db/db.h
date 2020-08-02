/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _DB_H_
#define _DB_H_

#include "node.h"

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_logging.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>

#include <mutex>
#include <queue>

using namespace beerocks_message;

namespace son {

// Forward decleration for master_thread context saving
class master_thread;

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
        bool topology_updates;
    } sBmlListener;

public:
    /**
     * @brief Client parameter names.
     * The parameter names can be used to set/get multiple parameters in one-shot.
     * This is done using key-value map (where key is the param name and value is it value)
     */
    static const std::string TIMESTAMP_STR;
    static const std::string TIMELIFE_DELAY_STR;
    static const std::string INITIAL_RADIO_ENABLE_STR;
    static const std::string INITIAL_RADIO_STR;
    static const std::string SELECTED_BAND_ENABLE_STR;
    static const std::string SELECTED_BANDS_STR;

    // VAPs info list type
    typedef std::list<std::shared_ptr<beerocks_message::sConfigVapInfo>> vaps_list_t;

    typedef struct {
        std::string vendor;
        std::string model;
        std::string gw_ip;
        std::string gw_netmask;
        std::string ire_ip_range_low;
        std::string ire_ip_range_high;
        std::string load_steer_on_vaps;
        std::vector<uint8_t> global_restricted_channels;
        int ucc_listener_port;
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
        bool persistent_db;
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
        int clients_persistent_db_max_size;
        int max_timelife_delay_days;
        int unfriendly_device_max_timelife_delay_days;

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

    db(sDbMasterConfig &config_, beerocks::logging &logger_, const std::string &local_bridge_mac)
        : config(config_), logger(logger_), m_local_bridge_mac(local_bridge_mac)
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

    //static
    /**
     * @brief Get string representation of node type.
     * 
     * @param type Type of a node.
     * @return std::string the string representation of the type.
     */
    static std::string type_to_string(beerocks::eType type);

    /**
     * @brief Get db entry from MAC address.
     * 
     * @param mac MAC address of a client.
     * @return std::string the string representation of the MAC address with ':' replaced with '_' removed.
     * @return An empty string is returned on failure.
     */
    static std::string client_db_entry_from_mac(const sMacAddr &mac);

    /**
     * @brief Get client MAC address from db entry.
     * 
     * @param db_entry Client entry name in persistent db.
     * @return sMacAddr MAC address of the client the db_entry is representing. On failure ZERO_MAC is returned.
     */
    static sMacAddr client_db_entry_to_mac(std::string db_entry);

    /**
     * @brief Get string representation of number of seconds in timestamp.
     * 
     * @param timestamp A time-point.
     * @return std::string the string representation of the integer number of seconds in the timestamp.
     */
    static std::string
    timestamp_to_string_seconds(const std::chrono::steady_clock::time_point timestamp);

    /**
     * @brief Translate an integer number of seconds to a timepoint.
     * 
     * @param timestamp_sec Number of seconds in the timestamp.
     * @return std::chrono::steady_clock::time_point a time-point representation of the number of seconds.
     */
    static std::chrono::steady_clock::time_point timestamp_from_seconds(int timestamp_sec);

    //logger
    void set_log_level_state(const beerocks::eLogLevel &log_level, const bool &new_state);

    // General set/get
    bool has_node(sMacAddr mac);

    bool add_virtual_node(sMacAddr mac, sMacAddr real_node_mac);
    bool add_node(const sMacAddr &mac,
                  const sMacAddr &parent_mac       = beerocks::net::network_utils::ZERO_MAC,
                  beerocks::eType type             = beerocks::TYPE_CLIENT,
                  const sMacAddr &radio_identifier = beerocks::net::network_utils::ZERO_MAC);
    bool remove_node(const sMacAddr &mac);

    bool set_node_type(const std::string &mac, beerocks::eType type);
    beerocks::eType get_node_type(const std::string &mac);

    bool set_local_slave_mac(const std::string &mac);
    std::string get_local_slave_mac();

    bool set_node_ipv4(const std::string &mac, const std::string &ipv4 = std::string());
    std::string get_node_ipv4(const std::string &mac);

    bool set_node_manufacturer(const std::string &mac, const std::string &manufacturer);
    std::string get_node_manufacturer(const std::string &mac);

    int get_node_channel(const std::string &mac);

    int get_hostap_operating_class(const sMacAddr &mac);

    bool set_node_vap_id(const std::string &mac, int8_t vap_id);
    int8_t get_node_vap_id(const std::string &mac);

    bool set_node_beacon_measurement_support_level(
        const std::string &mac,
        beerocks::eBeaconMeasurementSupportLevel support_beacon_measurement);
    beerocks::eBeaconMeasurementSupportLevel
    get_node_beacon_measurement_support_level(const std::string &mac);

    bool set_node_name(const std::string &mac, std::string name);
    std::string get_node_name(const std::string &mac);

    bool set_node_state(const std::string &mac, beerocks::eNodeState state);
    beerocks::eNodeState get_node_state(const std::string &mac);

    bool set_node_operational_state(const std::string &bridge_mac, bool operational);
    int8_t get_node_operational_state(const std::string &bridge_mac);

    std::chrono::steady_clock::time_point get_last_state_change(const std::string &mac);

    bool set_node_handoff_flag(const std::string &mac, bool handoff);
    bool get_node_handoff_flag(const std::string &mac);

    bool set_node_confined_flag(const std::string &mac, bool flag);
    bool get_node_confined_flag(const std::string &mac);

    bool update_node_last_seen(const std::string &mac);

    std::chrono::steady_clock::time_point get_node_last_seen(const std::string &mac);

    bool set_hostap_active(const std::string &mac, bool active);
    bool is_hostap_active(const std::string &mac);

    bool set_hostap_backhaul_manager(const std::string &mac, bool is_backhaul_manager);
    bool is_hostap_backhaul_manager(const std::string &mac);
    std::string get_hostap_backhaul_manager(std::string ire);

    bool is_ap_out_of_band(const std::string &mac, const std::string &sta_mac);

    bool is_node_wireless(const std::string &mac);

    std::string node_to_string(const std::string &mac);

    /**
     * @brief Get the link metric database
     * @return reference to the map that holds link metric data of all agents.
     */
    std::unordered_map<sMacAddr, std::unordered_map<sMacAddr, son::node::link_metrics_data>> &
    get_link_metric_data_map();

    /**
     * @brief Get the ap metric database
     * @return reference to the map that holds ap metric data of all agents.
     */
    std::unordered_map<sMacAddr, son::node::ap_metrics_data> &get_ap_metric_data_map();

    //
    // DB node functions (get only)
    //
    int get_node_hierarchy(const std::string &mac);
    std::set<std::string> get_nodes(int type = -1);
    std::set<std::string> get_device_nodes();
    std::set<std::string> get_active_hostaps();
    std::set<std::string> get_all_connected_ires();
    std::set<std::string> get_all_backhaul_manager_slaves();
    std::set<std::string> get_nodes_from_hierarchy(int hierarchy, int type = -1);
    std::string get_gw_mac();
    std::set<std::string> get_node_subtree(const std::string &mac);
    std::string get_node_parent(const std::string &mac);

    std::string get_node_parent_hostap(const std::string &mac);
    std::string get_node_previous_parent(const std::string &mac);
    std::string get_node_parent_ire(const std::string &mac);
    std::string get_node_parent_backhaul(const std::string &mac);
    std::set<std::string> get_node_siblings(const std::string &mac, int type = beerocks::TYPE_ANY);
    std::set<std::string> get_node_children(const std::string &mac, int type = beerocks::TYPE_ANY,
                                            int state = beerocks::STATE_ANY);
    std::list<sMacAddr> get_1905_1_neighbors(const sMacAddr &al_mac);
    std::string get_node_key(const std::string &al_mac, const std::string &ruid);

    //
    // Capabilities
    //

    const beerocks::message::sRadioCapabilities *
    get_station_current_capabilities(const std::string &mac);

    const beerocks::message::sRadioCapabilities *
    get_station_capabilities(const std::string &client_mac, bool is_bandtype_5ghz);
    bool set_station_capabilities(const std::string &client_mac,
                                  const beerocks::message::sRadioCapabilities &sta_cap);

    bool set_hostap_ant_num(const std::string &mac, beerocks::eWiFiAntNum ant_num);
    beerocks::eWiFiAntNum get_hostap_ant_num(const std::string &mac);

    bool set_hostap_ant_gain(const std::string &mac, int ant_gain);
    int get_hostap_ant_gain(const std::string &mac);

    bool set_hostap_tx_power(const std::string &mac, int tx_power);
    int get_hostap_tx_power(const std::string &mac);

    bool set_hostap_supported_channels(const std::string &mac,
                                       beerocks::message::sWifiChannel *supported_channels,
                                       int length);
    const std::vector<beerocks::message::sWifiChannel>
    get_hostap_supported_channels(const std::string &mac);
    std::string get_hostap_supported_channels_string(const std::string &radio_mac);

    bool add_hostap_supported_operating_class(const std::string &radio_mac, uint8_t operating_class,
                                              uint8_t tx_power,
                                              const std::vector<uint8_t> &non_operable_channels);

    bool set_hostap_band_capability(const std::string &mac,
                                    beerocks::eRadioBandCapability capability);
    beerocks::eRadioBandCapability get_hostap_band_capability(const std::string &mac);

    bool capability_check(const std::string &mac, int channel);

    bool get_node_5ghz_support(
        const std::string &mac); // TODO: add a real learning algorithm for per-channel support
    bool get_node_24ghz_support(const std::string &mac);
    bool is_node_5ghz(const std::string &mac);
    bool is_node_24ghz(const std::string &mac);
    bool update_node_failed_5ghz_steer_attempt(const std::string &mac);
    bool update_node_failed_24ghz_steer_attempt(const std::string &mac);

    bool update_node_11v_responsiveness(const std::string &mac, bool success);
    bool get_node_11v_capability(const std::string &mac);

    bool set_hostap_iface_name(const std::string &mac, std::string iface_name);
    std::string get_hostap_iface_name(const std::string &mac);

    bool set_hostap_iface_type(const std::string &mac, beerocks::eIfaceType iface_type);
    beerocks::eIfaceType get_hostap_iface_type(const std::string &mac);

    bool set_hostap_driver_version(const std::string &mac, std::string version);
    std::string get_hostap_driver_version(const std::string &mac);

    bool set_hostap_iface_id(const std::string &mac, int8_t iface_id);
    int8_t get_hostap_iface_id(const std::string &mac);

    bool set_hostap_vap_list(const std::string &mac,
                             const std::unordered_map<int8_t, sVapElement> &vap_list);
    std::unordered_map<int8_t, sVapElement> &get_hostap_vap_list(const std::string &mac);
    std::set<std::string> get_hostap_vaps_bssids(const std::string &mac);
    bool remove_vap(const std::string &mac, int vap_id);
    bool add_vap(const std::string &radio_mac, int vap_id, std::string bssid, std::string ssid,
                 bool backhual);

    std::string get_hostap_ssid(const std::string &mac);
    /**
     * @brief checks if vap name is on the steer list.
     * 
     * @param[in] bssid vap mac address.
     * @return true if vap name is on the steer list.
     */
    bool is_vap_on_steer_list(const std::string &bssid);
    std::string get_hostap_vap_with_ssid(const std::string &mac, const std::string &ssid);
    std::string get_hostap_vap_mac(const std::string &mac, const int vap_id);
    std::string get_node_parent_radio(const std::string &mac);
    int8_t get_hostap_vap_id(const std::string &mac);

    bool set_hostap_repeater_mode_flag(const std::string &mac, bool flag);
    bool get_hostap_repeater_mode_flag(const std::string &mac);

    bool set_node_backhaul_iface_type(const std::string &mac, beerocks::eIfaceType iface_type);
    beerocks::eIfaceType get_node_backhaul_iface_type(const std::string &mac);

    std::string get_5ghz_sibling_hostap(const std::string &mac);

    bool set_cs_op_flag(const std::string &mac, bool flag);
    bool get_cs_op_flag(const std::string &mac);
    bool set_cs_lb_flag(const std::string &mac, bool flag);
    bool get_cs_lb_flag(const std::string &mac);

    bool set_global_restricted_channels(uint8_t *restricted_channels);
    std::vector<uint8_t> get_global_restricted_channels();
    bool set_hostap_conf_restricted_channels(const std::string &hostap_mac,
                                             uint8_t *restricted_channels);
    std::vector<uint8_t> get_hostap_conf_restricted_channels(const std::string &hostap_mac);

    //
    // CS - DFS
    //
    bool set_hostap_activity_mode(const std::string &mac, beerocks::eApActiveMode ap_activity_mode);
    beerocks::eApActiveMode get_hostap_activity_mode(const std::string &mac);
    bool set_radar_hit_stats(const std::string &mac, uint8_t channel, uint8_t bw,
                             bool is_csa_entry);
    bool set_supported_channel_radar_affected(const std::string &mac, std::vector<uint8_t> channels,
                                              bool affected);
    //bool get_supported_channel_all_availble(const std::string &mac );

    bool set_hostap_is_dfs(const std::string &mac, bool enable);
    bool get_hostap_is_dfs(const std::string &mac);

    bool set_hostap_cac_completed(const std::string &mac, bool enable);
    bool get_hostap_cac_completed(const std::string &mac);

    bool set_hostap_is_on_fail_safe(const std::string &mac, bool enable);
    bool get_hostap_is_on_fail_safe(const std::string &mac);

    bool set_hostap_is_on_sub_band(const std::string &mac, bool enable);
    bool get_hostap_is_on_sub_band(const std::string &mac);

    bool set_hostap_on_dfs_reentry(const std::string &mac, bool enable);
    bool get_hostap_on_dfs_reentry(const std::string &mac);

    bool set_hostap_dfs_reentry_clients(const std::string &mac,
                                        const std::set<std::string> &dfs_reentry_clients);
    std::set<std::string> get_hostap_dfs_reentry_clients(const std::string &mac);
    bool clear_hostap_dfs_reentry_clients(const std::string &mac);

    bool set_hostap_is_acs_enabled(const std::string &mac, bool enable);
    bool get_hostap_is_acs_enabled(const std::string &mac);

    //
    // Channel Scan
    //
    /**
     * @brief Set the channel scan is enabled flag
     * 
     * @param mac:    MAC address of radio
     * @param enable: enable flag to be set
     * @return true on success
     * @return false on failure 
     */
    bool set_channel_scan_is_enabled(const sMacAddr &mac, bool enable);

    /**
     * @brief Get the channel scan is enabled flag
     * 
     * @param [out] mac: MAC address of radio 
     * @return current channel scan enable flag
     */
    bool get_channel_scan_is_enabled(const sMacAddr &mac);

    /**
     * @brief Set the channel scan interval sec object
     * 
     * @param mac 
     * @param interval_sec 
     * @return true 
     * @return false 
     */
    bool set_channel_scan_interval_sec(const sMacAddr &mac, int interval_sec);

    /**
     * @brief Get the channel scan interval sec object
     * 
     * @param mac: MAC address of radio 
     * @return value o interval sec object
     */
    int get_channel_scan_interval_sec(const sMacAddr &mac);

    /**
     * @brief Set the channel scan in progress object
     * 
     * @param mac:              MAC address of radio
     * @param scan_in_progress: Flag of current channel scan
     * @param single_scan:      Indicated if to use single scan or continuous
     * @return true on success
     * @return false on failure 
     */
    bool set_channel_scan_in_progress(const sMacAddr &mac, bool scan_in_progress,
                                      const bool single_scan);
    /**
     * @brief Get the channel scan in progress object
     * 
     * @param mac          MAC address of radio
     * @param single_scan: Indicated if to use single scan or continuous
     * @return Flag of current channel scan
     */
    bool get_channel_scan_in_progress(const sMacAddr &mac, bool single_scan);

    /**
     * @brief Set the channel scan results status object
     * 
     * @param mac:         MAC address of radio
     * @param error_code:  Current status of channel scan results
     * @param single_scan: Indicated if to use single scan or continuous
     * @return true on success
     * @return false on failure 
     */
    bool set_channel_scan_results_status(const sMacAddr &mac,
                                         beerocks::eChannelScanStatusCode error_code,
                                         bool single_scan);

    /**
     * @brief Get the channel scan results status object
     * 
     * @param mac:         MAC address of radio
     * @param single_scan: Indicated if to use single scan or continuous
     * @return Current status of channel scan results
     */
    beerocks::eChannelScanStatusCode get_channel_scan_results_status(const sMacAddr &mac,
                                                                     bool single_scan);

    /**
     * @brief Set the channel scan dwell time msec object
     * 
     * @param mac:             MAC address of radio
     * @param dwell_time_msec: Dwell time of channel scan
     * @param single_scan:     Indicated if to use single scan or continuous
     * @return true on success
     * @return false on failure 
     */
    bool set_channel_scan_dwell_time_msec(const sMacAddr &mac, int dwell_time_msec,
                                          bool single_scan);

    /**
     * @brief Get the channel scan dwell time msec object
     * 
     * @param mac          MAC address of radio
     * @param single_scan: Indicated if to use single scan or continuous
     * @return Dwell time of channel scan
     */
    int get_channel_scan_dwell_time_msec(const sMacAddr &mac, bool single_scan);

    /**
     * @brief Set the channel scan pool object
     * 
     * @param mac:          MAC address of radio
     * @param channel_pool: Channel pool of channel scan
     * @param single_scan:  Indicated if to use single scan or continuous
     * @return true on success
     * @return false on failure 
     */
    bool set_channel_scan_pool(const sMacAddr &mac, const std::unordered_set<uint8_t> &channel_pool,
                               bool single_scan);

    /**
     * @brief Validate the channel scan pool
     * 
     * @param mac:          MAC address of radio
     * @param channel_pool: Channel pool of channel scan
     * @return true if pool is valid
     * @return false if pool is invalid
     */
    bool is_channel_scan_pool_supported(const sMacAddr &mac,
                                        const std::unordered_set<uint8_t> &channel_pool);

    /**
     * @brief Get the channel scan pool object
     * 
     * @param mac:         MAC address of radio
     * @param single_scan: Indicated if to use single scan or continuous
     * @return Channel pool of channel scan
     */
    const std::unordered_set<uint8_t> &get_channel_scan_pool(const sMacAddr &mac, bool single_scan);

    /**
     * @brief Checks whather a given channel is in the currently set channel pool
     * 
     * @param mac:         MAC address of radio
     * @param channel:     Given channel to be checked
     * @param single_scan: Indicated if to use single scan or continuous
     * @return true if given channel is in current channel pool
     * @return false if given channel isn't in current channel pool
     */
    bool is_channel_in_pool(const sMacAddr &mac, uint8_t channel, bool single_scan);

    /**
     * @brief Clears any existing results for the given channel scan
     * 
     * @param mac:         MAC address of radio
     * @param single_scan: Indicated if to use single scan or continuous
     * @return true on success
     * @return false on failure 
     */
    bool clear_channel_scan_results(const sMacAddr &mac, bool single_scan);

    /**
     * @brief Adds a new scan result to the current scan results
     * 
     * @param mac:         MAC address of radio
     * @param scan_result: Scan result to be added to current scan results
     * @param single_scan: Indicated if to use single scan or continuous
     * @return true on success
     * @return false on failure 
     */
    bool add_channel_scan_results(const sMacAddr &mac, const sChannelScanResults &scan_result,
                                  bool single_scan);

    /**
     * @brief Get the channel scan results object
     * 
     * @param mac:         MAC address of radio
     * @param single_scan: Indicated if to use single scan or continuous
     * @return const std::list<sChannelScanResults>& 
     */
    const std::list<sChannelScanResults> &get_channel_scan_results(const sMacAddr &mac,
                                                                   bool single_scan);

    //
    // Client Persistent Data
    //
    /**
     * @brief Check if client exists in persistent db.
     * 
     * @param mac MAC address of a client.
     * @return true if client exists, false otherwise.
     */
    bool is_client_in_persistent_db(const sMacAddr &mac);

    /**
     * @brief Adds a client to the persistent db, if already exists, remove old entry and add a new one.
     * 
     * @param mac MAC address of a client.
     * @param params An unordered map of key-value of client parameters.
     * @return true on success, otherwise false.
     */
    bool add_client_to_persistent_db(const sMacAddr &mac,
                                     const std::unordered_map<std::string, std::string> &params =
                                         std::unordered_map<std::string, std::string>());

    /**
     * @brief Get the client's parameters last edit time.
     * 
     * @param mac MAC address of a client.
     * @return Client persistent data last edit time (even if edit was done only to runtime-dbb and not saved to persistent db), or time_point::min() if not-configured or failure.
     */
    std::chrono::steady_clock::time_point get_client_parameters_last_edit(const sMacAddr &mac);

    /**
     * @brief Set the client's time-life delay.
     * 
     * @param mac MAC address of a client.
     * @param time_life_delay_sec Client-specific aging time.
     * @param save_to_persistent_db If set to true, update the persistent-db (write-through), default is true.
     * @return true on success, otherwise false.
     */
    bool set_client_time_life_delay(const sMacAddr &mac,
                                    const std::chrono::seconds &time_life_delay_sec,
                                    bool save_to_persistent_db = true);

    /**
     * @brief Get the client's time-life delay.
     * 
     * @param mac MAC address of a client.
     * @return Client time-life delay, value of 0 means not-configured.
     */
    std::chrono::seconds get_client_time_life_delay(const sMacAddr &mac);

    /**
     * @brief Set the client's stay-on-initial-radio.
     * 
     * @param mac MAC address of a client.
     * @param stay_on_initial_radio Enable client stay on the radio it initially connected to.
     * @param save_to_persistent_db If set to true, update the persistent-db (write-through), default is true.
     * @return true on success, otherwise false.
     */
    bool set_client_stay_on_initial_radio(const sMacAddr &mac, bool stay_on_initial_radio,
                                          bool save_to_persistent_db = true);

    /**
     * @brief Get the client's stay-on-initial-radio.
     * 
     * @param mac MAC address of a client.
     * @return Enable client stay on the radio it initially connected to.
     */
    eTriStateBool get_client_stay_on_initial_radio(const sMacAddr &mac);

    /**
     * @brief Set the client's initial-radio.
     * 
     * @param mac MAC address of a client.
     * @param initial_radio_mac The MAC address of the radio that the client has initially connected to.
     * @param save_to_persistent_db If set to true, update the persistent-db (write-through), default is true.
     * @return true on success, otherwise false.
     */
    bool set_client_initial_radio(const sMacAddr &mac, const sMacAddr &initial_radio_mac,
                                  bool save_to_persistent_db = true);

    /**
     * @brief Get the client's initial-radio.
     * 
     * @param mac MAC address of a client.
     * @return MAC adddress of the radio that the client has initially connected to.
     */
    sMacAddr get_client_initial_radio(const sMacAddr &mac);

    /**
     * @brief Set the client's stay-on-selected-band.
     * 
     * @param mac MAC address of a client.
     * @param stay_on_selected_band Enable client stay on the selected band/bands.
     * @param save_to_persistent_db If set to true, update the persistent-db (write-through), default is true.
     * @return true on success, otherwise false.
     */
    bool set_client_stay_on_selected_band(const sMacAddr &mac, bool stay_on_selected_band,
                                          bool save_to_persistent_db = true);

    /**
     * @brief Get the client's stay-on-selected-band.
     * 
     * @param mac MAC address of a client.
     * @return Enable client stay on the selected band/bands.
     */
    eTriStateBool get_client_stay_on_selected_band(const sMacAddr &mac);

    /**
     * @brief Set the client's selected-bands.
     * 
     * @param mac MAC address of a client.
     * @param selected_bands Client selected band/bands. FREQ_UNKNOWN is considered as "not-configured".
     * @param save_to_persistent_db If set to true, update the persistent-db (write-through), default is true.
     * @return true on success, otherwise false.
     */
    bool set_client_selected_bands(const sMacAddr &mac, beerocks::eFreqType selected_bands,
                                   bool save_to_persistent_db = true);

    /**
     * @brief Get the client's selected-bands.
     * 
     * @param mac MAC address of a client.
     * @return Selected band/bands.
     */
    beerocks::eFreqType get_client_selected_bands(const sMacAddr &mac);

    /**
     * @brief Clear client's persistent information.
     * 
     * @param mac MAC address of a client.
     * @return true on success, otherwise false.
     */
    bool clear_client_persistent_db(const sMacAddr &mac);

    /**
     * @brief Update client's persistent information with the runtime information.
     * 
     * @param mac MAC address of a client.
     * @return true on success, otherwise false.
     */
    bool update_client_persistent_db(const sMacAddr &mac);

    /**
     * @brief Load all clients from persistent db.
     * Creates nodes for the clients in runtime-db and set persistent parameters values accordingly.
     * Aged Clients and Clients with invalid data are filtered-out and removed from persistent-DB.
     * 
     * @return true on success, otherwise false.
     */
    bool load_persistent_db_clients();

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
    bool get_bml_topology_update_enable(Socket *sd);
    bool set_bml_topology_update_enable(Socket *sd, bool update_enable);
    Socket *get_bml_socket_at(int idx);
    bool is_bml_listener_exist();

    void set_vap_list(std::shared_ptr<vaps_list_t> vaps_list);
    const std::shared_ptr<vaps_list_t> get_vap_list();
    void clear_vap_list();

    //
    // Measurements
    //
    bool set_node_beacon_measurement(const std::string &sta_mac, std::string ap_mac, int8_t rcpi,
                                     uint8_t rsni);
    bool get_node_beacon_measurement(const std::string &sta_mac, std::string ap_mac, int8_t &rcpi,
                                     uint8_t &rsni);

    bool set_node_cross_rx_rssi(const std::string &sta_mac, std::string ap_mac, int8_t rssi,
                                int8_t rx_packets);
    bool get_node_cross_rx_rssi(const std::string &sta_mac, std::string ap_mac, int8_t &rssi,
                                int8_t &rx_packets);

    bool set_node_cross_rx_phy_rate_100kb(const std::string &mac, uint16_t rx_phy_rate_100kb);
    bool set_node_cross_tx_phy_rate_100kb(const std::string &mac, uint16_t tx_phy_rate_100kb);

    uint16_t get_node_cross_rx_phy_rate_100kb(const std::string &mac);
    uint16_t get_node_cross_tx_phy_rate_100kb(const std::string &mac);

    bool clear_node_cross_rssi(const std::string &sta_mac);

    bool set_node_cross_estimated_tx_phy_rate(const std::string &mac, double phy_rate);
    double get_node_cross_estimated_tx_phy_rate(const std::string &mac);

    bool set_hostap_stats_info(const std::string &mac, beerocks_message::sApStatsParams *params);
    void clear_hostap_stats_info(const std::string &mac);
    bool set_node_stats_info(const std::string &mac, beerocks_message::sStaStatsParams *params);
    void clear_node_stats_info(const std::string &mac);

    int get_hostap_stats_measurement_duration(const std::string &mac);
    std::chrono::steady_clock::time_point get_node_stats_info_timestamp(const std::string &mac);
    std::chrono::steady_clock::time_point get_hostap_stats_info_timestamp(const std::string &mac);

    uint32_t get_node_rx_bytes(const std::string &mac);
    uint32_t get_node_tx_bytes(const std::string &mac);

    double get_node_rx_bitrate(const std::string &mac);
    double get_node_tx_bitrate(const std::string &mac);

    bool set_node_rx_phy_rate_100kb(const std::string &mac, uint16_t rx_phy_rate_100kb);
    bool set_node_tx_phy_rate_100kb(const std::string &mac, uint16_t tx_phy_rate_100kb);

    uint16_t get_node_rx_phy_rate_100kb(const std::string &mac);
    uint16_t get_node_tx_phy_rate_100kb(const std::string &mac);

    int get_hostap_channel_load_percent(const std::string &mac);

    uint32_t get_hostap_total_sta_rx_bytes(const std::string &mac);
    uint32_t get_hostap_total_sta_tx_bytes(const std::string &mac);

    int get_hostap_total_client_tx_load_percent(const std::string &mac);
    int get_hostap_total_client_rx_load_percent(const std::string &mac);

    int get_node_rx_load_percent(const std::string &mac);
    int get_node_tx_load_percent(const std::string &mac);

    int8_t get_load_rx_rssi(const std::string &sta_mac);
    uint16_t get_load_rx_phy_rate_100kb(const std::string &sta_mac);
    uint16_t get_load_tx_phy_rate_100kb(const std::string &sta_mac);

    bool set_measurement_delay(const std::string &mac, int measurement_delay);
    int get_measurement_delay(const std::string &mac);

    std::chrono::steady_clock::time_point get_measurement_sent_timestamp(const std::string &mac);
    bool set_measurement_sent_timestamp(const std::string &mac);

    int get_measurement_recv_delta(const std::string &mac);
    bool set_measurement_recv_delta(const std::string &mac);

    int get_measurement_window_size(const std::string &mac);
    bool set_measurement_window_size(const std::string &mac, int window_size);

    bool get_hostap_exclude_from_steering_flag(const std::string &mac);
    bool set_hostap_exclude_from_steering_flag(const std::string &mac, bool flag);

    bool set_node_channel_bw(const std::string &mac, int channel, beerocks::eWiFiBandwidth bw,
                             bool channel_ext_above_secondary, int8_t channel_ext_above_primary,
                             uint16_t vht_center_frequency);
    beerocks::eWiFiBandwidth get_node_bw(const std::string &mac);
    int get_node_bw_int(const std::string &mac);
    bool get_hostap_channel_ext_above_primary(const std::string &hostap_mac);
    bool get_node_channel_ext_above_secondary(const std::string &mac);
    uint16_t get_hostap_vht_center_frequency(const std::string &mac);

    void add_bss_info_configuration(const sMacAddr &al_mac,
                                    const wireless_utils::sBssInfoConf &bss_info);
    std::list<wireless_utils::sBssInfoConf> &get_bss_info_configuration(const sMacAddr &al_mac);
    void clear_bss_info_configuration();
    void clear_bss_info_configuration(const sMacAddr &al_mac);

    //
    // tasks
    //
    bool assign_association_handling_task_id(const std::string &mac, int new_task_id);
    int get_association_handling_task_id(const std::string &mac);

    bool assign_steering_task_id(const std::string &mac, int new_task_id);
    int get_steering_task_id(const std::string &mac);

    bool assign_roaming_task_id(const std::string &mac, int new_task_id);
    int get_roaming_task_id(const std::string &mac);

    bool assign_load_balancer_task_id(const std::string &mac, int new_task_id);
    int get_load_balancer_task_id(const std::string &mac);

    bool assign_client_locating_task_id(const std::string &mac, int new_task_id,
                                        bool new_connection);
    int get_client_locating_task_id(const std::string &mac, bool new_connection);

    bool assign_network_optimization_task_id(int new_task_id);
    int get_network_optimization_task_id();

    bool assign_bml_task_id(int new_task_id);
    int get_bml_task_id();

    bool assign_rdkb_wlan_task_id(int new_task_id);
    int get_rdkb_wlan_task_id();

    bool assign_ire_4addr_mode_transition_task_id(const std::string &mac, int new_task_id);
    int get_ire_4addr_mode_transition_task_id(const std::string &mac);

    bool assign_channel_selection_task_id(int new_task_id);
    int get_channel_selection_task_id();

    bool assign_dynamic_channel_selection_task_id(const sMacAddr &mac, int new_task_id);
    int get_dynamic_channel_selection_task_id(const sMacAddr &mac);

    void lock();
    void unlock();

    //
    // settings
    //
    std::string settings_vendor() { return config.vendor; }
    std::string settings_model() { return config.model; }

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
    void setting_certification_mode(bool en) { config.certification_mode = en; }

    bool setting_certification_mode() { return config.certification_mode; }

    void settings_client_optimal_path_roaming_prefer_signal_strength(bool en)
    {
        settings.client_optimal_path_roaming_prefer_signal_strength = en;
    }
    bool settings_client_optimal_path_roaming_prefer_signal_strength()
    {
        return settings.client_optimal_path_roaming_prefer_signal_strength;
    }

    bool is_prplmesh(const sMacAddr &mac);
    void set_prplmesh(const sMacAddr &mac);

    //
    // master_thread context
    //
    void set_master_thread_ctx(master_thread *ctx) { m_master_thread_ctx = ctx; }
    master_thread *get_master_thread_ctx() { return m_master_thread_ctx; }

    const std::string &get_local_bridge_mac() { return m_local_bridge_mac; }

    //
    // vars
    //
    sDbMasterConfig &config;

private:
    std::string local_slave_mac;
    std::shared_ptr<node> get_node(std::string key); //key can be <mac> or <al_mac>_<ruid>
    std::shared_ptr<node> get_node(sMacAddr mac);
    std::shared_ptr<node> get_node(sMacAddr al_mac, sMacAddr ruid);
    /**
     * @brief Returns the node object after verifing node type.
     * if node is found but type is not requested type a nullptr is returned.
     * 
     * @param mac MAC address of the node.
     * @param type The type of node used for node-type verification.
     * @return std::shared_ptr<node> pointer to the node on success, nullptr otherwise.
     */
    std::shared_ptr<node> get_node_verify_type(const sMacAddr &mac, beerocks::eType type);
    std::shared_ptr<node::radio> get_hostap_by_mac(const sMacAddr &mac);
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

    /**
     * @brief Updates the client values in the persistent db.
     * 
     * @param mac MAC address of a client.
     * @param values_map A map of client params and their values.
     * @return true on success, otherwise false.
     */
    bool update_client_entry_in_persistent_db(
        const sMacAddr &mac, const std::unordered_map<std::string, std::string> &values_map);

    /**
     * @brief Sets the node params (runtime db) from a param-value map.
     * 
     * @param mac MAC address of node to be updated.
     * @param values_map A map of client params and their values.
     * @return true on success, otherwise false.
     */
    bool set_node_params_from_map(const sMacAddr &mac,
                                  const std::unordered_map<std::string, std::string> &values_map);

    /**
     * @brief Adds a client entry to persistent_db with configured parameters and increments clients counter.
     * 
     * @param entry_name Client entry name in persistent db.
     * @param values_map A map of client params and their values.
     * @return true on success, otherwise false.
     */
    bool add_client_entry_and_update_counter(
        const std::string &entry_name,
        const std::unordered_map<std::string, std::string> &values_map);

    /**
     * @brief Removes a client entry from persistent_db and decrements clients counter.
     * 
     * @param entry_name Client entry name in persistent db.
     * @return true on success, otherwise false.
     */
    bool remove_client_entry_and_update_counter(const std::string &entry_name);

    /**
     * @brief Removes client with least timelife remaining from persistent db (with preference to disconnected clients).
     * 
     * @return true on success, otherwise false.
     */
    bool remove_candidate_client();

    /**
     * @brief Returns the preferred client to be removed.
     * Preference is determined as follows:
     * - Prefer disconnected clients over connected ones.
     * - According to above, the client with least time left before aging.
     *
     * @return sMacAddr mac of candidate client to be removed - if not found, string_utils::ZERO_MAC is returned.
     */
    sMacAddr get_candidate_client_for_removal();

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

    /*
    * This map holds link metric "data struct" per reporting Agent sMacAddr .
    * "data struct" holds map of the actual link_metrics_data vector (tx/rx) per reported Agent sMacAddr.
    * Map is Used in TYPE_GW/TYPE_IRE nodes.
    * Map created empty in all other nodes.
    */
    //TODO: This map should be moved to the agent nodes instead of being a separate map.
    std::unordered_map<sMacAddr, std::unordered_map<sMacAddr, son::node::link_metrics_data>>
        m_link_metric_data;

    /*
    * This map holds ap metric data per reporting Agent sMacAddr .
    * Map is Used in TYPE_GW/TYPE_IRE nodes.
    * Map created empty in all other nodes.
    */
    //TODO:  This map should be moved to the BSS nodes (which currently don't exist) instead of being a separate map.
    std::unordered_map<sMacAddr, son::node::ap_metrics_data> m_ap_metric_data;

    // certification
    std::shared_ptr<uint8_t> certification_tx_buffer;
    std::unordered_map<sMacAddr, std::list<wireless_utils::sBssInfoConf>> bss_infos; // key=al_mac

    master_thread *m_master_thread_ctx = nullptr;
    const std::string m_local_bridge_mac;

    int m_persistent_db_clients_count = 0;
};

} // namespace son

#endif
