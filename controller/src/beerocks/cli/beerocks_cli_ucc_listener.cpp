/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_cli_ucc_listener.h"
#include "beerocks_cli_bml.h"

#include <beerocks/bcl/beerocks_string_utils.h>

#define SELECT_TIMEOUT_MSC 100
#define NLF std::endl << std::flush // New Line Flush

using namespace beerocks;

cli_ucc_listener::cli_ucc_listener(const std::string &beerocks_conf_path, uint16_t port)
    : socket_thread()
{
    thread_name          = "ucc_listener";
    m_beerocks_conf_path = beerocks_conf_path;
    m_port               = port;
    set_select_timeout(SELECT_TIMEOUT_MSC);

    // Greetings message
    std::cout << "Welcome to beerocks TestSuite UCC Listener!" << std::endl
              << "In order to to send WFA-CA command, "
              << "send message with netcat on port " << int(port) << ":" << std::endl
              << "  nc <bridge_ip_addr> " << int(port) << std::endl
              << "  enter command in the session" << std::endl
              << "  -- OR --" << std::endl
              << "  echo -n <command> | nc <bridge_ip_addr> " << int(port) << NLF;
}

cli_ucc_listener::~cli_ucc_listener()
{
    disconnect();
    on_thread_stop();
}

int cli_ucc_listener::server_port() { return m_port; }

bool cli_ucc_listener::init()
{
    on_thread_stop();
    if (!connect()) {
        std::cout << "failed to connect to bml" << NLF;
    }
    return socket_thread::init();
}

void cli_ucc_listener::socket_connected(Socket *sd)
{
    std::cout << "New client has connected" << NLF;
    std::unique_lock<std::mutex> lock(m_ucc_sd_mutex);

    if (sd == nullptr) {
        std::cout << "ERROR: sd == nullptr" << NLF;
        return;
    }

    // Delete previous connection if exist
    if (m_ucc_sd != nullptr) {
        remove_socket(m_ucc_sd);
        delete m_ucc_sd;
    }

    m_ucc_sd = sd;
    add_socket(sd);
}

bool cli_ucc_listener::socket_disconnected(Socket *sd)
{
    std::unique_lock<std::mutex> lock(m_ucc_sd_mutex);
    std::cout << "Client has disconnected" << NLF;
    if (sd != m_ucc_sd) {
        std::cout << "Unfamiliar socket has disconnected" << NLF;
        m_ucc_sd = nullptr;
        return true;
    }

    m_ucc_sd = nullptr;

    // Socket thread will remove socket from Select and delete it
    return true;
}

bool cli_ucc_listener::connect()
{
    disconnect(); // Disconnect if already connected
    const char *conf_path = m_beerocks_conf_path.c_str();
    int ret               = bml_connect(&m_ctx, conf_path, this);
    return (ret == BML_RET_OK);
}

void cli_ucc_listener::disconnect()
{
    if (m_ctx != nullptr) {
        bml_disconnect(m_ctx);
        m_ctx = nullptr;
    }
}

bool cli_ucc_listener::custom_message_handler(Socket *sd, uint8_t *rx_buffer, size_t rx_buffer_size)
{
    std::string command_string;
    ssize_t available_bytes;
    do {
        available_bytes = sd->readBytes(rx_buffer, sizeof(rx_buffer), false, rx_buffer_size);
        if (available_bytes <= 0) {
            break;
        }
        rx_buffer[available_bytes] = '\0';
        std::string part(reinterpret_cast<char *>(rx_buffer));
        command_string += part;
    } while (available_bytes > 0);

    beerocks::string_utils::trim(command_string);

    std::cout << std::endl;
    std::cout << "Received message: '" << command_string << "'" << NLF;

    int ret = bml_wfa_ca_controller(m_ctx, command_string.c_str(), command_string.size() + 1,
                                    wfa_ca_reply_to_ucc_cb);

    // print error message only on error
    if (ret != BML_RET_OK) {
        std::cout << "sending command to bml has failed" << NLF;
    }

    return true;
}

void cli_ucc_listener::reply_ucc(const char *buffer)
{
    if (buffer == nullptr) {
        std::cout << "buffer == nullptr" << NLF;
        return;
    }

    std::string reply_string(buffer);

    std::cout << "Replying message: '" << reply_string << "'" << NLF;

    reply_string += "\r\n";

    std::unique_lock<std::mutex> lock(m_ucc_sd_mutex);
    if (m_ucc_sd == nullptr) {
        std::cout << "Client disconnected before message has been sent" << NLF;
        return;
    }
    auto written_size = m_ucc_sd->writeBytes(
        reinterpret_cast<const uint8_t *>(reply_string.c_str()), reply_string.size());
    if (written_size < 0) {
        std::cout << "writeBytes() failed, error=" << strerror(errno) << NLF;
    }
}

void cli_ucc_listener::wfa_ca_reply_to_ucc_cb(BML_CTX ctx, const char *buffer)
{
    cli_ucc_listener *pThis = reinterpret_cast<cli_ucc_listener *>(bml_get_user_data(ctx));
    if (!pThis) {
        std::cout << "ERROR: Internal error - invalid context!" << NLF;
        return;
    }
    pThis->reply_ucc(buffer);
}
