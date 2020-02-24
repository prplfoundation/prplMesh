/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "backhaul_manager/backhaul_manager_thread.h"
#include "platform_manager/platform_manager_thread.h"
#include "son_slave_thread.h"

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_utils.h>
#include <bcl/beerocks_version.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

// Do not use this macro anywhere else in ire process
// It should only be there in one place in each executable module
BEEROCKS_INIT_BEEROCKS_VERSION

static bool g_running = true;
static int s_signal   = 0;
static std::string g_son_slave_iface;

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
    while ((opt = getopt(argc, argv, "i:q:")) != -1) {
        switch (opt) {
        case 'i': {
            g_son_slave_iface.assign(optarg);
            break;
        }
        case 'q': // quary platfrom: is_master, is_gateway, is_onboarding
        {
            std::string request;
            request.assign(optarg);
            std::cout << std::endl
                      << request << "=" << beerocks::platform_manager::extern_query_db(request)
                      << std::endl;
            exit(0);
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

static std::string get_sta_iface_from_hostap_iface(const std::string &hostap_iface)
{
    // read the sta_iface from bpl and verify it is available
    char sta_iface_str[BPL_IFNAME_LEN];
    std::string sta_iface;

    if (beerocks::bpl::cfg_get_sta_iface(hostap_iface.c_str(), sta_iface_str) < 0) {
        LOG(ERROR) << "failed to read sta_iface for slave ";
        return std::string();
    } else {
        sta_iface = std::string(sta_iface_str);
        if (!beerocks::net::network_utils::linux_iface_exists(sta_iface)) {
            LOG(DEBUG) << "sta iface " << sta_iface << " does not exist, clearing it from config";
            sta_iface.clear();
        }
    }
    return sta_iface;
}

static void fill_son_slave_config(beerocks::config_file::sConfigSlave &beerocks_slave_conf,
                                  son::slave_thread::sSlaveConfig &son_slave_conf,
                                  const std::string &hostap_iface, bool no_vendor_specific,
                                  int slave_num)
{
    son_slave_conf.temp_path = beerocks_slave_conf.temp_path;
    son_slave_conf.vendor    = beerocks_slave_conf.vendor;
    son_slave_conf.model     = beerocks_slave_conf.model;
    son_slave_conf.ucc_listener_port =
        beerocks::string_utils::stoi(beerocks_slave_conf.ucc_listener_port);
    son_slave_conf.enable_keep_alive        = beerocks_slave_conf.enable_keep_alive == "1";
    son_slave_conf.debug_disable_arp        = beerocks_slave_conf.debug_disable_arp == "1";
    son_slave_conf.bridge_iface             = beerocks_slave_conf.bridge_iface;
    son_slave_conf.backhaul_preferred_bssid = beerocks_slave_conf.backhaul_preferred_bssid;
    son_slave_conf.backhaul_wire_iface      = beerocks_slave_conf.backhaul_wire_iface;
    son_slave_conf.backhaul_wire_iface_type =
        beerocks::utils::get_iface_type_from_string(beerocks_slave_conf.backhaul_wire_iface_type);
    son_slave_conf.enable_repeater_mode =
        beerocks_slave_conf.enable_repeater_mode[slave_num] == "1";
    son_slave_conf.hostap_iface_type = beerocks::utils::get_iface_type_from_string(
        beerocks_slave_conf.hostap_iface_type[slave_num]);
    son_slave_conf.hostap_iface = hostap_iface;
    son_slave_conf.hostap_ant_gain =
        beerocks::string_utils::stoi(beerocks_slave_conf.hostap_ant_gain[slave_num]);
    son_slave_conf.radio_identifier = beerocks_slave_conf.radio_identifier[slave_num];
    son_slave_conf.backhaul_wireless_iface =
        get_sta_iface_from_hostap_iface(son_slave_conf.hostap_iface);
    son_slave_conf.backhaul_wireless_iface_filter_low =
        beerocks::string_utils::stoi(beerocks_slave_conf.sta_iface_filter_low[slave_num]);
    son_slave_conf.backhaul_wireless_iface_type = son_slave_conf.hostap_iface_type;
    // son_slave_conf.no_vendor_specific = 

    // disable stopping on failure initially. Later on, it will be read from BPL as part of
    // cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE
    son_slave_conf.stop_on_failure_attempts = 0;
}

static void son_slave_watchdog(const std::string &beerocks_slave_temp_path,
                               const std::unordered_map<int, std::string> &interfaces_map)
{
    for (int slave_num = 0; slave_num < beerocks::IRE_MAX_SLAVES; slave_num++) {
        auto hostap_iface_elm = interfaces_map.find(slave_num);
        // if slave_num not mapped
        if (hostap_iface_elm == interfaces_map.end()) {
            continue;
        }

        std::string hostap_iface = hostap_iface_elm->second;
        // if slave has no interface configured
        if (hostap_iface.empty()) {
            continue;
        }
        // check if slave is still running
        std::string base_name = std::string(BEEROCKS_AGENT) + "_" + hostap_iface;
        std::string temp_path(beerocks_slave_temp_path);
        if (!beerocks::os_utils::is_pid_running(temp_path, base_name)) {
            //start new slave process
            std::string file_name = "./" + std::string(BEEROCKS_AGENT);
            if (access(file_name.c_str(), F_OK) == -1) { //file does not exist in current location
                file_name = BEEROCKS_BIN_PATH + std::string(BEEROCKS_AGENT);
            }
            std::string cmd = file_name + " -i " + hostap_iface;
            LOG(DEBUG) << "son_slave_watchdog(): sending SYSTEM_CALL with cmd = " << cmd
                       << std::endl;
            beerocks::SYSTEM_CALL(cmd, 2, true);
        }
    }
}

static int system_hang_test(beerocks::config_file::sConfigSlave &beerocks_slave_conf, int argc,
                            char *argv[])
{
    std::string name = std::string("system_hang_test");
    std::ofstream versionfile;

    //init logger
    beerocks::logging logger(beerocks_slave_conf.sLog, name);
    s_pLogger = &logger;
    logger.apply_settings();
    LOG(INFO) << std::endl
              << "Running " << name << " Version " << BEEROCKS_VERSION << " Build date "
              << BEEROCKS_BUILD_DATE << std::endl
              << std::endl;
    beerocks::version::log_version(argc, argv);

    // Redirect stdout / stderr to file
    if (logger.get_log_files_enabled()) {
        beerocks::os_utils::redirect_console_std(beerocks_slave_conf.sLog.files_path + name +
                                                 "_std.log");
    }

    //write pid file
    beerocks::os_utils::write_pid_file(beerocks_slave_conf.temp_path, name);
    std::string pid_file_path = beerocks_slave_conf.temp_path + "pid/" + name; // for file touching

    //initialize timers
    auto touch_time_stamp_timeout = std::chrono::steady_clock::now();
    auto error_time_stamp_timeout = std::chrono::steady_clock::now();

    int pid = getpid();

    while (g_running) {
        // Handle signals
        if (s_signal) {
            handle_signal();
            continue;
        }
        if (std::chrono::steady_clock::now() > touch_time_stamp_timeout) {
            LOG(INFO) << "system_hang_test, pid=" << pid;
            beerocks::os_utils::touch_pid_file(pid_file_path);
            touch_time_stamp_timeout =
                std::chrono::steady_clock::now() +
                std::chrono::seconds(beerocks::TOUCH_PID_TIMEOUT_SECONDS) / 4;

            auto err_time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(
                                     std::chrono::steady_clock::now() - error_time_stamp_timeout)
                                     .count();
            if (err_time_diff > ((1000 * beerocks::TOUCH_PID_TIMEOUT_SECONDS) / 2)) {
                LOG(ERROR) << int(err_time_diff)
                           << " msec have passed between the two last prints!";
            }
            error_time_stamp_timeout = std::chrono::steady_clock::now();
        }
        UTILS_SLEEP_MSEC(100);
    }

    s_pLogger = nullptr;

    return 0;
}

static int run_beerocks_slave(beerocks::config_file::sConfigSlave &beerocks_slave_conf,
                              const std::unordered_map<int, std::string> &interfaces_map, int argc,
                              char *argv[])
{
    std::string base_slave_name = std::string(BEEROCKS_AGENT);
    std::ofstream versionfile;

    //init logger
    beerocks::logging slave_logger(beerocks_slave_conf.sLog, base_slave_name);
    s_pLogger = &slave_logger;
    slave_logger.apply_settings();
    LOG(INFO) << std::endl
              << "Running " << base_slave_name << " Version " << BEEROCKS_VERSION << " Build date "
              << BEEROCKS_BUILD_DATE << std::endl
              << std::endl;
    beerocks::version::log_version(argc, argv);

    versionfile.open(beerocks_slave_conf.temp_path + "beerocks_slave_version");
    versionfile << BEEROCKS_VERSION << std::endl << BEEROCKS_REVISION;
    versionfile.close();

    //redirect stdout / stderr to file
    // int fd_log_file_std = beerocks::os_utils::redirect_console_std("/dev/null");
    if (slave_logger.get_log_files_enabled()) {
        beerocks::os_utils::redirect_console_std(beerocks_slave_conf.sLog.files_path +
                                                 base_slave_name + "_std.log");
    }

    //write pid file
    beerocks::os_utils::write_pid_file(beerocks_slave_conf.temp_path, base_slave_name);
    std::string pid_file_path =
        beerocks_slave_conf.temp_path + "pid/" + base_slave_name; // for file touching

    std::set<std::string> slave_ap_ifaces;
    for (auto &elm : interfaces_map) {
        if (!elm.second.empty()) {
            slave_ap_ifaces.insert(elm.second);
        }
    }

    beerocks::platform_manager::main_thread platform_mgr(beerocks_slave_conf, interfaces_map,
                                                         slave_logger);

    //start platform_manager
    if (platform_mgr.init()) {
        // read the number of failures allowed before stopping agent from platform configuration
        int stop_on_failure_attempts = beerocks::bpl::cfg_get_stop_on_failure_attempts();

        // The platform manager updates the beerocks_slave_conf.sta_iface in the init stage
        std::set<std::string> slave_sta_ifaces;
        for (int slave_num = 0; slave_num < beerocks::IRE_MAX_SLAVES; slave_num++) {
            if (!beerocks_slave_conf.sta_iface[slave_num].empty()) {
                slave_sta_ifaces.insert(beerocks_slave_conf.sta_iface[slave_num]);
            }
        }

        beerocks::backhaul_manager backhaul_mgr(beerocks_slave_conf, slave_ap_ifaces,
                                                slave_sta_ifaces, stop_on_failure_attempts);

        // Start backhaul manager
        if (!backhaul_mgr.start()) {
            LOG(ERROR) << "backhaul_mgr.start()";
        } else {
            //initialize watchdog timers
            int son_slave_watchdog_time_elapsed_ms = 0;
            std::chrono::steady_clock::time_point son_slave_watchdog_time =
                std::chrono::steady_clock::now();

            auto touch_time_stamp_timeout = std::chrono::steady_clock::now();
            while (g_running) {

                // Handle signals
                if (s_signal) {
                    handle_signal();
                    continue;
                }

                if (std::chrono::steady_clock::now() > touch_time_stamp_timeout) {
                    beerocks::os_utils::touch_pid_file(pid_file_path);
                    touch_time_stamp_timeout =
                        std::chrono::steady_clock::now() +
                        std::chrono::seconds(beerocks::TOUCH_PID_TIMEOUT_SECONDS);
                }

                if (stop_on_failure_attempts > 0 &&
                    beerocks_slave_conf.enable_son_slaves_watchdog == "1") {
                    //son_slave_watchdog periodic check son_slave pids are running
                    son_slave_watchdog_time_elapsed_ms =
                        std::chrono::duration_cast<std::chrono::milliseconds>(
                            std::chrono::steady_clock::now() - son_slave_watchdog_time)
                            .count();
                    if (son_slave_watchdog_time_elapsed_ms >=
                        beerocks::SON_SLAVE_WATCHDOG_INTERVAL_MSC) {
                        son_slave_watchdog(beerocks_slave_conf.temp_path, interfaces_map);
                        son_slave_watchdog_time = std::chrono::steady_clock::now();
                    }
                }

                // Check if backhaul manager still running and break on error
                if (!backhaul_mgr.is_running()) {
                    break;
                }

                //call platform manager work task and break on error
                if (!platform_mgr.work()) {
                    break;
                }
            }
        }

        LOG(DEBUG) << "backhaul_mgr.stop()";
        backhaul_mgr.stop();

        LOG(DEBUG) << "platform_mgr.stop()";
        platform_mgr.stop();
    }

    LOG(DEBUG) << "Bye Bye!";
    s_pLogger = nullptr;

    return 0;
}

static int run_son_slave(int slave_num, beerocks::config_file::sConfigSlave &beerocks_slave_conf,
                         const std::string &hostap_iface, bool no_vendor_specific, 
                         int argc, char *argv[])
{
    std::string base_slave_name = std::string(BEEROCKS_AGENT) + "_" + hostap_iface;

    //init logger
    beerocks::logging slave_logger(beerocks_slave_conf.sLog, base_slave_name);
    s_pLogger = &slave_logger;
    slave_logger.apply_settings();
    LOG(INFO) << std::endl
              << "Running SON " << base_slave_name << " Version " << BEEROCKS_VERSION
              << " Build date " << BEEROCKS_BUILD_DATE << std::endl
              << std::endl;
    beerocks::version::log_version(argc, argv);

    //redirect stdout / stderr to file
    //int fd_log_file_std = beerocks::os_utils::redirect_console_std("/dev/null");
    if (slave_logger.get_log_files_enabled()) {
        beerocks::os_utils::redirect_console_std(beerocks_slave_conf.sLog.files_path +
                                                 base_slave_name + "_std.log");
    }

    //write pid file
    beerocks::os_utils::write_pid_file(beerocks_slave_conf.temp_path, base_slave_name);
    std::string pid_file_path =
        beerocks_slave_conf.temp_path + "pid/" + base_slave_name; // for file touching

    // fill configuration
    son::slave_thread::sSlaveConfig son_slave_conf;
    fill_son_slave_config(beerocks_slave_conf, son_slave_conf, hostap_iface, no_vendor_specific,
                          slave_num);

    // disable stopping on failure initially. Later on, it will be read from BPL as part of
    // cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE
    son_slave_conf.stop_on_failure_attempts = 0;

    son::slave_thread son_slave(son_slave_conf, slave_logger);
    if (son_slave.init()) {
        auto touch_time_stamp_timeout = std::chrono::steady_clock::now();
        while (g_running) {

            // Handle signals
            if (s_signal) {
                handle_signal();
                continue;
            }

            if (std::chrono::steady_clock::now() > touch_time_stamp_timeout) {
                beerocks::os_utils::touch_pid_file(pid_file_path);
                touch_time_stamp_timeout =
                    std::chrono::steady_clock::now() +
                    std::chrono::seconds(beerocks::TOUCH_PID_TIMEOUT_SECONDS);
            }

            if (!son_slave.work()) {
                break;
            }
        }
        son_slave.stop();
    } else {
        LOG(ERROR) << "son_slave.init(), slave_num=" << slave_num;
    }

    s_pLogger = nullptr;

    return 0;
}

int main(int argc, char *argv[])
{
    int slave_num;

    init_signals();

    // Check for version query first, handle and exit if requested.
    std::string module_description;
    if (beerocks::version::handle_version_query(argc, argv, module_description)) {
        return 0;
    }

    //get command line options
    if (!parse_arguments(argc, argv)) {
        std::cout << "Usage: " << argv[0] << std::endl;
        return 0;
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

    // beerocks system hang tester
    if (g_son_slave_iface.empty() && beerocks_slave_conf.enable_system_hang_test == "1") {

        pid_t pid = fork();
        if (pid == 0) {
            // child process
            return system_hang_test(beerocks_slave_conf, argc, argv);
        }
    }

    beerocks::bpl::BPL_WLAN_IFACE interfaces[beerocks::IRE_MAX_SLAVES] = {0};
    int num_of_interfaces                                              = beerocks::IRE_MAX_SLAVES;
    if (beerocks::bpl::cfg_get_all_prplmesh_wifi_interfaces(interfaces, &num_of_interfaces)) {
        std::cout << "failed to read interfaces map" << std::endl;
        return 1;
    }

    //create unordered_map of interfaces
    std::unordered_map<int, std::string> interfaces_map;
    for (int i = 0; i < num_of_interfaces; i++) {
        if (beerocks::net::network_utils::linux_iface_exists(interfaces[i].ifname)) {
            LOG(DEBUG) << "radio" << i << ".hostap_iface=" << interfaces[i].ifname;
            interfaces_map[interfaces[i].radio_num] = std::string(interfaces[i].ifname);
        } else {
            // mask slave iface that do not exist
            LOG(DEBUG) << "hostap iface " << interfaces[i].ifname
                       << " does not exist, not adding it to config";
        }
    }

    if (interfaces_map.empty()) {
        LOG(INFO) << "no hostap interface is available";
        return 0;
    }

    //Handle -i option (start given son slave)
    if (!g_son_slave_iface.empty()) {
        //start given slave
        for (slave_num = 0; slave_num < beerocks::IRE_MAX_SLAVES; slave_num++) {
            auto hostap_iface_elm = interfaces_map.find(slave_num);
            if ((hostap_iface_elm != interfaces_map.end()) &&
                (g_son_slave_iface == hostap_iface_elm->second)) {
                int no_vendor_specific = beerocks::bpl::cfg_get_no_vendor_specific();
                if (no_vendor_specific == -1) {
                    LOG(ERROR) << "no_vendor_specific value not set. Assuming 0" << std::endl;
                    no_vendor_specific = 0;
                }
				LOG(DEBUG) << "no_vendor_specific = " << no_vendor_specific << " ZZZZZZZZZZZ" << std::endl;
                return run_son_slave(slave_num, beerocks_slave_conf, hostap_iface_elm->second,
                                     no_vendor_specific, argc, argv);
            }
        }
        LOG(ERROR) << "did not find g_son_slave_iface in hostap_iface array" << std::endl;
        return 0;
    }

    // killall running slave
    beerocks::os_utils::kill_pid(beerocks_slave_conf.temp_path, std::string(BEEROCKS_AGENT));

    // start all slave's
    for (slave_num = 0; slave_num < beerocks::IRE_MAX_SLAVES; slave_num++) {
        auto hostap_iface_elm = interfaces_map.find(slave_num);
        // if slave_num not mapped
        if (hostap_iface_elm == interfaces_map.end()) {
            continue;
        }

        std::string hostap_iface = hostap_iface_elm->second;
        // if slave has no interface configured
        if (hostap_iface.empty()) {
            continue;
        }
        // killall running son slave's
        std::string base_name = std::string(BEEROCKS_AGENT) + "_" + hostap_iface;
        beerocks::os_utils::kill_pid(beerocks_slave_conf.temp_path, base_name);

        //start new slave process
        LOG(INFO) << "Starting slave for iface '" << hostap_iface << "'";

        std::string file_name = "./" + std::string(BEEROCKS_AGENT);
        if (access(file_name.c_str(), F_OK) == -1) { //file does not exist in current location
            file_name = BEEROCKS_BIN_PATH + std::string(BEEROCKS_AGENT);
        }
        std::string cmd = file_name + " -i " + hostap_iface;
        beerocks::SYSTEM_CALL(cmd, 0, true);
    }

    // backhaul/platform manager slave
    return run_beerocks_slave(beerocks_slave_conf, interfaces_map, argc, argv);
}
