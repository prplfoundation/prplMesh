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

#include <net/if.h>
#include <unistd.h>

using namespace beerocks;
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

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("transport");

    /**
     * Create required objects in the order defined by the dependency tree.
     */
    auto event_loop = create_event_loop();
    auto broker     = create_broker_server(event_loop);

    /**
     * Application exit code: 0 on success and -1 on failure.
     * From this point on, there's a single exit point to allow for start/stop methods to be
     * cleanly called in pairs.
     */
    int exit_code = 0;

    /**
     * Create the IEEE1905 transport process.
     */
    Ieee1905Transport ieee1905_transport(broker, event_loop);

    /**
     * Start the message broker
     */
    if (broker->start()) {

        /**
         * Start the IEEE1905 transport process
         */
        ieee1905_transport.run();

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

        broker->stop();

    } else {
        LOG(ERROR) << "Unable to start message broker!";
        exit_code = -1;
    }

    return exit_code;
}
