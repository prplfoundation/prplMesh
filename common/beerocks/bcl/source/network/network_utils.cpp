/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_string_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/network/swap.h>

#include <arpa/inet.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <linux/ethtool.h>
#include <linux/if_bridge.h>
#include <linux/if_ether.h>  // ETH_P_ARP = 0x0806
#include <linux/if_packet.h> // struct sockaddr_ll (see man 7 packet)
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <easylogging++.h>

using namespace beerocks::net;

#define NL_BUFSIZE 8192
#define MAC_ADDR_CHAR_SIZE 17

struct route_info {
    struct in_addr dstAddr;
    struct in_addr srcAddr;
    struct in_addr gateWay;
    char ifName[IF_NAMESIZE];
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Fucntions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static int read_iface_flags(const std::string &strIface, struct ifreq &if_req)
{
    int socId = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (socId < 0)
        return errno;

    // Read the interface flags
    beerocks::string_utils::copy_string(if_req.ifr_name, strIface.c_str(), sizeof(if_req.ifr_name));
    int rv = ioctl(socId, SIOCGIFFLAGS, &if_req);
    close(socId);

    // Return the error code
    if (rv == -1)
        return errno;

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Constants ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

const std::string network_utils::ZERO_IP_STRING("0.0.0.0");
const std::string network_utils::ZERO_MAC_STRING("00:00:00:00:00:00");
const sMacAddr network_utils::ZERO_MAC{.oct = {0}};
const std::string network_utils::WILD_MAC_STRING("ff:ff:ff:ff:ff:ff");
const std::string network_utils::MULTICAST_1905_MAC_ADDR("01:80:c2:00:00:13");

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Converts uint64_t mac address to string format
std::string network_utils::mac_to_string(const uint64_t mac)
{
    uint8_t mac_address[MAC_ADDR_LEN];
    int8_t i;
    uint8_t *p = mac_address;
    for (i = 5; i >= 0; i--) {
        *p++ = mac >> (CHAR_BIT * i);
    }
    return mac_to_string(mac_address);
}

// Converts a mac address in a human-readable format
std::string network_utils::mac_to_string(const uint8_t *mac_address)
{
    std::string mac_addr_string;

    mac_addr_string = string_utils::int_to_hex_string((uint32_t)mac_address[0], 2) + ":" +
                      string_utils::int_to_hex_string((uint32_t)mac_address[1], 2) + ":" +
                      string_utils::int_to_hex_string((uint32_t)mac_address[2], 2) + ":" +
                      string_utils::int_to_hex_string((uint32_t)mac_address[3], 2) + ":" +
                      string_utils::int_to_hex_string((uint32_t)mac_address[4], 2) + ":" +
                      string_utils::int_to_hex_string((uint32_t)mac_address[5], 2);

    return mac_addr_string;
}

std::string network_utils::mac_to_string(const sMacAddr &mac)
{
    return mac_to_string((const uint8_t *)mac.oct);
}

sMacAddr network_utils::mac_from_string(const std::string &mac)
{
    sMacAddr ret;

    mac_from_string(ret.oct, mac);

    return ret;
}

void network_utils::mac_from_string(uint8_t *buf, const std::string &mac)
{
    if (mac.empty()) {
        memset(buf, 0, MAC_ADDR_LEN);
    } else {
        std::stringstream mac_ss(mac);
        std::string token;

        for (int i = 0; i < MAC_ADDR_LEN; i++) {
            std::getline(mac_ss, token, ':');
            buf[i] = std::stoul(token, nullptr, 16);
        }
    }
}

bool network_utils::is_valid_mac(std::string mac)
{
    if (mac.size() != MAC_ADDR_CHAR_SIZE) {
        return false;
    }
    std::transform(mac.begin(), mac.end(), mac.begin(), ::tolower);
    uint8_t str[20];
    mac_from_string(str, mac);
    auto mac_out = mac_to_string(str);
    return (mac == mac_out);
}

std::string network_utils::ipv4_to_string(const sIpv4Addr &ip)
{
    return ipv4_to_string((const uint8_t *)&ip.oct);
}

std::string network_utils::ipv4_to_string(const uint8_t *ipv4)
{
    std::string ipv4_str;
    std::for_each(ipv4, ipv4 + IP_ADDR_LEN,
                  [&](const uint8_t &oct) { ipv4_str += std::to_string(oct) + '.'; });

    ipv4_str.pop_back(); // remove last '.' character
    return ipv4_str;
}

std::string network_utils::ipv4_to_string(uint32_t ip)
{
    swap_32(ip);

    std::string ipv4_str;
    for (int i = IP_ADDR_LEN; i > 0; i--) {
        uint8_t oct = ip >> ((i - 1) * 8) & 0xFF;
        ipv4_str += (std::to_string(oct) + '.');
    }

    ipv4_str.pop_back(); // remove last '.' character

    return ipv4_str;
}

sIpv4Addr network_utils::ipv4_from_string(const std::string &ip)
{
    sIpv4Addr ret;

    ipv4_from_string(ret.oct, ip);

    return ret;
}

void network_utils::ipv4_from_string(uint8_t *buf, const std::string &ip_str)
{
    if (ip_str.empty()) {
        memset(buf, 0, IP_ADDR_LEN);
    } else {
        std::stringstream ipv4_ss(ip_str);
        std::string token;

        for (int i = 0; i < IP_ADDR_LEN; i++) {
            std::getline(ipv4_ss, token, '.');
            buf[i] = std::stoi(token);
        }
    }
}

uint32_t network_utils::uint_ipv4_from_array(void *ip)
{
    uint32_t ret;

    if (!ip) {
        return 0;
    }

    ret = (((uint32_t(*(((char *)ip) + 0)) << 24) & 0xFF000000) |
           ((uint32_t(*(((char *)ip) + 1)) << 16) & 0x00FF0000) |
           ((uint32_t(*(((char *)ip) + 2)) << 8) & 0x0000FF00) |
           (uint32_t(*(((char *)ip) + 3)) & 0x000000FF));

    swap_32(ret);

    return ret;
}

uint32_t network_utils::uint_ipv4_from_string(const std::string &ip_str)
{
    uint8_t ip_arr[IP_ADDR_LEN];
    ipv4_from_string(ip_arr, ip_str);

    uint32_t ret = ((uint32_t(ip_arr[0]) << 24) | (uint32_t(ip_arr[1]) << 16) |
                    (uint32_t(ip_arr[2]) << 8) | (uint32_t(ip_arr[3])));

    swap_32(ret);
    return ret;
}

int network_utils::get_iface_info(network_utils::iface_info &info, const std::string &iface_name)
{
    info.iface = iface_name;
    info.mac.clear();
    info.ip.clear();
    info.netmask.clear();
    info.ip_gw.clear();

    std::vector<network_utils::ip_info> ip_list = network_utils::get_ip_list();
    for (auto &ip_info : ip_list) {
        if (ip_info.iface == iface_name) {
            info.mac          = ip_info.mac;
            info.ip           = ip_info.ip;
            info.netmask      = ip_info.netmask;
            info.ip_gw        = ip_info.gw;
            info.broadcast_ip = ip_info.broadcast_ip;
            break;
        }
    }

    if (info.mac.empty()) {
        if (!network_utils::linux_iface_get_mac(iface_name, info.mac)) {
            return -1;
        }
    }

    return 0;
}

bool network_utils::get_raw_iface_info(const std::string &iface_name, raw_iface_info &info)
{
    int fd;
    struct ifreq ifr;

    if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP)) < 0) {
        LOG(ERROR) << "Failed opening DGRAM socket: " << strerror(errno);
        return false;
    }

    // Clear the memory
    info = {};

    // MAC Address
    ifr = {};
    beerocks::string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFHWADDR, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n(ifr.ifr_hwaddr.sa_data, 6, (uint8_t *)&info.hwa);

    // IP Address
    ifr = {};
    beerocks::string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFADDR, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n((uint8_t *)&(*(sockaddr_in *)&ifr.ifr_addr).sin_addr, 4, (uint8_t *)&info.ipa);

    // Network Mask
    ifr = {};
    beerocks::string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFNETMASK, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n((uint8_t *)&(*(sockaddr_in *)&ifr.ifr_netmask).sin_addr, 4, (uint8_t *)&info.nmask);

    // Broadcast Address
    ifr = {};
    beerocks::string_utils::copy_string(ifr.ifr_name, iface_name.c_str(), IF_NAMESIZE);
    if ((ioctl(fd, SIOCGIFBRDADDR, &ifr)) == -1) {
        LOG(ERROR) << "ioctl failed: " << strerror(errno);
        close(fd);
        return false;
    }
    std::copy_n((uint8_t *)&(*(sockaddr_in *)&ifr.ifr_broadaddr).sin_addr, 4,
                (uint8_t *)&info.bcast);

    close(fd);

    return true;
}

static int readNlSock(int fd, char *msg, uint32_t seq, uint32_t pid)
{
    int nl_msg_len = 0;

    for (;;) {

        // Read Netlink message
        auto ret = recv(fd, msg, NL_BUFSIZE - nl_msg_len, 0);

        if (ret == 0) {
            LOG(WARNING) << "netlink connection closed: recv returned 0";
            return -1;
        } else if (ret < 0) {
            LOG(ERROR) << "Failed reading netlink socket: " << strerror(errno);
            return -1;
        }

        // Netlink header
        auto header = (struct nlmsghdr *)msg;

        // Validate the header
        if (ret < int(sizeof(struct nlmsghdr)) || ret < int(header->nlmsg_len) ||
            header->nlmsg_len < sizeof(struct nlmsghdr)) {
            LOG(WARNING) << "Invalid netlink message header - msg len = " << int(header->nlmsg_len)
                         << " (" << int(ret) << ")";
            return -1;
        }

        if (header->nlmsg_type == NLMSG_ERROR) {
            LOG(WARNING) << "Read netlink error message";
            return -1;
        }

        // Not the last message
        if (header->nlmsg_type != NLMSG_DONE) {
            msg += ret;
            nl_msg_len += ret;
        } else {
            break;
        }

        // Multipart of someother message
        if (((header->nlmsg_flags & NLM_F_MULTI) == 0) || (header->nlmsg_seq != seq) ||
            (header->nlmsg_pid != pid)) {
            break;
        }
    }

    // Return the length of the read message
    return nl_msg_len;
}

/* For parsing the route info returned */
static int parseRoutes(struct nlmsghdr *nlHdr, std::shared_ptr<route_info> rtInfo)
{
    struct rtmsg *rtMsg;
    struct rtattr *rtAttr;
    int rtLen;

    rtMsg = (struct rtmsg *)NLMSG_DATA(nlHdr);

    //If the route is not for AF_INET then return
    if (rtMsg->rtm_family != AF_INET)
        return (0);

    //get the rtattr field
    rtAttr = (struct rtattr *)RTM_RTA(rtMsg);
    rtLen  = RTM_PAYLOAD(nlHdr);
    for (; RTA_OK(rtAttr, rtLen); rtAttr = RTA_NEXT(rtAttr, rtLen)) {
        switch (rtAttr->rta_type) {
        case RTA_OIF:
            if_indextoname(*(int *)RTA_DATA(rtAttr), rtInfo->ifName);
            break;
        case RTA_GATEWAY:
            rtInfo->gateWay.s_addr = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_PREFSRC:
            rtInfo->srcAddr.s_addr = *(u_int *)RTA_DATA(rtAttr);
            break;
        case RTA_DST:
            rtInfo->dstAddr.s_addr = *(u_int *)RTA_DATA(rtAttr);
            break;
        default:
            break;
        }
    }

    if ((rtInfo->dstAddr.s_addr == 0) && (rtInfo->gateWay.s_addr != 0)) {
        return (1);
    }
    if (rtInfo->dstAddr.s_addr == rtInfo->srcAddr.s_addr) {
        return (2);
    }
    return (0);
}

std::string network_utils::get_mac_from_arp_table(const std::string &ipv4)
{
    /**
 * /proc/net/arp looks like this:
 *
 * IP address       HW type     Flags       HW address            Mask     Device
 * 192.168.12.31    0x1         0x2         00:09:6b:00:02:03     *        eth0
 * 192.168.12.70    0x1         0x2         00:01:02:38:4c:85     *        eth0
 */
    std::ifstream arp_table("/proc/net/arp");

    if (!arp_table.is_open()) {
        LOG(ERROR) << "can't open arp table";
        return std::string();
    }

    std::string line;
    while (std::getline(arp_table, line)) {
        std::string entry_ipv4 = line.substr(0, line.find_first_of(' '));

        size_t mac_start, mac_end;
        mac_start = line.find_first_of(':') - 2;
        mac_end   = line.find_last_of(':') + 3;

        if (mac_start >= line.length() || mac_end >= line.length()) {
            LOG(DEBUG) << "line doesn't match expected format, skipping";
            continue;
        }

        std::string entry_mac = line.substr(mac_start, mac_end - mac_start);

        LOG(DEBUG) << "entry_ipv4=" << entry_ipv4 << " entry_mac=" << entry_mac;

        if (entry_ipv4 == ipv4) {
            return entry_mac;
        }
    }

    return std::string();
}

std::vector<std::string> network_utils::linux_get_iface_list_from_bridge(const std::string &bridge)
{
    std::vector<std::string> ifs;

    std::string path = "/sys/class/net/" + bridge + "/brif";

    DIR *d;
    struct dirent *dir;
    d = opendir(path.c_str());
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            std::string ifname = dir->d_name;
            if (ifname == "." || ifname == "..") {
                continue;
            }
            ifs.push_back(ifname);
        }
        closedir(d);
    }

    return ifs;
}

int network_utils::linux_get_iface_index(const std::string &iface)
{
    return if_nametoindex(iface.c_str());
}

bool network_utils::linux_add_iface_to_bridge(const std::string &bridge, const std::string &iface)
{
    LOG(DEBUG) << "add iface " << iface << " to bridge " << bridge;

    struct ifreq ifr;
    int err;
    unsigned long ifindex = if_nametoindex(iface.c_str());
    if (ifindex == 0) {
        LOG(ERROR) << "invalid iface index=" << ifindex << " for " << iface;
        return false;
    }

    int br_socket_fd;
    if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        LOG(ERROR) << "can't open br_socket_fd";
        return false;
    }

    beerocks::string_utils::copy_string(ifr.ifr_name, bridge.c_str(), IFNAMSIZ);
#ifdef SIOCBRADDIF
    ifr.ifr_ifindex = ifindex;
    err             = ioctl(br_socket_fd, SIOCBRADDIF, &ifr);
    if (err < 0)
#endif
    {
        unsigned long args[4] = {BRCTL_ADD_IF, ifindex, 0, 0};

        ifr.ifr_data = (char *)args;
        err          = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
    }

    close(br_socket_fd);
    return err < 0 ? false : true;
    /*
    std::string cmd;
    cmd = "brctl addif " + bridge + " " + iface;
    system(cmd.c_str());
    LOG(DEBUG) << cmd;
    return true;
    */
}

bool network_utils::linux_remove_iface_from_bridge(const std::string &bridge,
                                                   const std::string &iface)
{
    LOG(DEBUG) << "remove iface " << iface << " from bridge " << bridge;

    struct ifreq ifr;
    int err;
    unsigned long ifindex = if_nametoindex(iface.c_str());

    if (ifindex == 0) {
        LOG(ERROR) << "invalid iface index=" << ifindex << " for " << iface;
        return false;
    }

    int br_socket_fd;
    if ((br_socket_fd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        LOG(ERROR) << "can't open br_socket_fd";
        return false;
    }

    beerocks::string_utils::copy_string(ifr.ifr_name, bridge.c_str(), IFNAMSIZ);
#ifdef SIOCBRDELIF
    ifr.ifr_ifindex = ifindex;
    err             = ioctl(br_socket_fd, SIOCBRDELIF, &ifr);
    if (err < 0)
#endif
    {
        unsigned long args[4] = {BRCTL_DEL_IF, ifindex, 0, 0};

        ifr.ifr_data = (char *)args;
        err          = ioctl(br_socket_fd, SIOCDEVPRIVATE, &ifr);
    }

    close(br_socket_fd);
    return err < 0 ? false : true;
    /*
    std::string cmd;
    cmd = "brctl delif " + bridge + " " + iface;
    system(cmd.c_str());
    LOG(DEBUG) << cmd;
    return true;
    */
}

bool network_utils::linux_iface_ctrl(const std::string &iface, bool up, std::string ip,
                                     const std::string &netmask)
{
    bool ret = true;
    int fd;
    struct ifreq ifr;
    bool zero_ip;

    LOG(TRACE) << "linux_iface_ctrl iface=" << iface << (up ? " up" : " down") << " ip=" << ip
               << " netmask=" << netmask;

    if (ip.empty()) {
        ip      = "0.0.0.0";
        zero_ip = true;
    } else {
        zero_ip = false;
    }

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return false;
    }

    beerocks::string_utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);
    while (up) {
        ifr.ifr_addr.sa_family = AF_INET;
        if (!ip.empty()) {
            uint32_t uip = network_utils::uint_ipv4_from_string(ip);
            ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = uip;
            if (ioctl(fd, SIOCSIFADDR, &ifr) == -1) {
                LOG(ERROR) << "SIOCSIFADDR " << strerror(errno);
                ret = false;
                break;
            }

            if (!zero_ip) {
                uip |= 0xFF;
                ((struct sockaddr_in *)&ifr.ifr_broadaddr)->sin_addr.s_addr = uip;
                if (ioctl(fd, SIOCSIFBRDADDR, &ifr) == -1) {
                    LOG(ERROR) << "SIOCSIFBRDADDR " << strerror(errno);
                    ret = false;
                    break;
                }
            }
        }
        if (!netmask.empty()) {
            ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr =
                network_utils::uint_ipv4_from_string(netmask);
            if (ioctl(fd, SIOCSIFNETMASK, &ifr) == -1) {
                LOG(ERROR) << "SIOCGIFNETMASK " << strerror(errno);
                ret = false;
                break;
            }
        }
        ifr.ifr_flags = IFF_UP | IFF_BROADCAST | IFF_RUNNING;
        if (ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
            LOG(ERROR) << "SIOCSIFFLAGS " << strerror(errno);
            ret = false;
        }
        break;
    }
    while (!up) {
        if (ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
            LOG(ERROR) << "SIOCGIFFLAGS " << strerror(errno);
            ret = false;
        } else if (ifr.ifr_flags & IFF_UP) {
            ifr.ifr_flags &= (~IFF_UP);
            if (ioctl(fd, SIOCSIFFLAGS, &ifr) == -1) {
                LOG(ERROR) << "SIOCSIFFLAGS " << strerror(errno);
                ret = false;
            }
        }
        break;
    }
    close(fd);
    return ret;
}

bool network_utils::linux_iface_get_mac(const std::string &iface, std::string &mac)
{
    struct ifreq ifr;
    int fd;

    mac.clear();

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return false;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    beerocks::string_utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
        LOG(ERROR) << "SIOCGIFHWADDR";
        close(fd);
        return false;
    }
    close(fd);
    mac = network_utils::mac_to_string((uint8_t *)(ifr.ifr_ifru.ifru_hwaddr.sa_data));
    std::transform(mac.begin(), mac.end(), mac.begin(), ::tolower);
    return true;
}

bool network_utils::linux_iface_get_ip(const std::string &iface, std::string &ip)
{
    struct ifreq ifr;
    int fd;

    ip.clear();

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return false;
    }

    ifr.ifr_addr.sa_family = AF_INET;
    beerocks::string_utils::copy_string(ifr.ifr_name, iface.c_str(), IFNAMSIZ);

    if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        LOG(ERROR) << "SIOCGIFADDR";
        close(fd);
        return false;
    }
    close(fd);
    uint32_t ip_uint = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    ip               = network_utils::ipv4_to_string(ip_uint);
    return true;
}

bool network_utils::linux_iface_get_pci_info(const std::string &iface, std::string &pci_id)
{
    std::ifstream phy_device_file("/sys/class/net/" + iface + "/phy80211/device/device",
                                  std::ios::in);

    if (!phy_device_file.is_open()) {
        //LOG(ERROR) << "can't open phy80211 device file for interface " << iface;
        return false;
    }

    std::string device;
    std::getline(phy_device_file, device);
    device = device.substr(2); //remove trailing 0x

    std::ifstream phy_vendor_file("/sys/class/net/" + iface + "/phy80211/device/vendor",
                                  std::ios::in);

    if (!phy_vendor_file.is_open()) {
        //LOG(ERROR) << "can't open phy80211 vendor file for interface " << iface;
        return false;
    }

    std::string vendor;
    std::getline(phy_vendor_file, vendor);
    vendor = vendor.substr(2); //remove trailing 0x

    pci_id = vendor + ":" + device;

    return true;
}

bool network_utils::linux_iface_exists(const std::string &iface)
{
    struct ifreq flags;

    int err = read_iface_flags(iface, flags);
    return (err == 0);
}

bool network_utils::linux_iface_is_up(const std::string &iface)
{
    struct ifreq flags;

    int err = read_iface_flags(iface, flags);

    // Check if the interface us UP
    if (!err) {
        return (flags.ifr_flags & IFF_UP);
    }

    LOG(ERROR) << "Failed to read iface " << iface << " flags!!";

    return (false);
}

bool network_utils::linux_iface_is_up_and_running(const std::string &iface)
{
    struct ifreq flags;

    int err = read_iface_flags(iface, flags);

    // Check if the interface us UP and RUNNING (plugged)
    if (!err) {
        return (flags.ifr_flags & IFF_UP) && (flags.ifr_flags & IFF_RUNNING);
    }

    return (false);
}

#define ETHTOOL_LINK_MODE_MASK_MAX_KERNEL_NU32 (SCHAR_MAX)

bool network_utils::linux_iface_get_speed(const std::string &iface, uint32_t &speed)
{
    int sock;
    bool result = false;

    speed = SPEED_UNKNOWN;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
        LOG(ERROR) << "Can't create SOCK_DGRAM socket: " << strerror(errno);
    } else {
        struct ifreq ifr;
        struct {
            struct ethtool_link_settings req;
            __u32 link_mode_data[3 * ETHTOOL_LINK_MODE_MASK_MAX_KERNEL_NU32];
        } ecmd;
        int rc;

        beerocks::string_utils::copy_string(ifr.ifr_name, iface.c_str(), sizeof(ifr.ifr_name));
        ifr.ifr_data = reinterpret_cast<char *>(&ecmd);

        /* Handshake with kernel to determine number of words for link
         * mode bitmaps. When requested number of bitmap words is not
         * the one expected by kernel, the latter returns the integer
         * opposite of what it is expecting. We request length 0 below
         * (aka. invalid bitmap length) to get this info.
         */
        memset(&ecmd, 0, sizeof(ecmd));
        ecmd.req.cmd = ETHTOOL_GLINKSETTINGS;
        rc           = ioctl(sock, SIOCETHTOOL, &ifr);
        if (0 == rc) {
            /**
             * See above: we expect a strictly negative value from kernel.
             */
            if ((ecmd.req.link_mode_masks_nwords >= 0) || (ETHTOOL_GLINKSETTINGS != ecmd.req.cmd)) {
                LOG(ERROR) << "ETHTOOL_GLINKSETTINGS handshake failed";
            } else {
                /**
               * Got the real ecmd.req.link_mode_masks_nwords, now send the real request
              */
                ecmd.req.cmd                    = ETHTOOL_GLINKSETTINGS;
                ecmd.req.link_mode_masks_nwords = -ecmd.req.link_mode_masks_nwords;
                rc                              = ioctl(sock, SIOCETHTOOL, &ifr);
                if (0 == rc) {
                    speed  = ecmd.req.speed;
                    result = true;
                }
            }
        }

        /**
         * ETHTOOL_GSET is deprecated and must be used only if ETHTOOL_GLINKSETTINGS
         * didn't work
         */
        if (!result) {
            struct ethtool_cmd ecmd;

            ifr.ifr_data = reinterpret_cast<char *>(&ecmd);

            memset(&ecmd, 0, sizeof(ecmd));
            ecmd.cmd = ETHTOOL_GSET;

            rc = ioctl(sock, SIOCETHTOOL, &ifr);
            if (0 == rc) {
                speed  = ethtool_cmd_speed(&ecmd);
                result = true;
            }
        }

        if (rc < 0) {
            LOG(ERROR) << "ioctl failed: " << strerror(errno);
        }

        close(sock);
    }

    return result;
}

std::vector<network_utils::ip_info> network_utils::get_ip_list()
{
    std::vector<network_utils::ip_info> ip_list;
    struct nlmsghdr *nlMsg;
    struct ifreq ifr;
    char *msgBuf = NULL;

    int sock, fd, len;
    uint32_t msgSeq = 0;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        LOG(ERROR) << "Can't open SOCK_DGRAM socket";
        return ip_list;
    }

    if ((sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE)) < 0) {
        LOG(ERROR) << "Can't open netlink socket";
        close(fd);
        return ip_list;
    }

    msgBuf = new char[NL_BUFSIZE];
    memset(msgBuf, 0, NL_BUFSIZE);

    /* point the header and the msg structure pointers into the buffer */
    nlMsg = (struct nlmsghdr *)msgBuf;

    /* Fill in the nlmsg header*/
    nlMsg->nlmsg_len  = NLMSG_LENGTH(sizeof(struct rtmsg)); // Length of message.
    nlMsg->nlmsg_type = RTM_GETROUTE; // Get the routes from kernel routing table .

    nlMsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; // The message is a request for dump.
    nlMsg->nlmsg_seq   = msgSeq++;                   // Sequence of the message packet.
    nlMsg->nlmsg_pid   = (uint32_t)getpid();         // PID of process sending the request.

    /* Send the request */
    if (send(sock, nlMsg, nlMsg->nlmsg_len, 0) < 0) {
        LOG(ERROR) << "send()";
        delete[] msgBuf;
        close(sock);
        close(fd);
        return ip_list;
    }

    /* Read the response */
    if ((len = readNlSock(sock, msgBuf, msgSeq, nlMsg->nlmsg_pid)) < 0) {
        LOG(ERROR) << "readNlSock()";
        delete[] msgBuf;
        close(sock);
        close(fd);
        return ip_list;
    }

    /* Parse and print the response */
    uint32_t ip_uint;
    network_utils::ip_info gw_ip_info;
    int rtInfo_ret;
    auto rtInfo = std::make_shared<route_info>();
    if (!rtInfo) {
        delete[] msgBuf;
        close(sock);
        close(fd);
        LOG(ERROR) << "rtInfo allocation failed!";
        return std::vector<network_utils::ip_info>();
    }
    for (; NLMSG_OK(nlMsg, uint32_t(len)); nlMsg = NLMSG_NEXT(nlMsg, len)) {
        memset(rtInfo.get(), 0, sizeof(struct route_info));
        rtInfo_ret = parseRoutes(nlMsg, rtInfo);
        if (rtInfo_ret == 1) { // GW address
            gw_ip_info.gw    = network_utils::ipv4_to_string(rtInfo->gateWay.s_addr);
            gw_ip_info.iface = std::string(rtInfo->ifName);
            LOG(DEBUG) << "gw=" << gw_ip_info.gw << " iface=" << gw_ip_info.iface;
        } else if (rtInfo_ret == 2) { // Iface /IP addr
            network_utils::ip_info ip_info;

            ip_info.iface = std::string(rtInfo->ifName);

            ifr.ifr_addr.sa_family = AF_INET;
            beerocks::string_utils::copy_string(ifr.ifr_name, rtInfo->ifName, IFNAMSIZ);

            if (ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
                continue; // skip, if can't read ip
            }
            ip_uint           = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
            ip_info.ip        = network_utils::ipv4_to_string(ip_uint);
            ip_info.iface_idx = if_nametoindex(ifr.ifr_name);

            if (ioctl(fd, SIOCGIFNETMASK, &ifr) == -1) {
                ip_info.netmask.clear();
            } else {
                ip_uint         = ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr;
                ip_info.netmask = network_utils::ipv4_to_string(ip_uint);
            }

            ip_uint = (((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr) |
                      (~(((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr));
            ip_info.broadcast_ip = network_utils::ipv4_to_string(ip_uint);

            if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
                ip_info.mac.clear();
            } else {
                ip_info.mac =
                    network_utils::mac_to_string((uint8_t *)(ifr.ifr_ifru.ifru_hwaddr.sa_data));
                std::transform(ip_info.mac.begin(), ip_info.mac.end(), ip_info.mac.begin(),
                               ::tolower);
            }
            ip_list.push_back(ip_info);
        }
    }
    delete[] msgBuf;
    close(sock);
    close(fd);
    // update gw ip
    for (auto &ip_item : ip_list) {
        LOG(DEBUG) << "ip_item iface=" << ip_item.iface;
        if (ip_item.iface == gw_ip_info.iface) {
            ip_item.gw = gw_ip_info.gw;
        }
    }
    return ip_list;
}

bool network_utils::arp_send(const std::string &iface, const std::string &dst_ip,
                             const std::string &src_ip, sMacAddr dst_mac, sMacAddr src_mac,
                             int count, int arp_socket)
{
    int tx_len;
    arp_hdr arphdr;
    struct sockaddr_ll sock;
    uint8_t packet_buffer[128];

    // If the destination IP is empty, there is no point sending the arp, therefore replace the
    // with broadcast IP, so all clients will receive it and answer, but since the request is
    // being sent to a specific mac address, then only the requested client will answer.
    uint32_t dst_ip_uint;
    if (dst_ip.empty()) {
        dst_ip_uint = 0xFFFFFFFF; // "255.255.255.255" equivalent
    } else {
        dst_ip_uint = network_utils::uint_ipv4_from_string(dst_ip);
    }

    uint32_t src_ip_uint = network_utils::uint_ipv4_from_string(src_ip);

    // Fill out sockaddr_ll.
    sock             = {};
    sock.sll_family  = AF_PACKET;
    sock.sll_ifindex = if_nametoindex(iface.c_str());
    sock.sll_halen   = MAC_ADDR_LEN;
    std::copy_n(dst_mac.oct, MAC_ADDR_LEN, sock.sll_addr);

    // build ARP header
    arphdr.htype  = htons(1);        //type: 1 for ethernet
    arphdr.ptype  = htons(ETH_P_IP); // proto
    arphdr.hlen   = MAC_ADDR_LEN;    // mac addr len
    arphdr.plen   = IP_ADDR_LEN;     // ip addr len
    arphdr.opcode = htons(ARPOP_REQUEST);
    std::copy_n(src_mac.oct, MAC_ADDR_LEN, arphdr.sender_mac);
    std::copy_n((uint8_t *)&src_ip_uint, IP_ADDR_LEN, arphdr.sender_ip);
    std::copy_n(dst_mac.oct, MAC_ADDR_LEN, arphdr.target_mac);
    std::copy_n((uint8_t *)&dst_ip_uint, IP_ADDR_LEN, arphdr.target_ip);

    // build ethernet frame
    tx_len = 2 * MAC_ADDR_LEN + 2 + ARP_HDRLEN; // dest mac, src mac, type, arp header len
    std::copy_n(dst_mac.oct, MAC_ADDR_LEN, packet_buffer);
    std::copy_n(src_mac.oct, MAC_ADDR_LEN, packet_buffer + MAC_ADDR_LEN);
    packet_buffer[12] = ETH_P_ARP / 256;
    packet_buffer[13] = ETH_P_ARP % 256;

    // ARP header
    std::copy_n((uint8_t *)&arphdr, ARP_HDRLEN, packet_buffer + ETH_HDRLEN);

    bool new_socket = (arp_socket < 0);
    if (new_socket) {
        if ((arp_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
            LOG(ERROR) << "Opening ARP socket";
            return false;
        }
    }

    // Send ethernet frame to socket.
    for (int i = 0; i < count; i++) {
        //LOG_MONITOR(DEBUG) << "ARP to ip=" << dest_ip << " mac=" << dest_mac;
        if (sendto(arp_socket, packet_buffer, tx_len, 0, (struct sockaddr *)&sock, sizeof(sock)) <=
            0) {
            LOG(ERROR) << "sendto() failed";
        }
    }
    if (new_socket) {
        close(arp_socket);
    }
    return true;
}

bool network_utils::icmp_send(const std::string &ip, uint16_t id, int count, int icmp_socket)
{
    sockaddr_in pingaddr;
    uint8_t packet_buffer[128];

    if (icmp_socket < 0) {
        LOG(ERROR) << "icmp_socket=" << icmp_socket;
        return false;
    }

    pingaddr                 = {};
    pingaddr.sin_family      = AF_INET;
    pingaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    for (int i = 0; i < count; i++) {
        icmphdr *pkt          = (icmphdr *)packet_buffer;
        pkt->type             = ICMP_ECHO;
        pkt->code             = 0;
        pkt->un.echo.id       = htons(id);
        pkt->un.echo.sequence = htons(i);
        pkt->checksum =
            0; //keep cheksum = 0 before calling icmp_checksum -> needed for checksum calculation
        pkt->checksum = htons(icmp_checksum((uint16_t *)pkt, sizeof(packet_buffer)));
        ssize_t bytes = sendto(icmp_socket, packet_buffer, sizeof(packet_buffer), 0,
                               (sockaddr *)&pingaddr, sizeof(sockaddr_in));
        if (bytes < 0) {
            LOG(ERROR) << "writeBytes() failed: " << strerror(errno);
            return false;
        } else if (bytes < (ssize_t)sizeof(packet_buffer)) {
            LOG(ERROR) << "ICMP Failed to send, sent " << int(bytes) << " out of "
                       << int(sizeof(packet_buffer));
            return false;
        }
    }
    return true;
}

uint16_t network_utils::icmp_checksum(uint16_t *buf, int32_t len)
{
    int32_t nleft   = len;
    int32_t sum     = 0;
    uint16_t *w     = buf;
    uint16_t answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    if (nleft == 1) {
        *(uint16_t *)(&answer) = *(uint8_t *)w;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    answer = ~sum;
    return answer;
}
