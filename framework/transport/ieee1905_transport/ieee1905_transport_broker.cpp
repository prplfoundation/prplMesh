/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport_broker.h"

#include <bcl/beerocks_utils.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <tlvf/ieee_1905_1/tlvVendorSpecific.h>

// System
#include <cerrno>
#include <sys/eventfd.h>
#include <sys/uio.h>

#include <easylogging++.h>

namespace beerocks {
namespace transport {
namespace broker {

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static bool is_restricted_type(uint32_t type)
{
    // TODO: Until VS message filtering is implemented (based on action and action_op)
    //       assume there are no restricted types. After the implementation,
    //       subscribing to BROKER specific message should be restricted.

    return false;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Broker Implementation ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

BrokerServer::BrokerServer(const std::shared_ptr<SocketServer> &server_socket,
                           const std::shared_ptr<EventLoop> &event_loop)
    : m_server_socket(server_socket), m_event_loop(event_loop)
{
    LOG_IF(!m_server_socket, FATAL) << "Server socket is a null pointer!";
    LOG_IF(!m_event_loop, FATAL) << "Event loop is a null pointer!";

    // Check for errors
    LOG_IF(!m_server_socket->getError().empty(), FATAL)
        << "Failed opening server socket: " << m_server_socket
        << " [ERROR: " << m_server_socket->getError() << "]";
}

bool BrokerServer::start()
{
    EventLoop::EventHandlers handlers{
        // Accept incoming connections
        .on_read =
            [&](int fd, EventLoop &loop) {
                // Handle connections to one of the server sockets
                if (!socket_connected()) {
                    // NOTE: Do NOT stop the broker on connection errors...
                }

                return true;
            },

        // Not implemented
        .on_write = nullptr,

        // Fail on server socket disconnections or errors
        .on_disconnect =
            [&](int fd, EventLoop &loop) {
                LOG(ERROR) << "Broker socket disconnected!";
                return false;
            },
        .on_error =
            [&](int fd, EventLoop &loop) {
                LOG(ERROR) << "Broker socket error!";
                return false;
            },
    };

    if (!m_event_loop->register_handlers(m_server_socket->getSocketFd(), handlers)) {
        LOG(ERROR) << "Failed adding the broker server socket into the poll";
        return false;
    }

    return true;
}

bool BrokerServer::stop() { return m_event_loop->remove_handlers(m_server_socket->getSocketFd()); }

bool BrokerServer::publish(const messages::Message &msg)
{
    messages::SubscribeMessage::MsgType msg_opcode;

    switch (messages::Type(msg.type())) {
    // External message
    case messages::Type::CmduRxMessage: {
        auto &cmdu_msg  = dynamic_cast<const messages::CmduRxMessage &>(msg);
        msg_opcode.bits = {.internal        = 0, // external message
                           .vendor_specific = 0,
                           .reserved        = 0,
                           .type            = cmdu_msg.metadata()->msg_type};
    } break;
    // Internal messages
    default: {
        msg_opcode.bits = {.internal        = 1, // internal message
                           .vendor_specific = 0,
                           .reserved        = 0,
                           .type            = uint16_t(msg.type())};
    }
    }

    // Get the list of subscribers
    auto types_set_it = m_type_to_soc.find(msg_opcode.value);
    if (types_set_it == m_type_to_soc.end()) {
        LOG(DEBUG) << "No subscribers for message (internal = " << msg_opcode.bits.internal
                   << ", type = " << std::hex << msg_opcode.bits.type << ")" << std::dec
                   << " with length: " << msg.header().len;

        return true;
    }

    // Send the message to subscribed FDs
    for (auto soc : types_set_it->second) {
        LOG(DEBUG) << "Sending message with type (0x" << std::hex << msg_opcode.value << std::dec
                   << ") to FD (" << soc->getSocketFd() << ")";

        if (!messages::send_transport_message(*soc, msg)) {
            LOG(ERROR) << "Failed sending message with type (0x" << std::hex << msg_opcode.value
                       << std::dec << ") to FD (" << soc->getSocketFd() << ")";

            continue;
        }
    }

    return true;
}

void BrokerServer::register_internal_message_handler(const MessageHandler &handler)
{
    LOG_IF(m_internal_message_handler, WARNING) << "Overriding previously registered handler";
    m_internal_message_handler = handler;
}

void BrokerServer::register_external_message_handler(const MessageHandler &handler)
{
    LOG_IF(m_internal_message_handler, WARNING) << "Overriding previously registered handler";
    m_external_message_handler = handler;
}

bool BrokerServer::handle_msg(const std::shared_ptr<Socket> &sd)
{
    // Check if the socket contains enough bytes for the header
    if (sd->getBytesReady() < static_cast<ssize_t>(sizeof(messages::Message::Header))) {
        // Received partial header - do nothing...
        return true;
    }

    // Read and parse the message
    auto message = messages::read_transport_message(*sd);
    if (!message) {
        return false;
    }

    // Handle the specific message
    switch (messages::Type(message->type())) {
    // Broker Subscribe/Unsubscribe
    case messages::Type::SubscribeMessage: {
        auto msg_ptr = dynamic_cast<messages::SubscribeMessage *>(message.get());
        return handle_subscribe(sd, *msg_ptr);
    }
    // Outgoing CMDU messages
    case messages::Type::CmduTxMessage: {
        LOG(DEBUG) << "Processing external message from FD: " << sd->getSocketFd();
        if (!m_external_message_handler) {
            LOG(ERROR) << "External message handler not registered!";
            return false;
        }

        return m_external_message_handler(message, *this);
    }
    // Internal messages
    default: {
        LOG(DEBUG) << "Processing internal message from FD: " << sd->getSocketFd();
        if (!m_internal_message_handler) {
            LOG(ERROR) << "Internal message handler not registered!";
            return false;
        }

        return m_internal_message_handler(message, *this);
    }
    }

    return true;
}

bool BrokerServer::handle_subscribe(const std::shared_ptr<Socket> &sd,
                                    const messages::SubscribeMessage &msg)
{
    // Validate the message type
    if (msg.metadata()->type == messages::SubscribeMessage::ReqType::INVALID) {
        LOG(ERROR) << "Invalid subscribe message type!";
        return false;
    }

    // Make sure there's at least one message type
    if (msg.metadata()->msg_types_count == 0) {
        LOG(ERROR) << "Subscribe message does not contain any types!";
        return false;
    }

    // Iterate over the message types and subscribe/unsubscribe
    bool subscribe = msg.metadata()->type == messages::SubscribeMessage::ReqType::SUBSCRIBE;
    std::stringstream log_types;
    for (auto i = 0; i < msg.metadata()->msg_types_count; ++i) {
        auto msg_type = msg.metadata()->msg_types[i];

        // Skip restricted types
        if (is_restricted_type(msg_type.value)) {
            LOG(WARNING) << "FD (" << sd->getSocketFd()
                         << ") attempt subscribing to forbidden type " << msg_type.value;

            continue;
        }

        // Add to the list of requested types
        log_types << "0x" << std::hex << msg_type.value << " ";

        // Subscribe
        if (subscribe) {
            // Add the type to the list of this Socket subscriptions
            m_soc_to_type[sd].insert(msg_type.value);

            // Add the Socket to the list of this type subscriptions
            m_type_to_soc[msg_type.value].insert(sd);

            // Unsubscribe
        } else {
            // Delete the type from the list of this Socket subscriptions
            m_soc_to_type[sd].erase(msg_type.value);

            // Add the Socket from the list of this type subscriptions
            m_type_to_soc[msg_type.value].erase(sd);
        }
    }

    LOG(INFO) << "FD (" << sd->getSocketFd() << ") "
              << std::string(subscribe ? "subscribed to" : "unsubscribed from")
              << " the following types: " << log_types.str();

    LOG(DEBUG) << "FD (" << sd->getSocketFd() << ") subscriptions: " << std::hex
               << m_soc_to_type[sd] << std::dec;

    return true;
}

bool BrokerServer::socket_connected()
{
    // Accept the connection
    auto new_socket = std::shared_ptr<Socket>(m_server_socket->acceptConnections());

    // Check for errors
    const auto error_msg = m_server_socket->getError();
    if ((!new_socket) || (!error_msg.empty())) {
        LOG(ERROR) << "Socket error: " << error_msg;
        return false;
    }

    LOG(DEBUG) << "Accepted new connection, fd = " << new_socket->getSocketFd();

    // Add the newly accepted socket into the poll
    EventLoop::EventHandlers handlers{
        // Handle incoming data
        .on_read =
            [new_socket, this](int fd, EventLoop &loop) {
                // NOTE: Do NOT stop the broker on parsing errors...
                handle_msg(new_socket);
                return true;
            },

        // Not implemented
        .on_write = nullptr,

        // Remove the socket on disconnections or errors
        .on_disconnect =
            [new_socket, this](int fd, EventLoop &loop) {
                // NOTE: Do NOT stop the broker on errors...
                socket_disconnected(new_socket);
                return true;
            },
        .on_error =
            [new_socket, this](int fd, EventLoop &loop) {
                // NOTE: Do NOT stop the broker on errors...
                socket_disconnected(new_socket);
                return true;
            },
    };
    if (!m_event_loop->register_handlers(new_socket->getSocketFd(), handlers)) {
        LOG(ERROR) << "Failed adding new socket into the poll!";
        return false;
    }

    return true;
}

bool BrokerServer::socket_disconnected(const std::shared_ptr<Socket> &sd)
{
    LOG(DEBUG) << "Socket disconnected: FD(" << sd->getSocketFd() << ")";

    // Delete the Socket from the list of types subscriptions
    for (auto &type : m_soc_to_type[sd]) {
        m_type_to_soc[type].erase(sd);
    }

    // Delete the type from the list of this Socket subscriptions
    m_soc_to_type.erase(sd);

    return true;
}

} // namespace broker
} // namespace transport
} // namespace beerocks
