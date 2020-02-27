/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL_GENL_SOCKET_H__
#define __BWL_NL_GENL_SOCKET_H__

#include "netlink_socket.h"

namespace bwl {

/**
 * @brief C++ wrapper class around the generic Netlink protocol library (libnl-genl).
 *
 * This is a very simple C++ wrapper class around the generic Netlink protocol library. Its aim is
 * to facilitate the use of the library from C++ applications. This class extends the
 * netlink_socket base class for the generic Netlink protocol (NETLINK_GENERIC).
 *
 * Known derived classes: nl80211_socket.
 */
class nl_genl_socket : public netlink_socket {

public:
    /**
     * @brief Class constructor.
     *
     * Calls base class constructor with protocol NETLINK_GENERIC.
     */
    nl_genl_socket();

    /**
     * @brief Class destructor.
     */
    virtual ~nl_genl_socket() = default;
};

} // namespace bwl

#endif
