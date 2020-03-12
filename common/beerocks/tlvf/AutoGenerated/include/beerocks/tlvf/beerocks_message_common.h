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

#ifndef _BEEROCKS_TLVF_BEEROCKS_MESSAGE_COMMON_H_
#define _BEEROCKS_TLVF_BEEROCKS_MESSAGE_COMMON_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include "beerocks/tlvf/beerocks_message_action.h"
#include "bcl/beerocks_message_structs.h"

namespace beerocks_message {

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

typedef struct sVapInfo {
    sMacAddr mac;
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    uint8_t backhaul_vap;
    void struct_swap(){
        mac.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
    }
} __attribute__((packed)) sVapInfo;

typedef struct sSonConfig {
    uint8_t monitor_total_ch_load_notification_lo_th_percent;
    uint8_t monitor_total_ch_load_notification_hi_th_percent;
    uint8_t monitor_total_ch_load_notification_delta_th_percent;
    uint8_t monitor_min_active_clients;
    uint8_t monitor_active_client_th;
    uint8_t monitor_client_load_notification_delta_th_percent;
    int8_t monitor_rx_rssi_notification_threshold_dbm;
    uint8_t monitor_rx_rssi_notification_delta_db;
    uint32_t monitor_ap_idle_threshold_B;
    uint32_t monitor_ap_active_threshold_B;
    uint16_t monitor_ap_idle_stable_time_sec;
    uint8_t monitor_disable_initiative_arp;
    uint8_t slave_keep_alive_retries;
    uint8_t ire_rssi_report_rate_sec;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&monitor_ap_idle_threshold_B));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&monitor_ap_active_threshold_B));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&monitor_ap_idle_stable_time_sec));
    }
    void struct_init(){
    }
} __attribute__((packed)) sSonConfig;

typedef struct sPlatformSettings {
    char front_ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    char front_pass[beerocks::message::WIFI_PASS_MAX_LENGTH];
    char front_security_type[beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH];
    char back_ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    char back_pass[beerocks::message::WIFI_PASS_MAX_LENGTH];
    char back_security_type[beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH];
    uint8_t onboarding;
    uint8_t local_master;
    uint8_t local_gw;
    uint8_t operating_mode;
    uint8_t management_mode;
    uint8_t mem_only_psk;
    uint8_t certification_mode;
    uint8_t stop_on_failure_attempts;
    uint8_t client_band_steering_enabled;
    uint8_t client_optimal_path_roaming_enabled;
    uint8_t dfs_reentry_enabled;
    uint8_t client_optimal_path_roaming_prefer_signal_strength_enabled;
    uint8_t client_11k_roaming_enabled;
    uint8_t load_balancing_enabled;
    uint8_t service_fairness_enabled;
    uint8_t rdkb_extensions_enabled;
    uint8_t backhaul_max_vaps;
    uint8_t backhaul_network_enabled;
    uint8_t backhaul_preferred_radio_band;
    sMacAddr backhaul_vaps_bssid[12];
    void struct_swap(){
        for (size_t i = 0; i < 12; i++){
            (backhaul_vaps_bssid[i]).struct_swap();
        }
    }
    void struct_init(){
            for (size_t i = 0; i < 12; i++) {
                (backhaul_vaps_bssid[i]).struct_init();
            }
    }
} __attribute__((packed)) sPlatformSettings;

typedef struct sWlanSettings {
    uint8_t band_enabled;
    uint8_t channel;
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sWlanSettings;

typedef struct sApSetRestrictedFailsafe {
    uint8_t failsafe_channel;
    uint8_t failsafe_channel_bandwidth;
    uint8_t restricted_channels[beerocks::message::RESTRICTED_CHANNEL_LENGTH];
    uint16_t vht_center_frequency;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&vht_center_frequency));
    }
    void struct_init(){
    }
} __attribute__((packed)) sApSetRestrictedFailsafe;

typedef struct sApChannelSwitch {
    uint8_t channel;
    uint8_t bandwidth;
    int8_t channel_ext_above_primary;
    uint8_t switch_reason;
    uint8_t is_dfs_channel;
    uint16_t vht_center_frequency;
    int8_t tx_power;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&vht_center_frequency));
    }
    void struct_init(){
    }
} __attribute__((packed)) sApChannelSwitch;

typedef struct sDfsCacCompleted {
    uint32_t timeout;
    uint32_t frequency;
    uint8_t success;
    uint8_t channel;
    uint8_t bandwidth;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&timeout));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&frequency));
    }
    void struct_init(){
    }
} __attribute__((packed)) sDfsCacCompleted;

typedef struct sDfsChannelAvailable {
    uint32_t frequency;
    uint8_t channel;
    uint8_t bandwidth;
    uint16_t vht_center_frequency;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&frequency));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&vht_center_frequency));
    }
    void struct_init(){
    }
} __attribute__((packed)) sDfsChannelAvailable;

typedef struct sClientAssociationParams {
    sMacAddr mac;
    sMacAddr bssid;
    beerocks::message::sRadioCapabilities capabilities;
    int8_t vap_id;
    void struct_swap(){
        mac.struct_swap();
        bssid.struct_swap();
        capabilities.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
        bssid.struct_init();
        capabilities.struct_init();
    }
} __attribute__((packed)) sClientAssociationParams;

typedef struct sClientDisconnectionParams {
    sMacAddr mac;
    sMacAddr bssid;
    int8_t vap_id;
    uint8_t reason;
    uint8_t source;
    uint8_t type;
    void struct_swap(){
        mac.struct_swap();
        bssid.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sClientDisconnectionParams;

typedef struct sClientMonitoringParams {
    sMacAddr mac;
    sMacAddr bridge_4addr_mac;
    beerocks::net::sIpv4Addr ipv4;
    uint8_t channel;
    int8_t vap_id;
    uint8_t is_ire;
    void struct_swap(){
        mac.struct_swap();
        bridge_4addr_mac.struct_swap();
        ipv4.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
        bridge_4addr_mac.struct_init();
        ipv4.struct_init();
    }
} __attribute__((packed)) sClientMonitoringParams;

typedef struct sConfigVapInfo {
    uint8_t type;
    uint8_t auth;
    uint8_t enc;
    uint8_t al_mac[beerocks::net::MAC_ADDR_LEN];
    uint8_t ruid[beerocks::net::MAC_ADDR_LEN];
    uint8_t bssid[beerocks::net::MAC_ADDR_LEN];
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    char key[beerocks::message::WIFI_PASS_MAX_LENGTH];
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sConfigVapInfo;

typedef struct sStaStatsParams {
    sMacAddr mac;
    uint32_t rx_packets;
    uint32_t tx_packets;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
    uint32_t retrans_count;
    uint16_t tx_phy_rate_100kb;
    uint16_t rx_phy_rate_100kb;
    uint8_t tx_load_percent;
    uint8_t rx_load_percent;
    uint16_t stats_delta_ms;
    int8_t rx_rssi;
    void struct_swap(){
        mac.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&rx_packets));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&tx_packets));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&tx_bytes));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&rx_bytes));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&retrans_count));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&tx_phy_rate_100kb));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&rx_phy_rate_100kb));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&stats_delta_ms));
    }
    void struct_init(){
        mac.struct_init();
    }
} __attribute__((packed)) sStaStatsParams;

typedef struct sApStatsParams {
    uint32_t rx_packets;
    uint32_t tx_packets;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
    uint32_t errors_sent;
    uint32_t errors_received;
    uint32_t retrans_count;
    uint8_t client_count;
    uint8_t active_client_count;
    uint8_t channel_load_percent;
    uint8_t client_tx_load_percent;
    uint8_t client_rx_load_percent;
    int8_t noise;
    uint16_t stats_delta_ms;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&rx_packets));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&tx_packets));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&tx_bytes));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&rx_bytes));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&errors_sent));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&errors_received));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&retrans_count));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&stats_delta_ms));
    }
    void struct_init(){
    }
} __attribute__((packed)) sApStatsParams;

typedef struct sApLoadNotificationParams {
    uint16_t stats_delta_ms;
    uint8_t client_count;
    uint8_t active_client_count;
    uint8_t channel_load_percent;
    uint8_t client_tx_load_percent;
    uint8_t client_rx_load_percent;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&stats_delta_ms));
    }
    void struct_init(){
    }
} __attribute__((packed)) sApLoadNotificationParams;

typedef struct sApActivityNotificationParams {
    uint8_t ap_activity_mode;
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sApActivityNotificationParams;

typedef struct sNodeRssiMeasurementRequest {
    sMacAddr mac;
    beerocks::net::sIpv4Addr ipv4;
    uint8_t channel;
    uint8_t cross;
    uint8_t bandwidth;
    uint8_t mon_ping_burst_pkt_num;
    uint16_t vht_center_frequency;
    uint8_t measurement_delay;
    void struct_swap(){
        mac.struct_swap();
        ipv4.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&vht_center_frequency));
    }
    void struct_init(){
        mac.struct_init();
        ipv4.struct_init();
    }
} __attribute__((packed)) sNodeRssiMeasurementRequest;

typedef struct sNodeRssiMeasurement {
    beerocks::net::sScanResult result;
    uint16_t rx_phy_rate_100kb;
    uint16_t tx_phy_rate_100kb;
    int8_t rx_rssi;
    uint8_t rx_snr;
    int8_t rx_packets;
    uint8_t src_module;
    int8_t vap_id;
    void struct_swap(){
        result.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&rx_phy_rate_100kb));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&tx_phy_rate_100kb));
    }
    void struct_init(){
        result.struct_init();
    }
} __attribute__((packed)) sNodeRssiMeasurement;

typedef struct sNodeHostap {
    char iface_name[beerocks::message::IFACE_NAME_LENGTH];
    uint8_t iface_type;
    sMacAddr iface_mac;
    uint8_t iface_is_5ghz;
    uint8_t ant_num;
    uint8_t ant_gain;
    uint8_t conducted_power;
    char driver_version[beerocks::message::WIFI_DRIVER_VER_LENGTH];
    beerocks::message::sWifiChannel supported_channels[beerocks::message::SUPPORTED_CHANNELS_LENGTH];
    void struct_swap(){
        iface_mac.struct_swap();
        for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++){
            (supported_channels[i]).struct_swap();
        }
    }
    void struct_init(){
        iface_mac.struct_init();
            for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++) {
                (supported_channels[i]).struct_init();
            }
    }
} __attribute__((packed)) sNodeHostap;

typedef struct sVapsList {
    sVapInfo vaps[beerocks::IFACE_TOTAL_VAPS];
    void struct_swap(){
        for (size_t i = 0; i < beerocks::IFACE_TOTAL_VAPS; i++){
            (vaps[i]).struct_swap();
        }
    }
    void struct_init(){
            for (size_t i = 0; i < beerocks::IFACE_TOTAL_VAPS; i++) {
                (vaps[i]).struct_init();
            }
    }
} __attribute__((packed)) sVapsList;

typedef struct sArpMonitorData {
    sMacAddr mac;
    beerocks::net::sIpv4Addr ipv4;
    uint32_t iface_idx;
    uint8_t state;
    uint8_t source;
    uint8_t type;
    void struct_swap(){
        mac.struct_swap();
        ipv4.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&iface_idx));
    }
    void struct_init(){
        mac.struct_init();
        ipv4.struct_init();
    }
} __attribute__((packed)) sArpMonitorData;

typedef struct sArpQuery {
    sMacAddr mac;
    beerocks::net::sIpv4Addr ipv4;
    void struct_swap(){
        mac.struct_swap();
        ipv4.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
        ipv4.struct_init();
    }
} __attribute__((packed)) sArpQuery;

typedef struct sNodeBssSteerTarget {
    sMacAddr bssid;
    uint8_t operating_class;
    uint8_t channel;
    void struct_swap(){
        bssid.struct_swap();
    }
    void struct_init(){
        bssid.struct_init();
    }
} __attribute__((packed)) sNodeBssSteerTarget;

typedef struct sNodeBssSteerRequest {
    sMacAddr mac;
    sMacAddr cur_bssid;
    uint16_t disassoc_timer_ms;
    uint8_t disassoc_imminent;
    sNodeBssSteerTarget target;
    void struct_swap(){
        mac.struct_swap();
        cur_bssid.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&disassoc_timer_ms));
        target.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
        cur_bssid.struct_init();
        target.struct_init();
    }
} __attribute__((packed)) sNodeBssSteerRequest;

typedef struct sNodeBssSteerResponse {
    sMacAddr mac;
    sMacAddr source_bssid;
    sMacAddr target_bssid;
    uint8_t status_code;
    void struct_swap(){
        mac.struct_swap();
        source_bssid.struct_swap();
        target_bssid.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
        source_bssid.struct_init();
        target_bssid.struct_init();
    }
} __attribute__((packed)) sNodeBssSteerResponse;

typedef struct sNeighborSetParams11k {
    sMacAddr bssid;
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    uint8_t op_class;
    uint8_t channel;
    uint8_t phy_type;
    uint16_t ap_reachabilty;
    uint8_t security;
    uint8_t key_scope;
    uint8_t spectrum_managment;
    uint8_t qos;
    uint8_t apsd;
    uint8_t radio_measurement;
    uint8_t delayed_block_ack;
    uint8_t immediate_block_ack;
    uint8_t roaming_domain;
    uint8_t high_throughput;
    uint8_t very_high_throughput;
    uint8_t ftm;
    int8_t vap_id;
    void struct_swap(){
        bssid.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&ap_reachabilty));
    }
    void struct_init(){
        bssid.struct_init();
    }
} __attribute__((packed)) sNeighborSetParams11k;

typedef struct sNeighborRemoveParams11k {
    sMacAddr bssid;
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    int8_t vap_id;
    void struct_swap(){
        bssid.struct_swap();
    }
    void struct_init(){
        bssid.struct_init();
    }
} __attribute__((packed)) sNeighborRemoveParams11k;

typedef struct sStaChannelLoadRequest11k {
    uint8_t channel;
    uint8_t op_class;
    uint16_t repeats;
    uint16_t rand_ival;
    uint16_t duration;
    sMacAddr sta_mac;
    uint8_t parallel;
    uint8_t enable;
    uint8_t request;
    uint8_t report;
    uint8_t mandatory_duration;
    uint8_t use_optional_ch_load_rep;
    uint8_t ch_load_rep_first;
    uint8_t ch_load_rep_second;
    uint8_t use_optional_wide_band_ch_switch;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&repeats));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&rand_ival));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&duration));
        sta_mac.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_width));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_0));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_1));
    }
    void struct_init(){
        sta_mac.struct_init();
    }
} __attribute__((packed)) sStaChannelLoadRequest11k;

typedef struct sStaChannelLoadResponse11k {
    uint8_t channel;
    uint8_t channel_load;
    uint8_t op_class;
    uint8_t rep_mode;
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint16_t duration;
    uint64_t start_time;
    sMacAddr sta_mac;
    uint8_t use_optional_wide_band_ch_switch;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&duration));
        tlvf_swap(64, reinterpret_cast<uint8_t*>(&start_time));
        sta_mac.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_width));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_0));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_1));
    }
    void struct_init(){
        sta_mac.struct_init();
    }
} __attribute__((packed)) sStaChannelLoadResponse11k;

typedef struct sBeaconRequest11k {
    uint8_t measurement_mode;
    uint8_t channel;
    int16_t op_class;
    uint16_t repeats;
    uint16_t rand_ival;
    uint16_t duration;
    sMacAddr sta_mac;
    sMacAddr bssid;
    uint8_t parallel;
    uint8_t enable;
    uint8_t request;
    uint8_t report;
    uint8_t mandatory_duration;
    uint8_t expected_reports_count;
    uint8_t use_optional_ssid;
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    uint8_t use_optional_ap_ch_report;
    uint8_t ap_ch_report[237];
    uint8_t use_optional_req_elements;
    uint8_t req_elements[13];
    uint8_t use_optional_wide_band_ch_switch;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&op_class));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&repeats));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&rand_ival));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&duration));
        sta_mac.struct_swap();
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_width));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_0));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_1));
    }
    void struct_init(){
        sta_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sBeaconRequest11k;

typedef struct sBeaconResponse11k {
    uint8_t channel;
    uint8_t op_class;
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint8_t rep_mode;
    uint8_t phy_type;
    uint8_t frame_type;
    int8_t rcpi;
    uint8_t rsni;
    uint8_t ant_id;
    uint16_t duration;
    uint32_t parent_tsf;
    uint64_t start_time;
    sMacAddr sta_mac;
    sMacAddr bssid;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    uint8_t use_optional_wide_band_ch_switch;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&duration));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&parent_tsf));
        tlvf_swap(64, reinterpret_cast<uint8_t*>(&start_time));
        sta_mac.struct_swap();
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_width));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_0));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&new_ch_center_freq_seg_1));
    }
    void struct_init(){
        sta_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sBeaconResponse11k;

typedef struct sStatisticsRequest11k {
    sMacAddr sta_mac;
    sMacAddr peer_mac_addr;
    uint8_t group_identity;
    uint8_t parallel;
    uint8_t enable;
    uint8_t request;
    uint8_t report;
    uint8_t mandatory_duration;
    uint16_t repeats;
    uint16_t rand_ival;
    uint16_t duration;
    uint8_t use_optional_trig_rep_sta_counters;
    uint8_t measurement_count_1;
    uint8_t trigger_timeout_1;
    uint8_t sta_counter_trigger_condition;
    uint8_t dot11FailedCountThreshold;
    uint8_t dot11FCSErrorCountThreshold;
    uint8_t dot11MultipleRetryCountThreshold;
    uint8_t dot11FrameDuplicateCountThreshold;
    uint8_t dot11RTSFailureCountThreshold;
    uint8_t dot11AckFailureCountThreshold;
    uint8_t dot11RetryCountThreshold;
    uint8_t use_optional_trig_rep_qos_sta_counters;
    uint8_t measurement_count_2;
    uint8_t trigger_timeout_2;
    uint8_t qos_sta_counter_trigger_condition;
    uint8_t dot11QoSFailedCountThreshold;
    uint8_t dot11QoSRetryCountThreshold;
    uint8_t dot11QoSMultipleRetryCountThreshold;
    uint8_t dot11QoSFrameDuplicateCountThreshold;
    uint8_t dot11QoSRTSCountFailureThreshold;
    uint8_t dot11QoSAckFailureCountThreshold;
    uint8_t dot11QoSDiscardedCountThreshold;
    uint8_t use_optional_trig_rep_rsna_counters;
    uint8_t measurement_count_3;
    uint8_t trigger_timeout_3;
    uint8_t rsna_counter_trigger_condition;
    uint8_t dot11RSNAStatsCMACICVErrorsThreshold;
    uint8_t dot11RSNAStatsCMACReplaysThreshold;
    uint8_t dot11RSNAStatsRobustMgmtCCMPReplaysThreshold;
    uint8_t dot11RSNAStatsTKIPICVErrorsThreshold;
    uint8_t dot11RSNAStatsTKIPReplaysThreshold;
    uint8_t dot11RSNAStatsCCMPDecryptErrorsThreshold;
    uint8_t dot11RSNAStatsCCMPReplaysThreshold;
    void struct_swap(){
        sta_mac.struct_swap();
        peer_mac_addr.struct_swap();
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&repeats));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&rand_ival));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&duration));
    }
    void struct_init(){
        sta_mac.struct_init();
        peer_mac_addr.struct_init();
    }
} __attribute__((packed)) sStatisticsRequest11k;

typedef struct sStatisticsResponse11k {
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint8_t rep_mode;
    uint8_t group_identity;
    uint16_t statistics_group_data_size;
    uint16_t duration;
    uint32_t statistics_group_data[13];
    sMacAddr sta_mac;
    uint8_t use_optional_rep_reason;
    uint8_t average_trigger;
    uint8_t consecutive_trigger;
    uint8_t delay_trigger;
    void struct_swap(){
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&statistics_group_data_size));
        tlvf_swap(16, reinterpret_cast<uint8_t*>(&duration));
        for (size_t i = 0; i < 13; i++){
            tlvf_swap(32, reinterpret_cast<uint8_t*>(&(statistics_group_data[i])));
        }
        sta_mac.struct_swap();
    }
    void struct_init(){
        sta_mac.struct_init();
    }
} __attribute__((packed)) sStatisticsResponse11k;

typedef struct sLinkMeasurementsResponse11k {
    uint8_t dialog_token;
    uint8_t rep_mode;
    uint8_t rx_ant_id;
    uint8_t tx_ant_id;
    uint8_t rcpi;
    uint8_t rsni;
    uint8_t transmit_power;
    uint8_t link_margin;
    sMacAddr sta_mac;
    uint8_t use_optional_dmg_link_margin;
    uint8_t dmg_link_margin_activity;
    uint8_t dmg_link_margin_mcs;
    uint8_t dmg_link_margin_link_margin;
    uint8_t dmg_link_margin_snr;
    uint8_t use_optional_dmg_link_adapt_ack;
    uint8_t dmg_link_adapt_ack_activity;
    uint32_t dmg_link_margin_reference_timestamp;
    uint32_t dmg_link_adapt_ack_reference_timestamp;
    void struct_swap(){
        sta_mac.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&dmg_link_margin_reference_timestamp));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&dmg_link_adapt_ack_reference_timestamp));
    }
    void struct_init(){
        sta_mac.struct_init();
    }
} __attribute__((packed)) sLinkMeasurementsResponse11k;

typedef struct sBackhaulParams {
    beerocks::net::sIpv4Addr gw_ipv4;
    sMacAddr gw_bridge_mac;
    sMacAddr controller_bridge_mac;
    sMacAddr bridge_mac;
    beerocks::net::sIpv4Addr bridge_ipv4;
    sMacAddr backhaul_mac;
    beerocks::net::sIpv4Addr backhaul_ipv4;
    sMacAddr backhaul_bssid;
    uint8_t backhaul_channel;
    uint8_t backhaul_is_wireless;
    uint8_t backhaul_iface_type;
    uint8_t is_backhaul_manager;
    uint8_t is_prplmesh_controller;
    beerocks::net::sScanResult backhaul_scan_measurement_list[beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH];
    void struct_swap(){
        gw_ipv4.struct_swap();
        gw_bridge_mac.struct_swap();
        controller_bridge_mac.struct_swap();
        bridge_mac.struct_swap();
        bridge_ipv4.struct_swap();
        backhaul_mac.struct_swap();
        backhaul_ipv4.struct_swap();
        backhaul_bssid.struct_swap();
        for (size_t i = 0; i < beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH; i++){
            (backhaul_scan_measurement_list[i]).struct_swap();
        }
    }
    void struct_init(){
        gw_ipv4.struct_init();
        gw_bridge_mac.struct_init();
        controller_bridge_mac.struct_init();
        bridge_mac.struct_init();
        bridge_ipv4.struct_init();
        backhaul_mac.struct_init();
        backhaul_ipv4.struct_init();
        backhaul_bssid.struct_init();
            for (size_t i = 0; i < beerocks::message::BACKHAUL_SCAN_MEASUREMENT_MAX_LENGTH; i++) {
                (backhaul_scan_measurement_list[i]).struct_init();
            }
    }
} __attribute__((packed)) sBackhaulParams;

typedef struct sBackhaulRoam {
    sMacAddr bssid;
    uint8_t channel;
    void struct_swap(){
        bssid.struct_swap();
    }
    void struct_init(){
        bssid.struct_init();
    }
} __attribute__((packed)) sBackhaulRoam;

typedef struct sBackhaulRssi {
    int8_t rssi;
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sBackhaulRssi;

typedef struct sLoggingLevelChange {
    sMacAddr mac;
    uint8_t module_name;
    uint8_t log_level;
    uint8_t enable;
    void struct_swap(){
        mac.struct_swap();
    }
    void struct_init(){
        mac.struct_init();
    }
} __attribute__((packed)) sLoggingLevelChange;

typedef struct sVersions {
    char master_version[beerocks::message::VERSION_LENGTH];
    char slave_version[beerocks::message::VERSION_LENGTH];
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sVersions;

typedef struct sWifiCredentials {
    eWiFiSec wifi_sec;
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    char pass[beerocks::message::WIFI_PASS_MAX_LENGTH];
    uint8_t sec;
    uint8_t vap_id;
    uint8_t force;
    uint8_t radio_dir;
    void struct_swap(){
        tlvf_swap(8*sizeof(eWiFiSec), reinterpret_cast<uint8_t*>(&wifi_sec));
    }
    void struct_init(){
    }
} __attribute__((packed)) sWifiCredentials;

typedef struct sOnboarding {
    uint8_t onboarding;
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sOnboarding;

typedef struct sAdminCredentials {
    char user_password[beerocks::message::USER_PASS_LEN];
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sAdminCredentials;

typedef struct sDeviceInfo {
    char manufacturer[beerocks::message::DEV_INFO_STR_MAX_LEN];
    char model_name[beerocks::message::DEV_INFO_STR_MAX_LEN];
    char serial_number[beerocks::message::DEV_INFO_STR_MAX_LEN];
    char lan_iface_name[beerocks::message::IFACE_NAME_LENGTH];
    uint32_t lan_ip_address;
    uint32_t lan_network_mask;
    char wan_iface_name[beerocks::message::IFACE_NAME_LENGTH];
    uint32_t wan_ip_address;
    uint32_t wan_network_mask;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&lan_ip_address));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&lan_network_mask));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&wan_ip_address));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&wan_network_mask));
    }
    void struct_init(){
    }
} __attribute__((packed)) sDeviceInfo;

typedef struct sRestrictedChannels {
    sMacAddr hostap_mac;
    uint8_t restricted_channels[beerocks::message::RESTRICTED_CHANNEL_LENGTH];
    uint8_t is_global;
    void struct_swap(){
        hostap_mac.struct_swap();
    }
    void struct_init(){
        hostap_mac.struct_init();
    }
} __attribute__((packed)) sRestrictedChannels;

typedef struct sSteeringApConfig {
    sMacAddr bssid;
    uint32_t utilCheckIntervalSec;
    uint32_t utilAvgCount;
    uint32_t inactCheckIntervalSec;
    uint32_t inactCheckThresholdSec;
    void struct_swap(){
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&utilCheckIntervalSec));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&utilAvgCount));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&inactCheckIntervalSec));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&inactCheckThresholdSec));
    }
    void struct_init(){
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringApConfig;

typedef struct sSteeringClientConfig {
    uint32_t snrProbeHWM;
    uint32_t snrProbeLWM;
    uint32_t snrAuthHWM;
    uint32_t snrAuthLWM;
    uint32_t snrInactXing;
    uint32_t snrHighXing;
    uint32_t snrLowXing;
    uint32_t authRejectReason;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrProbeHWM));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrProbeLWM));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrAuthHWM));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrAuthLWM));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrInactXing));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrHighXing));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snrLowXing));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&authRejectReason));
    }
    void struct_init(){
    }
} __attribute__((packed)) sSteeringClientConfig;

typedef struct sSteeringSetGroupRequest {
    uint32_t steeringGroupIndex;
    sSteeringApConfig cfg;
    uint8_t remove;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&steeringGroupIndex));
        cfg.struct_swap();
    }
    void struct_init(){
        cfg.struct_init();
    }
} __attribute__((packed)) sSteeringSetGroupRequest;

typedef struct sSteeringSetGroupResponse {
    int32_t error_code;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&error_code));
    }
    void struct_init(){
    }
} __attribute__((packed)) sSteeringSetGroupResponse;

typedef struct sSteeringClientSetRequest {
    uint32_t steeringGroupIndex;
    sMacAddr bssid;
    sMacAddr client_mac;
    sSteeringClientConfig config;
    uint8_t remove;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&steeringGroupIndex));
        bssid.struct_swap();
        client_mac.struct_swap();
        config.struct_swap();
    }
    void struct_init(){
        bssid.struct_init();
        client_mac.struct_init();
        config.struct_init();
    }
} __attribute__((packed)) sSteeringClientSetRequest;

typedef struct sSteeringClientSetResponse {
    int32_t error_code;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&error_code));
    }
    void struct_init(){
    }
} __attribute__((packed)) sSteeringClientSetResponse;

typedef struct sSteeringEvProbeReq {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint8_t rx_snr;
    uint8_t blocked;
    uint8_t broadcast;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringEvProbeReq;

typedef struct sSteeringEvAuthFail {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint8_t rx_snr;
    uint8_t blocked;
    uint8_t reject;
    uint8_t reason;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringEvAuthFail;

typedef struct sClientDisconnectResponse {
    int32_t error_code;
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&error_code));
    }
    void struct_init(){
    }
} __attribute__((packed)) sClientDisconnectResponse;

typedef struct sSteeringDatarateInfo {
    uint8_t maxChwidth;
    uint8_t maxStreams;
    uint8_t phyMode;
    uint8_t maxMCS;
    uint8_t maxTxpower;
    uint8_t isStaticSmps;
    uint8_t isMUMimoSupported;
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sSteeringDatarateInfo;

typedef struct sSteeringRrmCaps {
    uint8_t linkMeas;
    uint8_t neighRpt;
    uint8_t bcnRptPassive;
    uint8_t bcnRptActive;
    uint8_t bcnRptTable;
    uint8_t lciMeas;
    uint8_t ftmRangeRpt;
    void struct_swap(){
    }
    void struct_init(){
    }
} __attribute__((packed)) sSteeringRrmCaps;

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

typedef struct sSteeringEvConnect {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint32_t isBTMSupported;
    uint32_t isRRMSupported;
    uint8_t bandCap2G;
    uint8_t bandCap5G;
    sSteeringDatarateInfo datarateInfo;
    sSteeringRrmCaps rrmCaps;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&isBTMSupported));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&isRRMSupported));
        datarateInfo.struct_swap();
        rrmCaps.struct_swap();
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
        datarateInfo.struct_init();
        rrmCaps.struct_init();
    }
} __attribute__((packed)) sSteeringEvConnect;

typedef struct sSteeringEvDisconnect {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint32_t reason;
    eDisconnectSource source;
    eDisconnectType type;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&reason));
        tlvf_swap(8*sizeof(eDisconnectSource), reinterpret_cast<uint8_t*>(&source));
        tlvf_swap(8*sizeof(eDisconnectType), reinterpret_cast<uint8_t*>(&type));
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringEvDisconnect;

typedef struct sSteeringEvActivity {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint8_t active;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringEvActivity;

typedef struct sSteeringEvSnrXing {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint32_t snr;
    eSteeringSnrChange inactveXing;
    eSteeringSnrChange highXing;
    eSteeringSnrChange lowXing;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snr));
        tlvf_swap(8*sizeof(eSteeringSnrChange), reinterpret_cast<uint8_t*>(&inactveXing));
        tlvf_swap(8*sizeof(eSteeringSnrChange), reinterpret_cast<uint8_t*>(&highXing));
        tlvf_swap(8*sizeof(eSteeringSnrChange), reinterpret_cast<uint8_t*>(&lowXing));
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringEvSnrXing;

typedef struct sSteeringEvSnr {
    sMacAddr client_mac;
    sMacAddr bssid;
    uint32_t snr;
    void struct_swap(){
        client_mac.struct_swap();
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&snr));
    }
    void struct_init(){
        client_mac.struct_init();
        bssid.struct_init();
    }
} __attribute__((packed)) sSteeringEvSnr;

typedef struct sTriggerChannelScanParams {
    sMacAddr radio_mac;
    //time interval (msec) between channels during scan
    uint32_t dwell_time_ms;
    //size of provided channel_pool
    uint8_t channel_pool_size;
    //pool of channels to be scaned
    uint8_t channel_pool[beerocks::message::SUPPORTED_CHANNELS_LENGTH];
    void struct_swap(){
        radio_mac.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&dwell_time_ms));
    }
    void struct_init(){
        radio_mac.struct_init();
        dwell_time_ms = 0x0;
        channel_pool_size = 0x0;
    }
} __attribute__((packed)) sTriggerChannelScanParams;

typedef struct sChannelScanRequestParams {
    //an invalid (-1) value indicates this value is not requested
    int32_t dwell_time_ms;
    //an invalid (-1) value indicates this value is not requested
    int32_t interval_time_sec;
    //an invalid (-1) value indicates this value is not requested
    int8_t channel_pool_size;
    uint8_t channel_pool[beerocks::message::SUPPORTED_CHANNELS_LENGTH];
    void struct_swap(){
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&dwell_time_ms));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&interval_time_sec));
    }
    void struct_init(){
        dwell_time_ms = -0x1;
        interval_time_sec = -0x1;
        channel_pool_size = 0x0;
    }
} __attribute__((packed)) sChannelScanRequestParams;

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

typedef struct sChannelScanResults {
    //The current service set identifier in use by the neighboring WiFi SSID. The value MAY be empty for hidden SSIDs.
    char ssid[beerocks::message::WIFI_SSID_MAX_LENGTH];
    //The BSSID used for the neighboring WiFi SSID.
    sMacAddr bssid;
    //The mode the neighboring WiFi radio is operating in. Enumerate
    eChannelScanResultMode mode;
    //The current radio channel used by the neighboring WiFi radio.
    uint32_t channel;
    //An indicator of radio signal strength (RSSI) of the neighboring WiFi radio measured in dBm, as an average of the last 100 packets received.
    int32_t signal_strength_dBm;
    //The type of encryption the neighboring WiFi SSID advertises. Enumerate List.
    eChannelScanResultSecurityMode security_mode_enabled[beerocks::message::CHANNEL_SCAN_LIST_LENGTH];
    //The type of encryption the neighboring WiFi SSID advertises. Enumerate List.
    eChannelScanResultEncryptionMode encryption_mode[beerocks::message::CHANNEL_SCAN_LIST_LENGTH];
    //Indicates the frequency band at which the radio this SSID instance is operating. Enumerate
    eChannelScanResultOperatingFrequencyBand operating_frequency_band;
    //List items indicate which IEEE 802.11 standards thisResultinstance can support simultaneously, in the frequency band specified byOperatingFrequencyBand. Enumerate List
    eChannelScanResultStandards supported_standards[beerocks::message::CHANNEL_SCAN_LIST_LENGTH];
    //Indicates which IEEE 802.11 standard that is detected for this Result. Enumerate
    eChannelScanResultStandards operating_standards;
    //Indicates the bandwidth at which the channel is operating. Enumerate
    eChannelScanResultChannelBandwidth operating_channel_bandwidth;
    //Time interval (inms) between transmitting beacons.
    uint32_t beacon_period_ms;
    //Indicator of average noise strength (indBm) received from the neighboring WiFi radio.
    int32_t noise_dBm;
    //Basic data transmit rates (in Kbps) for the SSID.
    uint32_t basic_data_transfer_rates_kbps[beerocks::message::CHANNEL_SCAN_LIST_LENGTH];
    //Data transmit rates (in Kbps) for unicast frames at which the SSID will permit a station to connect.
    uint32_t supported_data_transfer_rates_kbps[beerocks::message::CHANNEL_SCAN_LIST_LENGTH];
    //The number of beacon intervals that elapse between transmission of Beacon frames containing a TIM element whose DTIM count field is 0. This value is transmitted in the DTIM Period field of beacon frames. [802.11-2012]
    uint32_t dtim_period;
    //Indicates the fraction of the time AP senses that the channel is in use by the neighboring AP for transmissions.
    uint32_t channel_utilization;
    void struct_swap(){
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&channel));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&signal_strength_dBm));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&beacon_period_ms));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&noise_dBm));
        for (size_t i = 0; i < beerocks::message::CHANNEL_SCAN_LIST_LENGTH; i++){
            tlvf_swap(32, reinterpret_cast<uint8_t*>(&(basic_data_transfer_rates_kbps[i])));
        }
        for (size_t i = 0; i < beerocks::message::CHANNEL_SCAN_LIST_LENGTH; i++){
            tlvf_swap(32, reinterpret_cast<uint8_t*>(&(supported_data_transfer_rates_kbps[i])));
        }
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&dtim_period));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&channel_utilization));
    }
    void struct_init(){
    }
} __attribute__((packed)) sChannelScanResults;

typedef struct sPerBssAssociatedStaLinkMetrics {
    sMacAddr bssid;
    //The time delta in ms between the time at which the earliest measurement
    //that contributed to the data rate estimates were made,
    //and the time at which this report was sent.
    uint32_t time_delta;
    uint32_t mac_data_rate_downlink;
    uint32_t mac_data_rate_uplink;
    uint8_t uplink_rcpi;
    void struct_swap(){
        bssid.struct_swap();
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&time_delta));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&mac_data_rate_downlink));
        tlvf_swap(32, reinterpret_cast<uint8_t*>(&mac_data_rate_uplink));
    }
    void struct_init(){
        bssid.struct_init();
    }
} __attribute__((packed)) sPerBssAssociatedStaLinkMetrics;


}; // close namespace: beerocks_message

#endif //_BEEROCKS/TLVF_BEEROCKS_MESSAGE_COMMON_H_
