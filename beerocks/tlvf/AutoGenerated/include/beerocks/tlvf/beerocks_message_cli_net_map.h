///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
*/

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CLI_NET_MAP_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_CLI_NET_MAP_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "beerocks/tlvf/beerocks_message_common.h"

namespace beerocks_message {

typedef struct sCliNetworkMapNodeAp {
    beerocks::net::sMacAddr mac;
    uint16_t length;
    uint8_t hierarchy;
    void struct_swap(){
        mac.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&length));
    }
    void struct_init(){
        mac.struct_init();
    }
} sCliNetworkMapNodeAp;

typedef struct sCliNetworkMapNodeSta {
    beerocks::net::sMacAddr mac;
    uint8_t type;
    int8_t rx_rssi;
    void struct_swap(){
        mac.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
    }
} sCliNetworkMapNodeSta;

typedef struct sCliNetworkMapsNodeInfo {
    beerocks::net::sMacAddr mac;
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
} sCliNetworkMapsNodeInfo;


}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_CLI_NET_MAP_H_
