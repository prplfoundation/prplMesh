/*
* INTEL CONFIDENTIAL
* Copyright 2016 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#ifndef __FAPI_INTERFACE_DEFS_H__
#define __FAPI_INTERFACE_DEFS_H__

// NOTE: Supported event names should all be part of hostapd now. (WPA_EVENT_x == "CTRL-EVENT-x")

// FAPI object names
#define OBJECT_ACCESSPOINT_ASSOCIATEDDEVICE "Device.WiFi.AccessPoint.AssociatedDevice"
#define OBJECT_ACCESSPOINT_VENDOR_ASSOCIATEDDEVICE "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice"
#define OBJECT_ENDPOINT "Device.WiFi.EndPoint"
#define OBJECT_ENDPOINT_PROFILE "Device.WiFi.EndPoint.Profile"
#define OBJECT_ENDPOINT_PROFILE_SECURITY "Device.WiFi.EndPoint.Profile.Security"
#define OBJECT_SSID "Device.WiFi.SSID"

// FAPI parameters and values
#define PARAM_3_ADDRESS_MODE "3_ADDRESS_MODE"
#define PARAM_4_ADDRESS_MODE "4_ADDRESS_MODE"
#define PARAM_BSSID "BSSID"
#define PARAM_DIALOG_TOKEN "dialog_token"
#define PARAM_MAC_ADDRESS "MACAddress"
#define PARAM_MODE_ENABLED "ModeEnabled"
#define PARAM_NAME "Name"
#define PARAM_NON_PREF_CHAN "non_pref_chan"
#define PARAM_SIGNAL_STRENGTH "SignalStrength"
#define PARAM_SSID "SSID"
#define PARAM_VENDOR_ADDRESS "X_LANTIQ_COM_Vendor_address"
#define PARAM_VENDOR_BSSID "X_LANTIQ_COM_Vendor_BSSID"
#define PARAM_VENDOR_BSS_LOAD "X_LANTIQ_COM_Vendor_BssLoad" // v.3.8.20 removed space
#define PARAM_VENDOR_BYTES_RECEIVED "X_LANTIQ_COM_Vendor_BytesReceived"
#define PARAM_VENDOR_BYTES_SENT "X_LANTIQ_COM_Vendor_BytesSent"
#define PARAM_VENDOR_CF1 "X_LANTIQ_COM_Vendor_Cf1"
#define PARAM_VENDOR_CHANNEL "X_LANTIQ_COM_Vendor_Channel"
#define PARAM_VENDOR_DFS "X_LANTIQ_COM_Vendor_DfsChan" // v.3.8.20 added "Chan"
#define PARAM_VENDOR_DISCARD_PACKETS_RECEIVED "X_LANTIQ_COM_Vendor_DiscardPacketsReceived"
#define PARAM_VENDOR_DISCARD_PACKETS_SENT "X_LANTIQ_COM_Vendor_DiscardPacketsSent"
#define PARAM_VENDOR_ERRORS_RECEIVED "X_LANTIQ_COM_Vendor_ErrorsReceived"
#define PARAM_VENDOR_ERRORS_SENT "X_LANTIQ_COM_Vendor_ErrorsSent"
#define PARAM_VENDOR_FCS_ERROR_COUNT "X_LANTIQ_COM_Vendor_FCSErrorCount"
#define PARAM_VENDOR_FREQ "X_LANTIQ_COM_Vendor_freq"
#define PARAM_VENDOR_FREQUENCY "X_LANTIQ_COM_Vendor_Frequency"
#define PARAM_VENDOR_GROUP_CIPHER "X_LANTIQ_COM_Vendor_GroupCipher"
#define PARAM_VENDOR_ID_STR "X_LANTIQ_COM_Vendor_IdStr"
#define PARAM_VENDOR_ID "X_LANTIQ_COM_Vendor_id"
#define PARAM_VENDOR_INVALID_MAC_COUNT "X_LANTIQ_COM_Vendor_InvalidMACCount"
#define PARAM_VENDOR_KEY_MGMT "X_LANTIQ_COM_Vendor_KeyMgmt"
#define PARAM_VENDOR_LOCALLY_GENERATED "X_LANTIQ_COM_Vendor_LocallyGenerated"
#define PARAM_VENDOR_MODE "X_LANTIQ_COM_Vendor_mode"
#define PARAM_VENDOR_NAME "X_LANTIQ_COM_Vendor_Name"
#define PARAM_VENDOR_NETWORK_ID "X_LANTIQ_COM_Vendor_NetworkId"
#define PARAM_VENDOR_NOISE "X_LANTIQ_COM_Vendor_Noise"
#define PARAM_VENDOR_OPERATING_CHANNEL_BANDWIDTH "X_LANTIQ_COM_Vendor_OperatingChannelBandwidt" // (intentional, that's how it appears in FAPI)
#define PARAM_VENDOR_PACKETS_OTHER_RECEIVED "X_LANTIQ_COM_Vendor_PacketsOtherReceived"
#define PARAM_VENDOR_PACKETS_RECEIVED "X_LANTIQ_COM_Vendor_PacketsReceived"
#define PARAM_VENDOR_PACKETS_SENT "X_LANTIQ_COM_Vendor_PacketsSent"
#define PARAM_VENDOR_PAIRWISE_CIPHER "X_LANTIQ_COM_Vendor_PairwiseCipher"
#define PARAM_VENDOR_PLCP_ERROR_COUNT "X_LANTIQ_COM_Vendor_PLCPErrorCount"
#define PARAM_VENDOR_REASON "X_LANTIQ_COM_Vendor_Reason"
#define PARAM_VENDOR_ROUTING_MODE "X_LANTIQ_COM_Vendor_RoutingMode"
#define PARAM_VENDOR_RX_ANTENNAS "X_LANTIQ_COM_Vendor_RxAntennas"
#define PARAM_VENDOR_SIGNAL_LEVEL "X_LANTIQ_COM_Vendor_SignalLevel"
#define PARAM_VENDOR_SSID "X_LANTIQ_COM_Vendor_SSID"
#define PARAM_VENDOR_TX_ANTENNAS "X_LANTIQ_COM_Vendor_TxAntennas"
#define PARAM_VENDOR_TX_ENABLED "X_LANTIQ_COM_Vendor_TxEnabled"
#define PARAM_VENDOR_TX_POWER "X_LANTIQ_COM_Vendor_TxPower"
#define PARAM_VENDOR_UUID "X_LANTIQ_COM_Vendor_uuid"
#define PARAM_VENDOR_WPA_STATE "X_LANTIQ_COM_Vendor_WpaState"
#define PARAM_VENDOR_WPA_SUPPLICANT_ENABLED "X_LANTIQ_COM_Vendor_WpaSupplicantEnabled"

#endif // __FAPI_INTERFACE_DEFS_H__