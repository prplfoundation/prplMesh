/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>

#include "beerocks_cli_proxy.h"

#define SELECT_TIMEOUT_MSC 1000

using namespace beerocks;

cli_proxy::cli_proxy(std::string master_uds_) : socket_thread()
{
    thread_name = "cli_proxy";
    master_uds  = master_uds_;
    set_select_timeout(SELECT_TIMEOUT_MSC);
}

cli_proxy::~cli_proxy() { on_thread_stop(); }

int cli_proxy::server_port() { return beerocks::CLI_PROXY_TCP_PORT; }

bool cli_proxy::init()
{
    on_thread_stop();
    return socket_thread::init();
}

void cli_proxy::on_thread_stop()
{
    while (master_sockets.size() > 0) {
        auto soc = master_sockets.back();
        if (soc) {
            if (soc->cli_client) {
                remove_socket(soc->cli_client);
                delete soc->cli_client;
            }
            if (soc->master) {
                remove_socket(soc->master);
                delete soc->master;
            }
        }
        master_sockets.pop_back();
    }
}

void cli_proxy::socket_connected(Socket *sd)
{
    LOG(DEBUG) << "new cli client connection";
    auto soc        = new sMasterSockets;
    soc->master     = new SocketClient(master_uds);
    std::string err = soc->master->getError();
    if (!err.empty()) {
        LOG(ERROR) << "soc->master: " << err;
        delete soc->master;
        delete soc;
        delete sd;
    } else {
        soc->cli_client = sd;
        master_sockets.push_back(soc);
        add_socket(soc->master);
        add_socket(soc->cli_client);
    }
}

bool cli_proxy::socket_disconnected(Socket *sd)
{
    for (auto it = master_sockets.begin(); it != master_sockets.end(); it++) {
        auto soc = *it;
        if (soc->cli_client == sd)
            LOG(DEBUG) << "cli client disconnected";
        else if (soc->master == sd)
            LOG(DEBUG) << "master disconnected";
        if ((soc->cli_client == sd) || (soc->master == sd)) {
            if (soc->cli_client) {
                remove_socket(soc->cli_client);
                delete soc->cli_client;
            }
            if (soc->master) {
                remove_socket(soc->master);
                delete soc->master;
            }
            master_sockets.erase(it);
            return false;
        }
    }
    return true;
}

bool cli_proxy::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    auto uds_header = message_com::get_uds_header(cmdu_rx);
    if (!uds_header) {
        LOG(ERROR) << "message_com::get_uds_header() failed";
        return false;
    }
    uint16_t length = uds_header->length;

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (beerocks_header == nullptr) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    if (beerocks_header->action() != beerocks_message::ACTION_CLI) {
        LOG(ERROR) << "Unknown message, action: " << int(beerocks_header->action());
        return true;
    }

    // find cli_client_num socket
    int cli_client_num   = -1;
    bool from_cli_client = false;
    for (int idx = 0; idx < int(master_sockets.size()); idx++) {
        if (master_sockets.at(idx)->cli_client == sd) {
            cli_client_num  = idx;
            from_cli_client = true;
            break;
        }
        if (master_sockets.at(idx)->master == sd) {
            cli_client_num = idx;
            break;
        }
    }

    if (cli_client_num == -1) {
        LOG(ERROR) << "Can't find cli_client socket";
        return true;
    }

    auto soc = master_sockets.at(cli_client_num);

    if (from_cli_client) {
        message_com::forward_cmdu_to_uds(soc->master, cmdu_rx, length);
        // LOG(DEBUG) << "send message to master action_op=" << int(beerocks_header->action_op())  << " size=" << int(len);
    } else {
        message_com::forward_cmdu_to_uds(soc->cli_client, cmdu_rx, length);
        // LOG(DEBUG) << "send message to cli_client action_op=" << int(beerocks_header->action_op()) << " size=" << int(len);
    }
    return true;
}
