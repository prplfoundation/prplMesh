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

#ifndef _TLVF_WFA_MAP_ETLVTYPEMAP_H_
#define _TLVF_WFA_MAP_ETLVTYPEMAP_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace wfa_map {

enum eTlvTypeMap: uint16_t {
    TLV_SEARCHED_SERVICE = 0x81,
    TLV_AP_RADIO_IDENTIFIER = 0x82,
    TLV_AP_OPERATIONAL_BSS = 0x83,
    TLV_ASSOCIATED_CLIENTS = 0x84,
    TLV_AP_RADIO_BASIC_CAPABILITIES = 0x85,
    TLV_AP_HT_CAPABILITIES = 0x86,
    TLV_AP_VHT_CAPABILITIES = 0x87,
    TLV_AP_HE_CAPABILITIES = 0x88,
    TLV_STEERING_POLICY = 0x89,
    TLV_METRIC_REPORTING_POLICY = 0x8a,
    TLV_CHANNEL_PREFERENCE = 0x8b,
    TLV_RADIO_OPERATION_RESTRICTION = 0x8c,
    TLV_TRANSMIT_POWER_LIMIT = 0x8d,
    TLV_CHANNEL_SELECTION_RESPONSE = 0x8e,
    TLV_OPERATING_CHANNEL_REPORT = 0x8f,
    TLV_CLIENT_INFO = 0x90,
    TLV_CLIENT_CAPABILITY_REPORT = 0x91,
    TLV_CLIENT_ASSOCIATION_EVENT = 0x92,
    TLV_AP_METRIC_QUERY = 0x93,
    TLV_AP_METRIC = 0x94,
    TLV_STAMAC_ADDRESS_TYPE = 0x95,
    TLV_ASSOCIATED_STA_LINK_METRICS = 0x96,
    TLV_UNASSOCIATED_STA_LINK_METRICS_QUERY = 0x97,
    TLV_UNASSOCIATED_STA_LINK_METRICS_RESPONSE = 0x98,
    TLV_BEACON_METRICS_QUERY = 0x99,
    TLV_BEACON_METRICS_RESPONSE = 0x9a,
    TLV_STEERING_REQUEST = 0x9b,
    TLV_STEERING_BTM_REPORT = 0x9c,
    TLV_CLIENT_ASSOCIATION_CONTROL_REQUEST = 0x9d,
    TLV_BACKHAUL_STEERING_REQUEST = 0x9e,
    TLV_BACKHAUL_STEERING_RESPONSE = 0x9f,
    TLV_HIGHER_LAYER_DATA = 0xa0,
    TLV_AP_CAPABILITY = 0xa1,
    TLV_ASSOCIATED_STA_TRAFFIC_STATS = 0xa2,
    TLV_ERROR_CODE = 0xa3,
};


}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_ETLVTYPEMAP_H_
