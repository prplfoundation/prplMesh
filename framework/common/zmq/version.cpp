/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/common/config.h>
#include <mapf/common/logger.h>
#include <zmq.h>

MAPF_INITIALIZE_LOGGER

int main(void)
{
    int major, minor, patch;
    zmq_version(&major, &minor, &patch);
    printf("Current 0MQ version is %d.%d.%d\n", major, minor, patch);
    printf("Current MAPF version is %s\n", MAPF_VERSION);
    return 0;
}
