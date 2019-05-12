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

#ifndef _TLVF_WFA_MAP_ETLVTYPEMAP_H_
#define _TLVF_WFA_MAP_ETLVTYPEMAP_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace wfa_map {

enum eTlvTypeMap : uint16_t {
    TLV_SUPPORTED_SERVICE                      = 0x80,
    TLV_SEARCHED_SERVICE                       = 0x81,
    TLV_AP_RADIO_IDENTIFIER                    = 0x82,
    TLV_AP_OPERATIONAL_BSS                     = 0x83,
    TLV_ASSOCIATED_CLIENTS                     = 0x84,
    TLV_AP_RADIO_BASIC_CAPABILITIES            = 0x85,
    TLV_AP_HT_CAPABILITIES                     = 0x86,
    TLV_AP_VHT_CAPABILITIES                    = 0x87,
    TLV_AP_HE_CAPABILITIES                     = 0x88,
    TLV_STEERING_POLICY                        = 0x89,
    TLV_METRIC_REPORTING_POLICY                = 0x8a,
    TLV_CHANNEL_PREFERENCE                     = 0x8b,
    TLV_RADIO_OPERATION_RESTRICTION            = 0x8c,
    TLV_TRANSMIT_POWER_LIMIT                   = 0x8d,
    TLV_CHANNEL_SELECTION_RESPONSE             = 0x8e,
    TLV_OPERATING_CHANNEL_REPORT               = 0x8f,
    TLV_CLIENT_INFO                            = 0x90,
    TLV_CLIENT_CAPABILITY_REPORT               = 0x91,
    TLV_CLIENT_ASSOCIATION_EVENT               = 0x92,
    TLV_AP_METRIC_QUERY                        = 0x93,
    TLV_AP_METRIC                              = 0x94,
    TLV_STAMAC_ADDRESS_TYPE                    = 0x95,
    TLV_ASSOCIATED_STA_LINK_METRICS            = 0x96,
    TLV_UNASSOCIATED_STA_LINK_METRICS_QUERY    = 0x97,
    TLV_UNASSOCIATED_STA_LINK_METRICS_RESPONSE = 0x98,
    TLV_BEACON_METRICS_QUERY                   = 0x99,
    TLV_BEACON_METRICS_RESPONSE                = 0x9a,
    TLV_STEERING_REQUEST                       = 0x9b,
    TLV_STEERING_BTM_REPORT                    = 0x9c,
    TLV_CLIENT_ASSOCIATION_CONTROL_REQUEST     = 0x9d,
    TLV_BACKHAUL_STEERING_REQUEST              = 0x9e,
    TLV_BACKHAUL_STEERING_RESPONSE             = 0x9f,
    TLV_HIGHER_LAYER_DATA                      = 0xa0,
    TLV_AP_CAPABILITY                          = 0xa1,
    TLV_ASSOCIATED_STA_TRAFFIC_STATS           = 0xa2,
    TLV_ERROR_CODE                             = 0xa3,
};

}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_ETLVTYPEMAP_H_
