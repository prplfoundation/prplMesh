/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport.h"

namespace beerocks {
namespace transport {

Ieee1905Transport::Ieee1905Transport(const std::shared_ptr<broker::BrokerServer> &broker,
                                     const std::shared_ptr<EventLoop> &event_loop)
    : m_broker(broker), m_event_loop(event_loop)
{
    LOG_IF(!m_broker, FATAL) << "Broker server is a null pointer!";
    LOG_IF(!m_event_loop, FATAL) << "Event loop is a null pointer!";
}

void Ieee1905Transport::run()
{
    LOG(INFO) << "Starting 1905 transport...";

    // Register broker handlers for internal and external messages
    m_broker->register_external_message_handler(
        [&](std::unique_ptr<messages::Message> &msg, broker::BrokerServer &broker) -> bool {
            LOG(DEBUG) << "Processing external message: " << uint32_t(msg->type());
            handle_broker_pollin_event(msg);
            return true;
        });

    m_broker->register_internal_message_handler(
        [&](std::unique_ptr<messages::Message> &msg, broker::BrokerServer &broker) -> bool {
            LOG(DEBUG) << "Processing internal message: " << uint32_t(msg->type());
            handle_broker_pollin_event(msg);
            return true;
        });

    // init netlink socket
    if (!open_netlink_socket()) {
        MAPF_ERR("cannot open netlink socket.");
        return;
    }

    // Create a shared_ptr socket wrapper for the netlink socket
    auto netlink_socket = std::shared_ptr<Socket>(new Socket(netlink_fd_), [](Socket *socket) {
        // Close the socket file descriptor
        if (socket) {
            close(socket->getSocketFd());
        }
    });

    // Add the netlink socket into the broker's event loop
    m_event_loop->register_handlers(netlink_socket->getSocketFd(),
                                    {
                                        // Accept incoming connections
                                        .on_read =
                                            [&](int fd, EventLoop &loop) {
                                                LOG(DEBUG)
                                                    << "incoming message on the netlink socket";
                                                handle_netlink_pollin_event();
                                                return true;
                                            },

                                        // Not implemented
                                        .on_write = nullptr,

                                        // Fail on server socket disconnections or errors
                                        .on_disconnect =
                                            [&](int fd, EventLoop &loop) {
                                                LOG(ERROR) << "netlink socket disconnected";
                                                return false;
                                            },
                                        .on_error =
                                            [&](int fd, EventLoop &loop) {
                                                LOG(ERROR) << "netlink socket error";
                                                return false;
                                            },
                                    });
}

} // namespace transport
} // namespace beerocks
