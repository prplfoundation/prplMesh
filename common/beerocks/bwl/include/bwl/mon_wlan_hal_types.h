/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_MON_WLAN_HAL_TYPES_H_
#define _BWL_MON_WLAN_HAL_TYPES_H_

#include "base_wlan_hal_types.h"
#include <stdint.h>

namespace bwl {

const int WIFI_SSID_MAX_LENGTH = 32 + 1 + 3; // 1 byte for null termination + 3 for alignment

struct SMacAddr {
    uint8_t oct[6];
    uint8_t channel;
    int8_t rssi;
};

struct SRadioStats {
    uint64_t tx_bytes_cnt;
    uint64_t tx_packets_cnt;
    uint64_t rx_packets_cnt;
    uint64_t rx_bytes_cnt;
    uint32_t tx_packets;
    uint32_t tx_bytes;
    uint32_t rx_packets;
    uint32_t rx_bytes;
    uint32_t errors_sent;
    uint32_t errors_received;
    uint8_t bss_load;
    int8_t noise;
    // uint8_t  channel_load_tot_prev;
    // uint8_t  channel_load_tot_curr;
    // uint8_t  channel_load_others;
    // uint8_t  channel_load_idle;
    // uint8_t  channel_load_tot_is_above_hi_th;
};

struct SVapStats {
    uint64_t tx_bytes_cnt;
    uint64_t tx_packets_cnt;
    uint64_t rx_packets_cnt;
    uint64_t rx_bytes_cnt;
    uint32_t tx_packets;
    uint32_t tx_bytes;
    uint32_t rx_packets;
    uint32_t rx_bytes;
    uint32_t errors_sent;
    uint32_t errors_received;
    uint32_t retrans_count;
    // uint8_t  client_tx_load_tot_prev=0;
    // uint8_t  client_rx_load_tot_prev=0;
    // uint8_t  client_tx_load_tot_curr=0;
    // uint8_t  client_rx_load_tot_curr=0;
    // bool active_client_count_is_above_th=false;
    // int active_client_count_prev=0;
    // int active_client_count_curr=0;
};

struct SStaStats {
    float rx_rssi_watt;
    uint8_t rx_rssi_watt_samples_cnt;
    float rx_snr_watt;
    uint8_t rx_snr_watt_samples_cnt;
    // int8_t   rx_rssi_prev=beerocks::RSSI_INVALID;
    // int8_t   rx_rssi_curr=beerocks::RSSI_INVALID;
    uint16_t tx_phy_rate_100kb;
    // uint16_t tx_phy_rate_100kb_avg;
    // uint16_t tx_phy_rate_100kb_min;
    // uint16_t tx_phy_rate_100kb_acc;
    uint16_t rx_phy_rate_100kb;
    // uint16_t rx_phy_rate_100kb_avg;
    // uint16_t rx_phy_rate_100kb_min;
    // uint16_t rx_phy_rate_100kb_acc;
    uint64_t tx_bytes_cnt;
    uint64_t rx_bytes_cnt;
    uint64_t tx_packets_cnt;
    uint64_t rx_packets_cnt;
    uint32_t tx_packets;
    uint32_t tx_bytes;
    uint32_t rx_packets;
    uint32_t rx_bytes;
    uint32_t retrans_count;
    // uint8_t  tx_load_percent_curr=0;
    // uint8_t  tx_load_percent_prev=0;
    // uint8_t  rx_load_percent_curr=0;
    // uint8_t  rx_load_percent_prev=0;
};

struct SStaChannelLoadRequest11k {
    uint8_t channel;
    uint8_t op_class;
    uint16_t
        repeats; // '0' = once, '65535' = repeats until cancel request, other (1-65534) = specific num of repeats
    uint16_t
        rand_ival; // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
    uint16_t duration; // measurement duration, expressed in units of TUs [=1024usec]
    SMacAddr sta_mac;

    // Measurement request mode booleans:
    uint8_t parallel; // (for multiple requests)'0' - measurements are to be performed in sequence,
    //  '1' - request that the measurement is to start at the same time as the measurement described
    //  by the next Measurement Request element in the same Measurement Request frame
    uint8_t enable;
    uint8_t request;
    uint8_t report;
    uint8_t
        mandatory_duration; // '0' - the duration can be lower than in the duration fiels, '1' - duration is mandantory

    // Optional:
    uint8_t use_optional_ch_load_rep; // bool
    uint8_t ch_load_rep_first;
    uint8_t ch_load_rep_second;

    uint8_t use_optional_wide_band_ch_switch; // bool
    uint32_t new_ch_width;                    // not sure if this type is most fit
    uint32_t new_ch_center_freq_seg_0;        // not sure if this type is most fit
    uint32_t new_ch_center_freq_seg_1;        // not sure if this type is most fit

    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
};

struct SStaChannelLoadResponse11k {
    uint8_t channel;
    uint8_t channel_load;
    uint8_t op_class;
    uint8_t rep_mode; // '0x00' - report ok, '0x01' - late, '0x02' - incapable, '0x04' - refused
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint16_t duration; // measurement duration, expressed in units of TUs [=1024usec]
    uint64_t start_time;
    SMacAddr sta_mac;

    // Optinal fields:
    uint8_t use_optional_wide_band_ch_switch; // bool
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;

    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
};

struct SBeaconRequest11k {

    enum class MeasurementMode : uint8_t {
        Passive = 0,
        Active,
        Table,
    };

    uint8_t
        measurement_mode; // MeasurementMode - Should be replaced with string "passive" / "active" / "table"
    uint8_t channel;
    int16_t op_class;
    uint16_t
        repeats; // '0' = once, '65535' = repeats until cancel request, other (1-65534) = specific num of repeats
    uint16_t
        rand_ival; // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
    uint16_t duration; // measurement duration, expressed in units of TUs [=1024usec]
    uint16_t reserved1;
    SMacAddr sta_mac;
    SMacAddr
        bssid; // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"

    // Measurement request mode booleans:
    uint8_t parallel; // (for multiple requests)'0' - measurements are to be performed in sequence,
    //  '1' - request that the measurement is to start at the same time as the measurement described
    //  by the next Measurement Request element in the same Measurement Request frame
    uint8_t enable;
    uint8_t request;
    uint8_t report;
    uint8_t
        mandatory_duration; // '0' - the duration can be lower than in the duration fiels, '1' - duration is mandantory

    uint8_t expected_reports_count;

    // Optional:
    uint8_t use_optional_ssid;          // bool
    uint8_t ssid[WIFI_SSID_MAX_LENGTH]; // 36 bytes

    uint8_t use_optional_ap_ch_report; // bool
    uint8_t ap_ch_report[237];

    uint8_t use_optional_req_elements; // bool
    uint8_t req_elements
        [13]; // NOTE: I didnt find any reference to "req_element", and set the max num of elements to 13 randomly

    uint8_t use_optional_wide_band_ch_switch; // bool
    uint32_t new_ch_width;                    // not sure if this type is most fit
    uint32_t new_ch_center_freq_seg_0;        // not sure if this type is most fit
    uint32_t new_ch_center_freq_seg_1;        // not sure if this type is most fit
};

struct SBeaconRequestStatus11k {
    SMacAddr sta_mac;
    uint8_t dialog_token;
    uint8_t ack;
};

struct SBeaconResponse11k {
    uint8_t
        channel; // A Channel Number of 0 indicates a request to make iterative measurements for all supported channels in the Regulatory Class
    uint8_t op_class;
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint8_t rep_mode; // '0x00' - report ok, '0x01' - late, '0x02' - incapable, '0x04' - refused
    uint8_t phy_type; // integer 0-127 (bits 0-6 of "frame info")
    uint8_t
        frame_type; // (bool) '0' - beacon/probe response frame, '1' - pilot frame (bits 7 of "frame info")
    int8_t rcpi;         // received channel power
    uint8_t rsni;        // received signal to noise
    uint8_t ant_id;      // number for the antennas used for this measurement
    uint16_t duration;   // measurement duration, expressed in units of TUs [=1024usec]
    uint64_t parent_tsf; // see IEEE part11, page 42
    uint64_t start_time;
    SMacAddr sta_mac; // mac to send response
    SMacAddr
        bssid; // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"

    // Optional:
    uint32_t new_ch_width;                    // not sure if this type is most fit
    uint32_t new_ch_center_freq_seg_0;        // not sure if this type is most fit
    uint32_t new_ch_center_freq_seg_1;        // not sure if this type is most fit
    uint8_t use_optional_wide_band_ch_switch; // bool
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
};

struct SStatisticsRequest11k {
    SMacAddr sta_mac;
    SMacAddr
        peer_mac_addr; // the bssid which will be reported. for all bssid, use wildcard "ff:ff:ff:ff:ff:ff"

    uint8_t group_identity;

    // Measurement request mode booleans:
    uint8_t parallel; // (for multiple requests)'0' - measurements are to be performed in sequence,
    //  '1' - request that the measurement is to start at the same time as the measurement described
    //  by the next Measurement Request element in the same Measurement Request frame
    uint8_t enable;
    uint8_t request;

    uint8_t report;
    uint8_t
        mandatory_duration; // '0' - the duration can be lower than in the duration fiels, '1' - duration is mandantory
    uint16_t
        repeats; // '0' = once, '65535' = repeats until cancel request, other (1-65534) = specific num of repeats

    uint16_t
        rand_ival; // random interval - specifies the upper bound of the random delay to be used prior to making the measurement, expressed in units of TUs [=1024usec]
    uint16_t duration; // measurement duration, expressed in units of TUs [=1024usec]

    // Optional:
    uint8_t use_optional_trig_rep_sta_counters; // bool
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

    uint8_t use_optional_trig_rep_qos_sta_counters; // bool
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

    uint8_t use_optional_trig_rep_rsna_counters; // bool
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

    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
};

struct SStatisticsResponse11k {
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint8_t rep_mode; // '0x00' - report ok, '0x01' - late, '0x02' - incapable, '0x04' - refused
    uint8_t group_identity;

    uint16_t statistics_group_data_size;
    uint16_t duration; // measurement duration, expressed in units of TUs [=1024usec]
    uint32_t statistics_group_data
        [13]; // different data for each group identity, 4 octets counters with commas between them
    SMacAddr sta_mac; // mac to send the request to

    // Optional:
    uint8_t use_optional_rep_reason;
    // Report Reason booleans:
    uint8_t average_trigger;     // bit 0
    uint8_t consecutive_trigger; // bit 1
    uint8_t delay_trigger;       // bit 2
};

struct SLinkMeasurementsResponse11k {
    uint8_t dialog_token;
    uint8_t rep_mode; // '0x00' - report ok, '0x01' - late, '0x02' - incapable, '0x04' - refused
    uint8_t rx_ant_id;
    uint8_t tx_ant_id;
    uint8_t rcpi;
    uint8_t rsni;
    uint8_t transmit_power;
    uint8_t link_margin;
    SMacAddr sta_mac; // mac to send the request to

    //Optional:
    uint8_t use_optional_dmg_link_margin; // bool
    uint8_t dmg_link_margin_activity;
    uint8_t dmg_link_margin_mcs;
    uint8_t dmg_link_margin_link_margin;

    uint8_t dmg_link_margin_snr;
    uint8_t use_optional_dmg_link_adapt_ack; // bool
    uint8_t dmg_link_adapt_ack_activity;
    uint8_t reserved1;
    uint32_t dmg_link_margin_reference_timestamp;
    uint32_t dmg_link_adapt_ack_reference_timestamp;
};

enum eChannelScanResultMode : uint8_t {
    eMode_NA             = 0x0,
    eMode_AdHoc          = 0x1,
    eMode_Infrastructure = 0x2,
};

enum eChannelScanResultEncryptionMode : uint8_t {
    eEncryption_Mode_NA   = 0x0,
    eEncryption_Mode_AES  = 0x1,
    eEncryption_Mode_TKIP = 0x2,
};

enum eChannelScanResultSecurityMode : uint8_t {
    eSecurity_Mode_None = 0x0,
    eSecurity_Mode_WEP  = 0x1,
    eSecurity_Mode_WPA  = 0x2,
    eSecurity_Mode_WPA2 = 0x3,
};

enum eChannelScanResultOperatingFrequencyBand : uint8_t {
    eOperating_Freq_Band_NA     = 0x0,
    eOperating_Freq_Band_2_4GHz = 0x1,
    eOperating_Freq_Band_5GHz   = 0x2,
};

enum eChannelScanResultStandards : uint8_t {
    eStandard_NA       = 0x0,
    eStandard_802_11a  = 0x1,
    eStandard_802_11b  = 0x2,
    eStandard_802_11g  = 0x3,
    eStandard_802_11n  = 0x4,
    eStandard_802_11ac = 0x5,
};

enum eChannelScanResultChannelBandwidth : uint8_t {
    eChannel_Bandwidth_NA     = 0x0,
    eChannel_Bandwidth_20MHz  = 0x1,
    eChannel_Bandwidth_40MHz  = 0x2,
    eChannel_Bandwidth_80MHz  = 0x3,
    eChannel_Bandwidth_160MHz = 0x4,
    eChannel_Bandwidth_80_80  = 0x5,
};

typedef struct sChannelScanResults {
    //The current service set identifier in use by the neighboring WiFi SSID. The value MAY be empty for hidden SSIDs.
    char ssid[WIFI_SSID_MAX_LENGTH] = {'\0'};

    //The BSSID used for the neighboring WiFi SSID.
    sMacAddr bssid = {.oct = {0}};

    //The mode the neighboring WiFi radio is operating in. Enumerate
    eChannelScanResultMode mode = eMode_NA;

    //The current radio channel used by the neighboring WiFi radio.
    uint32_t channel = 0;

    //An indicator of radio signal strength (RSSI) of the neighboring WiFi radio measured in dBm, as an average of the last 100 packets received.
    int32_t signal_strength_dBm = 0;

    //The type of encryption the neighboring WiFi SSID advertises. Enumerate List.
    std::vector<eChannelScanResultSecurityMode> security_mode_enabled;

    //The type of encryption the neighboring WiFi SSID advertises. Enumerate List.
    std::vector<eChannelScanResultEncryptionMode> encryption_mode;

    //Indicates the frequency band at which the radio this SSID instance is operating. Enumerate
    eChannelScanResultOperatingFrequencyBand operating_frequency_band = eOperating_Freq_Band_NA;

    //List items indicate which IEEE 802.11 standards thisResultinstance can support simultaneously, in the frequency band specified byOperatingFrequencyBand. Enumerate List
    std::vector<eChannelScanResultStandards> supported_standards;

    //Indicates which IEEE 802.11 standard that is detected for this Result. Enumerate
    eChannelScanResultStandards operating_standards = eStandard_NA;

    //Indicates the bandwidth at which the channel is operating. Enumerate
    eChannelScanResultChannelBandwidth operating_channel_bandwidth = eChannel_Bandwidth_NA;

    //Time interval (inms) between transmitting beacons.
    uint32_t beacon_period_ms = 0;

    //Indicator of average noise strength (indBm) received from the neighboring WiFi radio.
    int32_t noise_dBm = 0;

    //Basic data transmit rates (in Kbps) for the SSID.
    std::vector<uint32_t> basic_data_transfer_rates_kbps;

    //Data transmit rates (in Kbps) for unicast frames at which the SSID will permit a station to connect.
    std::vector<uint32_t> supported_data_transfer_rates_kbps;

    //The number of beacon intervals that elapse between transmission of Beacon frames containing a TIM element whose DTIM count field is 0. This value is transmitted in the DTIM Period field of beacon frames. [802.11-2012]
    uint32_t dtim_period = 0;

    //Indicates the fraction of the time AP senses that the channel is in use by the neighboring AP for transmissions.
    uint32_t channel_utilization = 0;
} sChannelScanResults;

typedef struct {
    sChannelScanResults channel_scan_results;
} sCHANNEL_SCAN_RESULTS_NOTIFICATION;

} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_TYPES_H_
