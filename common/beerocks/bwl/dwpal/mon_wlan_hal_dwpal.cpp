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

#include <easylogging++.h>

#include <cmath>

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

static void calc_curr_traffic(const uint64_t val, uint64_t &total, uint32_t &curr)
{
    if (val >= total) {
        curr = val - total;
    } else {
        curr = val;
    }
    total = val;
}

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

    size_t numOfValidArgs[9] = {0}, replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
    uint64_t BytesSent = 0, BytesReceived = 0, PacketsSent = 0, PacketsReceived = 0,
             LastDataDownlinkRate = 0, LastDataUplinkRate = 0;
    char ShortTermRSSIAverage[24] = {0};
    char SNR[24]                  = {0};
    FieldsToParse fieldsToParse[] = {
        {(void *)&BytesSent, &numOfValidArgs[0], DWPAL_LONG_LONG_INT_PARAM, "BytesSent=", 0},
        {(void *)&BytesReceived, &numOfValidArgs[1], DWPAL_LONG_LONG_INT_PARAM,
         "BytesReceived=", 0},
        {(void *)&PacketsSent, &numOfValidArgs[2], DWPAL_LONG_LONG_INT_PARAM, "PacketsSent=", 0},
        {(void *)&PacketsReceived, &numOfValidArgs[3], DWPAL_LONG_LONG_INT_PARAM,
         "PacketsReceived=", 0},
        {(void *)&sta_stats.retrans_count, &numOfValidArgs[4], DWPAL_INT_PARAM, "RetransCount=", 0},
        {(void *)ShortTermRSSIAverage, &numOfValidArgs[5], DWPAL_STR_PARAM,
         "ShortTermRSSIAverage=", sizeof(ShortTermRSSIAverage)},
        {(void *)&LastDataDownlinkRate, &numOfValidArgs[6], DWPAL_LONG_LONG_INT_PARAM,
         "LastDataDownlinkRate=", 0},
        {(void *)&LastDataUplinkRate, &numOfValidArgs[7], DWPAL_LONG_LONG_INT_PARAM,
         "LastDataUplinkRate=", 0},
        {(void *)SNR, &numOfValidArgs[8], DWPAL_STR_PARAM, "SNR=", sizeof(SNR)},

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
    // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " LastDataDownlinkRate= " << LastDataDownlinkRate;
    // LOG(DEBUG) << "numOfValidArgs[7]= " << numOfValidArgs[7] << " LastDataUplinkRate= " << LastDataUplinkRate;
    // LOG(DEBUG) << "numOfValidArgs[8]= " << numOfValidArgs[8] << " SNR= " << SNR;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    // Format ShortTermRSSIAverage = %d %d %d %d
    auto samples = beerocks::string_utils::str_split(ShortTermRSSIAverage, ' ');
    for (auto &s : samples) {
        float s_float = float(beerocks::string_utils::stoi(s));
        if (s_float > beerocks::RSSI_MIN) {
            sta_stats.rx_rssi_watt += std::pow(10, s_float / float(10));
            sta_stats.rx_rssi_watt_samples_cnt++;
        }
    }

    // Format SNR = %d %d %d %d
    auto samples_snr = beerocks::string_utils::str_split(SNR, ' ');
    for (auto &s : samples_snr) {
        float s_float = float(beerocks::string_utils::stoi(s));
        if (s_float >= beerocks::SNR_MIN) {
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

} // namespace dwpal

std::shared_ptr<mon_wlan_hal> mon_wlan_hal_create(std::string iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<dwpal::mon_wlan_hal_dwpal>(iface_name, callback);
}

} // namespace bwl
