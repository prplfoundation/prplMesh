/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee802_3_link_metrics_collector.h"

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

// SPEED values
#include <linux/ethtool.h>

#define IFLIST_REPLY_BUFFER 8192

namespace beerocks {

/**
 * @brief Netlink request type.
 */
struct nl_req_t {
    /**
     * Netlink message
     */
    struct nlmsghdr hdr;

    /**
     * "general form of address family dependent" message, i.e. how to tell which AF we are
     * interested in. */
    struct rtgenmsg gen;
};

/**
 * @brief Gets link metrics of an Ethernet network interface.
 *
 * Gets link metrics for given Ethernet network interface from the specified Netlink message of
 * type RTM_NEWLINK.
 *
 * @param[in] h Pointer to the Netlink message containing the data.
 * @param[in] local_interface_name Name of the Ethernet network interface.
 * @param[in, out] link_metris Link metrics structure with read values.
 *
 * @return True on success and false otherwise.
 */
static bool get_link_metrics(const struct nlmsghdr *h, const std::string &local_interface_name,
                             sLinkMetrics &link_metrics)
{
    bool result = false;

    struct ifinfomsg *iface = static_cast<ifinfomsg *>(NLMSG_DATA(h));

    size_t length = 0;
    if (h->nlmsg_len > NLMSG_LENGTH(sizeof(*iface))) {
        length = h->nlmsg_len - NLMSG_LENGTH(sizeof(*iface));
    }

    /**
     * Loop over all attributes of the RTM_NEWLINK message
     */
    for (struct rtattr *attribute = IFLA_RTA(iface); RTA_OK(attribute, length);
         attribute                = RTA_NEXT(attribute, length)) {
        switch (attribute->rta_type) {
        case IFLA_IFNAME:
            /**
             * This message contains the stats for the interface we are interested in
             */
            if (0 == std::strncmp(local_interface_name.c_str(), (char *)RTA_DATA(attribute),
                                  local_interface_name.length() + 1)) {
                result = true;
            }
            break;
        case IFLA_STATS:
            if (result) {
                struct rtnl_link_stats *stats = (struct rtnl_link_stats *)RTA_DATA(attribute);

                /**
                 * Get interface speed into PHY rate.
                 */
                uint32_t phy_rate_mbps = UINT32_MAX;
                beerocks::net::network_utils::linux_iface_get_speed(local_interface_name,
                                                                    phy_rate_mbps);

                link_metrics.transmitter.packet_errors       = stats->tx_errors;
                link_metrics.transmitter.transmitted_packets = stats->tx_packets;
                /**
                 * Note: The MAC throughput capacity is a function of the physical data rate and
                 * of the MAC overhead. We could somehow compute such overhead or, for simplicity,
                 * set the MAC throughput as a percentage of the physical data rate.
                 * For Ethernet, we can estimate the overhead: 7 bytes preamble, 1 byte SFD, 14
                 * bytes header, 4 bytes CRC and 12 bytes of interpacket gap on a 1500 byte
                 * payload. So 1500/1538.
                 * (see https://en.wikipedia.org/wiki/Ethernet_frame)
                 */
                const float layer2_payload_size = 1500;
                const float layer1_total_size   = 1538;
                link_metrics.transmitter.mac_throughput_capacity_mbps =
                    phy_rate_mbps * (layer2_payload_size / layer1_total_size);
                // Note: For simplicity, link availability is set to "100% of the time"
                link_metrics.transmitter.link_availability = 100;
                link_metrics.transmitter.phy_rate_mbps     = phy_rate_mbps;

                link_metrics.receiver.packet_errors    = stats->rx_errors;
                link_metrics.receiver.packets_received = stats->rx_packets;
                link_metrics.receiver.rssi             = UINT8_MAX;
            }
            break;
        }
    }

    return result;
}

/**
 * @brief Gets link metrics of an Ethernet network interface.
 *
 * Gets link metrics for given Ethernet network interface by sending a RTM_GETLINK Netlink request
 * through the specified Netlink socket and parsing received response.
 *
 * @param[in] fd File descriptor of a connected Netlink socket.
 * @param[in] local_interface_name Name of the Ethernet network interface.
 * @param[in, out] link_metris Link metrics structure with read values.
 *
 * @return True on success and false otherwise.
 */
static bool get_link_metrics(int fd, const std::string &local_interface_name,
                             sLinkMetrics &link_metrics)
{
    bool result = false;

    struct sockaddr_nl socket; /* the remote (kernel space) side of the communication */

    struct msghdr rtnl_msg {
    }; /* generic msghdr struct for use with sendmsg */
    struct iovec io {
    }; /* IO vector for sendmsg */
    struct nl_req_t req {
    }; /* structure that describes the Netlink packet itself */

    /**
     * Netlink socket is ready for use, prepare and send request
     */
    socket.nl_family = AF_NETLINK; /* fill-in kernel address (destination of our message) */

    req.hdr.nlmsg_len    = NLMSG_LENGTH(sizeof(struct rtgenmsg));
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

    if (sendmsg(fd, (struct msghdr *)&rtnl_msg, 0) < 0) {
        LOG(ERROR) << "Unable to send message through Netlink socket: " << strerror(errno);
    } else {
        int msg_done = 0; /* flag to end loop parsing */

        /**
         * Parse reply until message is done
         */
        while (!msg_done) {
            int length;
            struct nlmsghdr *msg_ptr; /* pointer to current message part */

            struct msghdr rtnl_reply {
            }; /* generic msghdr structure for use with recvmsg */

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
            if ((length = recvmsg(fd, &rtnl_reply, 0)) != 0) {
                for (msg_ptr = (struct nlmsghdr *)reply; NLMSG_OK(msg_ptr, length);
                     msg_ptr = NLMSG_NEXT(msg_ptr, length)) {
                    switch (msg_ptr->nlmsg_type) {
                    case NLMSG_DONE:
                        /**
                         * This is the special meaning NLMSG_DONE message we asked for by using NLM_F_DUMP flag
                         */
                        msg_done = 1;
                        break;
                    case RTM_NEWLINK:
                        /**
                         * This is a RTM_NEWLINK message, which contains lots of information about a link
                         */
                        if (get_link_metrics(msg_ptr, local_interface_name, link_metrics)) {
                            msg_done = 1;
                            result   = true;
                        }
                        break;
                    }
                }
            }
        }
    }

    return result;
}

/**
 * @brief Gets link metrics of an Ethernet network interface.
 *
 * Gets link metrics for given Ethernet network by means of a Netlink socket using NETLINK_ROUTE
 * protocol.
 *
 * @param[in] local_interface_name Name of the Ethernet network interface.
 * @param[in, out] link_metris Link metrics structure with read values.
 *
 * @return True on success and false otherwise.
 */
static bool get_link_metrics(const std::string &local_interface_name, sLinkMetrics &link_metrics)
{
    bool result = false;

    /**
     * Create Netlink socket for kernel/user-space communication.
     * No need to call bind() as packets are sent only between the kernel and the originating
     * process (no multicasting).
     */
    int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd < 0) {
        LOG(ERROR) << "Failed creating Netlink socket: " << strerror(errno);
    } else {
        /**
         * Get link metrics using Netlink socket
         */
        result = get_link_metrics(fd, local_interface_name, link_metrics);

        /**
         * Clean up and finish properly
         */
        close(fd);
    }

    return result;
}

ieee802_3_link_metrics_collector::~ieee802_3_link_metrics_collector() {}

bool ieee802_3_link_metrics_collector::get_link_metrics(
    const std::string &local_interface_name,
    [[gnu::unused]] const sMacAddr &neighbor_interface_address, sLinkMetrics &link_metrics)
{
    return beerocks::get_link_metrics(local_interface_name, link_metrics);
}

} // namespace beerocks
