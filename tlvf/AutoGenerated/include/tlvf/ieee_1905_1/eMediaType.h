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

#ifndef _TLVF_IEEE_1905_1_EMEDIATYPE_H_
#define _TLVF_IEEE_1905_1_EMEDIATYPE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

enum eMediaType: uint16_t {
    IEEE_802_3U_FAST_ETHERNET = 0x0,
    IEEE_802_3AB_GIGABIT_ETHERNET = 0x1,
    IEEE_802_11B_2_4_GHZ = 0x100,
    IEEE_802_11G_2_4_GHZ = 0x101,
    IEEE_802_11A_5_GHZ = 0x102,
    IEEE_802_11N_2_4_GHZ = 0x103,
    IEEE_802_11N_5_GHZ = 0x104,
    IEEE_802_11AC_5_GHZ = 0x105,
    IEEE_802_11AD_60_GHZ = 0x106,
    IEEE_802_11AF = 0x107,
    IEEE_1901_WAVELET = 0x200,
    IEEE_1901_FFT = 0x201,
    MOCA_V1_1 = 0x300,
    UNKNONWN_MEDIA = 0xffff,
};


}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_EMEDIATYPE_H_
