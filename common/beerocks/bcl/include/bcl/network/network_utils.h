/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _NETWORK_UTILS_H_
#define _NETWORK_UTILS_H_

#include "../beerocks_os_utils.h"
#include "socket.h"

#include "net_struct.h"
#include <cstdint>
#include <easylogging++.h>
#include <string>
#include <vector>

#include <netinet/ether.h>
#include <netinet/ip.h> // IP_MAXPACKET (which is 65535)

#define ETH_HDRLEN 14 // Ethernet header length
#define IP4_HDRLEN 20 // IPv4 header length
#define ARP_HDRLEN 28 // ARP header length
// Define some constants.
#ifndef ARPOP_REQUEST
#define ARPOP_REQUEST 1 // Taken from <linux/if_arp.h>
#define ARPOP_REPLY 2
#endif

namespace beerocks {
namespace net {

class network_utils {
public:
    static const std::string ZERO_IP_STRING;
    static const std::string ZERO_MAC_STRING;
    static const sMacAddr ZERO_MAC;
    static const std::string WILD_MAC_STRING;

    typedef struct {
        uint16_t htype;
        uint16_t ptype;
        uint8_t hlen;
        uint8_t plen;
        uint16_t opcode;
        uint8_t sender_mac[6];
        uint8_t sender_ip[4];
        uint8_t target_mac[6];
        uint8_t target_ip[4];
    } arp_hdr;

    typedef struct {
        std::string iface;
        uint32_t iface_idx;
        std::string connection_name;
        std::string mac;
        std::string ip;
        std::string netmask;
        std::string broadcast_ip;
        std::string gw;
    } ip_info;

    typedef struct {
        std::string iface;
        std::string mac;
        std::string ip;
        std::string netmask;
        std::string ip_gw;
        std::string broadcast_ip;
    } iface_info;

    typedef struct {
        struct ether_addr hwa;
        struct in_addr ipa;
        struct in_addr bcast;
        struct in_addr nmask;
    } raw_iface_info;

    static std::string mac_to_string(const uint64_t mac);
    static std::string mac_to_string(const sMacAddr &mac);
    static std::string mac_to_string(const uint8_t *mac_address);

    static sMacAddr mac_from_string(const std::string &mac);
    static void mac_from_string(uint8_t *buf, const std::string &mac);

    static bool is_valid_mac(std::string mac);

    static std::string ipv4_to_string(const net::sIpv4Addr &ip);
    static std::string ipv4_to_string(const uint8_t *ipv4);
    static std::string ipv4_to_string(uint32_t ip);
    static net::sIpv4Addr ipv4_from_string(const std::string &ip_str);
    static void ipv4_from_string(uint8_t *buf, const std::string &ip);
    static uint32_t uint_ipv4_from_array(void *ip);
    static uint32_t uint_ipv4_from_string(const std::string &ip_str);

    static std::vector<network_utils::ip_info> get_ip_list();
    static int get_iface_info(network_utils::iface_info &info, const std::string &iface_name);
    static bool get_raw_iface_info(const std::string &iface_name, raw_iface_info &info);

    //temp
    static std::string get_mac_from_arp_table(const std::string &ipv4);

    static std::vector<std::string> linux_get_iface_list_from_bridge(const std::string &bridge);
    static int linux_get_iface_index(const std::string &iface);
    static bool linux_add_iface_to_bridge(const std::string &bridge, const std::string &iface);
    static bool linux_remove_iface_from_bridge(const std::string &bridge, const std::string &iface);
    static bool linux_iface_ctrl(const std::string &iface, bool up, std::string ip = "",
                                 const std::string &netmask = "");
    static bool linux_iface_get_mac(const std::string &iface, std::string &mac);
    static bool linux_iface_get_ip(const std::string &iface, std::string &ip);
    static bool linux_iface_get_pci_info(const std::string &iface, std::string &pci_id);
    static bool linux_iface_exists(const std::string &iface);
    static bool linux_iface_is_up(const std::string &iface);
    static bool linux_iface_is_up_and_running(const std::string &iface);

    /**
     * @brief Gets the speed of a network interface.
     *
     * @param[in] iface Name of the network interface.
     * @param[out] speed On success, speed in Mbps of the network interface as defined in SPEED_*
     * macros included in ethtool.h
     *
     * @return True if speed could be successfully obtained and false otherwise.
     */
    static bool linux_iface_get_speed(const std::string &iface, uint32_t &speed);

    static bool arp_send(const std::string &iface, const std::string &dst_ip,
                         const std::string &src_ip, sMacAddr dst_mac, sMacAddr src_mac, int count,
                         int arp_socket = -1);

    static bool icmp_send(const std::string &ip, uint16_t id, int count, int icmp_socket);
    static uint16_t icmp_checksum(uint16_t *buf, int32_t len);
};
} // namespace net
} // namespace beerocks

inline std::ostream &operator<<(std::ostream &os, const sMacAddr &addr)
{
    return os << beerocks::net::network_utils::mac_to_string(addr);
}

inline el::base::MessageBuilder &operator<<(el::base::MessageBuilder &log, const sMacAddr &addr)
{
    return log << beerocks::net::network_utils::mac_to_string(addr);
}

#endif //_NETWORK_UTILS_H_
