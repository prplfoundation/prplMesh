/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/network_utils.h>

#include <easylogging++.h>

#include <errno.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define IFLIST_REPLY_BUFFER 8192

namespace beerocks {
namespace net {

/**
 * @brief Netlink request type.
 */
struct nl_req_t {
    /**
     * Netlink message
     */
    nlmsghdr hdr;

    /**
     * "general form of address family dependent" message, i.e. how to tell which AF we are
     * interested in. */
    rtgenmsg gen;
};

/**
 * @brief Gets interface statistics for the given network interface.
 *
 * Gets interface stats for given network interface from the specified Netlink message of type
 * RTM_NEWLINK.
 *
 * @param[in] msg_ptr Pointer to the Netlink message containing the data.
 * @param[in] iface_name Name of the network interface.
 * @param[in, out] iface_stats Interface statistics structure with values read.
 *
 * @return True on success and false otherwise.
 */
static bool get_iface_stats(const nlmsghdr *msg_ptr, const std::string &iface_name,
                            sInterfaceStats &iface_stats)
{
    size_t length = 0;
    if (msg_ptr->nlmsg_len <= NLMSG_LENGTH(sizeof(ifinfomsg))) {
        return false;
    }

    length = msg_ptr->nlmsg_len - NLMSG_LENGTH(sizeof(ifinfomsg));

    /**
     * Loop over all attributes of the RTM_NEWLINK message
     */
    bool iface_name_matched = false;
    ifinfomsg *iface        = static_cast<ifinfomsg *>(NLMSG_DATA(msg_ptr));
    for (const rtattr *attribute = IFLA_RTA(iface); RTA_OK(attribute, length);
         attribute               = RTA_NEXT(attribute, length)) {
        switch (attribute->rta_type) {
        case IFLA_IFNAME:
            /**
             * This message contains the stats for the interface we are interested in
             */
            if (0 == std::strncmp(iface_name.c_str(), static_cast<char *>(RTA_DATA(attribute)),
                                  iface_name.length() + 1)) {
                iface_name_matched = true;
            }
            break;
        case IFLA_STATS:
            if (iface_name_matched) {
                rtnl_link_stats *stats = static_cast<rtnl_link_stats *>(RTA_DATA(attribute));

                iface_stats.tx_bytes   = stats->tx_bytes;
                iface_stats.tx_errors  = stats->tx_errors;
                iface_stats.tx_packets = stats->tx_packets;
                iface_stats.rx_bytes   = stats->rx_bytes;
                iface_stats.rx_errors  = stats->rx_errors;
                iface_stats.rx_packets = stats->rx_packets;

                return true;
            }
            break;
        }
    }

    return false;
}

/**
 * @brief Gets interface statistics for the given network interface.
 *
 * Gets interface stats for given network interface by sending a RTM_GETLINK Netlink request
 * through the specified Netlink socket and parsing received response.
 *
 * @param[in] fd File descriptor of a connected Netlink socket.
 * @param[in] iface_name Name of the network interface.
 * @param[in, out] iface_stats Interface statistics structure with values read.
 *
 * @return True on success and false otherwise.
 */
static bool get_iface_stats(int fd, const std::string &iface_name, sInterfaceStats &iface_stats)
{
    bool result = false;

    sockaddr_nl socket{}; /* the remote (kernel space) side of the communication */
    msghdr rtnl_msg{};    /* generic msghdr struct for use with sendmsg */
    iovec io{};           /* IO vector for sendmsg */
    nl_req_t req{};       /* structure that describes the Netlink packet itself */

    /**
     * Netlink socket is ready for use, prepare and send request
     */
    socket.nl_family = AF_NETLINK; /* fill-in kernel address (destination of our message) */

    req.hdr.nlmsg_len    = NLMSG_LENGTH(sizeof(rtgenmsg));
    req.hdr.nlmsg_type   = RTM_GETLINK;
    req.hdr.nlmsg_flags  = NLM_F_REQUEST | NLM_F_DUMP;
    req.hdr.nlmsg_seq    = 1;
    req.hdr.nlmsg_pid    = 0;
    req.gen.rtgen_family = AF_PACKET; /*  no preferred AF, we will get *all* interfaces */

    io.iov_base          = &req;
    io.iov_len           = req.hdr.nlmsg_len;
    rtnl_msg.msg_iov     = &io;
    rtnl_msg.msg_iovlen  = 1;
    rtnl_msg.msg_name    = &socket;
    rtnl_msg.msg_namelen = sizeof(socket);

    if (sendmsg(fd, static_cast<msghdr *>(&rtnl_msg), 0) < 0) {
        LOG(ERROR) << "Unable to send message through Netlink socket: " << strerror(errno);
        return false;
    }

    bool msg_done = false; /* flag to end loop parsing */

    /**
     * Parse reply until message is done
     */
    while (!msg_done) {
        msghdr rtnl_reply{}; /* generic msghdr structure for use with recvmsg */

        /* a large buffer to receive lots of link information */
        char reply[IFLIST_REPLY_BUFFER];

        io.iov_base            = reply;
        io.iov_len             = IFLIST_REPLY_BUFFER;
        rtnl_reply.msg_iov     = &io;
        rtnl_reply.msg_iovlen  = 1;
        rtnl_reply.msg_name    = &socket;
        rtnl_reply.msg_namelen = sizeof(socket);

        /**
         * Read as much data as fits in the receive buffer
         */
        int length = recvmsg(fd, &rtnl_reply, 0);
        for (const nlmsghdr *msg_ptr            = reinterpret_cast<const nlmsghdr *>(reply);
             NLMSG_OK(msg_ptr, length); msg_ptr = NLMSG_NEXT(msg_ptr, length)) {
            switch (msg_ptr->nlmsg_type) {
            case NLMSG_DONE:
                /**
                 * This is the special meaning NLMSG_DONE message we asked for by using NLM_F_DUMP flag
                 */
                msg_done = true;
                break;
            case RTM_NEWLINK:
                /**
                 * This is a RTM_NEWLINK message, which contains lots of information about a link
                 */
                if (get_iface_stats(msg_ptr, iface_name, iface_stats)) {
                    msg_done = true;
                    result   = true;
                }
                break;
            }
        }
    }

    return result;
}

/**
 * @brief Gets interface statistics for the given network interface.
 *
 * Gets interface stats for given network by means of a Netlink socket using NETLINK_ROUTE
 * protocol.
 *
 * @param[in] iface_name Name of the network interface.
 * @param[in, out] iface_stats Interface statistics structure with values read.
 *
 * @return True on success and false otherwise.
 */
static bool get_iface_stats(const std::string &iface_name, sInterfaceStats &iface_stats)
{
    /**
     * Create Netlink socket for kernel/user-space communication.
     * No need to call bind() as packets are sent only between the kernel and the originating
     * process (no multicasting).
     */
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        LOG(ERROR) << "Failed creating Netlink socket: " << strerror(errno);
        return false;
    }

    /**
     * Get interface stats using Netlink socket
     */
    bool result = get_iface_stats(fd, iface_name, iface_stats);

    /**
     * Clean up and finish properly
     */
    close(fd);

    return result;
}

bool network_utils::get_iface_stats(const std::string &iface_name, sInterfaceStats &iface_stats)
{
    return beerocks::net::get_iface_stats(iface_name, iface_stats);
}

} // namespace net
} // namespace beerocks
