/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/common/config.h>
#include <mapf/common/logger.h>
#include <nng/nng.h>

int main(void)
{
    printf("Current nng version: %s\n", nng_version());
    printf("Current MAPF version is %s\n", MAPF_VERSION);
    return 0;
}
