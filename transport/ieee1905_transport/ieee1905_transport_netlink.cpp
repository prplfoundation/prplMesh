/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport.h>

#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

// Why is this netlink interface tracking required?
// ------------------------------------------------
// When an interface is down or comes down - a poll on the interface's raw socket returns an error. When this happens
// we have no choice but to close the socket for that interface. At a later time the interface may become again active
// and we should then re-open the interface's raw socket, and add it to the poll list. To do that we need an event
// to tell us that the interface is up and running - This is what we use netlink for.

namespace mapf {

bool Ieee1905Transport::open_netlink_socket()
{
    struct sockaddr_nl addr;

    // open a netlink socket of NETLINK_ROUTE family to get link updates
    int sockfd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (sockfd < 0) {
        MAPF_ERR("cannot open netlink socket.");
        return false;
    }

    memset((void *)&addr, 0, sizeof(addr));

    addr.nl_family = AF_NETLINK;
    addr.nl_pid    = 0;           // let the kernel assign nl_pid
    addr.nl_groups = RTMGRP_LINK; // subscribe for link updates

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        MAPF_ERR("cannot bind netlink socket.");
        close(sockfd);
        return false;
    }

    netlink_fd_ = sockfd;

    return true;
}

// handle a single netlink message (each netlink received buffer can contain many messages)
// return 1 if successful, or zero if last message or -1 on error.
int Ieee1905Transport::handle_netlink_message(struct nlmsghdr *msghdr)
{
    // done
    if (msghdr->nlmsg_type == NLMSG_DONE) {
        return 0;
    }

    // error
    if (msghdr->nlmsg_type == NLMSG_ERROR) {
        MAPF_ERR("received netlink NLMSG_ERROR message.");
        return -1;
    }

    // only consider interface state update messages (silently ignore other messages)
    if (msghdr->nlmsg_type == RTM_NEWLINK || msghdr->nlmsg_type == RTM_DELLINK) {
        struct ifinfomsg *ifi = (struct ifinfomsg *)NLMSG_DATA(msghdr);
        bool is_active        = (ifi->ifi_flags & IFF_RUNNING) && (ifi->ifi_flags & IFF_UP);

        MAPF_DBG("received netlink RTM_NEWLINK/RTM_DELLINK message (interface "
                 << ifi->ifi_index << " is " << (is_active ? "active" : "inactive") << ").");

        if (ifi->ifi_index > 0 && network_interfaces_.count(ifi->ifi_index) > 0) {
            handle_interface_status_change((unsigned)ifi->ifi_index, is_active);
        } else if (ifi->ifi_index < 0) {
            MAPF_WARN("bad interface index (" << ifi->ifi_index << ") in netlink message.");
        }
    }

    return 1;
}

void Ieee1905Transport::handle_netlink_pollin_event()
{
    if (netlink_fd_ < 0) {
        MAPF_ERR("invalid netlink socket file descriptor.");
        return;
    }

    char buf[4096];
    ssize_t len = recvfrom(netlink_fd_, buf, sizeof(buf), 0, NULL, 0);
    if (len < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return;

        MAPF_ERR("cannot read from netlink socket \"" << strerror(errno) << "\" (" << errno
                                                      << ").");
        return;
    }

    // the stream can contain multiple messages
    for (struct nlmsghdr *msghdr = (struct nlmsghdr *)buf; NLMSG_OK(msghdr, len);
         msghdr                  = NLMSG_NEXT(msghdr, len)) {
        if (handle_netlink_message(msghdr) <= 0) {
            break;
        }
    }

    return;
}

} // namespace mapf