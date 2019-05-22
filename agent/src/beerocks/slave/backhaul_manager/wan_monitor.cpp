/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "wan_monitor.h"

#include <beerocks/bcl/beerocks_logging.h>
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include <errno.h>           // errno
#include <linux/netlink.h>   // Netlink
#include <linux/rtnetlink.h> // Netlink
#include <net/if.h>          // IFF_*, ifreq
#include <netinet/in.h>      // IPPROTO_IP
#include <sys/ioctl.h>       // SIOCGIFFLAGS
#include <unistd.h>          // close

using namespace beerocks::net;

namespace beerocks {
namespace backhaul_manager {

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int netlink_open_socket()
{
    // Open a netlink socket
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        LOG(ERROR) << "Failed creating netlink socket";
        return -1;
    }

    // Initialize the netlink address (mainly groups)
    struct sockaddr_nl addr = {0};

    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK;
    // RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_NEIGH;

    // Bind the socket
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOG(ERROR) << "Failed binding the netlink socket";
        close(fd);

        return (-1);
    }

    return (fd);
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

wan_monitor::wan_monitor() : m_iNetlinkFD(-1) {}

wan_monitor::~wan_monitor()
{
    if (m_iNetlinkFD != -1)
        close(m_iNetlinkFD);
}

wan_monitor::ELinkState wan_monitor::initialize(std::string strWanIfaceName)
{
    // Close the previous FD
    if (m_iNetlinkFD != -1) {
        // NOTE: If the FD is used in an external select(), it's up to the
        //       user to remove it before calling this method.

        close(m_iNetlinkFD);
        m_iNetlinkFD = -1;
    }

    // Open a new netlink socket
    if ((m_iNetlinkFD = netlink_open_socket()) == -1)
        return (ELinkState::eInvalid);

    // Store the interface name
    m_strWanIfaceName = strWanIfaceName;

    // Return the interface state
    return ((network_utils::linux_iface_is_up_and_running(m_strWanIfaceName)) ? ELinkState::eUp
                                                                              : ELinkState::eDown);
}

wan_monitor::ELinkState wan_monitor::process()
{
    if (m_iNetlinkFD == -1) {
        LOG(ERROR) << "Invalid netlink socket!";
        return (ELinkState::eInvalid);
    }

    struct sockaddr_nl addr = {0};
    struct iovec iov        = {m_arrNLBuff, sizeof m_arrNLBuff};
    struct msghdr msg       = {(void *)&addr, sizeof addr, &iov, 1, NULL, 0, 0};

    // Read a message from the netlink socket
    ssize_t len = recvmsg(m_iNetlinkFD, &msg, 0);

    if (len < 0) {
        LOG(ERROR) << "recvmsg error: " << strerror(errno);
        return (ELinkState::eInvalid);
    } else if (len == 0) {
        LOG(DEBUG) << "recvmsg EOF";
        return (ELinkState::eInvalid);
    }

    // Process received message(s)
    struct nlmsghdr *hnl = nullptr;
    for (hnl = (struct nlmsghdr *)m_arrNLBuff; NLMSG_OK(hnl, uint32_t(len));
         hnl = NLMSG_NEXT(hnl, len)) {

        // Completed reading - exit gracefully (as no error occurred,
        // but this is not a valid state)
        if (hnl->nlmsg_type == NLMSG_DONE)
            return (ELinkState::eInvalid);

        // Error in the Message
        if (hnl->nlmsg_type == NLMSG_ERROR) {
            LOG(ERROR) << "NLMSG_ERROR - Invalid netlink message!";
            return (ELinkState::eInvalid);
        }

        // LINK related message
        if (hnl->nlmsg_type == RTM_NEWLINK || hnl->nlmsg_type == RTM_DELLINK) {
            char name[IFNAMSIZ]   = {0};
            struct ifinfomsg *ifi = (struct ifinfomsg *)NLMSG_DATA(hnl);

            // Convert the interface index to name
            if_indextoname(ifi->ifi_index, name);

            // Skip events for other interfaces
            if (m_strWanIfaceName.compare(name) != 0) {
                LOG(DEBUG) << "Link detected for non-WAN interface '" << name << "'. Skipping...";

                continue;
            }

            LOG(DEBUG) << "Interface '" << name << "', msg_type: " << int(hnl->nlmsg_type)
                       << ", running: " << int(ifi->ifi_flags & IFF_RUNNING);

            // Return WAN interface link state
            if ((hnl->nlmsg_type == RTM_NEWLINK && ifi->ifi_flags & IFF_RUNNING))
                return (ELinkState::eUp);
            else
                return (ELinkState::eDown);
        }
    }

    return (ELinkState::eInvalid);
}

} // namespace backhaul_manager
} // namespace beerocks