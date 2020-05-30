/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _ARP_MONITOR_H
#define _ARP_MONITOR_H

#include <bpl/bpl_arp.h>

#include <chrono>
#include <list>
#include <memory>

#include <linux/if_ether.h>

// Forward declaration
struct nl_sock;

namespace beerocks {
namespace bpl {

class arp_monitor {

    /*
 * Public methdos:
 */
public:
    arp_monitor();
    ~arp_monitor();

    // Start monitoring and return the FD of the monitoring socket
    int start(std::string strIface);

    // Stop monitoring
    void stop();

    // Process data from the monitoring interface
    bool process_mon(BPL_ARP_MON_ENTRY &sArpMonData);

    // Process data from the raw ARP socket
    // Returns the TaskID for the processed ARP (or -1 on error)
    int process_arp(BPL_ARP_MON_ENTRY &sArpMonData);

    // Send a unicast ARP message to the given client
    bool probe(const uint8_t mac[BPL_ARP_MON_MAC_LEN], const uint8_t ip[BPL_ARP_MON_IP_LEN],
               int iTaskID);

    // Query the ARP table for MAC/IP address
    // First arg is input, the second is output
    bool get_mac_for_ip(const uint8_t ip[BPL_ARP_MON_IP_LEN], uint8_t mac[BPL_ARP_MON_MAC_LEN]);
    bool get_ip_for_mac(const uint8_t mac[BPL_ARP_MON_MAC_LEN], uint8_t ip[BPL_ARP_MON_IP_LEN]);

    /*
 * Public getters:
 */
public:
    int get_mon_fd() const { return (m_fdMonSocket); }

    int get_arp_fd() const { return (m_fdArpSocket); }

    // TODO: TEMP!
    void print_arp_table();

private:
    // Monitoring Netlink Socket
    int m_fdMonSocket;

    // RAW Socket for sending unicast ARP messages
    int m_fdArpSocket;

    // Netlink Socket (for accessing the ARP table)
    struct nl_sock *m_pNlSocket;

    // Interface for sending ARP messages
    std::string m_strIface;

    struct SProbeEntry {
        int iTaskID;
        uint8_t mac[BPL_ARP_MON_MAC_LEN];
        uint8_t ip[BPL_ARP_MON_IP_LEN];
        std::chrono::steady_clock::time_point tpTimeout;
    };

    std::list<std::shared_ptr<SProbeEntry>> m_lstProbe;

    // Buffer for reading ARP packets
    char m_arrArpPacket[ETH_FRAME_LEN]{0};
};

} // namespace bpl
} // namespace beerocks

#endif // _ARP_MONITOR_H
