/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport.h"

#include <bcl/beerocks_backport.h>
#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_event_loop_impl.h>
#include <bcl/network/interface_flags_reader_impl.h>
#include <bcl/network/interface_state_manager_impl.h>
#include <bcl/network/interface_state_monitor_impl.h>
#include <bcl/network/interface_state_reader_impl.h>
#include <bcl/network/sockets_impl.h>

#include <net/if.h>
#include <unistd.h>

using namespace beerocks;
using namespace beerocks::net;
using namespace beerocks::transport;

static std::shared_ptr<EventLoop> create_event_loop()
{
    // Create application event loop to wait for blocking I/O operations.
    return std::make_shared<EventLoopImpl>();
}

static std::shared_ptr<broker::BrokerServer>
create_broker_server(const std::shared_ptr<EventLoop> &event_loop)
{
    // UDS path for broker server socker;
    constexpr const char *broker_uds_path = TMP_PATH "/" BEEROCKS_BROKER_UDS;

    // Number of concurrent connections on the server socket
    constexpr int listen_buffer_size = 10;

    // Create the server UDS socket for the message broker
    auto server_socket = std::make_shared<SocketServer>(broker_uds_path, listen_buffer_size);

    // Create the broker server
    return std::make_shared<broker::BrokerServer>(server_socket, event_loop);
}

static std::shared_ptr<InterfaceStateManager>
create_interface_state_manager(const std::shared_ptr<EventLoop> &event_loop)
{
    // Create NETLINK_ROUTE netlink socket for kernel/user-space communication
    auto socket = std::make_shared<NetlinkRouteSocket>();

    // Create client socket
    ClientSocketImpl<NetlinkRouteSocket> client(socket);

    // Bind client socket to "route netlink" multicast group to listen for multicast packets sent
    // from the kernel containing network interface create/delete/up/down events
    client.bind(NetlinkAddress(RTMGRP_LINK));

    // Create connection to send/receive data using this socket
    auto connection = std::make_shared<SocketConnectionImpl>(socket);

    // Create the interface state monitor
    auto interface_state_monitor =
        std::make_unique<InterfaceStateMonitorImpl>(connection, event_loop);

    // Create the interface flags reader
    auto interface_flags_reader = std::make_shared<InterfaceFlagsReaderImpl>();

    // Create the interface state reader
    auto interface_state_reader =
        std::make_unique<InterfaceStateReaderImpl>(interface_flags_reader);

    // Create the interface state manager
    return std::make_shared<InterfaceStateManagerImpl>(std::move(interface_state_monitor),
                                                       std::move(interface_state_reader));
}

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("transport");

    /**
     * Create required objects in the order defined by the dependency tree.
     */
    auto event_loop              = create_event_loop();
    auto broker                  = create_broker_server(event_loop);
    auto interface_state_manager = create_interface_state_manager(event_loop);

    /**
     * Application exit code: 0 on success and -1 on failure.
     * From this point on, there's a single exit point to allow for start/stop methods to be
     * cleanly called in pairs.
     */
    int exit_code = 0;

    /**
     * Create the IEEE1905 transport process.
     */
    Ieee1905Transport ieee1905_transport(interface_state_manager, broker, event_loop);

    /**
     * Start the interface state monitor
     */
    if (interface_state_manager->start()) {

        /**
         * Start the message broker
         */
        if (broker->start()) {

            /**
             * Start the IEEE1905 transport process
             */
            if (ieee1905_transport.start()) {

                /**
                 * Run the application event loop
                 */
                MAPF_INFO("starting main loop...");
                while (0 == exit_code) {
                    if (event_loop->run() < 0) {
                        LOG(ERROR) << "Broker event loop failure!";
                        exit_code = -1;
                    }
                }
                MAPF_INFO("done");

                ieee1905_transport.stop();

            } else {
                LOG(ERROR) << "Unable to start transport process!";
                exit_code = -1;
            }

            broker->stop();

        } else {
            LOG(ERROR) << "Unable to start message broker!";
            exit_code = -1;
        }

        interface_state_manager->stop();

    } else {
        LOG(ERROR) << "Unable to start interface state manager!";
        exit_code = -1;
    }

    return exit_code;
}
