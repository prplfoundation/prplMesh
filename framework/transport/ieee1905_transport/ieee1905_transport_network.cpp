/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport.h>

#include <arpa/inet.h>
#include <iomanip>
#include <linux/filter.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

namespace mapf {

void Ieee1905Transport::update_network_interfaces(
    std::map<unsigned int, NetworkInterface> updated_network_interfaces)
{
    // find and remove interfaces that are no longer in use
    for (auto it = network_interfaces_.begin(); it != network_interfaces_.end();) {
        unsigned int if_index   = it->first;
        auto &network_interface = it->second;

        if (updated_network_interfaces.count(it->first) == 0) {
            MAPF_DBG("interface " << if_index << " is no longer used.");
            if (network_interface.ieee1905_socket >= 0) {
                poller_.Remove(network_interface.ieee1905_socket);
                poller_.Remove(network_interface.lldp_socket);
                close(network_interface.ieee1905_socket);
                close(network_interface.lldp_socket);
            }

            it = network_interfaces_.erase(it);
        } else {
            ++it;
        }
    }

    // add new interfaces or update existing ones
    for (auto it = updated_network_interfaces.begin(); it != updated_network_interfaces.end();
         ++it) {
        unsigned int if_index           = it->first;
        auto &updated_network_interface = it->second;

        MAPF_DBG("interface " << if_index << " is used.");
        network_interfaces_[if_index].bridge_if_index = updated_network_interface.bridge_if_index;
        network_interfaces_[if_index].is_bridge       = updated_network_interface.is_bridge;

        // must be called before open_interface_socket (address is used for packet filtering)
        if (!get_interface_mac_addr(if_index, network_interfaces_[if_index].addr)) {
            MAPF_WARN("cannot get address of interface " << if_index << ".");
        }

        if (network_interfaces_[if_index].ieee1905_socket < 0) {
            // if the interface is not already open, try to open it and add it to the poller loop
            if (!open_interface_socket(if_index)) {
                MAPF_WARN("cannot open interface " << if_index << ".");
            }

            // add interface raw socket fd to poller loop (unless it's a bridge interface)
            else if (!network_interfaces_[if_index].is_bridge &&
                     network_interfaces_[if_index].ieee1905_socket >= 0) {
                poller_.Add(network_interfaces_[if_index].ieee1905_socket);
                poller_.Add(network_interfaces_[if_index].lldp_socket);
            }
        }

        if (network_interfaces_[if_index].is_bridge) {
            // use the last set bridge address as AL MAC address (there should be a single bridge interface configured at any given time)
            // TODO: let the Multi-AP Agent set the AL MAC address using proper API and remove this code
            set_al_mac_addr(network_interfaces_[if_index].addr);
        }
    }
}

int Ieee1905Transport::open_packet_socket(unsigned int if_index, uint16_t ether_type)
{
    // Note to developer: The current implementation uses AF_PACKET socket with SOCK_RAW protocol
    // which means we receive
    // and send packets with the Ethernet header included in the buffer. Please consider changing
    // implementation to use SOCK_DGRAM protocol (without L2 header handling)
    //TODO: Fixup code for SOCK_RAW -> SOCK_DGRAM transition
    // open packet raw socket - see man packet(7) https://linux.die.net/man/7/packet
    int sockfd;
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ether_type))) < 0) {
        MAPF_ERR("cannot open raw socket (errno: " << errno << " [" << strerror(errno) << "])");
        return -1;
    }

    // the interface should be instantly reusable, e.g. after a crash
    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        MAPF_ERR("cannot set socket option SO_REUSEADDR (errno: " << errno << " ["
                                                                  << strerror(errno) << "])");
        close(sockfd);
        return -1;
    }

    // bind to specified interface
    // note that we cannot use SO_BINDTODEVICE sockopt as it does not support AF_PACKET sockets
    struct sockaddr_ll sockaddr;
    memset(&sockaddr, 0, sizeof(struct sockaddr_ll));
    sockaddr.sll_family   = AF_PACKET;
    sockaddr.sll_protocol = htons(ether_type);
    sockaddr.sll_ifindex  = network_interfaces_[if_index].bridge_if_index;

    if (bind(sockfd, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0) {
        MAPF_ERR("cannot bind socket to interface (errno: " << errno << " [" << strerror(errno)
                                                            << "])");
        close(sockfd);
        return -1;
    }
    return sockfd;
}

bool Ieee1905Transport::open_interface_socket(unsigned int if_index)
{
    MAPF_DBG("opening raw socket on interface " << if_index << ".");

    if (network_interfaces_[if_index].ieee1905_socket != -1) {
        close(network_interfaces_[if_index].ieee1905_socket);
        close(network_interfaces_[if_index].lldp_socket);
        network_interfaces_[if_index].ieee1905_socket = -1;
        network_interfaces_[if_index].lldp_socket     = -1;
    }

    int ieee1905_socket = open_packet_socket(if_index, ETHERTYPE_IEEE1905);
    int lldp_socket     = open_packet_socket(if_index, ETHERTYPE_LLDP);

    if (ieee1905_socket < 0 || lldp_socket < 0) {
        // if only the second call failed, the first socket still needs to be close()d
        close(ieee1905_socket);
        return false;
    }
    network_interfaces_[if_index].ieee1905_socket = ieee1905_socket;
    network_interfaces_[if_index].lldp_socket     = lldp_socket;
    attach_interface_socket_filter(if_index);

    return true;
}

bool Ieee1905Transport::attach_interface_socket_filter(unsigned int if_index)
{
    if (!network_interfaces_.count(if_index)) {
        MAPF_ERR("un-tracked interface " << if_index << ".");
        return false;
    }

    // 1st step is to put the interface in promiscuous mode.
    // promiscuous mode is required since we expect to receive packets destined to
    // the AL MAC address (which is different the the interfaces HW address)
    //
    struct packet_mreq mr = {0};
    mr.mr_ifindex         = if_index;
    mr.mr_type            = PACKET_MR_PROMISC;
    if (setsockopt(network_interfaces_[if_index].ieee1905_socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP,
                   &mr, sizeof(mr)) == -1) {
        MAPF_ERR("cannot put interface in promiscuous mode \"" << strerror(errno) << "\" (" << errno
                                                               << ").");
        return false;
    }
    return true;
}

void Ieee1905Transport::handle_interface_status_change(unsigned int if_index, bool is_active)
{
    if (!network_interfaces_.count(if_index)) {
        MAPF_ERR("un-tracked interface " << if_index << ".");
        return;
    }

    MAPF_DBG("interface " << if_index << " is now " << (is_active ? "active" : "inactive") << ".");

    if (!is_active && network_interfaces_[if_index].ieee1905_socket >= 0) {
        poller_.Remove(network_interfaces_[if_index].ieee1905_socket);
        poller_.Remove(network_interfaces_[if_index].lldp_socket);
        close(network_interfaces_[if_index].ieee1905_socket);
        close(network_interfaces_[if_index].lldp_socket);
        network_interfaces_[if_index].ieee1905_socket = -1;
        network_interfaces_[if_index].lldp_socket     = -1;
    }

    if (is_active && network_interfaces_[if_index].ieee1905_socket < 0) {
        if (!open_interface_socket(if_index)) {
            MAPF_ERR("cannot open network interface " << if_index << ".");
        }
        if (network_interfaces_[if_index].ieee1905_socket >= 0) {
            poller_.Add(network_interfaces_[if_index].ieee1905_socket);
            poller_.Add(network_interfaces_[if_index].lldp_socket);
        }
    }
}

void Ieee1905Transport::handle_interface_pollin_event(int fd)
{
    if (fd < 0) {
        MAPF_ERR("illegal file descriptor " << fd << ".");
        return;
    }

    // Note to developer: add support for VLAN ethernet header (if required)?

    uint8_t buf[ETH_FRAME_LEN];
    struct sockaddr_ll addr;
    socklen_t addr_len = sizeof(addr);
    ssize_t len = recvfrom(fd, buf, sizeof(buf), MSG_DONTWAIT | MSG_TRUNC, (struct sockaddr *)&addr,
                           &addr_len);
    if (len == -1 && (errno == EWOULDBLOCK || errno == EAGAIN)) {
        return;
    }
    if (len == -1) {
        MAPF_ERR("cannot read from socket \"" << strerror(errno) << "\" (" << errno << ").");
        return;
    }
    if (len < (ssize_t)sizeof(struct ether_header)) {
        MAPF_WARN("received packet smaller than ethernet header size (dropped).");
        return;
    }
    if (len > int(sizeof(buf))) {
        MAPF_WARN("received oversized packet (truncated).");
        len = sizeof(buf);
    }

    MAPF_DBG("received packet on interface " << addr.sll_ifindex << ".");

    // convert packet to internal data structure for further handling
    struct ether_header *eh = (struct ether_header *)buf;
    struct Packet packet;
    packet.dst_if_type  = CmduRxMessage::IF_TYPE_NONE;
    packet.dst_if_index = 0;
    packet.src_if_type  = CmduRxMessage::IF_TYPE_NET;
    packet.src_if_index = (unsigned int)addr.sll_ifindex;
    std::copy_n(eh->ether_dhost, ETH_ALEN, packet.dst);
    std::copy_n(eh->ether_shost, ETH_ALEN, packet.src);
    packet.ether_type = ntohs(eh->ether_type);
    packet.header     = {.iov_base = buf, .iov_len = sizeof(struct ether_header)};
    packet.payload    = {.iov_base = buf + sizeof(struct ether_header),
                      .iov_len  = len - sizeof(struct ether_header)};

    counters_[CounterId::INCOMMING_NETWORK_PACKETS]++;
    handle_packet(packet);
}

bool Ieee1905Transport::get_interface_mac_addr(unsigned int if_index, uint8_t *addr)
{
    int sockfd;
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        MAPF_ERR("cannot open raw socket \"" << strerror(errno) << "\" (" << errno << ").");
        return false;
    }

    struct ifreq ifr = {};

    if (!if_indextoname(if_index, ifr.ifr_name)) {
        MAPF_ERR("cannot find name of interface  " << if_index << ".");
        close(sockfd);
        return false;
    }

    if ((ioctl(sockfd, SIOCGIFHWADDR, &ifr)) < 0) {
        MAPF_ERR("raw socket SIOCGIFHWADDR ioctl failed \"" << strerror(errno) << "\" (" << errno
                                                            << ").");
        close(sockfd);
        return false;
    }
    std::copy_n(ifr.ifr_hwaddr.sa_data, ETH_ALEN, addr);

    MAPF_DBG("address of interface "
             << if_index << " is " << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[0] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[1] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[2] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[3] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[4] << ":" << std::hex << std::setfill('0') << std::setw(2)
             << (unsigned)addr[5] << "." << std::dec);

    close(sockfd);
    return true;
}

bool Ieee1905Transport::send_packet_to_network_interface(unsigned int if_index, Packet &packet)
{
    MAPF_DBG("sending packet on interface " << if_index << ".");

    if (!network_interfaces_.count(if_index)) {
        MAPF_ERR("un-tracked interface " << if_index << ".");
        return false;
    }

    counters_[CounterId::OUTGOING_NETWORK_PACKETS]++;

    struct ether_header eh;
    std::copy_n(packet.dst, ETH_ALEN, eh.ether_dhost);
    std::copy_n(packet.src, ETH_ALEN, eh.ether_shost);
    eh.ether_type = htons(packet.ether_type);

    packet.header = {.iov_base = &eh, .iov_len = sizeof(eh)};

    int fd = packet.ether_type == ETHERTYPE_LLDP ? network_interfaces_[if_index].lldp_socket
                                                 : network_interfaces_[if_index].ieee1905_socket;
    struct iovec iov[] = {packet.header, packet.payload};
    int n              = writev(fd, iov, sizeof(iov) / sizeof(struct iovec));

    // Clear the packet header to prevent leaking locally allocated stack pointer
    packet.header = {.iov_base = nullptr, .iov_len = sizeof(eh)};
    if (size_t(n) != sizeof(eh) + packet.payload.iov_len) {
        MAPF_ERR("cannot write to socket \"" << strerror(errno) << "\" (" << errno << ").");
        return false;
    }

    return true;
}

void Ieee1905Transport::set_al_mac_addr(const uint8_t *addr)
{
    if (!addr)
        return;

    std::copy_n(addr, ETH_ALEN, al_mac_addr_);

    // refresh packet filtering on all active interfaces to use the new AL MAC address
    for (auto it = network_interfaces_.begin(); it != network_interfaces_.end(); ++it) {
        unsigned int if_index   = it->first;
        auto &network_interface = it->second;

        if (network_interface.ieee1905_socket >= 0) {
            attach_interface_socket_filter(if_index);
        }
    }
}

} // namespace mapf

#if 0
    static const uint8_t ieee1905_multicast_address[ETH_ALEN] = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x13 }; // 01:80:c2:00:00:13
    static const uint8_t lldp_multicast_address[ETH_ALEN]     = { 0x01, 0x80, 0xc2, 0x00, 0x00, 0x0e }; // 01:80:c2:00:00:0e

    // put interface in all-multicast/promiscuous mode
    struct ifreq ifr;
    strncpy(ifr.ifr_name, ifName, IFNAMSIZ-1);
    if ((ioctl(network_interfaces_[if_index].fd, SIOCGIFFLAGS, &ifr) < 0) {
        MAPF_ERR("raw socket SIOCGIFFLAGS ioctl failed (get flags)\"" << strerror(errno) << "\" (" << errno << ").");
        return false;
    }
    ifr.ifr_flags |= IFF_PROMISC;
    ifr.ifr_flags |= IFF_ALLMULTI;
    if ((ioctl(network_interfaces_[if_index].fd, SIOCSIFFLAGS, &ifr) < 0) {
        MAPF_ERR("raw socket SIOCSIFFLAGS ioctl failed (set flags)\"" << strerror(errno) << "\" (" << errno << ").");
        return false;
    }

    // promisc/multicast ioctl alternative code (uses specific multicast addresses - but not tested)
    struct packet_mreq mr;
    memset(&mr, 0, sizeof(mr));
    mr.mr_ifindex = addr.sll_ifindex;
    mr.mr_type = PACKET_MR_MULTICAST;
    mr.mr_alen = 6;
    memcpy(mr.mr_address, ieee1905_multicast_address, 6);
    if (setsockopt(sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        perror("PACKET_ADD_MEMBERSHIP");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    memcpy(mr.mr_address, lldp_multicast_address, 6);
    if (setsockopt(sockfd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        perror("PACKET_ADD_MEMBERSHIP");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // hand-made filter
    struct sock_filter ieee1905_filter[] = {
        // should we add support for 802.1Q (VLAN/QoS tagging)
        BPF_STMT(BPF_LD+BPF_H+BPF_ABS, 12),                /* load the EtherType field */
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETH_1905_1, 0, 1), /* if EtherType != IEEE1905 skip next instr. */
        BPF_STMT(BPF_RET+BPF_K, (u_int)-1),                /* return max int (the whole packet) */
        BPF_JUMP(BPF_JMP+BPF_JEQ+BPF_K, ETH_LLDP, 0, 1),   /* if EtherType != LLDP skip next instr. */
        BPF_STMT(BPF_RET+BPF_K, (u_int)-1),                /* return max int (the whole packet) */
        BPF_STMT(BPF_RET+BPF_K, 0),                        /* return zero */
    };
#endif
