/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <iostream>
#include <mapf/broker/broker.h>
#include <mapf/local_bus.h>
#include <mapf/topology_discovery_agent/topology_discovery_agent.h>
#include <signal.h>

static void signal_handler(int signum)
{
    MAPF_INFO("PID #" << getpid() << ": Interrupt Signal (" << signum << ") received.");
    exit(signum);
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, signal_handler);

    mapf::DiscAgent da;

    // run forever
    da.Run();
}
