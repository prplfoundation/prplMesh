/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "nl80211_socket.h"

#include <netlink/genl/ctrl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/genl.h>

#include <easylogging++.h>

/**
 * Receive and transmit socket buffer size in bytes.
 */
static constexpr int netlink_buffer_size = 8192;

/**
 * Will cause the netlink port to be set to the port assigned to the netlink socket just before
 * sending the message off.
 *
 * This macro is already defined in <netlink/msg.h> for some platforms, but not for others maybe
 * because it was introduced at a later version of the library being used. Thus we define the macro
 * if it's not already being defined.
 */
#ifndef NL_AUTO_PORT
#define NL_AUTO_PORT 0
#endif

namespace bwl {

bool nl80211_socket::connect()
{
    // Connect the socket
    bool result = netlink_socket::connect();

    // Increase the socket's internal buffer size
    if (result) {
        int rc =
            nl_socket_set_buffer_size(m_nl_socket.get(), netlink_buffer_size, netlink_buffer_size);
        if (rc < 0) {
            LOG(ERROR) << "Failed to set buffer size! Error: " << rc;
        }
    }

    // Resolve the generic nl80211 family id
    if (result) {
        const char *family_name = "nl80211";

        m_family_id = genl_ctrl_resolve(m_nl_socket.get(), family_name);
        if (0 > m_family_id) {
            LOG(ERROR) << "'" << family_name << "' family not found!";
            result = false;

            close();
        }
    }

    return result;
}

bool nl80211_socket::send_receive_msg(int command, int flags,
                                      std::function<bool(struct nl_msg *msg)> msg_create,
                                      std::function<void(struct nl_msg *msg)> msg_handle)
{
    return netlink_socket::send_receive_msg(
        [&](struct nl_msg *msg) -> bool {
            // Initialize the netlink message
            if (!genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, m_family_id, 0, flags, command, 0)) {
                LOG(ERROR) << "Failed initializing the netlink message!";
                return false;
            }

            // Call the user's message create function
            return msg_create(msg);
        },
        msg_handle);
}

} // namespace bwl
