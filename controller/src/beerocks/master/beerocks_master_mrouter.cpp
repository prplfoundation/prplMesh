/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "beerocks_master_mrouter.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_control.h>

using namespace beerocks;
using namespace net;

#define SELECT_TIMEOUT_MSC 1000

master_mrouter::master_mrouter(const std::string &mrouter_uds_, const std::string &master_uds_,
                               const std::string &bridge_iface_name)
    : transport_socket_thread(mrouter_uds_)
{
    thread_name = "master_mrouter";
    master_uds  = master_uds_;
    set_select_timeout(SELECT_TIMEOUT_MSC);

    network_utils::iface_info bridge_info;
    if (network_utils::get_iface_info(bridge_info, bridge_iface_name) != 0) {
        LOG(ERROR) << "Failed reading addresses from the bridge!";
        stop();
    }
    local_bridge_mac = bridge_info.mac;
}

master_mrouter::~master_mrouter() { on_thread_stop(); }

int master_mrouter::server_port() { return beerocks::MASTER_TCP_PORT; }

bool master_mrouter::init()
{
    on_thread_stop();
    if (!transport_socket_thread::init()) {
        LOG(ERROR) << "Failed init";
        stop();
        return false;
    }

    return (bus_subscribe(std::vector<ieee1905_1::eMessageType>{
        ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE,
        ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_SEARCH_MESSAGE,
        ieee1905_1::eMessageType::AP_AUTOCONFIGURATION_WIFI_SIMPLE_CONFIGURATION_MESSAGE,
    }));
}

void master_mrouter::on_thread_stop()
{
    while (master_sockets.size() > 0) {
        auto soc = master_sockets.back();
        if (soc) {
            if (soc->master) {
                remove_socket(soc->master);
                delete soc->master;
            }
        }
        master_sockets.pop_back();
    }
}

void master_mrouter::socket_connected(Socket *sd) { bus_connected(sd); }

bool master_mrouter::socket_disconnected(Socket *sd)
{
    bool socket_removed = false;
    master_sockets.remove_if([&](std::shared_ptr<sMasterSockets> soc_iter) {
        if (soc_iter->master == sd) {
            LOG(DEBUG) << "master disconnected sd=" << intptr_t(sd);
            if (soc_iter->master) {
                socket_removed = true;
                remove_socket(soc_iter->master);
                delete soc_iter->master;
            }
            return true;
        }
        return false;
    });

    if (!socket_removed) {
        LOG(ERROR) << "socket disconnected on non slave socket, assuming it socket to local "
                      "backhaul manager, socket="
                   << intptr_t(sd);
        // TODO: disconnect all uds sockets to controller?
    }

    return !socket_removed;
}

void master_mrouter::after_select(bool timeout) {}

std::string master_mrouter::print_cmdu_types(const message::sUdsHeader *uds_header)
{
    return message_com::print_cmdu_types(uds_header);
}

bool master_mrouter::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{
    std::shared_ptr<sMasterSockets> soc = nullptr;
    auto uds_header                     = message_com::get_uds_header(cmdu_rx);
    uint16_t length                     = uds_header->length;

    std::string src_mac = network_utils::mac_to_string(uds_header->src_bridge_mac);
    std::string dst_mac = network_utils::mac_to_string(uds_header->dst_bridge_mac);

    // LOG(DEBUG) << "handle_cmdu() - received msg from " << std::string(from_bus(sd) ? "bus" : "uds") << ", src=" << src_mac
    //            << ", dst=" << dst_mac << ", " << print_cmdu_types(uds_header); // floods the log

    // Find appropriate socket for forwarding
    for (auto soc_iter : master_sockets) {
        if (from_bus(sd)) {
            if (soc_iter->slave_bridge_mac == src_mac) { // TODO: Add here PID optimization
                soc = soc_iter;
                break;
            }
        } else {
            if (soc_iter->master == sd) {
                soc = soc_iter;
                break;
            }
        }
    }

    if (from_bus(sd)) {

        if (src_mac == network_utils::ZERO_MAC_STRING) {
            LOG(ERROR) << "src_mac is zero!";
            return false;
        }

        if (dst_mac == network_utils::ZERO_MAC_STRING) {
            LOG(ERROR) << "dst_mac is zero!";
            return false;
        }

        // Filter messages which are not destined to the controller
        if (dst_mac != MULTICAST_MAC_ADDR && dst_mac != local_bridge_mac) {
            return true;
        }

        // TODO: Add optimization of PID filtering for cases like the following:
        // 1. If VS message was sent by Controllers local agent to the controller, it is looped back.

        if (!soc) {
            LOG(DEBUG) << "Can't find master socket, creating dummy entry: slave_bridge_mac="
                       << src_mac;
            soc                   = std::make_shared<sMasterSockets>();
            soc->slave_bridge_mac = src_mac;
            LOG(DEBUG) << "opening new master socket";
            soc->master     = new SocketClient(master_uds);
            std::string err = soc->master->getError();
            if (!err.empty()) {
                LOG(ERROR) << "soc->master error: " << err;
                delete soc->master;
                soc->master = nullptr;
                return false;
            } else {
                add_socket(soc->master);
                master_sockets.push_back(soc);
            }
        }

        ////////// If got here, message needs to be forwarded //////////

        // Message from Agent (bus) to Controller (uds)
        // Send the data (uds_header + cmdu) how it is on UDS, without changing it

        if (!message_com::forward_cmdu_to_uds(soc->master, cmdu_rx, length)) {
            LOG(ERROR) << "forward_cmdu_to_uds() failed - " << print_cmdu_types(uds_header);
            return false;
        }

    } else { //from uds to bus
        // LOG(DEBUG) << "forwarding master->slave message, dst_mac=" << soc->slave_bridge_mac << ", src_mac=" << local_bridge_mac;

        if (!soc) {
            LOG(ERROR) << "soc=nullptr! Received message from unknown uds socket!";
            return false;
        }

        send_cmdu_to_bus(cmdu_rx, soc->slave_bridge_mac, local_bridge_mac, length);
    }

    return true;
}
