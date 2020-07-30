/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_socket_event_loop.h>

#include <mapf/common/config.h>

#include "ieee1905_transport.h"

#include <unistd.h>

namespace beerocks {
namespace transport {

using broker::BrokerServer;

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Number of concurrent connections on the server socket
static constexpr int listen_buffer_size = 10;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

void Ieee1905Transport::run()
{
    // Broker server UDS socket
    SocketServer broker_server_socket(std::string(TMP_PATH "/" BEEROCKS_BROKER_UDS),
                                      listen_buffer_size);

    // Broker socket based event loop
    SocketEventLoop broker_event_loop;

    LOG(INFO) << "Starting 1905 transport...";

    // Create the broker server
    m_broker = std::make_unique<BrokerServer>(broker_server_socket, broker_event_loop);
    LOG_IF(!m_broker, FATAL) << "Failed creating broker server!";

    // Register broker handlers for internal and external messages
    m_broker->register_external_message_handler(
        [&](std::unique_ptr<messages::Message> &msg, BrokerServer &broker) -> bool {
            LOG(DEBUG) << "Processing external message: " << uint32_t(msg->type());
            handle_broker_pollin_event(msg);
            return true;
        });

    m_broker->register_internal_message_handler(
        [&](std::unique_ptr<messages::Message> &msg, BrokerServer &broker) -> bool {
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
    broker_event_loop.add_event(netlink_socket,
                                {
                                    // Accept incoming connections
                                    .on_read =
                                        [&](BrokerServer::BrokerEventLoop::EventType socket,
                                            BrokerServer::BrokerEventLoop::EventLoopType &loop) {
                                            LOG(DEBUG) << "incoming message on the netlink socket";
                                            handle_netlink_pollin_event();
                                            return true;
                                        },

                                    // Not implemented
                                    .on_write   = nullptr,
                                    .on_timeout = nullptr,

                                    // Fail on server socket disconnections or errors
                                    .on_disconnect =
                                        [&](BrokerServer::BrokerEventLoop::EventType socket,
                                            BrokerServer::BrokerEventLoop::EventLoopType &loop) {
                                            LOG(ERROR) << "netlink socket disconnected";
                                            return false;
                                        },
                                    .on_error =
                                        [&](BrokerServer::BrokerEventLoop::EventType socket,
                                            BrokerServer::BrokerEventLoop::EventLoopType &loop) {
                                            LOG(ERROR) << "netlink socket error";
                                            return false;
                                        },
                                });

    // Run the broker event loop
    for (;;) {
        if (m_broker->run() < 0) {
            LOG(ERROR) << "Broker event loop failure!";
            return;
        }
    }
}

} // namespace transport
} // namespace beerocks
