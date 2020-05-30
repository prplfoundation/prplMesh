/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport.h>

#include <net/if.h>
#include <unistd.h>

using namespace mapf;

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("transport");
    Ieee1905Transport ieee1905_transport;

    MAPF_INFO("starting main loop...");
    ieee1905_transport.run();

    MAPF_INFO("done");

    return 0;
}
