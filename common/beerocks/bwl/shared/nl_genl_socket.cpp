/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "nl_genl_socket.h"

#include <linux/netlink.h>

namespace bwl {

nl_genl_socket::nl_genl_socket() : netlink_socket(NETLINK_GENERIC) {}

} // namespace bwl
