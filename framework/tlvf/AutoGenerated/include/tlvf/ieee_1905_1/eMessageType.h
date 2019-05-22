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

#ifndef _TLVF_IEEE_1905_1_EMESSAGETYPE_H_
#define _TLVF_IEEE_1905_1_EMESSAGETYPE_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace ieee1905_1 {

enum class eMessageType : uint16_t {
    TOPOLOGY_DISCOVERY_MESSAGE = 0x0,
    TOPOLOGY_NOTIFICATION_MESSAGE = 0x1,
    TOPOLOGY_QUERY_MESSAGE = 0x2,
    TOPOLOGY_RESPONSE_MESSAGE = 0x3,
    VENDOR_SPECIFIC_MESSAGE = 0x4,
    LINK_METRIC_QUERY_MESSAGE = 0x5,
    LINK_METRIC_RESPONSE_MESSAGE = 0x6,
    AP_AUTOCONFIGURATION_SEARCH_MESSAGE = 0x7,
    AP_AUTOCONFIGURATION_RESPONSE_MESSAGE = 0x8,
    AP_AUTOCONFIGURATION_WIFI_SIMPLE_CONFIGURATION_MESSAGE = 0x9,
    AP_AUTOCONFIGURATION_RENEW_MESSAGE = 0xa,
    PUSH_BUTTON_EVENT_NOTIFICATION_MESSAGE = 0xb,
    PUSH_BUTTON_JOIN_NOTIFICATION_MESSAGE = 0xc,
    HIGHER_LAYER_QUERY_MESSAGE = 0xd,
    HIGHER_LAYER_RESPONSE_MESSAGE = 0xe,
    INTERFACE_POWER_CHANGE_REQUEST_MESSAGE = 0xf,
    INTERFACE_POWER_CHANGE_RESPONSE_MESSAGE = 0x10,
    GENERIC_PHY_QUERY_MESSAGE = 0x11,
    GENERIC_PHY_RESPONSE_MESSAGE = 0x12,
    ACK_MESSAGE = 0x8000,
    AP_CAPABILITY_QUERY_MESSAGE = 0x8001,
    AP_CAPABILITY_REPORT_MESSAGE = 0x8002,
    MULTI_AP_POLICY_CONFIG_REQUEST_MESSAGE = 0x8003,
    CHANNEL_PREFERENCE_QUERY_MESSAGE = 0x8004,
    CHANNEL_PREFERENCE_REPORT_MESSAGE = 0x8005,
    CHANNEL_SELECTION_REQUEST_MESSAGE = 0x8006,
    CHANNEL_SELECTION_RESPONSE_MESSAGE = 0x8007,
    OPERATING_CHANNEL_REPORT_MESSAGE = 0x8008,
    CLIENT_CAPABILITY_QUERY_MESSAGE = 0x8009,
    CLIENT_CAPABILITY_REPORT_MESSAGE = 0x800a,
    AP_METRICS_QUERY_MESSAGE = 0x800b,
    AP_METRICS_RESPONSE_MESSAGE = 0x800c,
    ASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE = 0x800d,
    ASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE = 0x800e,
    UNASSOCIATED_STA_LINK_METRICS_QUERY_MESSAGE = 0x800f,
    UNASSOCIATED_STA_LINK_METRICS_RESPONSE_MESSAGE = 0x8010,
    BEACON_METRICS_QUERY_MESSAGE = 0x8011,
    BEACON_METRICS_RESPONSE_MESSAGE = 0x8012,
    COMBINED_INFRASTRUCTURE_METRICS_MESSAGE = 0x8013,
    CLIENT_STEERING_REQUEST_MESSAGE = 0x8014,
    CLIENT_STEERING_BTM_REPORT_MESSAGE = 0x8015,
    CLIENT_ASSOCIATION_CONTROL_REQUEST_MESSAGE = 0x8016,
    STEERING_COMPLETED_MESSAGE = 0x8017,
    HIGHER_LAYER_DATA_MESSAGE = 0x8018,
    BACKHAUL_STEERING_REQUEST_MESSAGE = 0x8019,
    BACKHAUL_STEERING_RESPONSE_MESSAGE = 0x801a,
};
class eMessageTypeValidate {
public:
    static bool check(uint16_t value) {
        bool ret = false;
        switch (value) {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
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
        case 0x8000:
        case 0x8001:
        case 0x8002:
        case 0x8003:
        case 0x8004:
        case 0x8005:
        case 0x8006:
        case 0x8007:
        case 0x8008:
        case 0x8009:
        case 0x800a:
        case 0x800b:
        case 0x800c:
        case 0x800d:
        case 0x800e:
        case 0x800f:
        case 0x8010:
        case 0x8011:
        case 0x8012:
        case 0x8013:
        case 0x8014:
        case 0x8015:
        case 0x8016:
        case 0x8017:
        case 0x8018:
        case 0x8019:
        case 0x801a:
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

#endif //_TLVF/IEEE_1905_1_EMESSAGETYPE_H_
