/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_wav.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <easylogging++.h>

#include <cmath>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// WAV ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

extern "C" {
    #include <wpa_ctrl.h>
}


namespace bwl {
namespace wav {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define GET_OP_CLASS(channel) ((channel < 14) ? 4 : 5)
#define BUFFER_SIZE 4096  

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                \
    std::shared_ptr<char>(new char[size],       \
    [](char* obj) { if (obj) delete [] obj; })


//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static mon_wlan_hal::Event wav_to_bwl_event(const std::string& opcode)
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

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_wav::mon_wlan_hal_wav(
    std::string iface_name, hal_event_cb_t callback) :
    base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Intel, false,  callback),
    base_wlan_hal_wav(bwl::HALType::Monitor, iface_name, false, callback, BUFFER_SIZE)
{
}

mon_wlan_hal_wav::~mon_wlan_hal_wav()
{
}

bool mon_wlan_hal_wav::update_radio_stats(SRadioStats& radio_stats)
{
    char* tmp_str;
    int64_t tmp_int;

    parsed_obj_map_t reply;

    if(!wpa_ctrl_send_msg("GET_RADIO_INFO", reply)){
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    // TX Bytes
    if (!wav_obj_read_str("BytesSent", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading BytesSent parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, radio_stats.tx_bytes_cnt, radio_stats.tx_bytes);

    // RX Bytes
    if (!wav_obj_read_str("BytesReceived", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading BytesReceived parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, radio_stats.rx_bytes_cnt, radio_stats.rx_bytes);

    // TX Packets
    if (!wav_obj_read_str("PacketsSent", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading PacketsSent parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, radio_stats.tx_packets_cnt, radio_stats.tx_packets);

    // RX Packets
    if (!wav_obj_read_str("PacketsReceived", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading PacketsReceived parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, radio_stats.rx_packets_cnt, radio_stats.rx_packets);

    // BSS Load
    if (!wav_obj_read_int("BSS load", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading BSS load parameter!";
        return false;
    }
    radio_stats.bss_load = tmp_int;

    // TX Errors
    if (!wav_obj_read_int("ErrorsSent", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading ErrorsSent parameter!";
        return false;
    }
    radio_stats.errors_sent = tmp_int;

    // RX Errors
    if (!wav_obj_read_int("ErrorsReceived", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading ErrorsReceived parameter!";
        return false;
    }
    radio_stats.errors_received = tmp_int;

    // Noise
    if (!wav_obj_read_int("Noise", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading Noise parameter!";
        return false;
    }
    radio_stats.noise = tmp_int;

    return true;
}

bool mon_wlan_hal_wav::update_vap_stats(const std::string vap_iface_name, SVapStats& vap_stats)
{
    char* tmp_str;
    int64_t temp_int;

    std::string cmd = "GET_VAP_MEASUREMENTS " + vap_iface_name;

    parsed_obj_map_t reply;

    if(!wpa_ctrl_send_msg(cmd, reply)){
        LOG(ERROR) << __func__ <<   " failed";
        return false;
    }

    // TX Bytes
    if (!wav_obj_read_str("BytesSent", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading BytesSent parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, vap_stats.tx_bytes_cnt, vap_stats.tx_bytes);

    // RX Bytes
    if (!wav_obj_read_str("BytesReceived", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading BytesReceived parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, vap_stats.rx_bytes_cnt, vap_stats.rx_bytes);

    // TX Packets
    if (!wav_obj_read_str("PacketsSent", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading PacketsSent parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, vap_stats.tx_packets_cnt, vap_stats.tx_packets);

    // RX Packets
    if (!wav_obj_read_str("PacketsReceived", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading PacketsReceived parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, vap_stats.rx_packets_cnt, vap_stats.rx_packets);

    // TX Errors
    // if (!wav_obj_read_int("ErrorsSent", reply, vap_stats.errors_sent, true)) {
    if (!wav_obj_read_int("ErrorsSent", reply, temp_int, true)) {
        LOG(ERROR) << "Failed reading ErrorsSent parameter!";
        return false;
    }

    vap_stats.errors_sent = temp_int;

    // RX Errors
    // if (!wav_obj_read_int("ErrorsReceived", reply, vap_stats.errors_received, true)) {
    if (!wav_obj_read_int("ErrorsReceived", reply, temp_int, true)) {
        LOG(ERROR) << "Failed reading ErrorsReceived parameter!";
        return false;
    }

    vap_stats.errors_received = temp_int;

    // Retransmissions
    // if (!wav_obj_read_int("RetransCount", reply, vap_stats.retrans_count, true)) {
    if (!wav_obj_read_int("RetransCount", reply, temp_int, true)) {
        LOG(ERROR) << "Failed reading RetransCount parameter!";
        return false;
    }

    vap_stats.retrans_count = temp_int;
    
    // TODO: Handle timeouts/deltas externally!
    // auto now = std::chrono::steady_clock::now(); 
    // auto time_span = std::chrono::duration_cast<std::chrono::milliseconds>(now - vap_stats->last_update_time);
    // vap_stats->delta_ms = float(time_span.count());
    // vap_stats->last_update_time = now;

    return true;
}

bool mon_wlan_hal_wav::update_stations_stats(const std::string vap_iface_name, const std::string sta_mac, SStaStats& sta_stats)
{
    char* tmp_str;
    int64_t tmp_int;

    std::string cmd = "GET_STA_MEASUREMENTS " +  vap_iface_name + " " + sta_mac;

    parsed_obj_map_t reply;

    if(!wpa_ctrl_send_msg(cmd, reply)){
        LOG(ERROR) << __func__ << " failed";
        return false;
    }

    if (!wav_obj_read_str("ShortTermRSSIAverage", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading ShortTermRSSIAverage parameter!";
        return false;
    } else {        
        // Format ShortTermRSSIAverage = %d %d %d %d
        auto samples = beerocks::string_utils::str_split(tmp_str, ' '); 
        for (auto s : samples) {
            float s_float = float(beerocks::string_utils::stoi(s));
            if (s_float > beerocks::RSSI_MIN) {
                sta_stats.rx_rssi_watt += std::pow(10, s_float / float(10));
                sta_stats.rx_rssi_watt_samples_cnt++;
            }
        }

        // TODO: Update RSSI externally!
    }

    //snr
    if (!wav_obj_read_str("SNR", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading SNR parameter!";
        return false;
    } else {        
        // Format SNR = %d %d %d %d
        auto samples = beerocks::string_utils::str_split(tmp_str, ' '); 
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
    if (!wav_obj_read_str("LastDataDownlinkRate", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading LastDataDownlinkRate parameter!";
        return false;
    }
    
    sta_stats.tx_phy_rate_100kb = (uint16_t)(beerocks::string_utils::stoi(tmp_str) / double(100));

    // Last Uplink (RX) Rate
    if (!wav_obj_read_str("LastDataUplinkRate", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading LastDataUplinkRate parameter!";
        return false;
    }
    
    sta_stats.rx_phy_rate_100kb = (uint16_t)(beerocks::string_utils::stoi(tmp_str) / double(100));
    
    // TX Bytes
    if (!wav_obj_read_str("BytesSent", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading BytesSent parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, sta_stats.tx_bytes_cnt, sta_stats.tx_bytes);

    // RX Bytes
    if (!wav_obj_read_str("BytesReceived", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading BytesReceived parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, sta_stats.rx_bytes_cnt, sta_stats.rx_bytes);

    // TX Packets
    if (!wav_obj_read_str("PacketsSent", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading PacketsSent parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, sta_stats.tx_packets_cnt, sta_stats.tx_packets);

    // RX Packets
    if (!wav_obj_read_str("PacketsReceived", reply, &tmp_str)) {
        LOG(ERROR) << "Failed reading PacketsReceived parameter!";
        return false;
    }

    calc_curr_traffic(tmp_str, sta_stats.rx_packets_cnt, sta_stats.rx_packets);

    // Retranmission Count
    if (!wav_obj_read_int("RetransCount", reply, tmp_int, true)) {
        LOG(ERROR) << "Failed reading RetransCount parameter!";
        return false;
    }
    sta_stats.retrans_count = tmp_int;

    return true;
}

bool mon_wlan_hal_wav::sta_channel_load_11k_request(const SStaChannelLoadRequest11k& req)
{
    LOG(TRACE) << __func__;

    return true;
}

bool mon_wlan_hal_wav::sta_beacon_11k_request(const SBeaconRequest11k& req, int& dialog_token)
{
    LOG(TRACE) << __func__;

    int64_t tmp_int;

    parsed_obj_map_t reply;

    // parameters preperations

    // Mode
    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel 
        | (req.enable               ? 0x02:0)
        | (request                  ? 0x04:0)
        | (report                   ? 0x08:0)
        | (req.mandatory_duration   ? 0x10:0));

    auto op_class = req.op_class < 0 ? GET_OP_CLASS(get_radio_info().channel) : req.op_class;

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
    

    // build command
    std::string cmd = "REQ_BEACON " + 
        beerocks::net::network_utils::mac_to_string(req.sta_mac.oct) + " " + // Destination MAC Address
        std::to_string(req.repeats)               + " " + // Number of repitions
        std::to_string(req_mode)                  + " " + // Measurements Request Mode
        std::to_string(op_class)                  + " " + // Operating Class
        std::to_string(req.channel)               + " " + // Channel
        std::to_string(req.rand_ival)             + " " + // Random Interval
        std::to_string(req.duration)              + " " + // Duration
        measurement_mode                                             + " " + // Measurement Mode
        beerocks::net::network_utils::mac_to_string(req.bssid.oct);      // Target BSSID


    /////////////////////////////////////////////////
    //////////////// Optional Fields ////////////////
    /////////////////////////////////////////////////

    // SSID
    if (req.use_optional_ssid) {
        std::string req_ssid = '"' + std::string((char*)req.ssid) + '"';

        cmd += " ssid=" + req_ssid;
    }

    // send command 
    if(!wpa_ctrl_send_msg(cmd, reply)){
        LOG(ERROR) << __func__ <<   " failed";
        return false;
    }
    
    // Read the Dialog Token value from the object and store it in the output parameter
    if (!wav_obj_read_int("dialog_token", reply, tmp_int)) {
        LOG(ERROR) << "Failed reading dialog_token parameter!";
        return false;
    }

    dialog_token = tmp_int;

    return true;
}

bool mon_wlan_hal_wav::sta_statistics_11k_request(const SStatisticsRequest11k& req)
{
    LOG(TRACE) << __func__;
    
    return true;
}

bool mon_wlan_hal_wav::sta_link_measurements_11k_request(const std::string& sta_mac)
{
    LOG(TRACE) << __func__;

    return true;
}

bool mon_wlan_hal_wav::process_wav_event(parsed_obj_map_t& parsed_obj)
{   
    char* tmp_str;
    int64_t tmp_int;
    
    // Filter out empty events
    std::string opcode;
    if (!(parsed_obj.find(WAV_EVENT_KEYLESS_PARAM_OPCODE) != parsed_obj.end() && !(opcode = parsed_obj[WAV_EVENT_KEYLESS_PARAM_OPCODE]).empty())) {
        return true;
    }

    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = wav_to_bwl_event(opcode);

    // Handle the event
    switch (event) {
        
        case Event::RRM_Channel_Load_Response: {
            
        } break;

        case Event::RRM_Beacon_Response: {

            // Allocate response object
            auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconResponse11k));
            auto resp = reinterpret_cast<SBeaconResponse11k*>(resp_buff.get());
            LOG_IF(!resp, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(resp_buff.get(), 0, sizeof(SBeaconResponse11k));

            // Channel
            if (!wav_obj_read_int("channel", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading channel parameter!";
                return false;
            }
            resp->channel = tmp_int;

            // STA MAC
            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading mac parameter!";
                return false;
            }

            beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, tmp_str);
            
            // Dialog Token
            if (!wav_obj_read_int("dialog_token", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading dialog_token parameter!";
                return false;
            }
            resp->dialog_token = tmp_int;

            // Measurement Response Mode
            if (!wav_obj_read_int("measurement_rep_mode", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading measurement_rep_mode parameter!";
                return false;
            }
            resp->rep_mode = tmp_int;

            // Operating Class
            if (!wav_obj_read_int("op_class", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading op_class parameter!";
                return false;
            }
            resp->op_class = tmp_int;

            // Measurement Duration
            if (!wav_obj_read_int("duration", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading duration parameter!";
                return false;
            }
            resp->duration = tmp_int;

            // RCPI
            if (!wav_obj_read_int("rcpi", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading rcpi parameter!";
                return false;
            }
            resp->rcpi = tmp_int;

            // RSNI
            if (!wav_obj_read_int("rsni", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading rsni parameter!";
                return false;
            }
            resp->rsni = tmp_int;
            
            // BSSID
            if (!wav_obj_read_str("bssid", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading mac parameter!";
                return false;
            }
            beerocks::net::network_utils::mac_from_string(resp->bssid.oct, tmp_str);
            
            // Add the message to the queue
            event_queue_push(event, resp_buff);
            
        } break;

        case Event::RRM_STA_Statistics_Response: {

        } break;

        case Event::RRM_Link_Measurement_Response: {

        } break;

        // Gracefully ignore unhandled events
        // TODO: Probably should be changed to an error once WAV will stop
        //       sending empty or irrelevant events...
        default: { LOG(WARNING) << "Unhandled event received: " << opcode; break; };
    }
    
    return true;
}

} // namespace wav
} // namespace bwl

// AP WAV HAL Factory Functions
extern "C" {

bwl::mon_wlan_hal* mon_wlan_hal_create(
    std::string iface_name, bwl::base_wlan_hal::hal_event_cb_t callback)
{ return new bwl::wav::mon_wlan_hal_wav(iface_name, callback); }

void mon_wlan_hal_destroy(bwl::mon_wlan_hal* obj)
{ delete obj; }

}
