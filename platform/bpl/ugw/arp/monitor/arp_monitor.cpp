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

#include <cstring>
#include <vector>

#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>

#include <linux/if_arp.h>

#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>
#include <netlink/route/neightbl.h>
#include <netlink/route/neighbour.h>

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

static bool read_arp_table(struct nl_sock* sock,  
    std::vector<arp_neigh>& vecNeigh)
{
	// ARP table cache
    struct nl_cache* neightbl_cache = nullptr;

    // Allocate the cache and fill it with data
    int ret;
    if ((ret = rtnl_neigh_alloc_cache(sock, &neightbl_cache)) != 0) {
        MAPF_ERR("Failed probing ARP table: " << nl_geterror(ret));
        return false;
    }
    
	// struct nl_dump_params params;
    // memset(&params, 0, sizeof(nl_dump_params));

	// params.dp_type = NL_DUMP_DETAILS;//NL_DUMP_LINE;
	// params.dp_fd = stdout;
    // // params.dp_type = NL_DUMP_DETAILS;
    // // params.dp_prefix = 30;
    // // params.dp_print_index = 1;
    // // params.dp_dump_msgtype = 1;
    // // params.dp_cb = NULL;
    // // params.dp_nl_cb = NULL;
    // // params.dp_data = NULL;
    // // params.dp_fd = stdout;
    // // params.dp_buf = NULL;
    // // params.dp_buflen = 0;

	// nl_cache_dump(neightbl_cache, &params);

    int num_of_items = nl_cache_nitems(neightbl_cache);

    struct rtnl_neigh* neigh = (rtnl_neigh*)nl_cache_get_first(neightbl_cache);
    if (!neigh) {
        MAPF_ERR("neigh is null");
        nl_cache_free(neightbl_cache);
        return false;
    }

    for (int i = 0; i < num_of_items; i++) {
        arp_neigh curr_neigh = {0};

        curr_neigh.ifindex = rtnl_neigh_get_ifindex(neigh);
        curr_neigh.state = rtnl_neigh_get_state(neigh);

        // Skip NOARP state and non IPv4 records
        if (curr_neigh.state != NUD_NOARP && rtnl_neigh_get_family(neigh) == AF_INET) {

            // Only store valid records
            if (rtnl_neigh_get_dst(neigh) && rtnl_neigh_get_lladdr(neigh)) {
                std::copy_n((uint8_t*)nl_addr_get_binary_addr(rtnl_neigh_get_dst(neigh)), 4, curr_neigh.ip);
                std::copy_n((uint8_t*)nl_addr_get_binary_addr(rtnl_neigh_get_lladdr(neigh)), 6, curr_neigh.mac);

                vecNeigh.push_back(curr_neigh);
            }
        }

        // Advance to the next record
        neigh = (rtnl_neigh*)nl_cache_get_next((nl_object*)neigh);
        if (!neigh) {
            MAPF_ERR("neigh is null");
            nl_cache_free(neightbl_cache);
            return false;
        }
    }

    // Free the cache
    nl_cache_free(neightbl_cache);
    
    return true;
}

static bool send_arp(std::string iface, std::string dst_ip, std::string src_ip,
        uint8_t* dst_mac, uint8_t* src_mac, int count, int arp_socket)
{
    int tx_len;
    arp_hdr arphdr;
    struct sockaddr_ll sock = {};
    uint8_t packet_buffer[128];

    uint32_t dst_ip_uint = utils::uint_ipv4_from_string(dst_ip);
    uint32_t src_ip_uint = utils::uint_ipv4_from_string(src_ip);

    // Fill out sockaddr_ll
    sock.sll_family = AF_PACKET;
    sock.sll_ifindex = utils::linux_iface_to_index(iface);
    sock.sll_halen = BPL_ARP_MON_MAC_LEN;
    std::copy_n(dst_mac, BPL_ARP_MON_MAC_LEN, sock.sll_addr);

    // build ARP header
    arphdr.htype = htons(1); //type: 1 for ethernet
    arphdr.ptype = htons(ETH_P_IP); // proto
    arphdr.hlen = BPL_ARP_MON_MAC_LEN; // mac addr len
    arphdr.plen = BPL_ARP_MON_IP_LEN; // ip addr len
    arphdr.opcode = htons (ARPOP_REQUEST);
    std::copy_n(src_mac, BPL_ARP_MON_MAC_LEN, arphdr.sender_mac);
    std::copy_n((uint8_t*)&src_ip_uint, BPL_ARP_MON_IP_LEN, arphdr.sender_ip);
    std::copy_n(dst_mac, BPL_ARP_MON_MAC_LEN, arphdr.target_mac);
    std::copy_n((uint8_t*)&dst_ip_uint, BPL_ARP_MON_IP_LEN, arphdr.target_ip);

    // build ethernet frame
    tx_len = 2 * BPL_ARP_MON_MAC_LEN + 2 + ARP_HDRLEN; // dest mac, src mac, type, arp header len
    std::copy_n(dst_mac, BPL_ARP_MON_MAC_LEN, packet_buffer);
    std::copy_n(src_mac, BPL_ARP_MON_MAC_LEN, packet_buffer + BPL_ARP_MON_MAC_LEN);
    packet_buffer[12] = ETH_P_ARP / 256;
    packet_buffer[13] = ETH_P_ARP % 256;

    // ARP header
    std::copy_n((uint8_t*)&arphdr, ARP_HDRLEN, packet_buffer + ETH_HDRLEN);

    bool new_socket = (arp_socket < 0);
    if (new_socket) {
        if((arp_socket = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0){
            MAPF_ERR("Opening ARP socket");
            return false;
        }
    }

    // Send ethernet frame to socket.
    for(int i = 0; i < count ; i++) {
        //LOG_MONITOR(DEBUG) << "ARP to ip=" << dest_ip << " mac=" <<utils::mac_to_string(dest_mac);
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
    m_fdArpSocket(-1),
    m_pNlSocket(nullptr)
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

    struct sockaddr_nl local = {};

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

    // Allocate a new netlink socket
    if ((m_pNlSocket = nl_socket_alloc()) == nullptr) {
        MAPF_ERR("Failed allocating netlink socket!");
        stop();
        return false;
    }

    // Connect the netlink socket
    int err;
    if ((err = nl_connect(m_pNlSocket, NETLINK_ROUTE)) != 0) {
        MAPF_ERR("Failed connecting the netlink socket: " << nl_geterror(err));
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

    if (m_pNlSocket != nullptr) {
        nl_socket_free(m_pNlSocket);
        m_pNlSocket = nullptr;
    }
}

bool arp_monitor::process_mon(BPL_ARP_MON_ENTRY& sArpMonData)
{
    char   buf_local[256];
    struct sockaddr_nl nladdr = {};
    struct iovec iov;

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

    memset((uint8_t*)&sArpMonData, 0, sizeof(sArpMonData));

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

        // IMPORTANT: Ignore RTM_DELNEIGH messages on the GRX350/IRE220 platforms.
        // Since the transport layer is accelerated, the OS may incorrectly decide
        // that a connected client has disconnected.
        if (h->nlmsg_type == RTM_DELNEIGH) {
            continue;
        }

        // Header type converted to beerocks_define.h ARP_TYPE
        if(h->nlmsg_type == RTM_NEWNEIGH) {
            sArpMonData.type = BPL_ARP_TYPE_NEWNEIGH;
        }
        //see "IMPORTANT" above
        // else if(h->nlmsg_type == RTM_DELNEIGH) {
        //     sArpMonData.type = BPL_ARP_TYPE_DELNEIGH;
        // }
        else if(h->nlmsg_type == RTM_GETNEIGH) {
            sArpMonData.type = BPL_ARP_TYPE_GETNEIGH;
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
            std::copy_n((uint8_t*)RTA_DATA(tb[NDA_DST]), BPL_ARP_MON_IP_LEN, sArpMonData.ip);
        }

        // Interface index
        if (r->ndm_ifindex)
            sArpMonData.iface_idx = r->ndm_ifindex;

        // MAC address
        if (tb[NDA_LLADDR]) {
            std::copy_n((uint8_t*)RTA_DATA(tb[NDA_LLADDR]), BPL_ARP_MON_MAC_LEN, sArpMonData.mac);
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

        if (status > 0) {
            MAPF_WARN("Data remaining in Netlink packet: " << int(status));
        }

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
        MAPF_DBG("Non ARP-REPLY Packet");
        return -1;
    }

    // Check whether the received MAC is in the list
    int iTaskID = -1;
    for (auto iter = m_lstProbe.begin(); iter != m_lstProbe.end();) {
        auto& node = *iter;

        // Found our node
        if (!std::memcmp(node->mac, pArpHeader->sender_mac, ETH_ALEN)) {
            iTaskID = node->iTaskID;
            m_lstProbe.erase(iter);
            break;
        }

        // Check the timeout of the node
        if (node->tpTimeout < std::chrono::steady_clock::now()) {
            MAPF_DBG("Removing the mac: " << utils::mac_to_string(node->mac) << " from probe list");
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
    std::copy_n(pArpHeader->sender_mac, sizeof(sArpMonData.mac), sArpMonData.mac);
    std::copy_n(pArpHeader->sender_ip, sizeof(sArpMonData.ip), sArpMonData.ip);

    //MAPF_DBG("ARP Response - IP: " << network_utils::ipv4_to_string(sArpMonData.ipv4) << 
    //         ", MAC: " << utils::mac_to_string(sArpMonData.mac) << " task_id:" << iTaskID);

    return iTaskID;
}

bool arp_monitor::probe(const uint8_t mac[BPL_ARP_MON_MAC_LEN], const uint8_t ip[BPL_ARP_MON_IP_LEN], int iTaskID)
{
    MAPF_DBG("probe, mac=" << utils::mac_to_string(mac) << " task_id=" << iTaskID);
    bool create_new_entrie = true;
    // Check whether the given node is already waiting for a response 
    for (auto iter = m_lstProbe.begin(); iter != m_lstProbe.end();) {
        auto& node = *iter;

        // If the MAC of the given node is already in the list
        if (!std::memcmp(node->mac, mac, BPL_ARP_MON_MAC_LEN)) {
            MAPF_DBG("node already in the list, mac=" << utils::mac_to_string(mac));
            create_new_entrie = false;
            // If timedout for this mac then update the exisitng entry with current data
            if (node->tpTimeout < std::chrono::steady_clock::now()) {
                MAPF_DBG("Updating the mac: " << utils::mac_to_string(node->mac) << " from probe list");
                std::copy_n(ip, BPL_ARP_MON_IP_LEN, node->ip);
                node->iTaskID = iTaskID;
                node->tpTimeout = std::chrono::steady_clock::now() + std::chrono::seconds(PROBE_TIMEOUT_SEC);
            }
            break;
        }

        // Advance to the next node
        ++iter;
    }

    // Create a new node entry
    if(create_new_entrie){
        auto pProbeEntry = std::make_shared<SProbeEntry>();
        std::copy_n(mac, BPL_ARP_MON_MAC_LEN, pProbeEntry->mac);
        std::copy_n(ip, BPL_ARP_MON_IP_LEN, pProbeEntry->ip);
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

    send_arp(m_strIface, utils::ipv4_to_string(ip), str_iface_ip, dst_mac, src_mac, 1, m_fdArpSocket);

    // TODO: optimize system to send unicast arp when dst_mac is in arp table 
    // network_utils::arp_send(m_strIface, network_utils::ipv4_to_string(sParams.ipv4), 
    //     str_iface_ip, sParams.mac, network_utils::mac_from_string(str_iface_mac),
    //     1, m_fdArpSocket);

    return (true);
}

bool arp_monitor::get_mac_for_ip(const uint8_t ip[BPL_ARP_MON_IP_LEN], uint8_t mac[BPL_ARP_MON_MAC_LEN])
{
    std::vector<arp_neigh> vecArpTable;

    if (!read_arp_table(m_pNlSocket, vecArpTable)) {
        MAPF_ERR("Failed reading the ARP table!");
        return false;
    }

    // Clear the MAC address
    memset(mac, 0, BPL_ARP_MON_MAC_LEN);

    for (auto neigh : vecArpTable) {
        if (memcmp(ip, neigh.ip, BPL_ARP_MON_IP_LEN) == 0) {
            std::copy_n(neigh.mac, BPL_ARP_MON_MAC_LEN, mac);
            return true;
        }
    }

    return false;
}

bool arp_monitor::get_ip_for_mac(const uint8_t mac[BPL_ARP_MON_MAC_LEN], uint8_t ip[BPL_ARP_MON_IP_LEN])
{
    std::vector<arp_neigh> vecArpTable;

    if (!read_arp_table(m_pNlSocket, vecArpTable)) {
        MAPF_ERR("Failed reading the ARP table!");
        return false;
    }

    // Clear the IP address
    memset(ip, 0, BPL_ARP_MON_IP_LEN);

    for (auto neigh : vecArpTable) {
        if (memcmp(mac, neigh.mac, BPL_ARP_MON_MAC_LEN) == 0) {
            std::copy_n(neigh.ip, BPL_ARP_MON_IP_LEN, ip);

            return true;
        }
    }

    return false;    
}

void arp_monitor::print_arp_table()
{
    std::vector<arp_neigh> vecArpTable;

    if (!read_arp_table(m_pNlSocket, vecArpTable)) {
        MAPF_ERR("Failed reading the ARP table!");
        return;
    }

    for (auto neigh : vecArpTable) {

        MAPF_DBG("IP: " << utils::ipv4_to_string(neigh.ip) << 
                 ", MAC: " << utils::mac_to_string(neigh.mac));

        // uint8_t mac[6];
        // get_mac_for_ip(neigh.ip, mac);
        // MAPF_DBG("MAC for IP: " << utils::ipv4_to_string(neigh.ip) << 
        //          " --> " << utils::mac_to_string(mac));
        
        // uint8_t ip[4];
        // get_ip_for_mac(neigh.mac, ip);
        // MAPF_DBG("IP for MAC: " << utils::mac_to_string(neigh.mac) << 
        //          " --> " << utils::ipv4_to_string(ip);
    }    
}

} // namespace bpl
} // namespace beerocks
