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

#ifndef _TLVF_IEEE_1905_1_ETLVTYPE_H_
#define _TLVF_IEEE_1905_1_ETLVTYPE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

enum class eTlvType : uint16_t {
    TLV_END_OF_MESSAGE                 = 0x0,
    TLV_AL_MAC_ADDRESS_TYPE            = 0x1,
    TLV_MAC_ADDRESS                    = 0x2,
    TLV_DEVICE_INFORMATION             = 0x3,
    TLV_DEVICE_BRIDGING_CAPABILITY     = 0x4,
    TLV_NON_1905_NEIGHBOR_DEVICE_LIST  = 0x6,
    TLV_1905_NEIGHBOR_DEVICE           = 0x7,
    TLV_LINK_METRIC_QUERY              = 0x8,
    TLV_TRANSMITTER_LINK_METRIC        = 0x9,
    TLV_RECEIVER_LINK_METRIC           = 0xa,
    TLV_VENDOR_SPECIFIC                = 0xb,
    TLV_LINK_METRIC_RESULT_CODE        = 0xc,
    TLV_SEARCHED_ROLE                  = 0xd,
    TLV_AUTOCONFIG_FREQ_BAND           = 0xe,
    TLV_SUPPORTED_ROLE                 = 0xf,
    TLV_SUPPORTED_FREQ_BAND            = 0x10,
    TLV_WSC                            = 0x11,
    TLV_PUSH_BUTTON_EVENT_NOTIFICATION = 0x12,
    TLV_PUSH_BUTTON_JOIN_NOTIFICATION  = 0x13,
};
class eTlvTypeValidate {
public:
    static bool check(uint16_t value)
    {
        bool ret = false;
        switch (value) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xa:
        case 0xb:
        case 0xc:
        case 0xd:
        case 0xe:
        case 0xf:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            ret = true;
            break;
        default:
            ret = false;
            break;
        }
        return ret;
    }
};

}; // close namespace: ieee1905_1

#endif //_TLVF/IEEE_1905_1_ETLVTYPE_H_
