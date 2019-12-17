/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../../common/utils/utils.h"
#include <bpl/bpl_arp.h>

#include "monitor/arp_monitor.h"

#include <sys/socket.h>
// must come after sys/socket.h - see https://lore.kernel.org/patchwork/patch/762406/
#include <linux/if.h>

extern "C" {
#define FEATURE_BRCTL_SHOWMACS_IFNAME
#include <libbridge.h>
}

#include <mapf/common/logger.h>

using namespace beerocks::bpl;

namespace bpl {

int bpl_arp_mon_start(BPL_ARP_MON_CTX *ctx, const char *iface)
{
    if (!ctx || !iface) {
        LOG(ERROR) << "Invalid arguments: ctx = " << ctx << ", iface = " << iface;

        return -1;
    }

    // Clear context pointer
    *ctx = nullptr;

    arp_monitor *pArpMon = new arp_monitor();
    if (pArpMon == nullptr)
        return -1;

    // Start the monitor
    if (pArpMon->start(iface) == false) {
        delete pArpMon;
        return -1;
    }

    // Store the context
    (*ctx) = pArpMon;

    return 0;
}

int bpl_arp_mon_stop(BPL_ARP_MON_CTX ctx)
{
    if (!ctx) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx;
        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;

    // Stop the monitor
    pArpMon->stop();

    // Delete the instance
    delete pArpMon;
    pArpMon = nullptr;

    return 0;
}

int bpl_arp_mon_get_fd(BPL_ARP_MON_CTX ctx)
{
    if (!ctx) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx;
        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;

    return pArpMon->get_mon_fd();
}

int bpl_arp_mon_get_raw_arp_fd(BPL_ARP_MON_CTX ctx)
{
    if (!ctx) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx;
        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;

    return pArpMon->get_arp_fd();
}

int bpl_arp_mon_process(BPL_ARP_MON_CTX ctx, struct BPL_ARP_MON_ENTRY *entry)
{
    if (!ctx || !entry) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx << ", entry = " << entry;

        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;
    if (pArpMon->process_mon(*entry) == false)
        return -1;

    return 0;
}

int bpl_arp_mon_process_raw_arp(BPL_ARP_MON_CTX ctx, struct BPL_ARP_MON_ENTRY *entry)
{
    if (!ctx || !entry) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx << ", entry = " << entry;

        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;
    return (pArpMon->process_arp(*entry));
}

int bpl_arp_mon_probe(BPL_ARP_MON_CTX ctx, const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                      const uint8_t ip[BPL_ARP_MON_IP_LEN], int task_id)
{
    if (!ctx || !mac || !ip) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx << ", mac = " << mac << ", ip = " << ip;

        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;
    return (pArpMon->probe(mac, ip, task_id) ? 0 : -1);
}

int bpl_arp_mon_get_mac_for_ip(BPL_ARP_MON_CTX ctx, const uint8_t ip[BPL_ARP_MON_IP_LEN],
                               uint8_t mac[BPL_ARP_MON_MAC_LEN])
{
    if (!ctx || !ip || !mac) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx << ", ip = " << ip << ", mac = " << mac;

        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;

    return (pArpMon->get_mac_for_ip(ip, mac) ? 0 : -1);
}

int bpl_arp_mon_get_ip_for_mac(BPL_ARP_MON_CTX ctx, const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                               uint8_t ip[BPL_ARP_MON_IP_LEN])
{
    if (!ctx || !mac || !ip) {
        LOG(ERROR) << "Invalid argument: ctx = " << ctx << ", mac = " << mac << ", ip = " << ip;

        return -1;
    }

    arp_monitor *pArpMon = (arp_monitor *)ctx;

    return (pArpMon->get_ip_for_mac(mac, ip) ? 0 : -1);
}

int bpl_arp_get_bridge_iface(const char bridge[BPL_ARP_IFACE_NAME_LEN],
                             const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                             char iface[BPL_ARP_IFACE_NAME_LEN])
{
    if (!bridge || !mac || !iface) {
        LOG(ERROR) << "Invalid argument: bridge = " << (void *)bridge << ", mac = " << (void *)mac
                   << ", iface = " << (void *)iface;

        return -1;
    }

    // Clear the target buffer
    std::memset(iface, 0, BPL_ARP_IFACE_NAME_LEN);

// Allocate buffer for bridge entries
// Support up to 256 bridge entries
// TODO: Dynamically increase...
#define MAX_BR_ENTRIES 256
    std::unique_ptr<fdb_entry[]> bridge_entries(new fdb_entry[MAX_BR_ENTRIES]);
    // fdb_entry* bridge_entries = new fdb_entry[MAX_BR_ENTRIES];

    if (!bridge_entries) {
        LOG(ERROR) << "Memory allocation failed!";
        return -1;
    }

    // Read bridge entries
    int ret;
    if ((ret = br_read_fdb(bridge, bridge_entries.get(), 0, MAX_BR_ENTRIES)) <= 0) {
        LOG(ERROR) << "Failed reading brdige '" << bridge
                   << "' entries: " << ((ret < 0) ? strerror(errno) : "0");

        return -1;
    }

    // LOG(DEBUG) << "Read " << ret << " entries from '"
    //             << bridge << "'";

    // Look for the received MAC address
    for (int i = 0; i < ret; i++) {

        const fdb_entry *pFDB = &bridge_entries[i];

        // Skip other MACs
        if (std::memcmp(mac, pFDB->mac_addr, BPL_ARP_MON_MAC_LEN) != 0)
            continue;

        char ifname[IFNAMSIZ] = {0};

        // Convert the port number to an interface name
        portno_to_iface(pFDB->port_no, bridge, ifname);

        // Return the interface name
        utils::copy_string(iface, ifname, BPL_ARP_IFACE_NAME_LEN);
        break;
    }

    return 0;
}

} // namespace bpl
