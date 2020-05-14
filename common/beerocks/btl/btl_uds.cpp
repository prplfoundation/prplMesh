/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_backport.h>
#include <bcl/network/network_utils.h>
#include <btl/btl.h>
#include <easylogging++.h>

using namespace beerocks::btl;
using namespace beerocks::net;

bool transport_socket_thread::bus_init()
{
    if (bus_server_socket) {
        remove_socket(bus_server_socket.get());
        bus_server_socket->closeSocket();
        bus_server_socket.reset(nullptr);
    }

    auto unix_socket_path_bus = unix_socket_path + "_bus";

    bus_server_socket = std::make_unique<SocketServer>(unix_socket_path_bus, 1);
    if (!bus_server_socket) {
        LOG(ERROR) << "bus_server_socket == nullptr";
        return false;
    } else if (!bus_server_socket->getError().empty()) {
        LOG(ERROR) << "failed to create server_socket, error:" << server_socket->getError();
        return false;
    }

    LOG(DEBUG) << "new SocketServer on BUS " << unix_socket_path_bus;
    add_socket(bus_server_socket.get(), false);
    return true;
}

bool transport_socket_thread::configure_ieee1905_transport_interfaces(
    const std::string &bridge_iface, const std::vector<std::string> &ifaces)
{
    return true;
}

void transport_socket_thread::add_socket(Socket *s, bool add_to_vector)
{
    socket_thread::add_socket(s, add_to_vector);
}

void transport_socket_thread::remove_socket(Socket *s) { socket_thread::remove_socket(s); }

bool transport_socket_thread::read_ready(Socket *s) { return socket_thread::read_ready(s); }

bool transport_socket_thread::bus_subscribe(const std::vector<ieee1905_1::eMessageType> &msg_types)
{
    m_subscribed_messages.insert(msg_types.begin(), msg_types.end());
    return true;
}

bool transport_socket_thread::bus_connect(const std::string &beerocks_temp_path,
                                          const bool local_master)
{
    // This function must be use by the Agent side only to open UDS Bus socket to the controller
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
    std::string mrouter_uds =
        beerocks_temp_path + std::string(BEEROCKS_MASTER_UDS + std::string("_bus"));

    bus             = new SocketClient(mrouter_uds);
    std::string err = bus->getError();
    if (!err.empty()) {
        delete bus;
        bus = nullptr;
        THREAD_LOG(ERROR) << "Can't connect to controller using UDS: err=" << err;
        return false;
    }

    THREAD_LOG(DEBUG)
        << "Link to between agent & controller has succesfully established!, bus_socket="
        << intptr_t(bus);
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

bool transport_socket_thread::bus_send(ieee1905_1::CmduMessage &cmdu, const std::string &dst_mac,
                                       const std::string &src_mac, uint16_t length)
{
    auto uds_header = message_com::get_uds_header(cmdu);
    if (!uds_header) {
        THREAD_LOG(ERROR) << "uds_header=nullptr";
        return false;
    }
    uds_header->length = length;
    tlvf::mac_from_string(uds_header->src_bridge_mac, src_mac);
    tlvf::mac_from_string(uds_header->dst_bridge_mac, dst_mac);
    return message_com::send_data(bus, cmdu.getMessageBuff() - sizeof(message::sUdsHeader),
                                  uds_header->length + sizeof(message::sUdsHeader));
}

bool transport_socket_thread::from_bus(Socket *sd) { return sd == bus; }

bool transport_socket_thread::skip_filtered_message_type(ieee1905_1::eMessageType msg_type)
{
    if (m_subscribed_messages.find(msg_type) == m_subscribed_messages.end()) {
        return true;
    }
    return false;
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

    //If something happened on the server socket (BUS), then its an incoming connection
    if (bus_server_socket && read_ready(bus_server_socket.get())) {
        clear_ready(bus_server_socket.get());
        THREAD_LOG(DEBUG) << "accept new connection on BUS server socket sd="
                          << bus_server_socket.get();
        Socket *sd = bus_server_socket->acceptConnections();
        if (sd == nullptr || (!bus_server_socket->getError().empty())) {
            THREAD_LOG(ERROR) << "acceptConnections == nullptr: " << bus_server_socket->getError();
            return false;
        }
        THREAD_LOG(DEBUG) << "new connection on " << unix_socket_path << "_bus, sd=" << sd;
        bus_connected(sd);
    }

    //If something happened on the server socket (UDS), then its an incoming connection
    if (server_socket && read_ready(server_socket.get())) {
        clear_ready(server_socket.get());
        THREAD_LOG(DEBUG) << "accept new connection on server socket sd=" << server_socket.get();
        Socket *sd = server_socket->acceptConnections();
        if (sd == nullptr || (!server_socket->getError().empty())) {
            THREAD_LOG(ERROR) << "acceptConnections == nullptr: " << server_socket->getError();
            return false;
        } else {
            if (unix_socket_path.empty()) {
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
                    THREAD_LOG(WARNING)
                        << "sd at select with index i=" << int(i) << " is nullptr, skipping";
                    continue;
                }

                bool bus_socket_event = (sd == bus);

                auto ret = socket_disconnected_uds(
                    sd); // '0' - socket not disconnected (bytes to read), '1' - socket disconnected, '-1' - error
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
