/*
* INTEL CONFIDENTIAL
* Copyright 2016 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#include "arp_monitor.h"

#include "../../../common/utils/utils_net.h"

#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>

#include <linux/if_arp.h>
#include <linux/rtnetlink.h>

#ifdef BEEROCKS_BRCM_DDWRT
#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>
#elif BEEROCKS_BRCM_ASUSWRT
#include <linux/netlink.h>
#include <linux/socket.h>
#include <linux/msg.h>
#endif

#include <mapf/common/logger.h>

namespace beerocks {
namespace bpl {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define ETH_HDRLEN 14 // Ethernet header length
#define IP4_HDRLEN 20 // IPv4 header length
#define ARP_HDRLEN 28 // ARP header length

#define SOCKET_SEND_BUF_SIZE    32768
#define SOCKET_RECV_BUF_SIZE    (1024 * 1024)

#define PROBE_TIMEOUT_SEC       5

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
    uint8_t ip[4];
    uint8_t mac[6];
    uint8_t state;
    uint8_t ifindex;
} arp_neigh;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static bool send_arp(std::string iface, std::string dst_ip, std::string src_ip,
        uint8_t* dst_mac, uint8_t* src_mac, int count, int arp_socket)
{
    int tx_len;
    arp_hdr arphdr;
    struct sockaddr_ll sock;
    uint8_t packet_buffer[128];

    uint32_t dst_ip_uint = utils::uint_ipv4_from_string(dst_ip);
    uint32_t src_ip_uint = utils::uint_ipv4_from_string(src_ip);

    // Fill out sockaddr_ll
    memset (&sock, 0, sizeof (sock));
    sock.sll_family = AF_PACKET;
    sock.sll_ifindex = utils::linux_iface_to_index(iface);
    sock.sll_halen = 6;
    memcpy (sock.sll_addr, dst_mac, 6 * sizeof (uint8_t));

    // build ARP header
    arphdr.htype = htons(1); //type: 1 for ethernet
    arphdr.ptype = htons(ETH_P_IP); // proto
    arphdr.hlen = 6; // mac addr len
    arphdr.plen = 4; // ip addr len
    arphdr.opcode = htons (ARPOP_REQUEST);
    memcpy (&arphdr.sender_mac, src_mac, 6);
    memcpy (&arphdr.sender_ip, &src_ip_uint, 4);
    memcpy (&arphdr.target_mac, dst_mac, 6);
    memcpy (&arphdr.target_ip, &dst_ip_uint, 4);

    // build ethernet frame
    tx_len = 6 + 6 + 2 + ARP_HDRLEN; // dest mac, src mac, type, arp header len
    memcpy (packet_buffer, dst_mac, 6 * sizeof (uint8_t));
    memcpy (packet_buffer + 6, src_mac, 6 * sizeof (uint8_t));
    packet_buffer[12] = ETH_P_ARP / 256;
    packet_buffer[13] = ETH_P_ARP % 256;

    // ARP header
    memcpy (packet_buffer + ETH_HDRLEN, &arphdr, ARP_HDRLEN * sizeof (uint8_t));

    bool new_socket = (arp_socket < 0);
    if (new_socket) {
        if((arp_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0){
            MAPF_ERR("Opening ARP socket");
            return false;
        }
    }

    // Send ethernet frame to socket.
    for(int i = 0; i < count ; i++) {
        if (sendto(arp_socket, packet_buffer, tx_len, 0, (struct sockaddr *) &sock, sizeof (sock)) <= 0) {
            MAPF_ERR("sendto() failed");
        }
    }
    if(new_socket) {
        close(arp_socket);
    }
    return true;
}


//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

arp_monitor::arp_monitor() :
    m_fdMonSocket(-1),
    m_fdArpSocket(-1)
{
}

arp_monitor::~arp_monitor()
{
}

bool arp_monitor::start(std::string strIface)
{
    // Stop the monitor (if running)
    stop();

    // Read interface values
    m_strIface = strIface;
    
    // open netlink sock
    if ((m_fdMonSocket = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_ROUTE)) < 0)
    {
        MAPF_ERR("Failed opening socket: " << strerror(errno));
        return false;
    }

    int sndbuf = SOCKET_SEND_BUF_SIZE;
    if (setsockopt(m_fdMonSocket, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf)) < 0)
    {
        MAPF_ERR("Failed setting SO_SNDBUF option: " << strerror(errno));
        stop();
        return false;
    }

    int rcvbuf = SOCKET_RECV_BUF_SIZE;
    if (setsockopt(m_fdMonSocket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf)) < 0) {
        MAPF_ERR("Failed setting SO_RCVBUF option: " << strerror(errno));
        stop();
        return false;
    }

    struct sockaddr_nl local;
    memset(&local, 0, sizeof(struct sockaddr_nl));

    local.nl_family = AF_NETLINK;
    local.nl_groups = RTNLGRP_TC;

    if (bind(m_fdMonSocket, (struct sockaddr*)&local, sizeof(local)) < 0) {
        MAPF_ERR("Failed binding socket: " << strerror(errno));
        stop();
        return false;
    }

    // Create a RAW socket for sending ARP messages
    if ((m_fdArpSocket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
        MAPF_ERR("Failed opening RAW socket");
        stop();
        return false;
    }

    return true;
}

void arp_monitor::stop()
{
    if (m_fdMonSocket != -1) {
        close(m_fdMonSocket);
        m_fdMonSocket = -1;
    }

    if (m_fdArpSocket != -1) {
        close(m_fdArpSocket);
        m_fdArpSocket = -1;
    }
}

bool arp_monitor::process_mon(BPL_ARP_MON_ENTRY& sArpMonData)
{
    char   buf_local[256];
    struct sockaddr_nl nladdr;
    struct iovec iov;

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = 0;
    nladdr.nl_groups = 0;
    iov.iov_base = buf_local;
    iov.iov_len = sizeof(buf_local);

    // Reading the message (non-blocking)
    struct msghdr msg;
    msg.msg_name    = &nladdr;
    msg.msg_namelen = sizeof(nladdr);
    msg.msg_iov     = &iov;
    msg.msg_iovlen  = 1;

    memset(&sArpMonData, 0, sizeof(sArpMonData));

    ssize_t status;
    if ((status = recvmsg(m_fdMonSocket, &msg, MSG_DONTWAIT | MSG_ERRQUEUE)) < 0) 
    {
       MAPF_ERR("Failed reading from socket: " << strerror(errno));
        // if (errno == EINTR || errno == EAGAIN || errno == ENOBUFS)
        return false;
    }
    
    if (status == 0) {
        MAPF_ERR("EOF on netlink\n");
        return false;
    }
    if (msg.msg_namelen != sizeof(nladdr)) {
        MAPF_ERR("ender address length == " << msg.msg_namelen);
        return false;
    }

    // for loop to retrieve all ARP data.
    for (nlmsghdr* h = (struct nlmsghdr*)buf_local; status >= ssize_t(sizeof(*h));) 
    {
        int len = h->nlmsg_len;
        int l = len - sizeof(*h);
        int len_at = len;

        if (l < 0 || len > status) {
            if (msg.msg_flags & MSG_TRUNC) {
                MAPF_ERR("Truncated message");
                return false;
            }

            MAPF_ERR("Malformed message: len = " << len);
            return false;
        }

        status -= NLMSG_ALIGN(len);

        // Skip irrelevant messages
        if (h->nlmsg_type != RTM_NEWNEIGH &&
            h->nlmsg_type != RTM_DELNEIGH &&
            h->nlmsg_type != RTM_GETNEIGH) {
                continue;
            // if (r->ndm_family != AF_INET) {
            //     MAPF_ERR("ndm_family != AF_INET");
            //     return false;
            //  }
        }

        struct ndmsg *r = (struct ndmsg *)NLMSG_DATA(h);
        if(r == nullptr) {
            MAPF_ERR("ndmsg is NULL");
            return false;
        }

        // State
        sArpMonData.state = r->ndm_state;

        struct rtattr* tb[NDA_MAX+1];
        len_at -= NLMSG_LENGTH(sizeof(*r));
        if (len_at < 0) {
            MAPF_ERR("BUG: wrong nlmsg len = " << len_at);
            return false;
        }

        // creating atrribute table
        unsigned short type;
        int max = NDA_MAX;
        struct rtattr *rta = ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))));
        len_at = h->nlmsg_len - NLMSG_LENGTH(sizeof(*r));
        unsigned short flags = 0;

        memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
        while (RTA_OK(rta, len_at)) {
            type = rta->rta_type & ~flags;
            if ((type <= max) && (!tb[type])){
                tb[type] = rta;
            }
            rta = RTA_NEXT(rta,len_at);
        }

        // IP address
        std::string ipv4_str;
        if (tb[NDA_DST]) {
            memcpy(sArpMonData.ip, RTA_DATA(tb[NDA_DST]), 4);
        }

        // Interface index
        if (r->ndm_ifindex)
            sArpMonData.iface_idx = r->ndm_ifindex;

        // MAC address
        if (tb[NDA_LLADDR]) {
            memcpy(sArpMonData.mac, RTA_DATA(tb[NDA_LLADDR]), 6);
        } 
        // else {
        //     h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
        //     MAPF_WARN("MAC address not found in ARP message.");
        //     continue;
        // }

        // // At the time the ARP response packet is received, when we check the ARP table for state,
        // // we only get STALE as the state. After some x seconds the local table gets updated
        // // to Reachable but we do not know when. Hence we do consider stale entry and process it.
        // // Anything other than reachable or stale state, we simply continue with the loop.
        // if(sArpMonData.state == NUD_REACHABLE || sArpMonData.state == NUD_STALE) {
        //     MAPF_DBG("ARP response state valid to proceed");
        // }
        // else
        // {
        //     h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));

        //     MAPF_WARN("Invalid ARP state: " << int(sArpMonData.state));
        //     continue;
        // }

        if (status > 0)
            MAPF_WARN("Data remaining in Netlink packet: " << int(status));

        break;
    }

    return true;
}

int arp_monitor::process_arp(BPL_ARP_MON_ENTRY& sArpMonData)
{
    // Receive the data
    int iLen = 0;
    if ((iLen = recv(m_fdArpSocket, m_arrArpPacket, sizeof(m_arrArpPacket), 0)) < 0) {
        MAPF_ERR("Failed reading ARP packet: " << iLen);
        return -1;
    }

    // Do NOT continue if the nodes list is empty
    if (!m_lstProbe.size()){
        return -1;
    }

    // Check packet size
    if (iLen < ETH_HLEN + int(sizeof(arp_hdr))) {
        MAPF_WARN("Packet too small: " << iLen);
        return -1;
    }

    struct ethhdr* pEthHeader = (struct ethhdr*)m_arrArpPacket;
    arp_hdr* pArpHeader = (arp_hdr*)(m_arrArpPacket + ETH_HLEN);

    // Non ARP-Reply packet
    if((pEthHeader->h_proto != ETH_P_ARP) || (ntohs(pArpHeader->opcode) != ARPOP_REPLY)) {      
        return -1;
    }

    // Check whether the received MAC is in the list
    int iTaskID = -1;
    for (auto iter = m_lstProbe.begin(); iter != m_lstProbe.end();) {
        auto& node = *iter;

        // Found our node
        if (!std::memcmp(&node->mac, pArpHeader->sender_mac, ETH_ALEN)) {
            iTaskID = node->iTaskID;
            m_lstProbe.erase(iter);
            break;
        }

        // Check the timeout of the node
        if (node->tpTimeout < std::chrono::steady_clock::now()) {
            iter = m_lstProbe.erase(iter);
            continue;
        }
        
        // Advance to the next node
        ++iter;
    }

    // MAC was not found in the list
    if (iTaskID <= 0) {
        MAPF_DBG("MAC " << utils::mac_to_string(sArpMonData.mac) << " was not found in the list");
        return -1;
    }

    // Fill the data structure
    std::memcpy(&sArpMonData.mac, pArpHeader->sender_mac, sizeof(sArpMonData.mac));
    std::memcpy(&sArpMonData.ip, pArpHeader->sender_ip, sizeof(sArpMonData.ip));

    // MAPF_DBG("ARP Response - IP: " << utils::ipv4_to_string(sArpMonData.ipv4) << 
    //          ", MAC: " << utils::mac_to_string(sArpMonData.mac) << " task_id:" << iTaskID);

    return iTaskID;
}

bool arp_monitor::probe(const uint8_t mac[BPL_ARP_MON_MAC_LEN], const uint8_t ip[BPL_ARP_MON_IP_LEN], int iTaskID)
{
    bool create_new_entrie = true;
    
    // Check whether the given node is already waiting for a response 
    for (auto iter = m_lstProbe.begin(); iter != m_lstProbe.end();) {
        auto& node = *iter;

        // If the MAC of the given node is already in the list
        if (!std::memcmp(&node->mac, mac, BPL_ARP_MON_MAC_LEN)) {
            MAPF_DBG("node already in the list, mac=" << utils::mac_to_string(mac));
            create_new_entrie = false;
            break;
        }

        // Advance to the next node
        ++iter;
    }

    // Create a new node entry
    if(create_new_entrie){
        auto pProbeEntry = std::make_shared<SProbeEntry>();
        std::memcpy(pProbeEntry->mac, mac, BPL_ARP_MON_MAC_LEN);
        std::memcpy(pProbeEntry->ip, ip, BPL_ARP_MON_IP_LEN);
        pProbeEntry->iTaskID = iTaskID;
        pProbeEntry->tpTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(PROBE_TIMEOUT_SEC);

        // Add the entry to the list
        m_lstProbe.push_back(pProbeEntry);
        MAPF_DBG("pushed entery to probe list, mac=" << utils::mac_to_string(mac));
    }

    // Send the ARP
    // TODO: Check for error?
    std::string str_iface_ip;
    std::string str_iface_mac;
    if (!utils::linux_iface_get_ip(m_strIface, str_iface_ip) ||
        !utils::linux_iface_get_mac(m_strIface, str_iface_mac)) {
        MAPF_ERR("Failed reading '" << m_strIface << "' IP/MAC!");
        return false;
    }

    MAPF_DBG("send arp, iface=" <<  m_strIface << " dst_ip=" << utils::ipv4_to_string(ip) << 
             " src_ip=" << str_iface_ip << " dst_mac=" << utils::mac_to_string(mac) << 
             " src_mac=" << str_iface_mac);

    uint8_t src_mac[BPL_ARP_MON_MAC_LEN];
    uint8_t dst_mac[BPL_ARP_MON_MAC_LEN];

    utils::mac_from_string(src_mac, str_iface_mac);
    utils::mac_from_string(dst_mac, "ff:ff:ff:ff:ff:ff");

    send_arp(m_strIface, ipv4_to_string(ip), str_iface_ip, dst_mac, src_mac, 1, m_fdArpSocket);

    // TODO: optimize system to send unicast arp when dst_mac is in arp table 
    // network_utils::arp_send(m_strIface, network_utils::ipv4_to_string(sParams.ipv4), 
    //     str_iface_ip, sParams.mac, network_utils::mac_from_string(str_iface_mac),
    //     1, m_fdArpSocket);

    return (true);
}

bool arp_monitor::get_mac_for_ip(const uint8_t ip[BPL_ARP_MON_IP_LEN], uint8_t mac[BPL_ARP_MON_MAC_LEN])
{
    return false;
}

bool arp_monitor::get_ip_for_mac(const uint8_t mac[BPL_ARP_MON_MAC_LEN], uint8_t ip[BPL_ARP_MON_IP_LEN])
{
    return false;
}

void arp_monitor::print_arp_table()
{
}

} // namespace bpl
} // namespace beerocks
