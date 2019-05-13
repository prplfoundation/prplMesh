/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_cli.h"
#include "beerocks_cli_bml.h"
#include "beerocks_cli_proxy.h"
#include "beerocks_cli_socket.h"

#include <beerocks/bcl/beerocks_config_file.h>
#include <beerocks/bcl/beerocks_defines.h>
#include <beerocks/bcl/beerocks_os_utils.h>
#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/beerocks_version.h>
#include <easylogging++.h>

#include <chrono>

INITIALIZE_EASYLOGGINGPP

// Do not use this macro anywhere else in ire process
// It should only be there in one place in each executable module
BEEROCKS_INIT_BEEROCKS_VERSION

bool g_running       = true;
bool g_loop_cmd_exec = false;
long g_wait_time     = 0;
std::vector<std::string> g_cli_cmds;

#define BML_PREFIX "bml_"

static void clear_screen(void) { std::cout << "\033[2J"; }

// static void clear_screen_above_cursor(void)
// {
//     std::cout << "\033[1J";
// }

static void set_cursor(int n, int m)
{
    // (n,m) = (1,1) -> top left corner
    std::cout << "\033[" << n << ";" << m << "H";
}

static void sigterm_handler()
{
    fclose(stdin);
    g_running = false;
}

// Invoked when sigint is invoked
static void sigint_handler()
{
    if (g_loop_cmd_exec) {
        g_loop_cmd_exec = false;
        clear_screen();
        set_cursor(1, 1);
        std::cout << "Stopped..." << std::endl << std::endl;
    } else {
        fclose(stdin);
        g_running = false;
    }
}

// Initialize the calls to handle Ctrl+C and Ctrl+x
static void init_signals()
{
    struct sigaction sigterm_action;
    sigterm_action.sa_handler = (__sighandler_t)sigterm_handler;
    sigemptyset(&sigterm_action.sa_mask);
    sigterm_action.sa_flags = 0;
    sigaction(SIGTERM, &sigterm_action, NULL);

    struct sigaction sigint_action;
    sigint_action.sa_handler = (__sighandler_t)sigint_handler;
    sigemptyset(&sigint_action.sa_mask);
    sigint_action.sa_flags = 0;
    sigaction(SIGINT, &sigint_action, NULL);
}

static void init_logger(bool to_standard_output, std::string logfile)
{
    el::Configurations defaultConf;
    if (!logfile.empty()) {
        std::remove(logfile.c_str());
    }
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::MaxLogFileSize, "10000000");
    defaultConf.setGlobally(el::ConfigurationType::Format,
                            "%level %datetime{%H:%m:%s} %fbase %line --> %msg");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, logfile.empty() ? "false" : "true");
    defaultConf.setGlobally(el::ConfigurationType::Filename, logfile.c_str());
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput,
                            to_standard_output ? "true" : "false");
    el::Loggers::addFlag(el::LoggingFlag::StrictLogFileSizeCheck);
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

static void *xmalloc(int alloc_size)
{
    void *m = malloc(alloc_size);
    if (m == nullptr) {
        LOG(FATAL) << "Error: Out of memory. Exiting";
        sigterm_handler();
        return nullptr;
    }
    return m;
}

static char *dupstr(const std::string &s)
{
    auto r_len = s.size() + 1;
    char *r    = (char *)xmalloc(r_len);
    if (r == nullptr) {
        LOG(FATAL) << "Error: Out of memory. Exiting";
        sigterm_handler();
        return nullptr;
    }
    beerocks::string_utils::copy_string(r, s.c_str(), r_len);
    return r;
}

static char *command_generator(const char *text, int state)
{
    static int list_index;

    std::string text_str(text);

    if (!state) {
        list_index = 0;
    }

    while (true) {
        list_index++;
        if (list_index >= int(g_cli_cmds.size()))
            break;
        if (g_cli_cmds[list_index].find(text_str.c_str(), 0, text_str.size()) !=
            std::string::npos) {
            return dupstr(g_cli_cmds[list_index]);
        }
    }
    return NULL;
}

static char **command_tab_completion(const char *text, int start, int end)
{
    char **matches;

    if (start == 0) {
        matches = rl_completion_matches((char *)text, &command_generator);
    } else {
        rl_bind_key('\t', rl_abort);
        matches = (char **)NULL;
    }
    return (matches);
}

static int repeated_cmd_parser(std::string &cmd)
{
    // searching for special signs to recognize repeated mode
    int executions     = 1; // -1 = inf , other = num of executions
    g_loop_cmd_exec    = false;
    auto commandTokens = beerocks::string_utils::str_split(cmd, ' ');
    cmd                = std::string();
    std::string::size_type pos;
    std::string num_str;
    for (auto token = commandTokens.begin(); token != commandTokens.end(); ++token) {
        if ((pos = token->find("*")) != std::string::npos) {
            num_str         = token->substr(pos + sizeof('*'));
            executions      = beerocks::string_utils::stoi(num_str);
            executions      = (executions > 0) ? executions : 0;
            g_loop_cmd_exec = true;
            commandTokens.erase(token);
            break;

        } else if ((pos = token->find("!")) != std::string::npos) {
            executions      = -1;
            g_loop_cmd_exec = true;
            commandTokens.erase(token);
            break;
        }
    }

    for (auto &token : commandTokens) {
        cmd += token + " ";
    }

    if (cmd.back() == ' ') {
        cmd.pop_back(); // remove last space
    }

    return executions;
}

static int cli_non_interactive(std::string path, std::string tmp_path, std::string ip,
                               std::string command_string)
{
    int status_code = 0;
    beerocks::cli_socket cli_soc(tmp_path, ip);
    beerocks::cli_bml cli_bml(path);
    beerocks::cli *cli_ptr = &cli_soc;
    std::string bml_prefix(BML_PREFIX);

    if (command_string.compare(0, bml_prefix.size(), bml_prefix)) { // Socket
        cli_ptr = &cli_soc;
        if (!cli_soc.connect()) {
            std::cout << "cli_socket: Can't connect to master, exit..." << std::endl;
            return 1;
        }
        if (!cli_soc.start()) {
            std::cout << "cli_socket: Can't start cli, exit" << std::endl;
            return 1;
        }
    } else { // BML
        cli_ptr = &cli_bml;
        if (!cli_bml.connect()) {
            std::cout << "cli_bml: Can't connect to master, exit..." << std::endl;
            return 1;
        }
    }

    if (-1 == cli_ptr->multiple_commands(command_string)) {
        status_code = 1;
    }

    UTILS_SLEEP_MSEC(1000 * g_wait_time);

    // This mechanism (cli_bml.is_pending_response() ) is only implemented for "bml_nw_map_query"
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(3);
    while (cli_bml.is_pending_response()) {
        if (std::chrono::steady_clock::now() > timeout) {
            break;
        }
    }

    cli_soc.stop();
    cli_bml.stop();
    cli_soc.disconnect();
    cli_bml.disconnect();

    return status_code;
}

static void cli_interactive(std::string path, std::string tmp_path, std::string ip)
{
    std::cout << "Welcome to BeeRocks CLI " << BEEROCKS_VERSION << " Build date "
              << BEEROCKS_BUILD_DATE << std::endl;
    std::cout << "Enter 'help' for help, 'exit' to exit" << std::endl;

    beerocks::cli_socket cli_soc(tmp_path, ip);
    beerocks::cli_bml cli_bml(path);
    beerocks::cli *cli_ptr = &cli_soc;
    int is_onboarding      = -1;

    if (!cli_bml.connect()) {
        std::cout << "cli_bml: Can't connect to BML, exit..." << std::endl;
    } else {
        std::cout << "Connected to BeeRocks BML CLI." << std::endl;
        is_onboarding = cli_bml.get_onboarding_status();
    }

    if (is_onboarding == 0) {
        if (!cli_soc.connect()) {
            std::cout << "cli_socket: Can't connect to master." << std::endl;
        } else if (!cli_soc.start()) {
            std::cout << "cli_socket: Can't start cli, exit" << std::endl;
            return;
        } else {
            std::cout << "Connected to BeeRocks Master CLI." << std::endl;
        }
    }

    //read from history file
    std::string historyFileName = "/tmp/widan_cli_history.txt";
    read_history(historyFileName.c_str());

    g_cli_cmds = cli_soc.get_available_commands();
    {
        auto cli_bml_cmds = cli_bml.get_available_commands();
        g_cli_cmds.insert(g_cli_cmds.end(), cli_bml_cmds.begin(), cli_bml_cmds.end());
    }

    //enable auto-complete
    rl_attempted_completion_function = command_tab_completion;
    rl_bind_key('\t', rl_complete);

    //read user input and execute functions
    char *buf         = nullptr;
    bool quit_request = false;
    std::string bml_prefix(BML_PREFIX);

    while (g_running && (buf = readline(">>"))) {
        //parse and call function
        std::string strBuf(buf);
        if (!strBuf.empty()) {
            int executions = repeated_cmd_parser(strBuf);

            if (strBuf.compare(0, bml_prefix.size(), bml_prefix)) { // Socket
                cli_ptr = &cli_soc;
            } else { // BML
                cli_ptr = &cli_bml;
            }

            if (cli_ptr->parsedCommandArgumentsLinux(strBuf)) {
                bool exe_cmd = true;
                auto command = cli_ptr->get_command();

                if (!((command.find("connect") != std::string::npos) ||
                      (command.find("help") != std::string::npos) || (command == "q") ||
                      (command == "exit"))) {
                    if (!cli_ptr->is_connected()) {
                        exe_cmd = false;
                        std::cout << "can't execute command, not connected to master." << std::endl;
                    }
                }

                if (exe_cmd) {
                    do {
                        if (executions < 0) {
                            clear_screen();
                            set_cursor(1, 1);
                            std::cout << "PRESS CTRL+C TO STOP (NOT EXIT)" << std::endl
                                      << std::endl;
                        } else if (executions > 1) {
                            std::cout << "PRESS CTRL+C TO STOP (NOT EXIT)" << std::endl
                                      << std::endl;
                        }
                        if (command == "help") {
                            if (is_onboarding == 0) {
                                cli_soc.print_help();
                            }
                            cli_bml.print_help(false);
                        } else {
                            if (cli_ptr->callCommand() < 0) {
                                quit_request = true;
                                break;
                            }
                        }
                        if (--executions) {
                            UTILS_SLEEP_MSEC(1000);
                        }
                    } while (g_running && g_loop_cmd_exec && executions);
                    g_loop_cmd_exec = false;

#if defined(BEEROCKS_UGW) || defined(BEEROCKS_RDKB)
                    //add to history
                    int idx = history_search_pos(buf, 0, 0);
                    if (idx != -1) {
                        HIST_ENTRY *entry = remove_history(idx);
                        if (entry) {
                            free(entry->line);
                            free(entry);
                        }
                    }
                    add_history(buf);
#endif
                }
            }
        }

        free(buf);

        if (quit_request) {
            break;
        }
    }

    std::cout << "exit..." << std::endl;

    cli_soc.stop();
    cli_bml.stop();
    cli_soc.disconnect();
    cli_bml.disconnect();

    //write to history file
    if (write_history(historyFileName.c_str())) {
        LOG(ERROR) << "failed to write to history file";
    }
}

static void cli_tcp_proxy(std::string temp_path)
{
    std::string master_uds = temp_path + std::string(BEEROCKS_MASTER_UDS);
    beerocks::cli_proxy cli_proxy_thread(master_uds);

    if (!cli_proxy_thread.start()) {
        LOG(ERROR) << "cli_proxy.start() failed ";
        g_running = false;
    }

    while (g_running) {
        UTILS_SLEEP_MSEC(1000);
    }
    std::cout << "beerocks_cli exit..." << std::endl;
    cli_proxy_thread.stop();
}

int main(int argc, char *argv[])
{
    init_signals();

    int opt;
    bool is_cli_proxy        = false;
    bool is_cli_bml_analyzer = false;
    std::string analyzer_ip;
    std::string ip;
    std::string command_string;

    while ((opt = getopt(argc, argv, "pi:c:w:a:")) != -1) {
        switch (opt) {
        case 'p': {
            is_cli_proxy = true;
            break;
        }
        case 'i': {
            ip = std::string(optarg);
            break;
        }
        case 'c': {
            command_string = std::string(optarg);
            break;
        }
        case 'w': {
            g_wait_time = std::atoi(optarg);
            break;
        }
        case 'a': {
            is_cli_bml_analyzer = true;
            analyzer_ip         = std::string(optarg);
            break;
        }
        default: { /* '?' */
            if (isprint(optopt)) {
                std::cout << "Unknown option -" << optopt << "!" << std::endl;
                return false;
            } else {
                std::cout << "Unknown character " << optopt << "!" << std::endl;
                ;
                return false;
            }
            break;
        }
        }
    }

    //read slave config file
    std::string config_file_path = BEEROCKS_PATH + std::string(BEEROCKS_AGENT) + ".conf";
    beerocks::config_file::sConfigSlave beerocks_slave_conf;
    if (!beerocks::config_file::read_slave_config_file(config_file_path, beerocks_slave_conf)) {
        std::cout << "config file '" << config_file_path << "' args error." << std::endl;
        return 1;
    }

    int status_return = 0;

    std::string log_file = beerocks_slave_conf.sLog.path + std::string(BEEROCKS_CLI) + ".log";
    init_logger(is_cli_proxy ? false : true, log_file);
    if (is_cli_proxy) {
        cli_tcp_proxy(beerocks_slave_conf.temp_path);
    } else if (!command_string.empty()) {
        status_return =
            cli_non_interactive(BEEROCKS_PATH, beerocks_slave_conf.temp_path, ip, command_string);
    } else if (is_cli_bml_analyzer) {
        beerocks::cli_bml cli_bml(BEEROCKS_PATH);
        cli_bml.analyzer_init(analyzer_ip);
    } else {
        cli_interactive(BEEROCKS_PATH, beerocks_slave_conf.temp_path, ip);
    }

    return status_return;
}
