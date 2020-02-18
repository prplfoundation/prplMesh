/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport.h>

#include <net/if.h>
#include <unistd.h>

//MAPF_INITIALIZE_LOGGER
///SHARE_EASYLOGGINGPP(el::Helpers::storage())
MAPF_INITIALIZE_LOGGER

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
