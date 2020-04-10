/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_version.h>
#include <bcl/network/network_utils.h>
#include <bpl/bpl_cfg.h>
#include <easylogging++.h>

#include "db/db.h"
#include "son_master_thread.h"

// #include <string>

// Do not use this macro anywhere else in ire process
// It should only be there in one place in each executable module
BEEROCKS_INIT_BEEROCKS_VERSION

static bool g_running     = true;
static bool s_kill_master = false;
static int s_signal       = 0;

// Pointer to logger instance
static beerocks::logging *s_pLogger = nullptr;

static void handle_signal()
{
    if (!s_signal)
        return;

    switch (s_signal) {

    // Terminate
    case SIGTERM:
    case SIGINT:
        LOG(INFO) << "Caught signal '" << strsignal(s_signal) << "' Exiting...";
        g_running = false;
        break;

    // Roll log file
    case SIGUSR1: {
        LOG(INFO) << "LOG Roll Signal!";
        if (!s_pLogger) {
            LOG(ERROR) << "Invalid logger pointer!";
            return;
        }

        s_pLogger->apply_settings();
        LOG(INFO) << "--- Start of file after roll ---";
        break;
    }

    default:
        LOG(WARNING) << "Unhandled Signal: '" << strsignal(s_signal) << "' Ignoring...";
        break;
    }

    s_signal = 0;
}

static void init_signals()
{
    // Signal handler function
    auto signal_handler = [](int signum) { s_signal = signum; };

    struct sigaction sigterm_action;
    sigterm_action.sa_handler = signal_handler;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_flags = 0;
    sigaction(SIGTERM, &sigterm_action, NULL);

    struct sigaction sigint_action;
    sigint_action.sa_handler = signal_handler;
    sigemptyset(&sigint_action.sa_mask);
    sigint_action.sa_flags = 0;
    sigaction(SIGINT, &sigint_action, NULL);

    struct sigaction sigusr1_action;
    sigusr1_action.sa_handler = signal_handler;
    sigemptyset(&sigusr1_action.sa_mask);
    sigusr1_action.sa_flags = 0;
    sigaction(SIGUSR1, &sigusr1_action, NULL);
}

static bool parse_arguments(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "k")) != -1) {
        switch (opt) {
        case 'k': {
            s_kill_master = true;
            break;
        }
        case '?': {
            if (isprint(optopt)) {
                LOG(ERROR) << "Unknown option -" << optopt << "!";
                return false;
            } else {
                LOG(ERROR) << "Unknown character " << optopt << "!";
                return false;
            }
            break;
        }
        }
    }
    return true;
}

static void fill_master_config(son::db::sDbMasterConfig &master_conf,
                               beerocks::config_file::sConfigMaster &main_master_conf)
{
    master_conf.vendor = main_master_conf.vendor;
    master_conf.model  = main_master_conf.model;
    master_conf.ucc_listener_port =
        beerocks::string_utils::stoi(main_master_conf.ucc_listener_port);
    master_conf.load_ire_roaming          = (main_master_conf.load_ire_roaming == "1");
    master_conf.load_service_fairness     = (main_master_conf.load_service_fairness == "1");
    master_conf.load_dfs_reentry          = (main_master_conf.load_dfs_reentry == "1");
    master_conf.load_rdkb_extensions      = (main_master_conf.load_rdkb_extensions == "1");
    master_conf.load_client_band_steering = (main_master_conf.load_client_band_steering == "1");
    master_conf.load_client_optimal_path_roaming =
        (main_master_conf.load_client_optimal_path_roaming == "1");
    master_conf.load_client_11k_roaming    = (main_master_conf.load_client_11k_roaming == "1");
    master_conf.load_legacy_client_roaming = (main_master_conf.load_legacy_client_roaming == "1");
    master_conf.load_load_balancing        = (main_master_conf.load_load_balancing == "1");
    master_conf.load_diagnostics_measurements =
        (main_master_conf.load_diagnostics_measurements == "1");
    master_conf.load_backhaul_measurements = (main_master_conf.load_backhaul_measurements == "1");
    master_conf.load_front_measurements    = (main_master_conf.load_front_measurements == "1");
    master_conf.load_health_check          = (main_master_conf.load_health_check == "1");
    master_conf.load_monitor_on_vaps       = (main_master_conf.load_monitor_on_vaps == "1");
    master_conf.diagnostics_measurements_polling_rate_sec =
        beerocks::string_utils::stoi(main_master_conf.diagnostics_measurements_polling_rate_sec);
    master_conf.ire_rssi_report_rate_sec =
        beerocks::string_utils::stoi(main_master_conf.ire_rssi_report_rate_sec);
    master_conf.roaming_hysteresis_percent_bonus =
        beerocks::string_utils::stoi(main_master_conf.roaming_hysteresis_percent_bonus);
    master_conf.roaming_unconnected_client_rssi_compensation_db = beerocks::string_utils::stoi(
        main_master_conf.roaming_unconnected_client_rssi_compensation_db);
    master_conf.roaming_hop_percent_penalty =
        beerocks::string_utils::stoi(main_master_conf.roaming_hop_percent_penalty);
    master_conf.roaming_band_pathloss_delta_db =
        beerocks::string_utils::stoi(main_master_conf.roaming_band_pathloss_delta_db);
    master_conf.roaming_5ghz_failed_attemps_threshold =
        beerocks::string_utils::stoi(main_master_conf.roaming_5ghz_failed_attemps_threshold);
    master_conf.roaming_24ghz_failed_attemps_threshold =
        beerocks::string_utils::stoi(main_master_conf.roaming_24ghz_failed_attemps_threshold);
    master_conf.roaming_11v_failed_attemps_threshold =
        beerocks::string_utils::stoi(main_master_conf.roaming_11v_failed_attemps_threshold);
    master_conf.roaming_rssi_cutoff_db =
        beerocks::string_utils::stoi(main_master_conf.roaming_rssi_cutoff_db);
    master_conf.monitor_total_ch_load_notification_lo_th_percent = beerocks::string_utils::stoi(
        main_master_conf.monitor_total_channel_load_notification_lo_th_percent);
    master_conf.monitor_total_ch_load_notification_hi_th_percent = beerocks::string_utils::stoi(
        main_master_conf.monitor_total_channel_load_notification_hi_th_percent);
    master_conf.monitor_total_ch_load_notification_delta_th_percent = beerocks::string_utils::stoi(
        main_master_conf.monitor_total_channel_load_notification_delta_th_percent);
    master_conf.monitor_min_active_clients =
        beerocks::string_utils::stoi(main_master_conf.monitor_min_active_clients);
    master_conf.monitor_active_client_th =
        beerocks::string_utils::stoi(main_master_conf.monitor_active_client_th);
    master_conf.monitor_client_load_notification_delta_th_percent = beerocks::string_utils::stoi(
        main_master_conf.monitor_client_load_notification_delta_th_percent);
    master_conf.monitor_ap_idle_threshold_B =
        beerocks::string_utils::stoi(main_master_conf.monitor_ap_idle_threshold_B);
    master_conf.monitor_ap_active_threshold_B =
        beerocks::string_utils::stoi(main_master_conf.monitor_ap_active_threshold_B);
    master_conf.monitor_ap_idle_stable_time_sec =
        beerocks::string_utils::stoi(main_master_conf.monitor_ap_idle_stable_time_sec);
    master_conf.monitor_rx_rssi_notification_threshold_dbm =
        beerocks::string_utils::stoi(main_master_conf.monitor_rx_rssi_notification_threshold_dbm);
    master_conf.monitor_rx_rssi_notification_delta_db =
        beerocks::string_utils::stoi(main_master_conf.monitor_rx_rssi_notification_delta_db);
    master_conf.monitor_disable_initiative_arp =
        beerocks::string_utils::stoi(main_master_conf.monitor_disable_initiative_arp);
    master_conf.slave_keep_alive_retries =
        beerocks::string_utils::stoi(main_master_conf.slave_keep_alive_retries);
    master_conf.channel_selection_random_delay =
        beerocks::string_utils::stoi(main_master_conf.channel_selection_random_delay);
    master_conf.fail_safe_5G_frequency =
        beerocks::string_utils::stoi(main_master_conf.fail_safe_5G_frequency);
    master_conf.fail_safe_5G_bw = beerocks::string_utils::stoi(main_master_conf.fail_safe_5G_bw);
    master_conf.fail_safe_5G_vht_frequency =
        beerocks::string_utils::stoi(main_master_conf.fail_safe_5G_vht_frequency);
    master_conf.channel_selection_long_delay =
        beerocks::string_utils::stoi(main_master_conf.channel_selection_long_delay);
    master_conf.roaming_sticky_client_rssi_threshold =
        beerocks::string_utils::stoi(main_master_conf.roaming_sticky_client_rssi_threshold);
    master_conf.credentials_change_timeout_sec =
        beerocks::string_utils::stoi(main_master_conf.credentials_change_timeout_sec);
    // get channel vector
    std::string s         = main_master_conf.global_restricted_channels;
    std::string delimiter = ",";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        master_conf.global_restricted_channels.push_back(beerocks::string_utils::stoi(token));
        s.erase(0, pos + delimiter.length());
    }
    if (!s.empty()) {
        master_conf.global_restricted_channels.push_back(beerocks::string_utils::stoi(s));
    }

    // platform settings
    master_conf.certification_mode = beerocks::bpl::cfg_get_certification_mode();
    char load_steer_on_vaps[BPL_LOAD_STEER_ON_VAPS_LEN] = {0};
    if (beerocks::bpl::cfg_get_load_steer_on_vaps(beerocks::IRE_MAX_SLAVES, load_steer_on_vaps) <
        0) {
        master_conf.load_steer_on_vaps = std::string();
    } else {
        master_conf.load_steer_on_vaps = std::string(load_steer_on_vaps);
    }
}

int main(int argc, char *argv[])
{
    init_signals();

    // Check for version query first, handle and exit if requested.
    std::string module_description;
    std::ofstream versionfile;
    if (beerocks::version::handle_version_query(argc, argv, module_description)) {
        return 0;
    }

    //get command line options
    if (!parse_arguments(argc, argv)) {
        std::cout << "Usage: " << argv[0] << " -k {kill master}" << std::endl;
        return 1;
    }

    // read master config file
    std::string master_config_file_path =
        "./" + std::string(BEEROCKS_CONTROLLER) + ".conf"; //search first in current directory
    beerocks::config_file::sConfigMaster beerocks_master_conf;
    if (!beerocks::config_file::read_master_config_file(master_config_file_path,
                                                        beerocks_master_conf)) {
        master_config_file_path = BEEROCKS_CONF_PATH + std::string(BEEROCKS_CONTROLLER) +
                                  ".conf"; // if not found, search in beerocks path
        if (!beerocks::config_file::read_master_config_file(master_config_file_path,
                                                            beerocks_master_conf)) {
            std::cout << "config file '" << master_config_file_path << "' args error." << std::endl;
            return 1;
        }
    }

    // read slave config file
    std::string slave_config_file_path =
        "./" + std::string(BEEROCKS_AGENT) + ".conf"; //search first in current directory
    beerocks::config_file::sConfigSlave beerocks_slave_conf;
    if (!beerocks::config_file::read_slave_config_file(slave_config_file_path,
                                                       beerocks_slave_conf)) {
        slave_config_file_path = BEEROCKS_CONF_PATH + std::string(BEEROCKS_AGENT) +
                                 ".conf"; // if not found, search in beerocks path
        if (!beerocks::config_file::read_slave_config_file(slave_config_file_path,
                                                           beerocks_slave_conf)) {
            std::cout << "config file '" << slave_config_file_path << "' args error." << std::endl;
            return 1;
        }
    }

    std::string base_master_name = std::string(BEEROCKS_CONTROLLER);

    //kill running master
    beerocks::os_utils::kill_pid(beerocks_master_conf.temp_path, base_master_name);

    // only kill and exit
    if (s_kill_master) {
        return 0;
    }

    //init logger
    beerocks::logging logger(beerocks_master_conf.sLog, base_master_name);
    s_pLogger = &logger;
    logger.apply_settings();
    LOG(INFO) << std::endl
              << "Running " << base_master_name << " Version " << BEEROCKS_VERSION << " Build date "
              << BEEROCKS_BUILD_DATE << std::endl
              << std::endl;
    beerocks::version::log_version(argc, argv);
    versionfile.open(beerocks_master_conf.temp_path + "beerocks_master_version");
    versionfile << BEEROCKS_VERSION << std::endl << BEEROCKS_REVISION;
    versionfile.close();

    // Redirect stdout / stderr
    if (logger.get_log_files_enabled()) {
        beerocks::os_utils::redirect_console_std(beerocks_master_conf.sLog.files_path +
                                                 base_master_name + "_std.log");
    }

    //write pid file
    beerocks::os_utils::write_pid_file(beerocks_master_conf.temp_path, base_master_name);
    std::string pid_file_path =
        beerocks_master_conf.temp_path + "pid/" + base_master_name; // for file touching

    // fill master configuration
    son::db::sDbMasterConfig master_conf;
    fill_master_config(master_conf, beerocks_master_conf);

    std::string master_uds = beerocks_master_conf.temp_path + std::string(BEEROCKS_MASTER_UDS);
    beerocks::net::network_utils::iface_info bridge_info;
    auto &bridge_iface = beerocks_slave_conf.bridge_iface;
    if (beerocks::net::network_utils::get_iface_info(bridge_info, bridge_iface) != 0) {
        LOG(ERROR) << "Failed reading addresses from the bridge!";
        return 0;
    }

    son::db master_db(master_conf, logger, bridge_info.mac);
    LOG(DEBUG) << "slave_keep_alive_retries=" << master_db.config.slave_keep_alive_retries;
    // diagnostics_thread diagnostics(master_db);
    son::master_thread son_master(master_uds, master_db);

    if (!son_master.init()) {
        LOG(ERROR) << "son_master.init() ";
        g_running = false;
    }

    auto touch_time_stamp_timeout = std::chrono::steady_clock::now();
    while (g_running) {

        // Handle signals
        if (s_signal) {
            handle_signal();
            continue;
        }

        if (std::chrono::steady_clock::now() > touch_time_stamp_timeout) {
            beerocks::os_utils::touch_pid_file(pid_file_path);
            touch_time_stamp_timeout = std::chrono::steady_clock::now() +
                                       std::chrono::seconds(beerocks::TOUCH_PID_TIMEOUT_SECONDS);
        }

        if (!son_master.work()) {
            break;
        }
    }

    s_pLogger = nullptr;

    son_master.stop();

    return 0;
}
