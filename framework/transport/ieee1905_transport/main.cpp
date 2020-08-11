/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport.h"

#include <bcl/beerocks_event_loop_impl.h>

#include <net/if.h>
#include <unistd.h>

using namespace beerocks;
using namespace beerocks::transport;

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("transport");

    auto event_loop = std::make_shared<EventLoopImpl>();

    Ieee1905Transport ieee1905_transport(event_loop);

    ieee1905_transport.run();

    // Run the application event loop
    MAPF_INFO("starting main loop...");
    for (;;) {
        if (event_loop->run() < 0) {
            LOG(ERROR) << "Broker event loop failure!";
            return -1;
        }
    }
    MAPF_INFO("done");

    return 0;
}
