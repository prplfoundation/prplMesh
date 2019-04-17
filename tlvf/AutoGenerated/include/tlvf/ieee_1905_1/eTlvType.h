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

#ifndef _TLVF_IEEE_1905_1_ETLVTYPE_H_
#define _TLVF_IEEE_1905_1_ETLVTYPE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

enum class eTlvType : uint16_t {
    TLV_END_OF_MESSAGE = 0x0,
    TLV_AL_MAC_ADDRESS_TYPE = 0x1,
    TLV_MAC_ADDRESS = 0x2,
    TLV_DEVICE_INFORMATION = 0x3,
    TLV_DEVICE_BRIDGING_CAPABILITY = 0x4,
    TLV_NON_1905_NEIGHBOR_DEVICE_LIST = 0x6,
    TLV_1905_NEIGHBOR_DEVICE = 0x7,
    TLV_LINK_METRIC_QUERY = 0x8,
    TLV_TRANSMITTER_LINK_METRIC = 0x9,
    TLV_RECEIVER_LINK_METRIC = 0xa,
    TLV_VENDOR_SPECIFIC = 0xb,
    TLV_LINK_METRIC_RESULT_CODE = 0xc,
    TLV_SEARCHED_ROLE = 0xd,
    TLV_AUTOCONFIG_FREQ_BAND = 0xe,
    TLV_SUPPORTED_ROLE = 0xf,
    TLV_SUPPORTED_FREQ_BAND = 0x10,
    TLV_WSC = 0x11,
    TLV_PUSH_BUTTON_EVENT_NOTIFICATION = 0x12,
    TLV_PUSH_BUTTON_JOIN_NOTIFICATION = 0x13,
};
class eTlvTypeValidate {
public:
    static bool check(uint16_t value) {
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
