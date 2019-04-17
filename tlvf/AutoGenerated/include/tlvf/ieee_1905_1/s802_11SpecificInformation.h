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

#ifndef _TLVF_IEEE_1905_1_S802_11SPECIFICINFORMATION_H_
#define _TLVF_IEEE_1905_1_S802_11SPECIFICINFORMATION_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "tlvf/common/sMacAddress.h"

namespace ieee1905_1 {

enum eRole: uint8_t {
    AP = 0x0,
    NON_AP_NON_PCP_STA = 0x40,
    WI_FI_P2P_CLIENT = 0x80,
    WI_FI_P2P_GROUP_OWNER = 0x90,
    IEEE_802_11AD_PCP = 0xa0,
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
    void struct_swap(){
        network_membership.struct_swap();
    }
    void struct_init(){
        network_membership.struct_init();
    }
} s802_11SpecificInformation;


}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_S802_11SPECIFICINFORMATION_H_
