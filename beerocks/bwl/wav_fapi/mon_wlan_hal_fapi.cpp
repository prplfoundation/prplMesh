/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_fapi.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include "ugw_helper.h"

#include <cmath>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// FAPI ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define LOGGING_ID mon_wlan_hal_fapi
#define PACKAGE_ID "0"

extern "C" {
#ifdef USE_FAPI_DAEMON

    #include <fapi_wlan_daemon.h>
    #include <fapi_wlan_public.h>
    
#else

    #include <fapi_wlan_beerock.h>
    #include <wpa_ctrl.h>

#endif
}

#ifndef LOG_LEVEL
uint16_t   LOGLEVEL = SYS_LOG_DEBUG + 1;
#else
uint16_t   LOGLEVEL = LOG_LEVEL + 1;
#endif
#ifndef LOG_TYPE
uint16_t   LOGTYPE = SYS_LOG_TYPE_FILE;
#else
uint16_t   LOGTYPE = LOG_TYPE;
#endif

extern ObjList* mapperObjPtr;

namespace bwl {
namespace wav_fapi {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                \
    std::shared_ptr<char>(new char[size],       \
    [](char* obj) { if (obj) delete [] obj; })


#define GET_OP_CLASS(channel) ((channel < 14) ? 4 : 5)

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void wav_fapi_debug_obj(const std::shared_ptr<ObjList>& debug_obj, 
                          const std::string& label) {
    
    ObjList* currentObj;
    ParamList* currentParam;

    LOG(DEBUG) << "============ " << label << " ============";
    FOR_EACH_OBJ(debug_obj.get(), currentObj) {
        LOG(DEBUG) << "OBJECT NAME: " << std::string(GET_OBJ_NAME(currentObj));
        FOR_EACH_PARAM(currentObj, currentParam) {
            LOG(DEBUG) << "PARAM: " << std::string(GET_PARAM_NAME(currentParam)) 
                       << "="  << std::string(GET_PARAM_VALUE(currentParam));
        }
    }
    LOG(DEBUG) << "============";
}

static mon_wlan_hal::Event fapi_to_bwl_event(const std::string& opcode)
{
    if (opcode == "RRM-CHANNEL-LOAD-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Channel_Load_Response;
    } else if (opcode == "RRM-BEACON-REP-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Beacon_Response;
    } else if (opcode == "RRM-STA-STATISTICS-RECEIVED") {
        return mon_wlan_hal::Event::RRM_STA_Statistics_Response;
    } else if (opcode == "RRM-LINK-MEASUREMENT-RECEIVED") {
        return mon_wlan_hal::Event::RRM_Link_Measurement_Response;
    }

    return mon_wlan_hal::Event::Invalid;
}

static void calc_curr_traffic(char* buff, uint64_t& total, uint32_t& curr)
{
    // Convert to numeric value
    uint64_t val = beerocks::string_utils::stoi(buff);

    if (val >= total) {
        curr = val - total;
    } else {
        curr = val;
    }
    total = val;
}

static int fapi_str_to_int(char* str, bool ignore_unknown = true)
{
    static const std::string unknown_string = "UNKNOWN";

    if (!unknown_string.compare(str) && ignore_unknown) {
        return 0;
    }

    return beerocks::string_utils::stoi(str);
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_fapi::mon_wlan_hal_fapi(
    std::string iface_name, hal_event_cb_t callback) :
    base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Intel, false, callback),
    base_wlan_hal_fapi(bwl::HALType::Monitor, iface_name, IfaceType::Intel, false, callback)
{
    // Pointer for parsing FAPI object values
    m_temp_fapi_value = std::shared_ptr<char>(new char[MAX_LEN_PARAM_VALUE],
        [](char* obj) { if(obj) delete [] obj; });
}

mon_wlan_hal_fapi::~mon_wlan_hal_fapi()
{
}

bool mon_wlan_hal_fapi::update_radio_stats(SRadioStats& radio_stats)
{
    auto tmpBuff = m_temp_fapi_value.get();

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    if (fapi_wlan_radio_info_get(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_radio_info_get failed!";
        return false;
    }

    // TX Bytes
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "BytesSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BytesSent is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, radio_stats.tx_bytes_cnt, radio_stats.tx_bytes);

    // RX Bytes
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "BytesReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BytesReceived is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, radio_stats.rx_bytes_cnt, radio_stats.rx_bytes);

    // TX Packets
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "PacketsSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "PacketsSent is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, radio_stats.tx_packets_cnt, radio_stats.tx_packets);

    // RX Packets
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "PacketsReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "PacketsReceived is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, radio_stats.rx_packets_cnt, radio_stats.rx_packets);

    // BSS Load
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "BSS load", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BSS load is missing from the FAPI object!";
        return false;
    }
    
    radio_stats.bss_load = beerocks::string_utils::stoi(tmpBuff);

    // TX Errors
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "ErrorsSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "ErrorsSent is missing from the FAPI object!";
        return false;
    }
    
    radio_stats.errors_sent = beerocks::string_utils::stoi(tmpBuff);

    // RX Errors
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "ErrorsReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "ErrorsReceived is missing from the FAPI object!";
        return false;
    }
    
    radio_stats.errors_received = beerocks::string_utils::stoi(tmpBuff);

    // Noise
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.Stats", 0, "Noise", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "Noise is missing from the FAPI object!";
        return false;
    }
    
    radio_stats.noise = beerocks::string_utils::stoi(tmpBuff);
    
    return true;
}

bool mon_wlan_hal_fapi::update_vap_stats(const std::string vap_iface_name, SVapStats& vap_stats)
{
    // Create a new object
    auto tmpBuff = m_temp_fapi_value.get();
    
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Read the VAP stats
    if (fapi_wlan_vap_measurement_get(vap_iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_vap_measurement_get failed! vap: " << vap_iface_name;
        return false;
    }

    // TX Bytes
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "BytesSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BytesSent is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, vap_stats.tx_bytes_cnt, vap_stats.tx_bytes);

    // RX Bytes
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "BytesReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BytesReceived is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, vap_stats.rx_bytes_cnt, vap_stats.rx_bytes);

    // TX Packets
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "PacketsSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "PacketsSent is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, vap_stats.tx_packets_cnt, vap_stats.tx_packets);

    // RX Packets
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "PacketsReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "PacketsReceived is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, vap_stats.rx_packets_cnt, vap_stats.rx_packets);

    // TX Errors
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "ErrorsSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "ErrorsSent is missing from the FAPI object!";
        return false;
    }

    vap_stats.errors_sent = fapi_str_to_int(tmpBuff);

    // RX Errors
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "ErrorsReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "ErrorsReceived is missing from the FAPI object!";
        return false;
    }

    vap_stats.errors_received = fapi_str_to_int(tmpBuff);

    // Retransmissions
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID.Stats", 0, "RetransCount", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "RetransCount is missing from the FAPI object!";
        return false;
    }

    vap_stats.retrans_count = fapi_str_to_int(tmpBuff);
    
    // TODO: Handle timeouts/deltas externally!
    // auto now = std::chrono::steady_clock::now(); 
    // auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(now - vap_stats->last_update_time);
    // vap_stats->delta_ms = float(time_span.count());
    // vap_stats->last_update_time = now;

    return true;
}

bool mon_wlan_hal_fapi::update_stations_stats(const std::string vap_iface_name, const std::string sta_mac, SStaStats& sta_stats)
{
    // Create a new object
    auto tmpBuff = m_temp_fapi_value.get();
    
    // TODO: Create the object as a class memeber to decrease memory allocations?
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Build the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.AssociatedDevice");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice",
                                 "MACAddress", sta_mac.c_str(), 0, 0);

    // Read STA statistics
    if (fapi_wlan_sta_measurement_get(vap_iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_sta_measurement_get failed! sta_mac = " << sta_mac << " vap = " << vap_iface_name;
        return false;
    }

    // Short Term RSSI
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "ShortTermRSSIAverage", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "ShortTermRSSIAverage is missing from the FAPI object!";
        return false;
    } else {        
        // Format ShortTermRSSIAverage = %d %d %d %d
        auto samples = beerocks::string_utils::str_split(tmpBuff, ' '); 
        for (auto s : samples) {
            float s_float = float(beerocks::string_utils::stoi(s));
            if (s_float > beerocks::RSSI_MIN) {
                sta_stats.rx_rssi_watt += std::pow(10, s_float / float(10));
                sta_stats.rx_rssi_watt_samples_cnt++;
            }
        }

        // LOG(DEBUG) << "sta_mac=" << sta_mac << ", fapi_rssi_db=" << tmpBuff << ", rx_rssi_watt=" << sta_stats.rx_rssi_watt << ", samples_cnt=" << int(sta_stats.rx_rssi_watt_samples_cnt);
    }

    // Short Term SNR
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "SNR", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "SNR is missing from the FAPI object!";
        return false;
    } else {        
        // Format ShortTermRSSIAverage = %d %d %d %d
        auto samples = beerocks::string_utils::str_split(tmpBuff, ' '); 
        for (auto s : samples) {
            float s_float = float(beerocks::string_utils::stoi(s));
            if (s_float >= beerocks::SNR_MIN) {
                sta_stats.rx_snr_watt += std::pow(10, s_float / float(10));
                sta_stats.rx_snr_watt_samples_cnt++;
            }
        }

      
        // TODO: Update SNR externally!
    }

    // Last Downlink (TX) Rate
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "LastDataDownlinkRate", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "LastDataDownlinkRate is missing from the FAPI object!";
        return false;
    }
    
    sta_stats.tx_phy_rate_100kb = (uint16_t)(beerocks::string_utils::stoi(tmpBuff) / double(100));

    // Last Uplink (RX) Rate
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "LastDataUplinkRate", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "LastDataUplinkRate is missing from the FAPI object!";
        return false;
    }
    
    sta_stats.rx_phy_rate_100kb = (uint16_t)(beerocks::string_utils::stoi(tmpBuff) / double(100));
    
    // TX Bytes
    if (UGW_READ_VALUE(wlObj, "Device.WIFI.AccessPoint.AssociatedDevice.Stats", 0, "BytesSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BytesSent is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, sta_stats.tx_bytes_cnt, sta_stats.tx_bytes);

    // RX Bytes
    if (UGW_READ_VALUE(wlObj, "Device.WIFI.AccessPoint.AssociatedDevice.Stats", 0, "BytesReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "BytesReceived is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, sta_stats.rx_bytes_cnt, sta_stats.rx_bytes);

    // TX Packets
    if (UGW_READ_VALUE(wlObj, "Device.WIFI.AccessPoint.AssociatedDevice.Stats", 0, "PacketsSent", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "PacketsSent is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, sta_stats.tx_packets_cnt, sta_stats.tx_packets);

    // RX Packets
    if (UGW_READ_VALUE(wlObj, "Device.WIFI.AccessPoint.AssociatedDevice.Stats", 0, "PacketsReceived", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "PacketsReceived is missing from the FAPI object!";
        return false;
    }

    calc_curr_traffic(tmpBuff, sta_stats.rx_packets_cnt, sta_stats.rx_packets);

    // Retranmission Count
    if (UGW_READ_VALUE(wlObj, "Device.WIFI.AccessPoint.AssociatedDevice.Stats", 0, "RetransCount", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "RetransCount is missing from the FAPI object!";
        return false;
    }

    sta_stats.retrans_count = fapi_str_to_int(tmpBuff);

    return true;
}

bool mon_wlan_hal_fapi::sta_channel_load_11k_request(const SStaChannelLoadRequest11k& req)
{
    LOG(TRACE) << __func__;

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Construct the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio");

    // Channel
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "Channel", 
        std::to_string(req.channel).c_str(), 0, 0);

    // Vendor specific parameters
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");

    // Destination MAC Address
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "dst", 
        beerocks::net::network_utils::mac_to_string(req.sta_mac.oct).c_str(), 0, 0);

    // Number of repitions
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "num_of_repetitions", 
        std::to_string(req.repeats).c_str(), 0, 0);

    // Mode
    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel 
        | (req.enable               ? 0x02:0)
        | (request                  ? 0x04:0)
        | (report                   ? 0x08:0)
        | (req.mandatory_duration   ? 0x10:0));

    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "measurement_req_mode", 
        std::to_string(req_mode).c_str(), 0, 0);

    // Operating Class
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "op_class", 
        std::to_string(GET_OP_CLASS(get_radio_info().channel)).c_str(), 0, 0);

    // Random Interval
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "rand_int", 
        std::to_string(req.rand_ival).c_str(), 0, 0);

    // Duration
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "duration", 
        std::to_string(req.duration).c_str(), 0, 0);

    /////////////////////////////////////////////////
    //////////////// Optional Fields ////////////////
    /////////////////////////////////////////////////

    // Channel load repitions
    if (req.use_optional_ch_load_rep == true) {
        auto ch_load_rep = 
            std::to_string(req.ch_load_rep_first) + "," + 
            std::to_string(req.ch_load_rep_second);

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "ch_load_rep", 
            ch_load_rep.c_str(), 0, 0);
    }

    // Wide-Band Channel Switch
    if (req.use_optional_wide_band_ch_switch == true) {
        auto wide_band_ch_switch = 
            std::to_string(req.new_ch_width)             + "," + 
            std::to_string(req.new_ch_center_freq_seg_0) + "," + 
            std::to_string(req.new_ch_center_freq_seg_1);

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "wide_band_ch_switch", 
            wide_band_ch_switch.c_str(), 0, 0);
    }

    // Execute the FAPI command
    if (fapi_wlan_channel_load_request(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE)
    {
        LOG(ERROR) << "fapi_wlan_channel_load_request failed!";
        return false;
    }

    return true;
}

bool mon_wlan_hal_fapi::sta_beacon_11k_request(const SBeaconRequest11k& req, int& dialog_token)
{
    LOG(TRACE) << __func__;
    
    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Construct the object
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.Radio");
    
    // Channel
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "Channel", 
        std::to_string(req.channel).c_str(), 0, 0);

    // Vendor specific parameters
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");

    // Destination MAC Address
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "dst", 
        beerocks::net::network_utils::mac_to_string(req.sta_mac.oct).c_str(), 0, 0);

    // Number of repitions
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "num_of_repetitions", 
        std::to_string(req.repeats).c_str(), 0, 0);

    // Mode
    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel 
        | (req.enable               ? 0x02:0)
        | (request                  ? 0x04:0)
        | (report                   ? 0x08:0)
        | (req.mandatory_duration   ? 0x10:0));

    // Measurements Request Mode
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "measurement_req_mode", 
        std::to_string(req_mode).c_str(), 0, 0);
    
    // Operating Class
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "op_class", 
        std::to_string(
            ((req.op_class < 0) ? GET_OP_CLASS(get_radio_info().channel) : req.op_class)).c_str(), 0, 0);
    
    // Random Interval
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "rand_int", 
        std::to_string(req.rand_ival).c_str(), 0, 0);

    // Duration
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "duration", 
        std::to_string(req.duration).c_str(), 0, 0);

    std::string measurement_mode;
    switch ((SBeaconRequest11k::MeasurementMode)(req.measurement_mode)) {
        case SBeaconRequest11k::MeasurementMode::Passive: measurement_mode = "passive"; break;
        case SBeaconRequest11k::MeasurementMode::Active:  measurement_mode = "active"; break;
        case SBeaconRequest11k::MeasurementMode::Table:   measurement_mode = "table"; break;
        default: {
            LOG(WARNING) << "Invalid measuremetn mode: " << int(req.measurement_mode)
                         << ", using PASSIVE...";

            measurement_mode = "passive";
        }
    }

    // Measurement Mode
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "mode", 
        measurement_mode.c_str(), 0, 0);

    // Target BSSID
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "bssid", 
        beerocks::net::network_utils::mac_to_string(req.bssid.oct).c_str(), 0, 0);

    /////////////////////////////////////////////////
    //////////////// Optional Fields ////////////////
    /////////////////////////////////////////////////

    // SSID
    if (req.use_optional_ssid) {
        std::string req_ssid = '"' + std::string((char*)req.ssid) + '"';

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "ssid", 
            req_ssid.c_str(), 0, 0);
    }

    // AP Channel Report
    if (req.use_optional_ap_ch_report) {
        std::string ap_ch_report;
        LOG(DEBUG) << "TODO: implement stop condition on ap_ch_report array";
        for(uint8_t i = 0; i < sizeof(req.ap_ch_report); ++i){
            ap_ch_report += (std::to_string(req.ap_ch_report[i]) + ",");
        }
        ap_ch_report.pop_back(); // remove last comma

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "ap_ch_report", 
            ap_ch_report.c_str(), 0, 0);
    }

    // ???
    if (req.use_optional_req_elements) {
        std::string req_elements;
        LOG(DEBUG) << "TODO: implement stop condition on req_elements array";
        for(uint8_t i = 0; i < sizeof(req.req_elements); ++i){
            req_elements += (std::to_string(req.req_elements[i]) + ",");
        }
        req_elements.pop_back(); // remove last comma

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "req_elements", 
            req_elements.c_str(), 0, 0);
    }

    // ???
    if (req.use_optional_wide_band_ch_switch == true) {
        std::string wide_band_ch_switch = 
            std::to_string(req.new_ch_width)             + "," + 
            std::to_string(req.new_ch_center_freq_seg_0) + "," + 
            std::to_string(req.new_ch_center_freq_seg_1);

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "wide_band_ch_switch", 
            wide_band_ch_switch.c_str(), 0, 0);
    }

    // FAPI_DEBUG_OBJ("BEACON_REQUEST", wlObj);

    // Execute the FAPI command
    if (fapi_wlan_beacon_request(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_beacon_request failed!";
        wav_fapi_debug_obj(wlObj, "FAILED BEACON_REQUEST:");
        return false;
    }

    // FAPI_DEBUG_OBJ("BEACON_REQUEST response", wlObj);

    auto tmpBuff = m_temp_fapi_value.get();

    // Read the Dialog Token value from the object
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dialog_token", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "dialog_token is missing from the FAPI object!";
        return false;
    }
    
    // Store the dialog_token value in the output parameter
    dialog_token = beerocks::string_utils::stoi(tmpBuff);
        
    return true;
}

bool mon_wlan_hal_fapi::sta_statistics_11k_request(const SStatisticsRequest11k& req)
{
    LOG(TRACE) << __func__;
    
    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Build the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");

    // Destination MAC Address
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "dst", 
        beerocks::net::network_utils::mac_to_string(req.sta_mac.oct).c_str(), 0, 0);

    // Peer MAC Address
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "peer_mac_addr", 
        beerocks::net::network_utils::mac_to_string(req.peer_mac_addr.oct).c_str(), 0, 0);
    
    // Number of Repitions
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "num_of_repetitions", 
    std::to_string(req.repeats).c_str(), 0, 0);

    // Mode
    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel 
        | (req.enable               ? 0x02:0)
        | (request                  ? 0x04:0)
        | (report                   ? 0x08:0)
        | (req.mandatory_duration   ? 0x10:0));

    // Measurements Request Mode
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "measurement_req_mode", 
        std::to_string(req_mode).c_str(), 0, 0);

    // Random Interval
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "rand_int", 
        std::to_string(req.rand_ival).c_str(), 0, 0);

    // Duration
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "duration", 
        std::to_string(req.duration).c_str(), 0, 0);

    // Group Identity
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "group_identity", 
        std::to_string(req.group_identity).c_str(), 0, 0);
            
    /////////////////////////////////////////////////
    //////////////// Optional Fields ////////////////
    /////////////////////////////////////////////////

    // Optional fields:
    if (req.use_optional_trig_rep_sta_counters) {
        std::string trig_rep_sta_counters =
            std::to_string(req.measurement_count_1)                  + "," +
            std::to_string(req.trigger_timeout_1)                    + "," +
            std::to_string(req.sta_counter_trigger_condition)        + "," +
            std::to_string(req.dot11FailedCountThreshold)            + "," +
            std::to_string(req.dot11FCSErrorCountThreshold)          + "," +
            std::to_string(req.dot11MultipleRetryCountThreshold)     + "," +
            std::to_string(req.dot11FrameDuplicateCountThreshold)    + "," +
            std::to_string(req.dot11RTSFailureCountThreshold)        + "," +
            std::to_string(req.dot11AckFailureCountThreshold)        + "," +
            std::to_string(req.dot11RetryCountThreshold);

        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "trig_rep_sta_counters", 
            trig_rep_sta_counters.c_str(), 0, 0);
    }

    if (req.use_optional_trig_rep_qos_sta_counters) {
        std::string trig_rep_qos_sta_counters =
            std::to_string(req.measurement_count_2)                  + "," +
            std::to_string(req.trigger_timeout_2)                    + "," +
            std::to_string(req.qos_sta_counter_trigger_condition)    + "," +
            std::to_string(req.dot11QoSFailedCountThreshold)         + "," +
            std::to_string(req.dot11QoSRetryCountThreshold)          + "," +
            std::to_string(req.dot11QoSMultipleRetryCountThreshold)  + "," +
            std::to_string(req.dot11QoSFrameDuplicateCountThreshold) + "," +
            std::to_string(req.dot11QoSRTSCountFailureThreshold)     + "," +
            std::to_string(req.dot11QoSAckFailureCountThreshold)     + "," +
            std::to_string(req.dot11QoSDiscardedCountThreshold);
        
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "trig_rep_qos_sta_counters", 
            trig_rep_qos_sta_counters.c_str(), 0, 0);
    }

    if (req.use_optional_trig_rep_rsna_counters) {
        std::string trig_rep_rsna_counters =
            std::to_string(req.measurement_count_3)                              + "," +
            std::to_string(req.trigger_timeout_3)                                + "," +
            std::to_string(req.rsna_counter_trigger_condition)                   + "," +
            std::to_string(req.dot11RSNAStatsCMACICVErrorsThreshold)             + "," +
            std::to_string(req.dot11RSNAStatsCMACReplaysThreshold)               + "," +
            std::to_string(req.dot11RSNAStatsRobustMgmtCCMPReplaysThreshold)     + "," +
            std::to_string(req.dot11RSNAStatsTKIPICVErrorsThreshold)             + "," +
            std::to_string(req.dot11RSNAStatsTKIPReplaysThreshold)               + "," +
            std::to_string(req.dot11RSNAStatsCCMPDecryptErrorsThreshold)         + "," +
            std::to_string(req.dot11RSNAStatsCCMPReplaysThreshold);

            UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "trig_rep_rsna_counters", 
                trig_rep_rsna_counters.c_str(), 0, 0);
    } 
    
    // Execute the FAPI command
    if (fapi_wlan_sta_statistics_request(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_sta_statistics_request failed!";
        return false;
    }
    
    return true;
}

bool mon_wlan_hal_fapi::sta_link_measurements_11k_request(const std::string& sta_mac)
{
    LOG(TRACE) << __func__;

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Build the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");

    // STA MAC Address
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "dst", sta_mac.c_str(), 0, 0);

    // Execute the FAPI function
    if (fapi_wlan_sta_link_measurements_request(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_sta_link_measurements_request failed!";
        return false;
    }

    return true;
}

bool mon_wlan_hal_fapi::parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj)
{   
    // Filter out empty events
    if (opcode.empty()) { return true; }
 
    auto event = fapi_to_bwl_event(opcode);

    if (event == mon_wlan_hal::Event::Invalid) { return true; }

    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto wlObj = std::static_pointer_cast<ObjList>(obj);
    auto tmpBuff = m_temp_fapi_value.get();

    if (!wlObj) {
        LOG(ERROR) << "FAPI Object is NULL!";
        return false;
    }

    // Handle the event
    switch (event) {
        
        case Event::RRM_Channel_Load_Response: {

            // Allocate response object
            auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SStaChannelLoadResponse11k));
            auto resp = reinterpret_cast<SStaChannelLoadResponse11k*>(resp_buff.get());
            LOG_IF(!resp, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(resp_buff.get(), 0, sizeof(SStaChannelLoadResponse11k));

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "Name", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }
            
            // Channel
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio", 0, "Channel", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Channel field is missing from the FAPI object!";
                return false;
            }
            resp->channel = beerocks::string_utils::stoi(tmpBuff);
            
            // STA MAC
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }
            beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, tmpBuff);

            // Dialog Token
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dialog_token", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "dialog_token field is missing from the FAPI object!";
                return false;
            }
            resp->dialog_token = beerocks::string_utils::stoi(tmpBuff);

            // Measurement Response Mode
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "measurement_rep_mode", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "measurement_rep_mode field is missing from the FAPI object!";
                return false;
            }
            resp->rep_mode = beerocks::string_utils::stoi(tmpBuff);
            
            // Operating Class
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "op_class", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "op_class field is missing from the FAPI object!";
                return false;
            }
            resp->op_class = beerocks::string_utils::stoi(tmpBuff);
            
            // Measurement Start Time
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "start_time", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "start_time field is missing from the FAPI object!";
                return false;
            }
            resp->start_time = beerocks::string_utils::stoi(tmpBuff);

            // Measurement Duration
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "duration", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "duration field is missing from the FAPI object!";
                return false;
            }
            resp->duration = beerocks::string_utils::stoi(tmpBuff);

            // Channel Load
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "channel_load", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "channel_load field is missing from the FAPI object!";
                return false;
            }
            resp->channel_load = beerocks::string_utils::stoi(tmpBuff);

            // OPTIONAL - Wide band channel switch
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "wide_band_ch_switch", tmpBuff) == UGW_SUCCESS) {
                auto data_vec = beerocks::string_utils::str_split(tmpBuff, ',');
                if (data_vec.size() == 3) {
                    resp->use_optional_wide_band_ch_switch = true;
                    resp->new_ch_width              = beerocks::string_utils::stoi(data_vec[0]);
                    resp->new_ch_center_freq_seg_0  = beerocks::string_utils::stoi(data_vec[1]);
                    resp->new_ch_center_freq_seg_1  = beerocks::string_utils::stoi(data_vec[2]);
                } else {
                    LOG(WARNING) << "Invalid 'wide_band_ch_switch' value: " << tmpBuff;
                    resp->use_optional_wide_band_ch_switch = false;
                }                
            } else {
                LOG(WARNING) << "wide_band_ch_switch is missing from the FAPI object...";
                resp->use_optional_wide_band_ch_switch = false;
            }

            // Add the message to the queue
            event_queue_push(event, resp_buff);
            
        } break;

        case Event::RRM_Beacon_Response: {

            // Allocate response object
            auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconResponse11k));
            auto resp = reinterpret_cast<SBeaconResponse11k*>(resp_buff.get());
            LOG_IF(!resp, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(resp_buff.get(), 0, sizeof(SBeaconResponse11k));

            // Channel
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio", 0, "Channel", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Channel field is missing from the FAPI object!";
                return false;
            }
            resp->channel = beerocks::string_utils::stoi(tmpBuff);
            
            // STA MAC
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }
            beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, tmpBuff);
            
            // Dialog Token
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dialog_token", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "dialog_token field is missing from the FAPI object!";
                return false;
            }
            resp->dialog_token = beerocks::string_utils::stoi(tmpBuff);

            // Measurement Response Mode
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "measurement_rep_mode", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "measurement_rep_mode field is missing from the FAPI object!";
                return false;
            }
            resp->rep_mode = beerocks::string_utils::stoi(tmpBuff);

            // Operating Class
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "op_class", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "op_class field is missing from the FAPI object!";
                return false;
            }
            resp->op_class = beerocks::string_utils::stoi(tmpBuff);
            
            // Measurement Start Time
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "start_time", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "start_time field is missing from the FAPI object!";
                return false;
            }
            resp->start_time = beerocks::string_utils::stoi(tmpBuff);

            // Measurement Duration
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "duration", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "duration field is missing from the FAPI object!";
                return false;
            }
            resp->duration = beerocks::string_utils::stoi(tmpBuff);

            // Frame information
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "frame_info", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "frame_info field is missing from the FAPI object!";
                return false;
            } else {
                auto val = (uint8_t)std::stoul(tmpBuff, nullptr, 16);
                resp->phy_type      =  (val & 0x7F);
                resp->frame_type    = ((val & 0x80) > 0);
            }

            // RCPI
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "rcpi", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "rcpi field is missing from the FAPI object!";
                return false;
            }
            resp->rcpi = beerocks::string_utils::stoi(tmpBuff);

            // RSNI
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "rsni", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "rsni field is missing from the FAPI object!";
                return false;
            }
            resp->rsni = beerocks::string_utils::stoi(tmpBuff);
            
            // BSSID
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "bssid", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "bssid field is missing from the FAPI object!";
                return false;
            }
            beerocks::net::network_utils::mac_from_string(resp->bssid.oct, tmpBuff);
            
            // Antenna ID
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "antenna_id", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "antenna_id field is missing from the FAPI object!";
                return false;
            }
            resp->ant_id = beerocks::string_utils::stoi(tmpBuff);

            // Parent TSF
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "parent_tsf", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "parent_tsf field is missing from the FAPI object!";
                return false;
            }
            resp->parent_tsf = beerocks::string_utils::stoi(tmpBuff);
            
            // OPTIONAL - Wide band channel switch
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "wide_band_ch_switch", tmpBuff) == UGW_SUCCESS) {
                auto data_vec = beerocks::string_utils::str_split(tmpBuff, ',');
                if (data_vec.size() == 3) {
                    resp->use_optional_wide_band_ch_switch = true;
                    resp->new_ch_width              = beerocks::string_utils::stoi(data_vec[0]);
                    resp->new_ch_center_freq_seg_0  = beerocks::string_utils::stoi(data_vec[1]);
                    resp->new_ch_center_freq_seg_1  = beerocks::string_utils::stoi(data_vec[2]);
                } else {
                    LOG(WARNING) << "Invalid 'wide_band_ch_switch' value: " << tmpBuff;
                    resp->use_optional_wide_band_ch_switch = false;
                }                
            } else {
                LOG(WARNING) << "wide_band_ch_switch is missing from the FAPI object...";
                resp->use_optional_wide_band_ch_switch = false;
            }

            // Add the message to the queue
            event_queue_push(event, resp_buff);
            
        } break;

        case Event::RRM_STA_Statistics_Response: {

            // Allocate response object
            auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SStatisticsResponse11k));
            auto resp = reinterpret_cast<SStatisticsResponse11k*>(resp_buff.get());
            LOG_IF(!resp, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(resp_buff.get(), 0, sizeof(SStatisticsResponse11k));

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "Name", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }
            
            // STA MAC
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }
            beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, tmpBuff);

            // Dialog Token
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dialog_token", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "dialog_token field is missing from the FAPI object!";
                return false;
            }
            resp->dialog_token = beerocks::string_utils::stoi(tmpBuff);
            
            // Measurement Response Mode
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "measurement_rep_mode", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "measurement_rep_mode field is missing from the FAPI object!";
                return false;
            }
            resp->rep_mode = beerocks::string_utils::stoi(tmpBuff);
            
            // Measurement Duration
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "duration", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "duration field is missing from the FAPI object!";
                return false;
            }
            resp->duration = beerocks::string_utils::stoi(tmpBuff);

            // Group Identity
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "group_identity", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "group_identity field is missing from the FAPI object!";
                return false;
            }
            resp->group_identity = beerocks::string_utils::stoi(tmpBuff);

            // Statistics Group Data
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "statistics_group_data", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "statistics_group_data field is missing from the FAPI object!";
                return false;
            } else {
                auto vec = beerocks::string_utils::str_split(tmpBuff, ',');
                resp->statistics_group_data_size = vec.size();
                for (uint i = 0; i < vec.size(); i++) {
                    resp->statistics_group_data[i] = beerocks::string_utils::stoi(vec[i]);
                }
            }

            // Response Reason?
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "rep_reason", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "rep_reason field is missing from the FAPI object!";
                return false;
            } else {
                auto val = beerocks::string_utils::stoi(tmpBuff);
                resp->use_optional_rep_reason   = true;
                resp->average_trigger           = (val & 0x01);
                resp->consecutive_trigger       = (val & 0x02);
                resp->delay_trigger             = (val & 0x04);
            }
            
            // Add the message to the queue
            event_queue_push(event, resp_buff);
            
        } break;

        case Event::RRM_Link_Measurement_Response: {

            // Allocate response object
            auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SLinkMeasurementsResponse11k));
            auto resp = reinterpret_cast<SLinkMeasurementsResponse11k*>(resp_buff.get());
            LOG_IF(!resp, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(resp_buff.get(), 0, sizeof(SLinkMeasurementsResponse11k));

            // Interface Name
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "Name", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }
                        
            // STA MAC
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }
            beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, tmpBuff);

            // Dialog Token
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dialog_token", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "dialog_token field is missing from the FAPI object!";
                return false;
            }
            resp->dialog_token = beerocks::string_utils::stoi(tmpBuff);

            // TPC Report
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "tpc_report", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "tpc_report field is missing from the FAPI object!";
                return false;
            } else {
                auto vec = beerocks::string_utils::str_split(tmpBuff, ',');
                if (vec.size() == 2) {
                    resp->transmit_power    = beerocks::string_utils::stoi(vec[0]);
                    resp->link_margin       = beerocks::string_utils::stoi(vec[1]);
                } else {
                    LOG(ERROR) << "Invalid 'tpc_report' field value: " << tmpBuff;
                    return false;
                }
            }
            
            // RX Antenna ID
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "rx_ant_id", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "rx_ant_id field is missing from the FAPI object!";
                return false;
            }
            resp->rx_ant_id = beerocks::string_utils::stoi(tmpBuff);

            // TX Antenna ID
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "tx_ant_id", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "tx_ant_id field is missing from the FAPI object!";
                return false;
            }
            resp->tx_ant_id = beerocks::string_utils::stoi(tmpBuff);
            
            // RCPI
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "rcpi", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "rcpi field is missing from the FAPI object!";
                return false;
            }
            resp->rcpi = beerocks::string_utils::stoi(tmpBuff);

            // RSNI
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "rsni", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "rsni field is missing from the FAPI object!";
                return false;
            }
            resp->rsni = beerocks::string_utils::stoi(tmpBuff);

            // ???
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dmg_link_margin", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "dmg_link_margin field is missing from the FAPI object!";
                return false;
            } else {
                auto vec = beerocks::string_utils::str_split(tmpBuff, ',');
                if (vec.size() == 5) {
                    resp->use_optional_dmg_link_margin = true;
                    resp->dmg_link_margin_activity              = beerocks::string_utils::stoi(vec[0]);
                    resp->dmg_link_margin_mcs                   = beerocks::string_utils::stoi(vec[1]);
                    resp->dmg_link_margin_link_margin           = beerocks::string_utils::stoi(vec[2]);
                    resp->dmg_link_margin_snr                   = beerocks::string_utils::stoi(vec[3]);
                    resp->dmg_link_margin_reference_timestamp   = beerocks::string_utils::stoi(vec[4]);
                } else {
                    LOG(ERROR) << "Invalid 'dmg_link_margin' field value: " << tmpBuff;
                    return false;
                }
            }
            
            // ???
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "dmg_link_adapt_ack", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "dmg_link_adapt_ack field is missing from the FAPI object!";
                return false;
            } else {
                auto vec = beerocks::string_utils::str_split(tmpBuff, ',');
                if (vec.size() == 2) {
                    resp->use_optional_dmg_link_adapt_ack           = true;
                    resp->dmg_link_adapt_ack_activity               = beerocks::string_utils::stoi(vec[0]);
                    resp->dmg_link_adapt_ack_reference_timestamp    = beerocks::string_utils::stoi(vec[1]);
                } else {
                    LOG(ERROR) << "Invalid 'dmg_link_adapt_ack' field value: " << tmpBuff;
                    return false;
                }
            }
            
            // Add the message to the queue
            event_queue_push(event, resp_buff);

        } break;

        // Gracefully ignore unhandled events
        // TODO: Probably should be changed to an error once FAPI will stop
        //       sending empty or irrelevant events...
        default: { 
            // LOG(WARNING) << "Unhandled event received: " << opcode; 
            break; 
        };
    }
    
    return true;
}

} // namespace wav_fapi
} // namespace bwl

// AP FAPI HAL Factory Functions
extern "C" {
bwl::mon_wlan_hal* mon_wlan_hal_create(
    std::string iface_name, bwl::base_wlan_hal::hal_event_cb_t callback)
{ return new bwl::wav_fapi::mon_wlan_hal_fapi(iface_name, callback); }

void mon_wlan_hal_destroy(bwl::mon_wlan_hal* obj)
{ delete obj; }

}
