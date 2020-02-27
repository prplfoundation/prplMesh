/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_SOCKET_H__
#define __BWL_NL80211_SOCKET_H__

#include "nl_genl_socket.h"

namespace bwl {

/**
 * @brief Extension to the generic Netlink protocol class for NL80211.
 *
 * The NL80211 is the 802.11 Netlink-based userspace interface for the cfg80211 configuration
 * system for wireless hardware.
 */
class nl80211_socket : public nl_genl_socket {

public:
    /**
     * @brief Class destructor.
     */
    virtual ~nl80211_socket() = default;

    /**
     * @brief Connects the Netlink socket.
     *
     * Calls base class method with the same name and then, after connection has been established,
     * increases the internal buffer size of the socket and obtains the "nl80211" family id, which
     * will later be used to create NL80211 messages.
     *
     * @return True on success and false otherwise (e.g.: socket is already connected or family
     * not found).
     */
    virtual bool connect() override;

    /**
     * @brief Performs a request/response transaction.
     *
     * Requires Netlink socket to be connected.
     *
     * Installs a hook on the message creation callback to add generic Netlink headers to Netlink
     * message before calling provided callback. Uses numeric family identifier obtained right
     * after socket is connected as well as provided command identifier and optional message flags.
     * Then calls base class method with the same name.
     *
     * @param[in] command Numeric command identifier. Possible values are listed in <linux/nl80211.h>
     * @param[in] flags Additional Netlink message flags (optional).
     * @param[in] msg_create Message creation callback function (called on message to be sent).
     * @param[in] msg_handle Message handling callback function (called on received message).
     *
     * @return True on success and false otherwise.
     */
    virtual bool send_receive_msg(int command, int flags,
                                  std::function<bool(struct nl_msg *msg)> msg_create,
                                  std::function<bool(struct nl_msg *msg)> msg_handle);

private:
    /**
     * NL80211 family id
     * We ask the Kernel to resolve family name "nl80211" to family id (using genl_ctrl_resolve).
     * Family id is used during netlink message initialization, when we add generic netlink
     * headers to it (using genlmsg_put).
     */
    int m_family_id = 0;
};

} // namespace bwl

#endif
