///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
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

enum class eTlvTypeMap : uint8_t {
    TLV_SUPPORTED_SERVICE = 0x80,
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
class eTlvTypeMapValidate {
public:
    static bool check(uint8_t value) {
        bool ret = false;
        switch (value) {
        case 0x80:
        case 0x81:
        case 0x82:
        case 0x83:
        case 0x84:
        case 0x85:
        case 0x86:
        case 0x87:
        case 0x88:
        case 0x89:
        case 0x8a:
        case 0x8b:
        case 0x8c:
        case 0x8d:
        case 0x8e:
        case 0x8f:
        case 0x90:
        case 0x91:
        case 0x92:
        case 0x93:
        case 0x94:
        case 0x95:
        case 0x96:
        case 0x97:
        case 0x98:
        case 0x99:
        case 0x9a:
        case 0x9b:
        case 0x9c:
        case 0x9d:
        case 0x9e:
        case 0x9f:
        case 0xa0:
        case 0xa1:
        case 0xa2:
        case 0xa3:
                ret = true;
                break;
            default:
                ret = false;
                break;
        }
        return ret;
    }
};


}; // close namespace: wfa_map

#endif //_TLVF/WFA_MAP_ETLVTYPEMAP_H_
