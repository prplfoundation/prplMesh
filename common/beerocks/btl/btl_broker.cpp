/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_backport.h>
#include <bcl/beerocks_string_utils.h>
#include <bcl/network/network_utils.h>
#include <btl/btl.h>

#include <mapf/common/config.h>
#include <mapf/transport/ieee1905_transport_messages.h>

#include <sys/uio.h>

#include <easylogging++.h>

using namespace beerocks::btl;
using namespace beerocks::net;
using namespace beerocks::transport::messages;

bool transport_socket_thread::bus_init()
{
    if (bus) {
        remove_socket(bus.get());
        bus->closeSocket();
        bus.reset(nullptr);
    }

    auto unix_socket_path_broker = TMP_PATH "/" BEEROCKS_BROKER_UDS;

    bus = std::make_unique<SocketClient>(unix_socket_path_broker);
    if (!bus) {
        LOG(ERROR) << "bus == nullptr";
        return false;
    } else if (!bus->getError().empty()) {
        LOG(ERROR) << "Failed connecting to the broker, error:" << bus->getError();
        return false;
    }

    LOG(DEBUG) << "Broker socket connected on: " << unix_socket_path_broker;
    add_socket(bus.get(), false);
    return true;
}

bool transport_socket_thread::configure_ieee1905_transport_interfaces(
    const std::string &bridge_iface, const std::vector<std::string> &ifaces)
{
    LOG_IF(!bus, FATAL) << "Broker socket is not connected!";

    InterfaceConfigurationRequestMessage interface_configuration_request_msg;
    using Flags = InterfaceConfigurationRequestMessage::Flags;

    uint32_t n = 0;
    string_utils::copy_string(interface_configuration_request_msg.metadata()->interfaces[n].ifname,
                              bridge_iface.c_str(), IF_NAMESIZE);

    interface_configuration_request_msg.metadata()->interfaces[n].flags |= Flags::IS_BRIDGE;
    n++;
    THREAD_LOG(DEBUG) << "adding bridge " << bridge_iface
                      << " to ieee1905 transport, bridge iface=" << bridge_iface;
    for (const auto &iface : ifaces) {
        string_utils::copy_string(
            interface_configuration_request_msg.metadata()->interfaces[n].ifname, iface.c_str(),
            IF_NAMESIZE);
        string_utils::copy_string(
            interface_configuration_request_msg.metadata()->interfaces[n].bridge_ifname,
            bridge_iface.c_str(), IF_NAMESIZE);
        interface_configuration_request_msg.metadata()->interfaces[n].flags |=
            Flags::ENABLE_IEEE1905_TRANSPORT;
        n++;
        THREAD_LOG(DEBUG) << "adding interface " << iface << " to ieee1905 transport"
                          << " bridge=" << bridge_iface;
    }
    interface_configuration_request_msg.metadata()->numInterfaces = n;
    THREAD_LOG(DEBUG) << "numInterfaces=" << n;

    return transport::messages::send_transport_message(*bus, interface_configuration_request_msg);
}

void transport_socket_thread::add_socket(Socket *s, bool add_to_vector)
{
    socket_thread::add_socket(s, add_to_vector);
}

void transport_socket_thread::remove_socket(Socket *s) { socket_thread::remove_socket(s); }

bool transport_socket_thread::read_ready(Socket *s) { return socket_thread::read_ready(s); }

bool transport_socket_thread::bus_subscribe(const std::vector<ieee1905_1::eMessageType> &msg_types)
{
    LOG_IF(!bus, FATAL) << "Broker socket not initialized!";

    if (msg_types.size() > SubscribeMessage::MAX_SUBSCRIBE_TYPES) {
        LOG(ERROR) << "Subscribing to " << msg_types.size()
                   << " is not supported. Maximal allowed types: "
                   << SubscribeMessage::MAX_SUBSCRIBE_TYPES;

        return false;
    }

    // Build a subscription message
    SubscribeMessage subscribe;
    subscribe.metadata()->type = SubscribeMessage::ReqType::SUBSCRIBE;

    subscribe.metadata()->msg_types_count = 0;
    for (const auto &msg_type : msg_types) {
        subscribe.metadata()->msg_types[subscribe.metadata()->msg_types_count].bits = {
            .internal = 0, .vendor_specific = 0, .reserved = 0, .type = uint32_t(msg_type)};

        ++subscribe.metadata()->msg_types_count;
    }

    return transport::messages::send_transport_message(*bus, subscribe);
}

bool transport_socket_thread::bus_connect(const std::string &beerocks_temp_path,
                                          const bool local_master)
{
    return true;
}

bool transport_socket_thread::bus_send(ieee1905_1::CmduMessage &cmdu, const std::string &iface_name,
                                       const std::string &dst_mac, const std::string &src_mac,
                                       uint16_t length)
{
    LOG_IF(!bus, FATAL) << "Broker socket not initialized!";

    CmduTxMessage msg;

    tlvf::mac_from_string(msg.metadata()->src, src_mac);
    tlvf::mac_from_string(msg.metadata()->dst, dst_mac);

    // LOG(DEBUG) << "Message Type: " << std::hex << uint16_t(cmdu.getMessageType()) << std::dec
    //            << ", Finalized: " << cmdu.is_finalized() << ", Swapped: " << cmdu.is_swapped();

    msg.metadata()->ether_type        = ETH_P_1905_1;
    msg.metadata()->length            = length;
    msg.metadata()->msg_type          = uint16_t(cmdu.getMessageType());
    msg.metadata()->preset_message_id = cmdu.getMessageId() ? 1 : 0;
    msg.metadata()->if_index          = if_nametoindex(iface_name.c_str());

    std::copy_n((uint8_t *)cmdu.getMessageBuff(), msg.metadata()->length, (uint8_t *)msg.data());
    return transport::messages::send_transport_message(*bus, msg);
}

bool transport_socket_thread::handle_cmdu_message_bus()
{
    auto msg = transport::messages::read_transport_message(*bus);
    if (msg == nullptr) {
        THREAD_LOG(ERROR) << "Received msg is null";
        return false;
    }

    auto cmdu_rx_msg = dynamic_cast<CmduRxMessage *>(msg.get());
    if (cmdu_rx_msg) {
    } else {
        THREAD_LOG(ERROR) << "Received non CmduRxMessage:\n\tMessage: " << *msg
                          << "\n\tFrame: " << msg->frame().str();
        return false;
    }

    // Copy the data to rx_buffer
    if (sizeof(message::sUdsHeader) + cmdu_rx_msg->metadata()->length > sizeof(rx_buffer)) {
        THREAD_LOG(ERROR)
            << "sizeof(message::sUdsHeader) + cmdu_rx_msg->metadata()->length > sizeof(rx_buffer)";
        return false;
    }

    std::copy_n((uint8_t *)cmdu_rx_msg->data(), cmdu_rx_msg->metadata()->length,
                rx_buffer + sizeof(message::sUdsHeader));

    // fill UDS Header
    message::sUdsHeader *uds_header = (message::sUdsHeader *)rx_buffer;
    uds_header->if_index            = cmdu_rx_msg->metadata()->if_index;
    std::copy_n((uint8_t *)cmdu_rx_msg->metadata()->src, sizeof(CmduRxMessage::Metadata::src),
                uds_header->src_bridge_mac);
    std::copy_n((uint8_t *)cmdu_rx_msg->metadata()->dst, sizeof(CmduRxMessage::Metadata::dst),
                uds_header->dst_bridge_mac);
    uds_header->length = cmdu_rx_msg->metadata()->length;

    if (!verify_cmdu(uds_header)) {
        THREAD_LOG(ERROR) << "Failed verifying cmdu header";
        return false;
    }

    if (!cmdu_rx.parse()) {
        THREAD_LOG(ERROR) << "parsing cmdu failure, rx_buffer" << std::hex << rx_buffer << std::dec
                          << ", uds_header->length=" << int(uds_header->length);
        return false;
    }

    if (!handle_cmdu(bus.get(), cmdu_rx)) {
        return false;
    }

    return true;
}

bool transport_socket_thread::from_bus(Socket *sd)
{
    if (!sd || !bus) {
        return false;
    }

    return sd->getSocketFd() == bus->getSocketFd();
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

    // If something happened on the server socket (UDS), then its an incoming connection
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

                bool bus_socket_event = bus && (bus->getSocketFd() == sd->getSocketFd());

                // '0' - socket not disconnected (bytes to read), '1' - socket disconnected, '-1' - error
                auto ret = socket_disconnected_uds(sd);
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

                // LOG(DEBUG) << "Received message on FD: " << sd->getSocketFd()
                //            << " (bus_socket_event = " << bus_socket_event << ")";

                if (bus_socket_event) {
                    if (!handle_cmdu_message_bus()) {
                        continue;
                    }
                } else {
                    if (!handle_cmdu_message_uds(sd)) {
                        continue;
                    }
                }
            }
        }
        // The loop should go over all the sockets. In case something break the for loop before it ended,
        // start iterating over the sockets again.
    } while (i < sockets_count);

    return true;
}
