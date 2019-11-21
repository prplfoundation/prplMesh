/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../common/utils/utils.h"
#include <bpl/bpl_arp.h>

#include <sys/socket.h>
// must come after sys/socket.h - see https://lore.kernel.org/patchwork/patch/762406/
#include <linux/if.h>

#include <mapf/common/logger.h>

using namespace beerocks::bpl;

int bpl_arp_mon_start(BPL_ARP_MON_CTX *ctx, const char *iface) { return 0; }

int bpl_arp_mon_stop(BPL_ARP_MON_CTX ctx) { return 0; }

int bpl_arp_mon_get_fd(BPL_ARP_MON_CTX ctx) { return -1; }

int bpl_arp_mon_get_raw_arp_fd(BPL_ARP_MON_CTX ctx) { return -1; }

int bpl_arp_mon_process(BPL_ARP_MON_CTX ctx, struct BPL_ARP_MON_ENTRY *entry) { return 0; }

int bpl_arp_mon_process_raw_arp(BPL_ARP_MON_CTX ctx, struct BPL_ARP_MON_ENTRY *entry) { return 0; }

int bpl_arp_mon_probe(BPL_ARP_MON_CTX ctx, const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                      const uint8_t ip[BPL_ARP_MON_IP_LEN], int task_id)
{
    return 0;
}

int bpl_arp_mon_get_mac_for_ip(BPL_ARP_MON_CTX ctx, const uint8_t ip[BPL_ARP_MON_IP_LEN],
                               uint8_t mac[BPL_ARP_MON_MAC_LEN])
{
    return -1;
}

int bpl_arp_mon_get_ip_for_mac(BPL_ARP_MON_CTX ctx, const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                               uint8_t ip[BPL_ARP_MON_IP_LEN])
{
    return -1;
}

int bpl_arp_get_bridge_iface(const char bridge[BPL_ARP_IFACE_NAME_LEN],
                             const uint8_t mac[BPL_ARP_MON_MAC_LEN],
                             char iface[BPL_ARP_IFACE_NAME_LEN])
{
    return -1;
}
