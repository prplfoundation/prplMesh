/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "btl.h"

#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

using namespace beerocks::btl;
using namespace beerocks::net;

void transport_socket_thread::bus_init()
{
}
bool transport_socket_thread::configure_ieee1905_transport_interfaces(const std::string& bridge_iface, const std::vector<std::string>& ifaces)
{
    return true;
}

void transport_socket_thread::add_socket(Socket *s, bool add_to_vector)
{ 
    socket_thread::add_socket(s, add_to_vector); 
}

void transport_socket_thread::remove_socket(Socket *s)
{ 
    socket_thread::remove_socket(s); 
}

bool transport_socket_thread::read_ready(Socket *s)
{ 
    return socket_thread::read_ready(s);
}

bool transport_socket_thread::bus_subscribe(const std::vector<ieee1905_1::eMessageType>& msg_types)
{
    return true;
}

bool transport_socket_thread::bus_connect(const std::string& beerocks_temp_path, const bool local_master)
{
    if (!local_master) {
        LOG(FATAL) << "UDS_BUS is defined on non local master platform! STOPPING!";
        should_stop = true;
        return false;
    }

    // Delete previous connections
    if (bus) {
        remove_socket(bus);
        delete bus;
        bus = nullptr;
    }

    // Open a new connection to the master TCP
    THREAD_LOG(DEBUG) << "Connecting to controller on UDS...";
    std::string mrouter_uds = beerocks_temp_path + std::string(BEEROCKS_MROUTER_UDS);
    bus = new SocketClient(mrouter_uds);
    std::string err = bus->getError();
    if (!err.empty()) {
        delete bus;
        bus = nullptr;
        THREAD_LOG(ERROR) << "Can't connect to controller using UDS: err=" << err;
        return false;
    }
    
    THREAD_LOG(DEBUG) << "Link to between agent & controller has succesfully established!, bus_socket=" << int(bus);
    add_socket(bus);

    return true;
}

void transport_socket_thread::bus_connected(Socket *sd)
{
    if (bus) {
        remove_socket(bus);
        delete bus;
    }
    bus = sd;
    add_socket(bus);
}

bool transport_socket_thread::bus_send(ieee1905_1::CmduMessage& cmdu, const std::string& dst_mac, const std::string& src_mac, uint16_t length)
{
    auto uds_header = message_com::get_uds_header(cmdu);
    if (!uds_header) {
        THREAD_LOG(ERROR) << "uds_header=nullptr";
        return false;
    }
    uds_header->length = length;
    uds_header->swap_needed = true;
    net::network_utils::mac_from_string(uds_header->src_bridge_mac, src_mac);
    net::network_utils::mac_from_string(uds_header->dst_bridge_mac, dst_mac);
    return message_com::send_data(bus, cmdu.getMessageBuff() - sizeof(message::sUdsHeader), uds_header->length + sizeof(message::sUdsHeader));
}

bool transport_socket_thread::from_bus(Socket *sd)
{
    return sd == bus;
}

bool transport_socket_thread::work()
{
    before_select();

    int sel_ret = select.selectSocket();
    if (sel_ret < 0) {
        // Do not fail for the following "errors"
        if (errno == EAGAIN || errno == EINTR) {
            THREAD_LOG(DEBUG) << "Select returned: " << strerror(errno);
            return true;
        }

        THREAD_LOG(ERROR) << "select error: " << strerror(errno);
        return false;
    }

    after_select(bool(sel_ret == 0));

    if (sel_ret == 0) {
        return true;
    }

    //If something happened on the server socket , then its an incoming connection
    if (server_socket && read_ready(server_socket.get())) {
        clear_ready(server_socket.get());
        THREAD_LOG(DEBUG) << "accept new connection on server socket sd=" << server_socket.get();
        Socket *sd = server_socket->acceptConnections();
        if (sd == nullptr || (!server_socket->getError().empty())) {
            THREAD_LOG(ERROR) << "acceptConnections == nullptr: " << server_socket->getError();
            return false;
        } else {
            if(unix_socket_path.empty()) {
                THREAD_LOG(DEBUG) << "new connection from ip=" << sd->getPeerIP()
                       << " port=" << sd->getPeerPort() << " sd=" << sd;
            } else {
                THREAD_LOG(DEBUG) << "new connection on " << unix_socket_path << " sd=" << sd;
            }
            socket_connected(sd);
        }
    }

    int sockets_count;
    int i = 0;
    do {
        sockets_count = select.count();
        for (i = 0; i < sockets_count; i++) {
            if (read_ready(select.at(i))) {
                Socket *sd = select.at(i);
                if (!sd) {
                    THREAD_LOG(WARNING) << "sd at select with index i=" << int(i) << " is nullptr, skipping";
                    continue;
                }

                bool bus_socket_event = (sd == bus);

                auto ret = socket_disconnected_uds(sd); // '0' - socket not disconnected (bytes to read), '1' - socket disconnected, '-1' - error
                if (ret == 1) {
                    if (bus_socket_event) {
                        THREAD_LOG(FATAL) << "setting bus to nullptr";
                        bus = nullptr;
                    }
                    // breaking instead of continue because socket_disconnected_uds() may erase element from Select Socket Vector while iterating it
                    break;
                } else if (ret == -1) {
                    continue;
                }

                if (!handle_cmdu_message_uds(sd)) {
                    continue;
                }
            }
        }
    // The loop should go over all the sockets. In case something break the for loop before it ended,
    // start iterating over the sockets again.
    } while (i < sockets_count);

    return true;
}