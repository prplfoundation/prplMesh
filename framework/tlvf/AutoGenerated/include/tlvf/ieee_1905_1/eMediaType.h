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
    IEEE_802_11AX = 0x108,
    IEEE_1901_WAVELET = 0x200,
    IEEE_1901_FFT = 0x201,
    MOCA_V1_1 = 0x300,
    UNKNONWN_MEDIA = 0xffff,
};

enum eMediaTypeGroup: uint8_t {
    IEEE_802_3 = 0x0,
    IEEE_802_11 = 0x1,
    IEEE_1901 = 0x2,
    MoCA = 0x3,
    UNKNOWN = 0xff,
};


}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_EMEDIATYPE_H_
