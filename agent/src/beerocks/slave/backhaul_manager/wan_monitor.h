/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __WAN_MONITOR_H__
#define __WAN_MONITOR_H__

#include <string>

namespace beerocks {

/*!
 * \brief Monitors the wired WAN port.
 */
class wan_monitor {

    /*
 * Public definitions
 */
public:
    enum class ELinkState {
        eInvalid, //!< Invalid
        eUp,      //!< Link detected on the wired WAN interface
        eDown     //!< Link NOT detected on the wired WAN interface
    };

public:
    wan_monitor();
    ~wan_monitor();

    // Initialize the WAN monitor
    ELinkState initialize(std::string strWanIfaceName);

    // Process incoming netlink message
    ELinkState process();

    int get_netlink_fd() const { return (m_iNetlinkFD); }

private:
    // WAN interface name
    std::string m_strWanIfaceName;

    // Netlink socket file descriptor
    int m_iNetlinkFD;

    // Netlink receive buffer
    char m_arrNLBuff[4096];
};

} // namespace beerocks

#endif
