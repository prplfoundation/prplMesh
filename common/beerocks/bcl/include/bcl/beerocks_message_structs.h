/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_MESSAGE_STRUCTS_H_
#define _BEEROCKS_MESSAGE_STRUCTS_H_

#include "beerocks_defines.h"
#include "network/net_struct.h"
#include "network/swap.h"

namespace beerocks {
namespace message {

typedef struct {
    uint8_t dst_bridge_mac[net::MAC_ADDR_LEN] = {};
    uint8_t src_bridge_mac[net::MAC_ADDR_LEN] = {};
    uint16_t length                           = 0;
    uint8_t swap_needed                       = 0;
} __attribute__((packed)) sUdsHeader;

//////////////////// tlvf includes /////////////////////////////

// Note:
// 1. Only structs which are needed for BCL should be defined here instead in a YAML file.
// 2. If you decided to add here a struct, make sure it has a construcor and swap function.
//    TIP: The best way to do it is to create the the struct in a yaml file temporary (beerocks_message_common.yaml),
//         and copy the auto generated struct to here.

// used in bcl on "son_wireless_utils.h"
typedef struct sRadioCapabilities {
    uint8_t ht_mcs                = 0; //beerocks::eWiFiMCS RX
    uint8_t vht_mcs               = 0; //beerocks::eWiFiMCS RX
    uint8_t ht_ss                 = 0; //beerocks::eWiFiSS RX
    uint8_t vht_ss                = 0; //beerocks::eWiFiSS RX
    uint8_t ht_bw                 = 0; //beerocks::eWiFiBandwidth
    uint8_t vht_bw                = 0; //beerocks::eWiFiBandwidth
    uint8_t ht_low_bw_short_gi    = 0; //20 Mhz
    uint8_t ht_high_bw_short_gi   = 0; //40 Mhz
    uint8_t vht_low_bw_short_gi   = 0; //80 Mhz
    uint8_t vht_high_bw_short_gi  = 0; //160 Mhz
    uint8_t ht_sm_power_save      = 0; //beerocks::eHtCapsSmPowerSaveMode
    uint8_t ant_num               = 0;
    uint8_t wifi_standard         = 0;
    uint8_t default_short_gi      = 0;
    uint8_t default_mcs           = 0;
    uint8_t btm_supported         = 0;
    uint8_t nr_enabled            = 0;
    uint8_t cell_capa             = 0;
    uint8_t cap_flag              = 0;
    uint8_t rrm_supported         = 0;
    uint8_t band_2g_capable       = 0;
    uint8_t band_5g_capable       = 0;
    uint8_t max_ch_width          = 0;
    uint8_t phy_mode              = 0;
    uint8_t max_mcs               = 0;
    uint8_t max_tx_power          = 0;
    uint8_t link_meas             = 0;
    uint8_t beacon_report_passive = 0;
    uint8_t beacon_report_active  = 0;
    uint8_t beacon_report_table   = 0;
    uint8_t lci_meas              = 0;
    uint8_t fmt_range_report      = 0;
    uint8_t mumimo_supported      = 0;
    uint8_t max_streams           = 0;
    sRadioCapabilities() {}
    void struct_swap() {}
    void struct_init()
    {
        ht_mcs               = 0;
        vht_mcs              = 0;
        ht_ss                = 0;
        vht_ss               = 0;
        ht_bw                = 0;
        vht_bw               = 0;
        ht_low_bw_short_gi   = 0;
        ht_high_bw_short_gi  = 0;
        vht_low_bw_short_gi  = 0;
        vht_high_bw_short_gi = 0;
        ht_sm_power_save     = 0;
        ant_num              = 0;
        wifi_standard        = 0;
        default_short_gi     = 0;
        default_mcs          = 0;
        btm_supported        = 0;
        nr_enabled           = 0;
        cell_capa            = 0;
        cap_flag             = 0;
    }
} __attribute__((packed)) sRadioCapabilities;

//////////////////// tlvf includes - END/////////////////////////////

#define MACRO_IS_ENABLE                                                                            \
    uint8_t isEnable;                                                                              \
    uint8_t reserved1;                                                                             \
    uint8_t reserved2;                                                                             \
    uint8_t reserved3;

#define MACRO_CLI_IS_ENABLE                                                                        \
    int8_t isEnable;                                                                               \
    uint8_t reserved1;                                                                             \
    uint8_t reserved2;                                                                             \
    uint8_t reserved3;

} // namespace message
} // namespace beerocks

#endif
