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

#ifndef _BPL_ARP_H_
#define _BPL_ARP_H_

#include "bpl.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/******************************* Definitions ********************************/
/****************************************************************************/

/* ARP Monitor internal context */
typedef void* BPL_ARP_MON_CTX;

#define BPL_ARP_MON_MAC_LEN     6  /* MAC address length */
#define BPL_ARP_MON_IP_LEN      4  /* IP address length */
#define BPL_ARP_IFACE_NAME_LEN  16 /* Interface name length */

#define BPL_ARP_TYPE_NEWNEIGH   0
#define BPL_ARP_TYPE_DELNEIGH   1
#define BPL_ARP_TYPE_GETNEIGH   2

/****************************************************************************/
/******************************* Structures *********************************/
/****************************************************************************/

/* ARP Monitor Entry */
struct BPL_ARP_MON_ENTRY {
    uint8_t  mac[BPL_ARP_MON_MAC_LEN];  /* Client's MAC address */
    uint8_t  ip[BPL_ARP_MON_IP_LEN];    /* Client's IP address */
    uint32_t iface_idx;                 /* Interface index of the ARP entry */
    uint8_t  state;                     /* ARP entry state */
    uint8_t  source;                    /* ??? */
    uint8_t  type;                      /* ARP message header type */
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
};

/****************************************************************************/
/******************************** Functions *********************************/
/****************************************************************************/

/**
 * Start the ARP monitor on the specified interface.
 *
 * @param [out] ctx BPL ARP monitor context.
 * @param [in] iface String representation of the interface to monitor.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_mon_start(BPL_ARP_MON_CTX* ctx, const char* iface);

/**
 * Stop the ARP monitor.
 *
 * @param [in] ctx BPL ARP monitor context.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_mon_stop(BPL_ARP_MON_CTX ctx);

/**
 * Get the file descriptor of the monitoring interface.
 *
 * @param [in] ctx BPL ARP monitor context.
 *
 * @return file descriptor on success or -1 on error.
 */
int bpl_arp_mon_get_fd(BPL_ARP_MON_CTX ctx);

/**
 * Get the file descriptor of the raw arp interface.
 *
 * @param [in] ctx BPL ARP monitor context.
 *
 * @return file descriptor on success or -1 on error.
 */
int bpl_arp_mon_get_raw_arp_fd(BPL_ARP_MON_CTX ctx);

/**
 * Process data from the monitoring socket.
 *
 * @param [in] ctx BPL ARP monitor context.
 * @param [out] entry ARP monitor entry.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_mon_process(BPL_ARP_MON_CTX ctx, 
        struct BPL_ARP_MON_ENTRY* entry);

/**
 * Process data from the raw arp socket.
 *
 * @param [in] ctx BPL ARP Monitor Context.
 * @param [out] entry ARP monitor entry.
 *
 * @return -1  on error.
 * @return >=0 Value of the task_id that generated the ARP query/probe.
 */
int bpl_arp_mon_process_raw_arp(BPL_ARP_MON_CTX ctx, 
        struct BPL_ARP_MON_ENTRY* entry);

/**
 * Send unicast ARP message to the specified MAC & IP addresses.
 *
 * @param [in] ctx BPL ARP Monitor Context.
 * @param [in] mac MAC address for the ARP message.
 * @param [in] ip IP address for the ARP message.
 * @param [in] task_id Task ID that triggered the probing.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_mon_probe(BPL_ARP_MON_CTX ctx, 
        const uint8_t mac[BPL_ARP_MON_MAC_LEN], 
        const uint8_t ip[BPL_ARP_MON_IP_LEN],
        int task_id);

/**
 * Query the ARP table for the MAC address of a given IP.
 *
 * @param [in] ctx BPL ARP Monitor Context.
 * @param [in] ip IP address to query.
 * @param [out] mac MAC address for the requested IP.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_mon_get_mac_for_ip(BPL_ARP_MON_CTX ctx, 
        const uint8_t ip[BPL_ARP_MON_IP_LEN], 
              uint8_t mac[BPL_ARP_MON_MAC_LEN]);

/**
 * Query the ARP table for the IP address of a given MAC.
 *
 * @param [in] ctx BPL ARP Monitor Context.
 * @param [in] mac MAC address to query.
 * @param [out] ip IP address for the requested MAC.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_mon_get_ip_for_mac(BPL_ARP_MON_CTX ctx, 
        const uint8_t mac[BPL_ARP_MON_MAC_LEN], 
              uint8_t ip[BPL_ARP_MON_IP_LEN]);

/**
 * Queries the given bridge for the given MAC address and 
 * returns the spcific interface name which contains the MAC.
 *
 * @param [in] bridge Bridge name.
 * @param [in] mac MAC address to query.
 * @param [out] iface Interface name containing the MAC.
 *
 * @return 0 on success or -1 on error.
 */
int bpl_arp_get_bridge_iface(
        const char bridge[BPL_ARP_IFACE_NAME_LEN], 
        const uint8_t mac[BPL_ARP_MON_MAC_LEN], 
              char iface[BPL_ARP_IFACE_NAME_LEN]);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BPL_ARP_H_ */
