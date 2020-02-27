/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "nl_genl_socket.h"

#include <linux/netlink.h>

namespace bwl {

nl_genl_socket::nl_genl_socket() : netlink_socket(NETLINK_GENERIC) {}

} // namespace bwl
