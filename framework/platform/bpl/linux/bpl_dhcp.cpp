/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bpl/bpl_dhcp.h>

namespace beerocks {
namespace bpl {

int dhcp_mon_start(dhcp_mon_cb cb) { return -2; }

int dhcp_mon_handle_event() { return 0; }

int dhcp_mon_stop() { return 0; }

} // namespace bpl
} // namespace beerocks
