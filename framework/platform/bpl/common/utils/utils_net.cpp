/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "utils_net.h"
#include "utils.h"
#include <bpl/bpl_cfg.h>

#include <algorithm> // std::transform
#include <cstring>

#include <arpa/inet.h>
#include <endian.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <mapf/common/logger.h>

namespace beerocks {
namespace bpl {
namespace utils {

std::string mac_to_string(const uint8_t *mac_address)
{
    std::string mac_addr_string;

    mac_addr_string = int_to_hex_string((uint32_t)mac_address[0], 2) + ":" +
                      int_to_hex_string((uint32_t)mac_address[1], 2) + ":" +
                      int_to_hex_string((uint32_t)mac_address[2], 2) + ":" +
                      int_to_hex_string((uint32_t)mac_address[3], 2) + ":" +
                      int_to_hex_string((uint32_t)mac_address[4], 2) + ":" +
                      int_to_hex_string((uint32_t)mac_address[5], 2);

    return mac_addr_string;
}

void mac_from_string(uint8_t *buf, const std::string &mac)
{
    if (mac.empty()) {
        memset(buf, 0, BPL_MAC_ADDR_OCTETS_LEN);
    } else {
        std::stringstream mac_ss(mac);
        std::string token;

        for (int i = 0; i < BPL_MAC_ADDR_OCTETS_LEN; i++) {
            std::getline(mac_ss, token, ':');
            buf[i] = std::stoul(token, nullptr, 16);
        }
    }
}

std::string ipv4_to_string(uint32_t ip)
{
    ip = htobe32(ip);

    std::string ipv4_str;
    for (int i = BPL_IPV4_ADDR_OCTETS_LEN; i > 0; i--) {
        uint8_t oct = ip >> ((i - 1) * 8) & 0xFF;
        ipv4_str += (std::to_string(oct) + '.');
    }

    ipv4_str.pop_back(); // remove last '.' character

    return ipv4_str;
}

std::string ipv4_to_string(const uint8_t *ipv4)
{
    std::string ipv4_str;
    std::for_each(ipv4, ipv4 + BPL_IPV4_ADDR_OCTETS_LEN,
                  [&](const uint8_t &oct) { ipv4_str += std::to_string(oct) + '.'; });

    ipv4_str.pop_back(); // remove last '.' character
    return ipv4_str;
}

void ipv4_from_string(uint8_t *buf, const std::string &ip_str)
{
    if (ip_str.empty()) {
        memset(buf, 0, BPL_IPV4_ADDR_OCTETS_LEN);
    } else {
        std::stringstream ipv4_ss(ip_str);
        std::string token;

        for (int i = 0; i < BPL_IPV4_ADDR_OCTETS_LEN; i++) {
            std::getline(ipv4_ss, token, '.');
            buf[i] = std::stoi(token);
        }
    }
}

uint32_t uint_ipv4_from_string(const std::string &ip_str)
{
    uint8_t ip_arr[BPL_IPV4_ADDR_OCTETS_LEN];
    ipv4_from_string(ip_arr, ip_str);

    uint32_t ret = ((uint32_t(ip_arr[0]) << 24) | (uint32_t(ip_arr[1]) << 16) |
                    (uint32_t(ip_arr[2]) << 8) | (uint32_t(ip_arr[3])));

    ret = htobe32(ret);
    return ret;
}

bool linux_iface_get_mac(const std::string &iface, std::string &mac)
{
    struct ifreq ifr;
    int fd;

    mac.clear();

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        MAPF_ERR("Can't open SOCK_DGRAM socket");
        return false;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        MAPF_ERR("SIOCGIFHWADDR");
        close(fd);
        return false;
    }
    close(fd);
    mac = mac_to_string((uint8_t *)(ifr.ifr_ifru.ifru_hwaddr.sa_data));
    std::transform(mac.begin(), mac.end(), mac.begin(), ::tolower);
    return true;
}

bool linux_iface_get_ip(const std::string &iface, std::string &ip)
{
    struct ifreq ifr;
    int fd;

    ip.clear();

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        MAPF_ERR("Can't open SOCK_DGRAM socket");
        return false;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);

    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        MAPF_ERR("SIOCGIFADDR");
        close(fd);
        return false;
    }
    close(fd);
    uint32_t ip_uint = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    ip               = ipv4_to_string(ip_uint);
    return true;
}

uint32_t linux_iface_to_index(const std::string &iface) { return if_nametoindex(iface.c_str()); }

} // namespace utils
} // namespace bpl
} // namespace beerocks
