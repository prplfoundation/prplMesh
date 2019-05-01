/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../include/beerocks/bcl/beerocks_config_file.h"
#include "../include/beerocks/bcl/beerocks_string_utils.h"

#include <tuple>
#include <fstream>
#include <iostream>

using namespace beerocks;

static bool read_log_section(std::string config_file_path, config_file::SConfigLog &sLogConf)
{
    int mandatory=1;
    int optional=0;

    config_file::tConfig log_conf_args = {
        std::make_tuple("log_path=", &sLogConf.path, mandatory),
        std::make_tuple("log_global_levels=", &sLogConf.global_levels, mandatory),
        std::make_tuple("log_global_syslog_levels=", &sLogConf.syslog_levels, mandatory),
        std::make_tuple("log_global_size=", &sLogConf.global_size, mandatory),
        std::make_tuple("log_netlog_host=", &sLogConf.netlog_host, optional),
        std::make_tuple("log_netlog_port=", &sLogConf.netlog_port, optional)
    };
    
    std::string section = "log";
    bool ret_val = config_file::read_config_file(config_file_path, log_conf_args, section);

    // check path has trailing slash
    if(sLogConf.path.back() != '/'){
        sLogConf.path += '/';
    }
    return ret_val;
}

bool config_file::read_master_config_file(std::string config_file_path, sConfigMaster &conf)
{
    int mandatory_master=1;
    std::string config_type = "controller";

    //                      file string  config string  type mandatory
    tConfig master_conf_args = {
        std::make_tuple("temp_path=", &conf.temp_path, mandatory_master),
        std::make_tuple("load_dfs_reentry=", &conf.load_dfs_reentry, 0),
        std::make_tuple("load_client_band_steering=", &conf.load_client_band_steering, mandatory_master),
        std::make_tuple("load_client_optimal_path_roaming=", &conf.load_client_optimal_path_roaming, mandatory_master),
        std::make_tuple("load_client_11k_roaming=", &conf.load_client_11k_roaming, mandatory_master),
        std::make_tuple("load_legacy_client_roaming=", &conf.load_legacy_client_roaming, mandatory_master),
        std::make_tuple("load_ire_roaming=", &conf.load_ire_roaming, mandatory_master),
        std::make_tuple("load_load_balancing=", &conf.load_load_balancing, mandatory_master),
        std::make_tuple("load_service_fairness=", &conf.load_service_fairness, mandatory_master),
        std::make_tuple("load_diagnostics_measurements=", &conf.load_diagnostics_measurements, 0),
        std::make_tuple("load_backhaul_measurements=", &conf.load_backhaul_measurements, 0),
        std::make_tuple("load_front_measurements=", &conf.load_front_measurements, 0),
        std::make_tuple("load_health_check=", &conf.load_health_check, 0),
        std::make_tuple("load_monitor_on_vaps=", &conf.load_monitor_on_vaps, 0),
        std::make_tuple("load_rdkb_extensions=", &conf.load_rdkb_extensions, 0),
        std::make_tuple("global_restricted_channels=", &conf.global_restricted_channels, 0),
        std::make_tuple("diagnostics_measurements_polling_rate_sec=", &conf.diagnostics_measurements_polling_rate_sec, 0),
        std::make_tuple("ire_rssi_report_rate_sec=", &conf.ire_rssi_report_rate_sec, mandatory_master),
        std::make_tuple("roaming_hysteresis_percent_bonus=", &conf.roaming_hysteresis_percent_bonus, mandatory_master),
        std::make_tuple("roaming_unconnected_client_rssi_compensation_db=", &conf.roaming_unconnected_client_rssi_compensation_db, mandatory_master),
        std::make_tuple("roaming_hop_percent_penalty=", &conf.roaming_hop_percent_penalty, mandatory_master),
        std::make_tuple("roaming_band_pathloss_delta_db=", &conf.roaming_band_pathloss_delta_db, mandatory_master),
        std::make_tuple("roaming_5ghz_failed_attemps_threshold=", &conf.roaming_5ghz_failed_attemps_threshold, mandatory_master),
        std::make_tuple("roaming_24ghz_failed_attemps_threshold=", &conf.roaming_24ghz_failed_attemps_threshold, mandatory_master),
        std::make_tuple("roaming_11v_failed_attemps_threshold=", &conf.roaming_11v_failed_attemps_threshold, mandatory_master),
        std::make_tuple("roaming_rssi_cutoff_db=", &conf.roaming_rssi_cutoff_db, mandatory_master),
        std::make_tuple("roaming_sticky_client_rssi_threshold=", &conf.roaming_sticky_client_rssi_threshold, mandatory_master),
        std::make_tuple("monitor_total_channel_load_notification_hi_th_percent=", &conf.monitor_total_channel_load_notification_hi_th_percent, mandatory_master),
        std::make_tuple("monitor_total_channel_load_notification_lo_th_percent=", &conf.monitor_total_channel_load_notification_lo_th_percent, mandatory_master),
        std::make_tuple("monitor_total_channel_load_notification_delta_th_percent=", &conf.monitor_total_channel_load_notification_delta_th_percent, mandatory_master),
        std::make_tuple("monitor_min_active_clients=", &conf.monitor_min_active_clients, mandatory_master),
        std::make_tuple("monitor_active_client_th=", &conf.monitor_active_client_th, mandatory_master),
        std::make_tuple("monitor_client_load_notification_delta_th_percent=", &conf.monitor_client_load_notification_delta_th_percent, mandatory_master),
        std::make_tuple("monitor_ap_idle_threshold_B=", &conf.monitor_ap_idle_threshold_B, mandatory_master),
        std::make_tuple("monitor_ap_active_threshold_B=", &conf.monitor_ap_active_threshold_B, mandatory_master),
        std::make_tuple("monitor_ap_idle_stable_time_sec=", &conf.monitor_ap_idle_stable_time_sec, mandatory_master),
        std::make_tuple("monitor_rx_rssi_notification_threshold_dbm=", &conf.monitor_rx_rssi_notification_threshold_dbm, mandatory_master),
        std::make_tuple("monitor_rx_rssi_notification_delta_db=", &conf.monitor_rx_rssi_notification_delta_db, mandatory_master),
        std::make_tuple("monitor_disable_initiative_arp=", &conf.monitor_disable_initiative_arp, 0),
        std::make_tuple("slave_keep_alive_retries=", &conf.slave_keep_alive_retries, 0),
        std::make_tuple("channel_selection_random_delay=", &conf.channel_selection_random_delay, mandatory_master),
        std::make_tuple("fail_safe_5G_frequency=", &conf.fail_safe_5G_frequency, mandatory_master),
        std::make_tuple("fail_safe_5G_bw=", &conf.fail_safe_5G_bw, mandatory_master),
        std::make_tuple("fail_safe_5G_vht_frequency=", &conf.fail_safe_5G_vht_frequency, mandatory_master),
        std::make_tuple("channel_selection_long_delay=", &conf.channel_selection_long_delay, mandatory_master),
        std::make_tuple("credentials_change_timeout_sec=", &conf.credentials_change_timeout_sec, mandatory_master),
    };

    bool ret_val = (read_config_file(config_file_path, master_conf_args, config_type) &&
                    read_log_section(config_file_path, conf.sLog));

    // check temp_path has trailing slash
    if(conf.temp_path.back() != '/'){
        conf.temp_path += '/';
    }
    return ret_val;       
}

bool config_file::read_slave_config_file(std::string config_file_path, sConfigSlave &conf)
{
    int mandatory_slave=1;

    {
        tConfig slave_global_conf_args = {
            std::make_tuple("platform=", &conf.platform,  mandatory_slave),
            std::make_tuple("temp_path=", &conf.temp_path,  mandatory_slave),
            std::make_tuple("enable_arp_monitor=", &conf.enable_arp_monitor,  mandatory_slave),
            std::make_tuple("enable_keep_alive=", &conf.enable_keep_alive,  mandatory_slave),
            std::make_tuple("enable_credentials_automatic_unify=", &conf.enable_credentials_automatic_unify, mandatory_slave),
            std::make_tuple("debug_disable_arp=", &conf.debug_disable_arp, 0),
            std::make_tuple("enable_bpl_iface_status_notifications=", &conf.enable_bpl_iface_status_notifications,  mandatory_slave),
            std::make_tuple("bridge_iface=", &conf.bridge_iface,  0),
            std::make_tuple("stop_on_failure_attempts=", &conf.stop_on_failure_attempts,  0),
            std::make_tuple("enable_system_hang_test=", &conf.enable_system_hang_test, 0),
            std::make_tuple("const_backhaul_slave=", &conf.const_backhaul_slave, 0),
        };
        std::string config_type = "global";
        if(!read_config_file(config_file_path, slave_global_conf_args, config_type)){
            return false;
        }

        // check temp_path has trailing slash
        if(conf.temp_path.back() != '/'){
            conf.temp_path += '/';
        }
    }


    {
        tConfig slave_global_conf_args = {
            std::make_tuple("backhaul_preferred_bssid=", &conf.backhaul_preferred_bssid,  0),
            std::make_tuple("backhaul_wire_iface_type=", &conf.backhaul_wire_iface_type, mandatory_slave),
            std::make_tuple("backhaul_wire_iface=", &conf.backhaul_wire_iface, mandatory_slave),
        };
        std::string config_type = "backhaul";
        if(!read_config_file(config_file_path, slave_global_conf_args, config_type)){
            return false;
        }
    }
    
    for(int slave_num=0;slave_num<IRE_MAX_SLAVES;slave_num++){
        if(slave_num > 0) mandatory_slave=0;
        tConfig slave_conf_args = {
            std::make_tuple("radio_identifier=", &conf.radio_identifier[slave_num], mandatory_slave),
            std::make_tuple("enable_repeater_mode=", &conf.enable_repeater_mode[slave_num], mandatory_slave),
            std::make_tuple("hostap_iface_type=", &conf.hostap_iface_type[slave_num], mandatory_slave),
            std::make_tuple("hostap_iface=", &conf.hostap_iface[slave_num], mandatory_slave),
            std::make_tuple("hostap_ant_gain=", &conf.hostap_ant_gain[slave_num], mandatory_slave),
            std::make_tuple("sta_iface=", &conf.sta_iface[slave_num], 0),
            std::make_tuple("sta_iface_filter_low=", &conf.sta_iface_filter_low[slave_num], 0),
        };
        std::string config_type = "agent" + std::to_string(slave_num);
        if(!read_config_file(config_file_path, slave_conf_args, config_type)){
            return false;
        }
    }

    return read_log_section(config_file_path, conf.sLog);
}

static std::string getFileName(const std::string& s) {

   char sep = '/';

   size_t i = s.rfind(sep, s.length());
   if (i != std::string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return("");
}


bool config_file::read_config_file(std::string config_file_path, tConfig &conf_args, std::string config_type)
{
    std::ifstream in_conf_file;
    std::string line;
    size_t pos;

    //First check if there is config file in /tmp directory and use it if exist
    std::string tmp_config_file_path = "/tmp/" + getFileName(config_file_path);
    in_conf_file.open(tmp_config_file_path);
    if (in_conf_file.is_open()) {
        //Config file exist in /tmp - use it
        config_file_path = tmp_config_file_path;
        std::cout << "Override the original config file with: " << config_file_path << std::endl;
    } else {
        //Not exist in /tmp - use the original path
        in_conf_file.open(config_file_path);
        if (!in_conf_file.is_open()) {
           return false;
        }
    }

    bool read_config=false;
    while (std::getline(in_conf_file, line)) {
        string_utils::ltrim(line);
        string_utils::rtrim(line);
        if (!line.empty()) {
            if (line.at(0) == '#') continue;
            if (line.at(0) == '[') {
                read_config=false;
                if (line.find(config_type) != std::string::npos) {
                    read_config=true;
                }
                continue;
            }
            if(!read_config) continue;
            
            bool hit = false;
            for (auto & conf : conf_args) {
                auto arg = std::get<0>(conf);
                if ( line.compare(0, arg.size(), arg) == 0 ) {
                    hit = true;
                    std::string line_arg = line.substr(arg.size(), line.size());
                    pos = line_arg.find("#");
                    if(pos != std::string::npos){
                        line_arg.erase(pos, line_arg.size());
                        string_utils::rtrim(line_arg);
                    }
                    if (line_arg.size() >= 1) {
                        std::get<1>(conf)->assign(line_arg);
                    }
                    break;
                }
            }
            if (!hit) {
                std::cout << "unknown configuration line '" << line << "'" << std::endl;
                std::cout << "Valid configuration arguments:" << std::endl;
                for (auto & conf : conf_args) {
                    std::cout <<  "    " << std::get<0>(conf) << ", type=" << config_type << ", mandatory=" << int(std::get<2>(conf)) << std::endl;
                }
                return false;
            }
        }
    }
    in_conf_file.close();

    // check mandatory
    for (auto & conf : conf_args) {
        if (std::get<1>(conf)->empty() && (std::get<2>(conf) != 0) ) {
            std::cout  << config_file_path << ": mandatory config arg not set: " << std::get<0>(conf) << " ,type:" << std::get<2>(conf) << std::endl;
            return false;
        }
    }
    return true;
}
