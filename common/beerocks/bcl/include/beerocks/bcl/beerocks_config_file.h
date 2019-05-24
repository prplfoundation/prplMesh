/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_CONFIG_FILE_H_
#define _BEEROCKS_CONFIG_FILE_H_

#include "beerocks_defines.h"
#include "beerocks_string_utils.h"

namespace beerocks {

class config_file {
public:
    typedef std::vector<std::tuple<std::string, std::string *, int>> tConfig;

    struct SConfigLog {
        //[log]
        std::string path;
        std::string global_levels;
        std::string syslog_levels;
        std::string global_size;
        std::string netlog_host;
        std::string netlog_port;
    };

    // config file parameters master / slave
    typedef struct { // master
        //[master]
        std::string temp_path;
        std::string load_dfs_reentry;
        std::string load_client_band_steering;
        std::string load_client_optimal_path_roaming;
        std::string load_client_11k_roaming;
        std::string load_legacy_client_roaming;
        std::string load_ire_roaming;
        std::string load_load_balancing;
        std::string load_service_fairness;
        std::string load_diagnostics_measurements;
        std::string load_backhaul_measurements;
        std::string load_front_measurements;
        std::string load_health_check;
        std::string load_monitor_on_vaps;
        std::string load_rdkb_extensions;
        std::string global_restricted_channels;
        std::string diagnostics_measurements_polling_rate_sec;
        std::string ire_rssi_report_rate_sec;
        std::string roaming_hysteresis_percent_bonus;
        std::string roaming_unconnected_client_rssi_compensation_db;
        std::string roaming_hop_percent_penalty;
        std::string roaming_band_pathloss_delta_db;
        std::string roaming_5ghz_failed_attemps_threshold;
        std::string roaming_24ghz_failed_attemps_threshold;
        std::string roaming_11v_failed_attemps_threshold;
        std::string roaming_rssi_cutoff_db;
        std::string monitor_total_channel_load_notification_lo_th_percent;
        std::string monitor_total_channel_load_notification_hi_th_percent;
        std::string monitor_total_channel_load_notification_delta_th_percent;
        std::string monitor_min_active_clients;
        std::string monitor_active_client_th;
        std::string monitor_client_load_notification_delta_th_percent;
        std::string monitor_rx_rssi_notification_threshold_dbm;
        std::string monitor_rx_rssi_notification_delta_db;
        std::string monitor_ap_idle_threshold_B;
        std::string monitor_ap_active_threshold_B;
        std::string monitor_ap_idle_stable_time_sec;
        std::string monitor_disable_initiative_arp;
        std::string slave_keep_alive_retries;
        std::string channel_selection_random_delay;
        std::string fail_safe_5G_frequency;
        std::string fail_safe_5G_bw;
        std::string fail_safe_5G_vht_frequency;
        std::string channel_selection_long_delay;
        std::string roaming_sticky_client_rssi_threshold;
        std::string credentials_change_timeout_sec;
        //[log]
        SConfigLog sLog;
    } sConfigMaster;

    typedef struct { // slave
        //[global]
        std::string platform;
        std::string temp_path;
        std::string enable_arp_monitor;
        std::string enable_keep_alive;
        std::string enable_credentials_automatic_unify;
        std::string debug_disable_arp;
        std::string enable_bpl_iface_status_notifications;
        std::string bridge_iface;
        std::string backhaul_preferred_bssid;
        std::string backhaul_wire_iface;
        std::string backhaul_wire_iface_type;
        std::string stop_on_failure_attempts;
        std::string enable_system_hang_test;
        std::string enable_son_slaves_watchdog;
        std::string const_backhaul_slave;
        //[slaveX]
        std::string radio_identifier[IRE_MAX_SLAVES]; // mAP RUID
        std::string enable_repeater_mode[IRE_MAX_SLAVES];
        std::string hostap_iface_type[IRE_MAX_SLAVES];
        std::string hostap_iface[IRE_MAX_SLAVES];
        std::string sta_iface[IRE_MAX_SLAVES];
        std::string sta_iface_filter_low[IRE_MAX_SLAVES];
        std::string hostap_ant_gain[IRE_MAX_SLAVES];
        //[log]
        SConfigLog sLog;
    } sConfigSlave;

    static bool read_config_file(std::string config_file_path, tConfig &conf,
                                 std::string config_type);
    static bool read_master_config_file(std::string config_file_path, sConfigMaster &conf);
    static bool read_slave_config_file(std::string config_file_path, sConfigSlave &conf);
};

} //  namespace beerocks

#endif
