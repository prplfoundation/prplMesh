/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "monitor_thread.h"

#include <bcl/beerocks_logging.h>
#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_version.h>
#include <easylogging++.h>

/// Do not use this macro anywhere else in gateway process
/// It should only be there in one place and easylogging++ recommends to be in the file where
/// main function is defined.
INITIALIZE_EASYLOGGINGPP

// Do not use this macro anywhere else in ire process
// It should only be there in one place in each executable module
BEEROCKS_INIT_BEEROCKS_VERSION

static bool g_running = true;
static int s_signal   = 0;
static std::string monitor_iface;

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
    while ((opt = getopt(argc, argv, "i:")) != -1) {
        switch (opt) {
        case 'i': {
            monitor_iface.assign(optarg);
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

int main(int argc, char *argv[])
{
    init_signals();

    // Close all inhereted files descriptors.
    int openFds = sysconf(_SC_OPEN_MAX);
    for (int fd = 0; fd < openFds; fd++) {
        // Don't close stdin/stderr/stdout
        if ((fd != STDOUT_FILENO) && (fd != STDIN_FILENO) && (fd != STDERR_FILENO)) {
            close(fd);
        }
    }

    // Check for version query first, handle and exit if requested.
    std::string module_description;
    if (beerocks::version::handle_version_query(argc, argv, module_description)) {
        return 0;
    }

    //get command line options
    if (!parse_arguments(argc, argv)) {
        std::cout << "Usage: " << argv[0] << " -i <monitor iface>" << std::endl;
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

    //init logger
    std::string base_monitor_name = std::string(BEEROCKS_MONITOR) + "_" + monitor_iface;
    beerocks::logging logger(beerocks_slave_conf.sLog, base_monitor_name);
    s_pLogger = &logger;
    logger.apply_settings();
    LOG(INFO) << std::endl
              << "Running " << base_monitor_name << " Version " << BEEROCKS_VERSION
              << " Build date " << BEEROCKS_BUILD_DATE << std::endl
              << std::endl;
    beerocks::version::log_version(argc, argv);

    //redirect stdout / stderr to file
    // int fd_log_file_std = beerocks::os_utils::redirect_console_std("/dev/null");
    int fd_log_file_std = beerocks::os_utils::redirect_console_std(beerocks_slave_conf.sLog.path +
                                                                   base_monitor_name + "_std.log");

    //kill running monitor and write pid file
    beerocks::os_utils::kill_pid(beerocks_slave_conf.temp_path, base_monitor_name);
    beerocks::os_utils::write_pid_file(beerocks_slave_conf.temp_path, base_monitor_name);
    std::string pid_file_path =
        beerocks_slave_conf.temp_path + "pid/" + base_monitor_name; // for file touching

    // start monitor
    std::string slave_uds =
        beerocks_slave_conf.temp_path + std::string(BEEROCKS_SLAVE_UDS) + "_" + monitor_iface;
    son::monitor_thread monitor(slave_uds, monitor_iface, beerocks_slave_conf, logger);
    if (monitor.init()) {
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

            if (!monitor.work()) {
                break;
            }
        }
        monitor.stop();
    } else {
        LOG(ERROR) << "monitor.init(), iface=" << monitor_iface << " slave_uds=" << slave_uds;
    }

    beerocks::os_utils::close_file(fd_log_file_std);
    s_pLogger = nullptr;

    return 0;
}
