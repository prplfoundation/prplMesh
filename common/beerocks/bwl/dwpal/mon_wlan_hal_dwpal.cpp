/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_dwpal.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <easylogging++.h>
#include <net/if.h>

#include <cmath>
#include <functional>

extern "C" {
#include <dwpal.h>
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// DWPAL////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace dwpal {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define GET_OP_CLASS(channel) ((channel < 14) ? 4 : 5)

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static mon_wlan_hal::Event dwpal_to_bwl_event(const std::string &opcode)
{
    if (opcode == "RRM-CHANNEL-LOAD-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Channel_Load_Response;
    } else if (opcode == "RRM-BEACON-REP-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Beacon_Response;
    } else if (opcode == "RRM-STA-STATISTICS-RECEIVED") {
        return mon_wlan_hal::Event::RRM_STA_Statistics_Response;
    } else if (opcode == "RRM-LINK-MEASUREMENT-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Link_Measurement_Response;
    } else if (opcode == "AP-ENABLED") {
        return mon_wlan_hal::Event::AP_Enabled;
    } else if (opcode == "AP-DISABLED") {
        return mon_wlan_hal::Event::AP_Disabled;
    }

    return mon_wlan_hal::Event::Invalid;
}

static mon_wlan_hal::Event dwpal_nl_to_bwl_event(uint8_t cmd)
{
    switch (cmd) {
    case NL80211_CMD_TRIGGER_SCAN:
        return mon_wlan_hal::Event::Channel_Scan_Triggered;
    case NL80211_CMD_NEW_SCAN_RESULTS:
        return mon_wlan_hal::Event::Channel_Scan_Dump_Result;
    case NL80211_CMD_SCAN_ABORTED:
        return mon_wlan_hal::Event::Channel_Scan_Abort;
    case SCAN_FINISH_CB:
        return mon_wlan_hal::Event::Channel_Scan_Finished;
    default:
        LOG(ERROR) << "Unknown event received: " << int(cmd);
        return mon_wlan_hal::Event::Invalid;
    }
}

static void calc_curr_traffic(const uint64_t val, uint64_t &total, uint32_t &curr)
{
    if (val >= total) {
        curr = val - total;
    } else {
        curr = val;
    }
    total = val;
}

/**
 * @brief get channel pool frquencies for channel scan parameters.
 *
 * @param [in] channel_pool list of channels to be scanned.
 * @param [in] curr_channel channel teh radio is currently on.
 * @param [in] iface radio interface name.
 * @param [out] scan_params for saving channel frequencies for next scan. 
 * @return true on success
 */
static bool dwpal_get_channel_scan_freq(const std::vector<unsigned int> &channel_pool,
                                        unsigned int curr_channel, const std::string &iface,
                                        ScanParams &scan_params)
{
    int freq_index = 0;
    //configure center frequency for each scanned channel
    for (auto channel : channel_pool) {
        //channel validation
        LOG(DEBUG) << "validating pool channel=" << channel;
        if (son::wireless_utils::which_freq(curr_channel) !=
            son::wireless_utils::which_freq(channel)) {
            LOG(ERROR) << "cannot scan channel = " << channel
                       << " not on the same radio interface =  " << iface;
            return false;
        }

        scan_params.freq[freq_index] = beerocks::utils::wifi_channel_to_freq(int(channel));
        LOG(DEBUG) << "channel scan pool add center frequency=" << scan_params.freq[freq_index];
        freq_index++;
    }
    return true;
};
//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_dwpal::mon_wlan_hal_dwpal(std::string iface_name, hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Intel, callback),
      base_wlan_hal_dwpal(bwl::HALType::Monitor, iface_name, callback)
{
}

mon_wlan_hal_dwpal::~mon_wlan_hal_dwpal() {}

bool mon_wlan_hal_dwpal::update_radio_stats(SRadioStats &radio_stats)
{
    char *reply = nullptr;

    if (!dwpal_send_cmd("GET_RADIO_INFO", &reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    size_t numOfValidArgs[8] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    uint64_t BytesSent = 0, BytesReceived = 0, PacketsSent = 0, PacketsReceived = 0;
    FieldsToParse fieldsToParse[] = {
        {(void *)&BytesSent, &numOfValidArgs[0], DWPAL_LONG_LONG_INT_PARAM, "BytesSent=", 0},
        {(void *)&BytesReceived, &numOfValidArgs[1], DWPAL_LONG_LONG_INT_PARAM,
         "BytesReceived=", 0},
        {(void *)&PacketsSent, &numOfValidArgs[2], DWPAL_LONG_LONG_INT_PARAM, "PacketsSent=", 0},
        {(void *)&PacketsReceived, &numOfValidArgs[3], DWPAL_LONG_LONG_INT_PARAM,
         "PacketsReceived=", 0},
        {(void *)&radio_stats.bss_load, &numOfValidArgs[4], DWPAL_CHAR_PARAM, "BSS load=", 0},
        {(void *)&radio_stats.errors_sent, &numOfValidArgs[5], DWPAL_INT_PARAM, "ErrorsSent=", 0},
        {(void *)&radio_stats.errors_received, &numOfValidArgs[6], DWPAL_INT_PARAM,
         "ErrorsReceived=", 0},
        {(void *)&radio_stats.noise, &numOfValidArgs[7], DWPAL_CHAR_PARAM, "Noise=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(SRadioStats)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    // LOG(DEBUG) << "GET_RADIO_INFO reply= \n" << reply;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " BytesSent= " << BytesSent;
    // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " BytesReceived= " << BytesReceived;
    // LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " PacketsSent= " << PacketsSent;
    // LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " PacketsReceived= " << PacketsReceived;
    // LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " BSS load= " << (int)radio_stats.bss_load;
    // LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " ErrorsSent= " << radio_stats.errors_sent;
    // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " ErrorsReceived= " << radio_stats.errors_received;
    // LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7] << " Noise= " << (int)radio_stats.noise;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    calc_curr_traffic((uint64_t)BytesSent, radio_stats.tx_bytes_cnt, radio_stats.tx_bytes);
    calc_curr_traffic((uint64_t)BytesReceived, radio_stats.rx_bytes_cnt, radio_stats.rx_bytes);
    calc_curr_traffic((uint64_t)PacketsSent, radio_stats.tx_packets_cnt, radio_stats.tx_packets);
    calc_curr_traffic((uint64_t)PacketsReceived, radio_stats.rx_packets_cnt,
                      radio_stats.rx_packets);

    return true;
}

bool mon_wlan_hal_dwpal::update_vap_stats(const std::string vap_iface_name, SVapStats &vap_stats)
{
    char *reply = nullptr;

    std::string cmd = "GET_VAP_MEASUREMENTS " + vap_iface_name;

    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    size_t numOfValidArgs[7] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    uint64_t BytesSent = 0, BytesReceived = 0, PacketsSent = 0, PacketsReceived = 0;
    FieldsToParse fieldsToParse[] = {
        {(void *)&BytesSent, &numOfValidArgs[0], DWPAL_LONG_LONG_INT_PARAM, "BytesSent=", 0},
        {(void *)&BytesReceived, &numOfValidArgs[1], DWPAL_LONG_LONG_INT_PARAM,
         "BytesReceived=", 0},
        {(void *)&PacketsSent, &numOfValidArgs[2], DWPAL_LONG_LONG_INT_PARAM, "PacketsSent=", 0},
        {(void *)&PacketsReceived, &numOfValidArgs[3], DWPAL_LONG_LONG_INT_PARAM,
         "PacketsReceived=", 0},
        {(void *)&vap_stats.retrans_count, &numOfValidArgs[4], DWPAL_INT_PARAM, "RetransCount=", 0},
        {(void *)&vap_stats.errors_sent, &numOfValidArgs[5], DWPAL_INT_PARAM, "ErrorsSent=", 0},
        {(void *)&vap_stats.errors_received, &numOfValidArgs[6], DWPAL_INT_PARAM,
         "ErrorsReceived=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(SVapStats)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    // LOG(DEBUG) << "GET_VAP_MEASUREMENTS reply= \n" << reply;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " BytesSent= " << BytesSent;
    // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " BytesReceived= " << BytesReceived;
    // LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " PacketsSent= " << PacketsSent;
    // LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " PacketsReceived= " << PacketsReceived;
    // LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " RetransCount= " << vap_stats.retrans_count;
    // LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " ErrorsSent= " << vap_stats.errors_sent;
    // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " ErrorsReceived= " << vap_stats.errors_received;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    calc_curr_traffic(BytesSent, vap_stats.tx_bytes_cnt, vap_stats.tx_bytes);
    calc_curr_traffic(BytesReceived, vap_stats.rx_bytes_cnt, vap_stats.rx_bytes);
    calc_curr_traffic(PacketsSent, vap_stats.tx_packets_cnt, vap_stats.tx_packets);
    calc_curr_traffic(PacketsReceived, vap_stats.rx_packets_cnt, vap_stats.rx_packets);

    // TODO: Handle timeouts/deltas externally!
    // auto now = std::chrono::steady_clock::now();
    // auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(now - vap_stats->last_update_time);
    // vap_stats->delta_ms = float(time_span.count());
    // vap_stats->last_update_time = now;

    return true;
}

bool mon_wlan_hal_dwpal::update_stations_stats(const std::string vap_iface_name,
                                               const std::string sta_mac, SStaStats &sta_stats)
{
    char *reply = nullptr;

    std::string cmd = "GET_STA_MEASUREMENTS " + vap_iface_name + " " + sta_mac;

    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    size_t numOfValidArgs[10] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    uint64_t BytesSent = 0, BytesReceived = 0, PacketsSent = 0, PacketsReceived = 0,
             LastDataDownlinkRate = 0, LastDataUplinkRate = 0, Active = 0;
    char ShortTermRSSIAverage[32][HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH] = {'\0'};
    char SNR[32][HOSTAPD_TO_DWPAL_VALUE_STRING_LENGTH]                  = {'\0'};
    FieldsToParse fieldsToParse[]                                       = {
        {(void *)&BytesSent, &numOfValidArgs[0], DWPAL_LONG_LONG_INT_PARAM, "BytesSent=", 0},
        {(void *)&BytesReceived, &numOfValidArgs[1], DWPAL_LONG_LONG_INT_PARAM,
         "BytesReceived=", 0},
        {(void *)&PacketsSent, &numOfValidArgs[2], DWPAL_LONG_LONG_INT_PARAM, "PacketsSent=", 0},
        {(void *)&PacketsReceived, &numOfValidArgs[3], DWPAL_LONG_LONG_INT_PARAM,
         "PacketsReceived=", 0},
        {(void *)&sta_stats.retrans_count, &numOfValidArgs[4], DWPAL_INT_PARAM, "RetransCount=", 0},
        {(void *)ShortTermRSSIAverage, &numOfValidArgs[5], DWPAL_STR_ARRAY_PARAM,
         "ShortTermRSSIAverage=", sizeof(ShortTermRSSIAverage)},
        {(void *)SNR, &numOfValidArgs[6], DWPAL_STR_ARRAY_PARAM, "SNR=", sizeof(SNR)},
        {(void *)&Active, &numOfValidArgs[7], DWPAL_LONG_LONG_INT_PARAM, "Active=", 0},
        {(void *)&LastDataDownlinkRate, &numOfValidArgs[8], DWPAL_LONG_LONG_INT_PARAM,
         "LastDataDownlinkRate=", 0},
        {(void *)&LastDataUplinkRate, &numOfValidArgs[9], DWPAL_LONG_LONG_INT_PARAM,
         "LastDataUplinkRate=", 0},

        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(SStaStats)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    // LOG(DEBUG) << "GET_STA_MEASUREMENTS reply= \n" << reply;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " BytesSent= " << BytesSent;
    // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " BytesReceived= " << BytesReceived;
    // LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " PacketsSent= " << PacketsSent;
    // LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " PacketsReceived= " << PacketsReceived;
    // LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " RetransCount= " << sta_stats.retrans_count;
    // LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " ShortTermRSSIAverage= " << ShortTermRSSIAverage;
    // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " SNR= " << SNR;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[7] << " Active= " << Active;
    // LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[8] << " LastDataDownlinkRate= " << LastDataDownlinkRate;
    // LOG(DEBUG) << "numOfValidArgs[8]= " << numOfValidArgs[9] << " LastDataUplinkRate= " << LastDataUplinkRate;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    //save avarage RSSI in watt
    for (uint8_t i = 0; i < numOfValidArgs[5]; i++) {
        float s_float = float(beerocks::string_utils::stoi(std::string(ShortTermRSSIAverage[i])));
        if (s_float > beerocks::RSSI_MIN) {
            sta_stats.rx_rssi_watt += std::pow(10, s_float / float(10));
            sta_stats.rx_rssi_watt_samples_cnt++;
        }
    }

    //save avarage SNR in watt
    for (uint8_t i = 0; i < numOfValidArgs[6]; i++) {
        float s_float = float(beerocks::string_utils::stoi(std::string(SNR[i])));
        if (s_float > beerocks::SNR_MIN) {
            sta_stats.rx_snr_watt += std::pow(10, s_float / float(10));
            sta_stats.rx_snr_watt_samples_cnt++;
        }
    }

    // TODO: Update RSSI externally!
    sta_stats.tx_phy_rate_100kb = (LastDataDownlinkRate / 100);
    sta_stats.rx_phy_rate_100kb = (LastDataUplinkRate / 100);
    calc_curr_traffic(BytesSent, sta_stats.tx_bytes_cnt, sta_stats.tx_bytes);
    calc_curr_traffic(BytesReceived, sta_stats.rx_bytes_cnt, sta_stats.rx_bytes);
    calc_curr_traffic(PacketsSent, sta_stats.tx_packets_cnt, sta_stats.tx_packets);
    calc_curr_traffic(PacketsReceived, sta_stats.rx_packets_cnt, sta_stats.rx_packets);

    return true;
}

bool mon_wlan_hal_dwpal::sta_channel_load_11k_request(const SStaChannelLoadRequest11k &req)
{
    LOG(TRACE) << __func__;

    return true;
}

bool mon_wlan_hal_dwpal::sta_beacon_11k_request(const SBeaconRequest11k &req, int &dialog_token)
{
    LOG(TRACE) << __func__;
    char *reply = nullptr;

    // parameters preperations

    // Mode
    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel | (req.enable ? 0x02 : 0) | (request ? 0x04 : 0) |
                        (report ? 0x08 : 0) | (req.mandatory_duration ? 0x10 : 0));

    auto op_class = req.op_class < 0 ? GET_OP_CLASS(get_radio_info().channel) : req.op_class;

    std::string measurement_mode;
    switch ((SBeaconRequest11k::MeasurementMode)(req.measurement_mode)) {
    case SBeaconRequest11k::MeasurementMode::Passive:
        measurement_mode = "passive";
        break;
    case SBeaconRequest11k::MeasurementMode::Active:
        measurement_mode = "active";
        break;
    case SBeaconRequest11k::MeasurementMode::Table:
        measurement_mode = "table";
        break;
    default:
        LOG(WARNING) << "Invalid measuremetn mode: " << int(req.measurement_mode)
                     << ", using PASSIVE...";
        measurement_mode = "passive";
    }

    // build command
    std::string cmd = "REQ_BEACON " + beerocks::net::network_utils::mac_to_string(req.sta_mac.oct) +
                      " " +                                 // Destination MAC Address
                      std::to_string(req.repeats) + " " +   // Number of repitions
                      std::to_string(req_mode) + " " +      // Measurements Request Mode
                      std::to_string(op_class) + " " +      // Operating Class
                      std::to_string(req.channel) + " " +   // Channel
                      std::to_string(req.rand_ival) + " " + // Random Interval
                      std::to_string(req.duration) + " " +  // Duration
                      measurement_mode + " " +              // Measurement Mode
                      beerocks::net::network_utils::mac_to_string(req.bssid.oct); // Target BSSID

    /////////////////////////////////////////////////
    //////////////// Optional Fields ////////////////
    /////////////////////////////////////////////////

    // SSID
    if (req.use_optional_ssid) {
        std::string req_ssid = '"' + std::string((char *)req.ssid) + '"';
        cmd += " ssid=" + req_ssid;
    }

    // send command
    if (!dwpal_send_cmd(cmd, &reply)) {
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    size_t numOfValidArgs[1] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    FieldsToParse fieldsToParse[] = {
        {(void *)&dialog_token, &numOfValidArgs[0], DWPAL_INT_PARAM, "dialog_token=", 0},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(dialog_token)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error ==> Abort";
        return false;
    }

    /* TEMP: Traces... */
    LOG(DEBUG) << "REQ_BEACON reply= \n" << reply;
    LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " dialog_token= " << dialog_token;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    return true;
}

bool mon_wlan_hal_dwpal::sta_statistics_11k_request(const SStatisticsRequest11k &req)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_dwpal::sta_link_measurements_11k_request(const std::string &sta_mac)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_dwpal::channel_scan_trigger(int dwell_time_msec,
                                              const std::vector<unsigned int> &channel_pool)
{
    LOG(DEBUG) << "Channel scan trigger received on interface=" << m_radio_info.iface_name;

    //build scan parameters
    ScanParams channel_scan_params = {0};
    sScanCfgParams org_fg, new_fg;   //foreground scan param
    sScanCfgParamsBG org_bg, new_bg; //background scan param

    // get original scan params
    if (!dwpal_get_scan_params_fg(org_fg) || !dwpal_get_scan_params_bg(org_bg)) {
        LOG(ERROR) << "Failed getting original scan parameters";
        return false;
    }

    // prepare new scan params with changed dwell time
    new_fg                    = org_fg;
    new_bg                    = org_bg;
    new_fg.passive_dwell_time = dwell_time_msec;
    new_fg.active_dwell_time  = dwell_time_msec;
    new_bg.passive_dwell_time = dwell_time_msec;
    new_bg.active_dwell_time  = dwell_time_msec;

    // set new scan params & get newly set values for validation
    if (!dwpal_set_scan_params_fg(new_fg) || !dwpal_set_scan_params_bg(new_bg)) {
        LOG(ERROR) << "Failed setting new values, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    if (!dwpal_get_scan_params_fg(new_fg) || !dwpal_get_scan_params_bg(new_bg) ||
        (new_fg.active_dwell_time != dwell_time_msec) ||
        (new_fg.passive_dwell_time != dwell_time_msec) ||
        (new_bg.active_dwell_time != dwell_time_msec) ||
        (new_bg.passive_dwell_time != dwell_time_msec)) {
        LOG(ERROR) << "Validation failed, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    // get frequencies from channel pool and set in scan_params
    if (!dwpal_get_channel_scan_freq(channel_pool, m_radio_info.channel, m_radio_info.iface_name,
                                     channel_scan_params)) {
        LOG(ERROR) << "Failed getting frequencies, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    // must as single wifi won't allow scan on ap without this flag
    channel_scan_params.ap_force = 1;

    if (dwpal_driver_nl_scan_trigger(get_dwpal_nl_ctx(), (char *)m_radio_info.iface_name.c_str(),
                                     &channel_scan_params) != DWPAL_SUCCESS) {
        LOG(ERROR) << " scan trigger failed! Abort scan, restoring original scan parameters";
        dwpal_set_scan_params_fg(org_fg);
        dwpal_set_scan_params_bg(org_bg);
        return false;
    }

    // restoring channel scan params with original dwell time.
    // dwpal_driver_nl_scan_trigger() API doesn't include dwell time parameter
    // so we have to change and restore driver scan parameters on the fly.
    // no reason to check since we restore the original params here anyway
    // and the next validation will validate the change.
    dwpal_set_scan_params_fg(org_fg);
    dwpal_set_scan_params_bg(org_bg);

    // validate if "set" function to original values worked
    if (!dwpal_get_scan_params_fg(new_fg) || !dwpal_get_scan_params_bg(new_bg) ||
        (new_fg.active_dwell_time != org_fg.active_dwell_time) ||
        (new_fg.passive_dwell_time != org_fg.passive_dwell_time) ||
        (new_bg.active_dwell_time != org_bg.active_dwell_time) ||
        (new_bg.passive_dwell_time != org_bg.passive_dwell_time)) {
        LOG(ERROR) << "Validation failed, original scan parameters were not restored";
        return false;
    }

    return true;
}

bool mon_wlan_hal_dwpal::channel_scan_dump_results()
{
    if (!dwpal_nl_cmd_scan_dump()) {
        LOG(ERROR) << "Channel scan results dump failed";
        return false;
    }

    return true;
}

bool mon_wlan_hal_dwpal::process_dwpal_event(char *buffer, int bufLen, const std::string &opcode)
{
    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = dwpal_to_bwl_event(opcode);

    // Handle the event
    switch (event) {
    case Event::RRM_Beacon_Response: {
        LOG(DEBUG) << "RRM-BEACON-REP-RECEIVED buffer= \n" << buffer;
        // Allocate response object
        auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconResponse11k));
        auto resp      = reinterpret_cast<SBeaconResponse11k *>(resp_buff.get());

        if (!resp) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        // Initialize the message
        memset(resp_buff.get(), 0, sizeof(SBeaconResponse11k));

        size_t numOfValidArgs[11]      = {0};
        char MACAddress[MAC_ADDR_SIZE] = {0}, bssid[MAC_ADDR_SIZE] = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {NULL, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, 0},
            {(void *)MACAddress, &numOfValidArgs[2], DWPAL_STR_PARAM, NULL, sizeof(MACAddress)},
            {(void *)&resp->channel, &numOfValidArgs[3], DWPAL_CHAR_PARAM, "channel=", 0},
            {(void *)&resp->dialog_token, &numOfValidArgs[4], DWPAL_CHAR_PARAM, "dialog_token=", 0},
            {(void *)&resp->rep_mode, &numOfValidArgs[5], DWPAL_CHAR_PARAM,
             "measurement_rep_mode=", 0},
            {(void *)&resp->op_class, &numOfValidArgs[6], DWPAL_CHAR_PARAM, "op_class=", 0},
            {(void *)&resp->duration, &numOfValidArgs[7], DWPAL_SHORT_INT_PARAM, "duration=", 0},
            {(void *)&resp->rcpi, &numOfValidArgs[8], DWPAL_CHAR_PARAM, "rcpi=", 0},
            {(void *)&resp->rsni, &numOfValidArgs[9], DWPAL_CHAR_PARAM, "rsni=", 0},
            {(void *)bssid, &numOfValidArgs[10], DWPAL_STR_PARAM, "bssid=", sizeof(bssid)},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse,
                                         sizeof(SBeaconResponse11k)) == DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " MACAddress= " << MACAddress;
        LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3]
                   << " channel= " << (int)resp->channel;
        LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4]
                   << " Retransmissions= " << (int)resp->dialog_token;
        LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5]
                   << " measurement_rep_mode= " << (int)resp->rep_mode;
        LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6]
                   << " op_class= " << (int)resp->op_class;
        LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7]
                   << " duration= " << (int)resp->duration;
        LOG(DEBUG) << "numOfValidArgs[8]= " << numOfValidArgs[8] << " rcpi= " << (int)resp->rcpi;
        LOG(DEBUG) << "numOfValidArgs[9]= " << numOfValidArgs[9] << " rsni= " << (int)resp->rsni;
        LOG(DEBUG) << "numOfValidArgs[10]= " << numOfValidArgs[10] << " bssid= " << bssid;
        /* End of TEMP: Traces... */

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, MACAddress);
        beerocks::net::network_utils::mac_from_string(resp->bssid.oct, bssid);

        // Add the message to the queue
        event_queue_push(event, resp_buff);
        break;
    }

    case Event::AP_Enabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        if (!msg_buff) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }
        auto msg = reinterpret_cast<sHOSTAP_ENABLED_NOTIFICATION *>(msg_buff.get());
        if (!msg) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_ENABLED_NOTIFICATION));
        LOG(DEBUG) << "AP_ENABLED buffer= \n" << buffer;

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(interface)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        if (iface_ids.vap_id == beerocks::IFACE_RADIO_ID) {
            // Ignore AP-ENABLED on radio
            return true;
        }

        event_queue_push(event, msg_buff);
        break;
    }

    case Event::AP_Disabled: {
        auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
        if (!msg_buff) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        auto msg = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION *>(msg_buff.get());
        if (!msg) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        memset(msg_buff.get(), 0, sizeof(sHOSTAP_DISABLED_NOTIFICATION));
        LOG(INFO) << "AP_Disabled buffer= \n" << buffer;

        char interface[SSID_MAX_SIZE] = {0};
        size_t numOfValidArgs[2]      = {0};
        FieldsToParse fieldsToParse[] = {
            {NULL /*opCode*/, &numOfValidArgs[0], DWPAL_STR_PARAM, NULL, 0},
            {(void *)interface, &numOfValidArgs[1], DWPAL_STR_PARAM, NULL, sizeof(interface)},

            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(buffer, bufLen, fieldsToParse, sizeof(interface)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        auto iface_ids = beerocks::utils::get_ids_from_iface_string(interface);
        msg->vap_id    = iface_ids.vap_id;

        event_queue_push(event, msg_buff);
        break;
    }

    case Event::RRM_STA_Statistics_Response:
    case Event::RRM_Link_Measurement_Response:
    case Event::RRM_Channel_Load_Response:
        break;
    // Gracefully ignore unhandled events
    // TODO: Probably should be changed to an error once dwpal will stop
    //       sending empty or irrelevant events...
    default:
        LOG(WARNING) << "Unhandled event received: " << opcode;
        break;
    }

    return true;
}

bool mon_wlan_hal_dwpal::process_dwpal_nl_event(struct nl_msg *msg)
{
    struct nlmsghdr *nlh     = nlmsg_hdr(msg);
    struct genlmsghdr *gnlh  = (genlmsghdr *)nlmsg_data(nlh);
    char ifname[IF_NAMESIZE] = "\0";
    struct nlattr *tb[NL80211_ATTR_MAX + 1];
    nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0), genlmsg_attrlen(gnlh, 0), NULL);

    if ((tb[NL80211_ATTR_IFINDEX] == NULL) ||
        (if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), ifname) == NULL)) {
        LOG(ERROR) << __func__ << " failed to get ifname";
        return false;
    }

    auto event = dwpal_nl_to_bwl_event(gnlh->cmd);

    switch (event) {
    case Event::Channel_Scan_Triggered: {
        if (m_radio_info.iface_name.compare(ifname) != 0) {
            // ifname doesn't match current interface
            // meaning the event was recevied for a diffrent channel
            return true;
        }
        LOG(DEBUG) << "DWPAL NL event channel scan triggered";

        // Setting to distingwish from 1st and rest of NL80211_CMD_NEW_SCAN_RESULTS events.
        // 1st event "empty dump result" need to send back scan dump request.
        // rest of events are the actual scan dumps.
        m_wait_for_channel_scan_results_ready = true;
        event_queue_push(event);
        break;
    }
    case Event::Channel_Scan_Dump_Result: {
        if (m_radio_info.iface_name.compare(ifname) != 0) {
            // ifname doesn't match current interface
            // meaning the event was recevied for a diffrent channel
            return true;
        }

        if (m_wait_for_channel_scan_results_ready) {
            if (m_nl_seq == 0 && nlh->nlmsg_seq != 0) {
                LOG(DEBUG) << "Results dump are ready with sequence number: "
                           << (int)nlh->nlmsg_seq;
                m_nl_seq                              = nlh->nlmsg_seq;
                m_wait_for_channel_scan_results_ready = false;
            }

            event_queue_push(Event::Channel_Scan_New_Results_Ready);
            channel_scan_dump_results();
            break;
        }

        LOG(DEBUG) << "DWPAL NL event channel scan results dump";

        //TODO: Translate results from NL format to usable format

        event_queue_push(event);
        break;
    }
    case Event::Channel_Scan_Abort: {

        if (m_radio_info.iface_name.compare(ifname) != 0) {
            // ifname doesn't match current interface
            // meaning the event was recevied for a diffrent channel
            return true;
        }
        LOG(DEBUG) << "DWPAL NL event channel scan aborted";

        m_nl_seq                              = 0;
        m_wait_for_channel_scan_results_ready = false;

        event_queue_push(event);
        break;
    }
    case Event::Channel_Scan_Finished: {
        // ifname is corrupted for Channel_Scan_Finished event using nlh->nlmsg_seq instead.
        if (nlh->nlmsg_seq != m_nl_seq) {
            // Current event has a sequence number not matching the current sequence number
            // meaning the event was recevied for a diffrent channel
            return true;
        }

        LOG(DEBUG) << "DWPAL NL event channel scan results finished for sequence: "
                   << (int)nlh->nlmsg_seq;

        m_nl_seq                              = 0;
        m_wait_for_channel_scan_results_ready = false;

        event_queue_push(event);
        break;
    }
    // Gracefully ignore unhandled events
    default:
        LOG(ERROR) << "Unknown DWPAL NL event received: " << int(event);
        break;
    }
    return true;
}

} // namespace dwpal

std::shared_ptr<mon_wlan_hal> mon_wlan_hal_create(std::string iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<dwpal::mon_wlan_hal_dwpal>(iface_name, callback);
}

} // namespace bwl
