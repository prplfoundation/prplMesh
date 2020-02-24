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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_ENUMS_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_ENUMS_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>

namespace beerocks_message {

enum eStructsConsts {
    VERSION_LENGTH = 0x10,
    NODE_NAME_LENGTH = 0x20,
    IFACE_NAME_LENGTH = 0x21,
    SUPPORTED_CHANNELS_LENGTH = 0x40,
    WIFI_DRIVER_VER_LENGTH = 0x21,
    WIFI_SSID_MAX_LENGTH = 0x21,
    WIFI_PASS_MAX_LENGTH = 0x41,
    USER_PASS_LEN = 0x41,
    DEV_INFO_STR_MAX_LEN = 0x20,
    RESTRICTED_CHANNEL_LENGTH = 0x28,
    WIFI_SECURITY_TYPE_MAX_LENGTH = 0x20,
    BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH = 0x10,
    PLATFORM_ERROR_DATA_SIZE = 0x100,
    CHANNEL_SCAN_LIST_LENGTH = 0x8,
};

enum eWiFiSec: uint8_t {
    eWiFiSec_None = 0x0,
    eWiFiSec_WEP64 = 0x1,
    eWiFiSec_WEP128 = 0x2,
    eWiFiSec_WPA_PSK = 0x3,
    eWiFiSec_WPA2_PSK = 0x4,
    eWiFiSec_WPA_WPA2_PSK = 0x5,
};

enum eDHCPOp: uint8_t {
    eDHCPOp_Add = 0x0,
    eDHCPOp_Del = 0x1,
    eDHCPOp_Old = 0x2,
};

enum eDisconnectSource: uint8_t {
    eDisconnect_Source_Unknown = 0x0,
    eDisconnect_Source_Local = 0x1,
    eDisconnect_Source_Remote = 0x2,
};

enum eDisconnectType: uint8_t {
    eDisconnect_Type_Unknown = 0x0,
    eDisconnect_Type_Disassoc = 0x1,
    eDisconnect_Type_Deauth = 0x2,
};

enum eSteeringSnrChange: uint8_t {
    eWifi_Steering_Snr_Unchanged = 0x0,
    eWifi_Steering_Snr_Higher = 0x1,
    eWifi_Steering_Snr_Lower = 0x2,
};

enum eSteeringEventType: uint8_t {
    eWifi_Steering_Event_Probe_Req = 0x1,
    eWifi_Steering_Event_Client_Connect = 0x2,
    eWifi_Steering_Event_Client_Disconnect = 0x3,
    eWifi_Steering_Event_Client_Activity = 0x4,
    eWifi_Steering_Event_Snr_Xing = 0x5,
    eWifi_Steering_Event_Snr = 0x6,
    eWifi_Steering_Event_Auth_Fail = 0x7,
};

enum eChannelScanResultMode: uint8_t {
    eMode_NA = 0x0,
    eMode_AdHoc = 0x1,
    eMode_Infrastructure = 0x2,
};

enum eChannelScanResultEncryptionMode: uint8_t {
    eEncryption_Mode_NA = 0x0,
    eEncryption_Mode_AES = 0x1,
    eEncryption_Mode_TKIP = 0x2,
};

enum eChannelScanResultSecurityMode: uint8_t {
    eSecurity_Mode_None = 0x0,
    eSecurity_Mode_WEP = 0x1,
    eSecurity_Mode_WPA = 0x2,
    eSecurity_Mode_WPA2 = 0x3,
};

enum eChannelScanResultOperatingFrequencyBand: uint8_t {
    eOperating_Freq_Band_NA = 0x0,
    eOperating_Freq_Band_2_4GHz = 0x1,
    eOperating_Freq_Band_5GHz = 0x2,
};

enum eChannelScanResultStandards: uint8_t {
    eStandard_NA = 0x0,
    eStandard_802_11a = 0x1,
    eStandard_802_11b = 0x2,
    eStandard_802_11g = 0x3,
    eStandard_802_11n = 0x4,
    eStandard_802_11ac = 0x5,
};

enum eChannelScanResultChannelBandwidth: uint8_t {
    eChannel_Bandwidth_NA = 0x0,
    eChannel_Bandwidth_20MHz = 0x1,
    eChannel_Bandwidth_40MHz = 0x2,
    eChannel_Bandwidth_80MHz = 0x3,
    eChannel_Bandwidth_160MHz = 0x4,
    eChannel_Bandwidth_80_80 = 0x5,
};


}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_ENUMS_H_
