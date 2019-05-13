///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_IEEE_1905_1_S802_11SPECIFICINFORMATION_H_
#define _TLVF_IEEE_1905_1_S802_11SPECIFICINFORMATION_H_

#include "tlvf/common/sMacAddress.h"
#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

enum eRole : uint8_t {
    AP                    = 0x0,
    NON_AP_NON_PCP_STA    = 0x40,
    WI_FI_P2P_CLIENT      = 0x80,
    WI_FI_P2P_GROUP_OWNER = 0x90,
    IEEE_802_11AD_PCP     = 0xa0,
};

typedef struct s802_11SpecificInformation {
    sMacAddress network_membership;
    eRole role;
    //Hex value of dot11CurrentChannelBandwidth
    uint8_t ap_channel_band;
    //Hex value of dot11CurrentChannelCenterFrequencyIndex1
    uint8_t ap_channel_center_frequency_index1;
    //Hex value of dot11CurrentChannelCenterFrequencyIndex2
    uint8_t ap_channel_center_frequency_index2;
    void struct_swap() { network_membership.struct_swap(); }
    void struct_init() { network_membership.struct_init(); }
} s802_11SpecificInformation;

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_S802_11SPECIFICINFORMATION_H_
