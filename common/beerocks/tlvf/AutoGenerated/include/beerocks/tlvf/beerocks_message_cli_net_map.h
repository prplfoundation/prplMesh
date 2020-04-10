///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CLI_NET_MAP_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CLI_NET_MAP_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {

typedef struct sCliNetworkMapNodeAp {
    sMacAddr mac;
    uint16_t length;
    uint8_t hierarchy;
    void struct_swap(){
        mac.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&length));
    }
    void struct_init(){
        mac.struct_init();
    }
} __attribute__((packed)) sCliNetworkMapNodeAp;

typedef struct sCliNetworkMapNodeSta {
    sMacAddr mac;
    uint8_t type;
    int8_t rx_rssi;
    void struct_swap(){
        mac.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
    }
} __attribute__((packed)) sCliNetworkMapNodeSta;

typedef struct sCliNetworkMapsNodeInfo {
    sMacAddr mac;
    beerocks::net::sIpv4Addr ipv4;
    char name[beerocks::message::NODE_NAME_LENGTH];
    uint8_t type;
    uint8_t state;
    uint8_t channel;
    uint8_t bandwidth;
    uint8_t channel_ext_above_secondary;
    uint8_t is_dfs;
    uint8_t cac_completed;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
    uint16_t stats_delta_ms;
    uint8_t tx_load_percent;
    uint8_t rx_load_percent;
    uint8_t channel_load_percent;
    uint8_t iface_type;
    int8_t vap_id;
    void struct_swap(){
        mac.struct_swap();
        ipv4.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&tx_bytes));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&rx_bytes));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&stats_delta_ms));
    }
    void struct_init(){
        mac.struct_init();
        ipv4.struct_init();
    }
} __attribute__((packed)) sCliNetworkMapsNodeInfo;


}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_CLI_NET_MAP_H_
