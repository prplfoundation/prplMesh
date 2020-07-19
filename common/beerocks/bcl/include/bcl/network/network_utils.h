/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
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
    static const std::string MULTICAST_1905_MAC_ADDR;

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

    /**
     * @brief Get the arp table as unordered map object of pairs <MAC, IP> or <IP, MAC>.
     * The key of the map can be either a MAC address or an IP address, depends on the 'mac_as_key'
     * argument value.
     * If 'mac_as_key' is 'true' then the key is a MAC address, otherwise the key is an IP address.
     * 
     * @param[in] mac_as_key Decide whether the key of the returned unordered map object 
     * is a MAC or an IP.
     * @return std::shared_ptr<std::unordered_map<std::string, std::string>> 
     */
    static std::shared_ptr<std::unordered_map<std::string, std::string>>
    get_arp_table(bool mac_as_key = true);

    //temp
    static std::string get_mac_from_arp_table(const std::string &ipv4);

    static std::vector<std::string> linux_get_iface_list_from_bridge(const std::string &bridge);

    /**
     * @brief Gets the interface index corresponding to a particular name.
     *
     * @param iface_name The name of the network interface.
     * @return interface index or 0 if no interface exists with the name given.
     */
    static uint32_t linux_get_iface_index(const std::string &iface_name);

    /**
     * @brief Gets the interface name corresponding to a particular index.
     *
     * @param iface_index The index of the network interface.
     * @return interface name or empty string if no interface exists with the index given.
     */
    static std::string linux_get_iface_name(uint32_t iface_index);

    static bool linux_add_iface_to_bridge(const std::string &bridge, const std::string &iface);
    static bool linux_remove_iface_from_bridge(const std::string &bridge, const std::string &iface);
    static bool linux_iface_ctrl(const std::string &iface, bool up, std::string ip = "",
                                 const std::string &netmask = "");
    static bool linux_iface_get_mac(const std::string &iface, std::string &mac);

    /**
     * @brief Gets the interface name of the network interface with given MAC address.
     *
     * @param[in] mac MAC address of the network interface.
     * @param[out] iface On success, name of the network interface. On error, empty string.
     *
     * @return True on success and false otherwise.
     */
    static bool linux_iface_get_name(const sMacAddr &mac, std::string &iface);

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

    /**
     * @brief Gets interface statistics for the given network interface.
     *
     * @param[in] iface Name of the local network interface.
     * @param[out] iface_stats Interface statistics.
     *
     * @return True on success and false otherwise.
     */
    static bool get_iface_stats(const std::string &iface, sInterfaceStats &iface_stats);

    static bool arp_send(const std::string &iface, const std::string &dst_ip,
                         const std::string &src_ip, sMacAddr dst_mac, sMacAddr src_mac, int count,
                         int arp_socket = -1);

    static bool icmp_send(const std::string &ip, uint16_t id, int count, int icmp_socket);
    static uint16_t icmp_checksum(uint16_t *buf, int32_t len);
};
} // namespace net
} // namespace beerocks

#endif //_NETWORK_UTILS_H_
