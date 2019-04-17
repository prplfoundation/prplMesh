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

#include "ap_wlan_hal_wav.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/beerocks_version.h>
#include <beerocks/bcl/beerocks_os_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

#include <cmath>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace wav {

#define BUFFER_SIZE                     4096       
#define CSA_EVENT_FILTERING_TIMEOUT_MS  1000

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                \
    std::shared_ptr<char>(new char[size],       \
    [](char* obj) { if (obj) delete [] obj; })

// Temporary storage for station capabilities
struct SRadioCapabilitiesStrings {
    std::string supported_rates;
    std::string ht_cap;
    std::string ht_mcs;
    std::string vht_cap;
    std::string vht_mcs;
    std::string btm_supported;
    std::string nr_enabled;
    std::string non_pref_chan;
    std::string cell_capa; 
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static ap_wlan_hal::Event wav_to_bwl_event(const std::string& opcode)
{
    if (opcode == "AP-ENABLED") {
        return ap_wlan_hal::Event::AP_Enabled;
    } else if (opcode == "AP-DISABLED") {
        return ap_wlan_hal::Event::AP_Disabled;
    } else if (opcode == "AP-STA-CONNECTED") {
        return ap_wlan_hal::Event::STA_Connected;
    } else if (opcode == "AP-STA-DISCONNECTED") {
        return ap_wlan_hal::Event::STA_Disconnected;
    } else if (opcode == "UNCONNECTED_STA_RSSI") {
        return ap_wlan_hal::Event::STA_Unassoc_RSSI;
    } else if (opcode == "INTERFACE-ENABLED") {
        return ap_wlan_hal::Event::Interface_Enabled;
    } else if (opcode == "INTERFACE-DISABLED") {
        return ap_wlan_hal::Event::Interface_Disabled;
    } else if (opcode == "ACS-STARTED") {
        return ap_wlan_hal::Event::ACS_Started;
    } else if (opcode == "ACS-COMPLETED") {
        return ap_wlan_hal::Event::ACS_Completed;
    } else if (opcode == "ACS-FAILED") {
        return ap_wlan_hal::Event::ACS_Failed;
    } else if (opcode == "AP-CSA-FINISHED") {
        return ap_wlan_hal::Event::CSA_Finished;
    } else if (opcode == "BSS-TM-RESP") {
        return ap_wlan_hal::Event::BSS_TM_Response;
    } else if (opcode == "DFS-CAC-COMPLETED") {
        return ap_wlan_hal::Event::DFS_CAC_Completed;
    } else if (opcode == "DFS-NOP-FINISHED") {
        return ap_wlan_hal::Event::DFS_NOP_Finished;
    }

    return ap_wlan_hal::Event::Invalid;
}

static uint8_t wav_bw_to_beerocks_bw(const char* tmpBuff) {

    // 0 => CHAN_WIDTH_20_NOHT
    // 1 => CHAN_WIDTH_20
    // 2 => CHAN_WIDTH_40
    // 3 => CHAN_WIDTH_80
    // 4 => CHAN_WIDTH_80P80
    // 5 => CHAN_WIDTH_160
    // 6 => CHAN_WIDTH_UNKNOWN
    uint8_t bw = (uint8_t)beerocks::string_utils::stoi(tmpBuff);
    
    if (bw > 6) {
        LOG(ERROR) << "Invalid bandwidth value: " << int(bw);
        return 0;
    }

    // Convert to beerocks value (we have only one value for 20Mhz bandwidth)
    if (bw > 0) {
        bw -= 1;
    }

    // Treat 80P80 as 160
    if (bw >= 3) {
        bw = (uint8_t)BANDWIDTH_160;
    }

    return bw;
}

static bool get_sta_caps(SRadioCapabilitiesStrings &caps_strings, 
                         sRadioCapabilities& sta_caps,
                         bool is_5ghz)
{
    bool break_upper_loop = false;
    std::stringstream str_stream;
    std::string item;

    memset(&sta_caps, 0, sizeof(sta_caps));
    sta_caps.ht_bw = 0xFF;
    sta_caps.vht_bw = 0xFF;

    // extract btm_supported, nr_enabled, cell_capa
    sta_caps.btm_supported = ((caps_strings.btm_supported == "true")? true:false);
    sta_caps.nr_enabled = ((caps_strings.nr_enabled == "true")? true:false);
    sta_caps.cell_capa = (uint8_t)beerocks::string_utils::stoi(caps_strings.cell_capa);

    // HT, VHT caps parsing
    if (!caps_strings.ht_cap.empty()) {
        uint16_t ht_cap = uint16_t(std::stoul(caps_strings.ht_cap,nullptr, 16));
        sta_caps.ht_bw = (ht_cap >> 1) & 0x01; //20 == 0 / 40 == 1
        sta_caps.ht_low_bw_short_gi = (ht_cap >> 5) & 0x01; // 20 long == 0 / short == 1
        sta_caps.ht_high_bw_short_gi = (ht_cap >> 6) & 0x01; // 40 

        // parsing "AA BB CC DD XX ..." to 0xDDCCBBAA 
        uint32_t ht_mcs = 0;
        str_stream.str(caps_strings.ht_mcs);
        for (int i = 0; i < 4 ;i++) {
            getline(str_stream, item, ' ');
            ht_mcs |= (uint8_t)std::stoul(item, nullptr, 16) << (8 * i);
        }

        uint32_t mask = pow(2, 4 * 8 - 1);
        for (int8_t i=4; i > 0; i--) { // 4ss
            for (int8_t j = 7; j >= 0; j--){ // 8bits
                if ((ht_mcs & mask) > 0) {
                    sta_caps.ht_ss = i;
                    sta_caps.ant_num = i;
                    sta_caps.ht_mcs = j;
                    break_upper_loop = true;
                    break;
                }
                mask /= 2;
            }
            if(break_upper_loop){
                break;
            }
        }
    }

    if (is_5ghz && (!caps_strings.vht_cap.empty())) {
        uint32_t vht_cap = uint16_t(std::stoul(caps_strings.vht_cap, nullptr, 16));
        uint8_t supported_bw_bits = (vht_cap >> 2) & 0x03;
        if (supported_bw_bits == 0x03) { // reserved mode
            LOG(ERROR) << "INFORMATION ERROR! STA SENT RESERVED BIT COMBINATION";
        }

        // if supported_bw_bits=0 max bw is 80 Mhz, else max bw is 160 Mhz
        sta_caps.vht_bw = (BANDWIDTH_80 + (supported_bw_bits > 0 ? 1 : 0));
        sta_caps.vht_low_bw_short_gi = (vht_cap >> 5) & 0x01;    // 80 Mhz
	    sta_caps.vht_high_bw_short_gi = (vht_cap >> 6) & 0x01;   // 160 Mhz

        str_stream.str(caps_strings.vht_mcs);
        uint16_t vht_mcs_rx = 0;
        uint16_t vht_mcs_temp;

        for (int8_t i = 0 ;i < 3 ;i++) {
            getline(str_stream, item, ' ');
            if (i == 0) {
                vht_mcs_rx = (uint16_t)std::stoul(item, nullptr, 16);
                break;
            }
        }

        for(int8_t i = 4; i > 0; i--) // 4ss
        { 
            vht_mcs_temp = (vht_mcs_rx >> (2 * (i - 1))) & 0x03;
            if (vht_mcs_temp != 0x3) { //0x3 == not supported
                sta_caps.vht_ss = i;
                sta_caps.ant_num = i;
                sta_caps.vht_mcs = vht_mcs_temp + 7;
                break;
            }
        }
    }

    // find deafult mcs
    str_stream.str(caps_strings.supported_rates);
    uint16_t temp_rate;
    uint16_t max_rate=0;
    while (!std::getline(str_stream, item, ' ').eof()) {
        temp_rate = (uint8_t)beerocks::string_utils::stoi(item);
        temp_rate = (temp_rate & 0x7F) * 5; // *5 because rate/2 * 10
        if(temp_rate>max_rate){
            max_rate = temp_rate;
        }
    }
    LOG(DEBUG) << "deafult mcs max rate: " << int(max_rate) << " [100Kb]";

   if (!son::wireless_utils::get_mcs_from_rate(max_rate, 
        beerocks::ANT_MODE_1X1_SS1, beerocks::BANDWIDTH_20, 
        sta_caps.default_mcs, sta_caps.default_short_gi)) {
       LOG(DEBUG) << "No MCS rate match -> USING NEAREST VALUE!";
   }

    // update standard
    if (is_5ghz) {
        if (sta_caps.vht_ss) {
            sta_caps.wifi_standard = STANDARD_AC;
        } else if(sta_caps.ht_ss) {
            sta_caps.wifi_standard = STANDARD_N;
        } else {
            sta_caps.wifi_standard = STANDARD_A;     
        }
    } else if(sta_caps.ht_ss) { // 2.4 && HT
        sta_caps.wifi_standard = STANDARD_N;
    } else {
        sta_caps.wifi_standard = STANDARD_B | STANDARD_G;
    }

    LOG(DEBUG) 
        << std::endl << " sta HT_CAPS:"
        << std::endl << "bw20 short gi = "    << (sta_caps.ht_low_bw_short_gi ? "true" : "false")
        << std::endl << "bw40 short gi = "    << (sta_caps.ht_high_bw_short_gi ? "true" : "false")
        << std::endl << "ht_mcs = "           << ((int(sta_caps.ht_mcs))? std::to_string(sta_caps.ht_mcs) : "n/a")
        << std::endl << "ht_ss = "            << ((int(sta_caps.ht_ss)) ? std::to_string(sta_caps.ht_ss) : "n/a")
        << std::endl << "ht_bw = "            << ((sta_caps.ht_bw!=0xFF) ? std::to_string(
            beerocks::utils::convert_bandwidth_to_int(beerocks::eWiFiBandwidth(sta_caps.ht_bw))) : "n/a")

        << std::endl << "\n sta VHT_CAPS:"
        << std::endl << "bw80 short gi = "    << (sta_caps.vht_low_bw_short_gi ? "true" : "false")
        << std::endl << "bw160 short gi = "   << (sta_caps.vht_high_bw_short_gi ? "true" : "false")
        << std::endl << "vht_mcs = "          << ((int(sta_caps.vht_mcs)) ? std::to_string(sta_caps.vht_mcs) : "n/a")
        << std::endl << "vht_ss = "           << ((int(sta_caps.vht_ss)) ? std::to_string(sta_caps.vht_ss) : "n/a")
        << std::endl << "vht_bw = "           << ((sta_caps.vht_bw!=0xFF) ? std::to_string(
            beerocks::utils::convert_bandwidth_to_int(beerocks::eWiFiBandwidth(sta_caps.vht_bw))) : "n/a")

        << std::endl << "\n sta DEFAULT_CAPS:"
        << std::endl << "default_mcs = "      << int(sta_caps.default_mcs)
        << std::endl << "default_short_gi = " << int(sta_caps.default_short_gi)

        << std::endl << "\n sta OTHER_CAPS:"
        << std::endl << "wifi_standard [enum] = "   << int(sta_caps.wifi_standard)
        << std::endl << "btm_supported = "          << (sta_caps.btm_supported ? "true" : "false")
        << std::endl << "nr_enabled = "             << (sta_caps.nr_enabled ? "true" : "false")
        << std::endl << "cell_capa = "              << int(sta_caps.cell_capa);

    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_wav* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_wav::ap_wlan_hal_wav(
    std::string iface_name, bool acs_enabled, hal_event_cb_t callback) :
    base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, acs_enabled, callback),
    base_wlan_hal_wav(bwl::HALType::AccessPoint, iface_name, acs_enabled, callback, BUFFER_SIZE)
{
}

ap_wlan_hal_wav::~ap_wlan_hal_wav()
{
}

HALState ap_wlan_hal_wav::attach(bool block)
{
    auto state = base_wlan_hal_wav::attach(block);

    // On Operational send the AP_Attached event to the AP Manager
    if (state == HALState::Operational) {
        event_queue_push(Event::AP_Attached);
    }

    return state;
}

bool ap_wlan_hal_wav::sta_allow(const std::string& mac) 
{ 
    LOG(TRACE) << __func__ << " mac: " << mac;

    // Build command string
    std::string cmd = "STA_ALLOW " + mac;

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_allow() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_wav::sta_deny(const std::string& mac, int reject_sta) 
{ 
    LOG(TRACE) << __func__ << " mac: " << mac << ", reject_sta=" << reject_sta;

    // Build command string
    std::string cmd = "DENY_MAC " + mac;

    if (reject_sta) {
        cmd += " reject_sta=" + std::to_string(reject_sta);
    }

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_deny() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_wav::sta_disassoc(int8_t vap_id, const std::string& mac, uint32_t reason) 
{ 
    LOG(TRACE) << __func__ << " mac: " << mac;
    
    // Build command string
    const std::string ifname = beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    const std::string cmd = "DISASSOCIATE " + ifname + " " + mac + " tx=0";

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_disassoc() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_wav::sta_deauth(int8_t vap_id, const std::string& mac, uint32_t reason) 
{ 
    return sta_disassoc(vap_id, mac, reason);
}


bool ap_wlan_hal_wav::sta_bss_steer(const std::string& mac, const std::string& bssid, int chan, int disassoc_timer, int valid_int)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", BSS: " << bssid 
               << ", channel: " << chan << ", disassoc: " << disassoc_timer << ", valid_int: " << valid_int;

    // Build command string

    std::string cmd = 
        // Set the STA MAC address
        "BSS_TM_REQ "           + mac 

        // Transition management parameters
        + " dialog_token="      + "0"
        + " Mode="              + "0"
        + " pref="              + "1"
        + " abridged="          + "1"
        + " neighbor="          + bssid + ",0,0," + std::to_string(chan) + ",0,255";

    if (disassoc_timer) {
        cmd += std::string()
        + " disassoc_imminent=" + "1"
        + " disassoc_timer="     + std::to_string(disassoc_timer);
    }
        // " bss_term="  // Unused Param
        // " url="       // Unused Param
        // " mbo="       // Unused Param

    if (valid_int) {
        cmd += " valid_int=" + std::to_string(valid_int);
    }
        

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_bss_steer() failed!";
        return false;
    }  
 
    return true;
}

bool ap_wlan_hal_wav::sta_unassoc_rssi_measurement(const std::string& mac, int chan, int bw, int vht_center_frequency, int delay, int window_size)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", channel: " << chan 
               << ", bw: " << bw << ", delay: " << delay;

    // Convert values to strings
    std::string chanBandwidth       = std::to_string(bw);
    std::string centerFreq          = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
    std::string waveVhtCenterFreq   = std::to_string(vht_center_frequency);

    // Build command string
    // TODO

    // Delay the first measurement...
    UTILS_SLEEP_MSEC(delay);

    m_unassoc_measure_start         = std::chrono::steady_clock::now();
    m_unassoc_measure_delay         = delay;
    m_unassoc_measure_window_size   = window_size;

    std::string cmd = "UNCONNECTED_STA_RSSI " + mac + " " + centerFreq
    + " center_freq1=" + waveVhtCenterFreq
    + " bandwidth=" + chanBandwidth;

    // Trigger a measurement
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "sta_unassoc_rssi_measurement() failed!";
        return false;
    }
    
    return true;
}

bool ap_wlan_hal_wav::switch_channel(int chan, int bw, int vht_center_frequency) 
{ 
    LOG(TRACE) << __func__ << " channel: " << chan << ", bw: " << bw 
               << ", vht_center_frequency: " << vht_center_frequency;

    std::string cmd = "CHAN_SWITCH 5 ";

    // ACS
    if (chan == 0) {
        m_drop_csa = true;

        cmd += "0"; // CenterFrequency
      
    // Specific Channel
    } else {
        m_drop_csa = false;
        
        // int bandwidth_int = utils::convert_bandwidth_to_int(bandwidth);
        std::string bandwidth_str = std::to_string(bw);
        int freq = beerocks::utils::wifi_channel_to_freq(chan);
        std::string freq_str = std::to_string(freq);
        std::string wave_vht_center_frequency = std::to_string(vht_center_frequency);

        // Center Freq
        cmd += freq_str; // CenterFrequency

        
        // Extension Channel
        if (bw != 20) {
            if(freq < vht_center_frequency) {
                cmd += " sec_channel_offset=1";
            } else {
                cmd += " sec_channel_offset=-1";
            }
        }
        
        // Channel bandwidth
        if (bw == 80) {
            cmd += " center_freq1=" + wave_vht_center_frequency;
        }
        
        cmd += " bandwidth=" + bandwidth_str;
        
        // Supported Standard n/ac
        if (bw == 20 || bw == 40) {
            cmd += " ht";   //n
        } else if (bw == 80) {
            cmd += " vht";  // ac
        }

    }

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "switch_channel() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_wav::set_antenna_mode(AntMode mode) 
{ 
    std::string cmd = "iwpriv " + get_radio_info().iface_name + " sCoCPower 0 ";

    switch (mode) {
        case AntMode::ANT_2X2: { cmd += "2 2"; } break;
        case AntMode::ANT_4X4: { cmd += "4 4"; } break;
        default: {
            LOG(ERROR) << "Invalid antenna mode: " << int(mode);
            return false;
        }
    }

    // Execute the command
    beerocks::os_utils::system_call(cmd, 2);
    
    return true;
}

bool ap_wlan_hal_wav::wds_set_mode(WDSMode mode)
{
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " sFourAddrMode " + std::to_string(int(mode));

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;    
}

bool ap_wlan_hal_wav::wds_add_sta(const std::string& mac)
{
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " sAddFourAddrSta " + mac;

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;    
}

bool ap_wlan_hal_wav::wds_del_sta(const std::string& mac)
{
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " sDelFourAddrSta " + mac;

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_wav::wds_clear_list()
{
    // TODO: Check if works... Line extracted from beerocks_utils.sh script
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " gFourAddrStas | while read -r line; do iwpriv " + 
                      get_radio_info().iface_name + 
                      " sDelFourAddrSta \"$line\"; done";

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_wav::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    // Channel number of the new channel or ‘0’ to trigger low level channel selection algorithm.
    // '0' triggers the same behavior as if the failsafe channel is NOT set.
   
    LOG(TRACE) << __func__;

    std::string cmd = "SET_FAILSAFE_CHAN ";

    // Build command string
    if (chan) {
        std::string bw_str   = std::to_string(bw);
        std::string chan_str = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
        std::string freq_str = std::to_string(vht_center_frequency);
        LOG(DEBUG) << "chan_str = " << chan_str << " bw_str = " << bw_str << " vht_freq_str = " <<  freq_str;

        cmd += chan_str + 
            " center_freq1=" +  freq_str + 
            " bandwidth=" + bw_str;

    } else {
        cmd += "0";
    }

    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "failsafe_channel_set() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_wav::failsafe_channel_get(int& chan, int& bw)
{
    LOG(TRACE) << __func__;

    // Build command string
    std::string cmd = "GET_FAILSAFE_CHAN";
    char* reply;

    // Send command
    if (!wpa_ctrl_send_msg(cmd, &reply)) {
        LOG(ERROR) << "failsafe_channel_get() failed!";
        return false;
    }

    // Custom reply parsing
    if(!strncmp(reply, "UNSPECIFIED", 11)) {
        chan = -1;
        bw = -1;
    } else if (!strncmp(reply, "ACS", 3)) {
        chan = bw = 0;

    } else {
        int freq;
        sscanf(reply, "%d %*s %*s bandwidth=%d", &freq, &bw);
        chan = beerocks::utils::wifi_freq_to_channel(freq);
    }

    return true;
}

bool ap_wlan_hal_wav::restricted_channels_set(char* channel_list)
{
    LOG(TRACE) << __func__;

    // For example, the channel_list_str: “1 6 11 12 13”
    // *** WARNING: It is possible to set restricted channel only from the supported channels list!
    // *** setting channel that is not in the list, will cause this function to fail!

    std::stringstream channel_list_str;
    for (int i = 0; i < beerocks::message::RESTRICTED_CHANNEL_LENGTH; i++) {
        
        // Break after the last channel
        if (channel_list[i] == 0) break; 

        // Convert the byte-long channels into unsigned integers
        if (i > 0) channel_list_str << " ";
        channel_list_str << int(uint8_t(channel_list[i]));
    }

    auto temp_channel_list = channel_list_str.rdbuf()->str();
    LOG(DEBUG) << "temp_channel_list = " << temp_channel_list;
    
    std::string cmd = "RESTRICTED_CHANNELS " + temp_channel_list;
    
    // Send command
    if (!wpa_ctrl_send_msg(cmd)) {
        LOG(ERROR) << "restricted_channels_set() failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_wav::restricted_channels_get(char* channel_list)
{
    LOG(TRACE) << __func__;
    
    std::string cmd = "GET_RESTRICTED_CHANNELS";
    char* reply;

    // Send command
    if (!wpa_ctrl_send_msg(cmd, &reply)) {
        LOG(ERROR) << "restricted_channels_get() failed!";
        return false;
    }
    
    memset(channel_list, 0, beerocks::message::RESTRICTED_CHANNEL_LENGTH);

    // Convert the string to an array of bytes (int values)
    std::stringstream channel_list_stream(reply);
    int idx = 0;
    while (true) {
        int n;
        channel_list_stream >> n;
        channel_list[idx] = n;

        // Break on end-of-stream
        if(!channel_list_stream) break;
        
        idx++;
    }

    return true;
}

bool ap_wlan_hal_wav::read_acs_report() 
{ 
    LOG(TRACE) << __func__ << " for interface: " << get_radio_info().iface_name;

    std::string cmd = "GET_ACS_REPORT";
    parsed_obj_listed_map_t reply;

    // Send command
    if (!wpa_ctrl_send_msg(cmd, reply)) {
        LOG(ERROR) << "read_acs_report() failed!";
        return false;
    }

    LOG(DEBUG) << "cmd has been sent"; 
    

    // Initialize default values
    m_radio_info.is_5ghz = false;

    // Resize the supported channels vector
    if (MAX_SUPPORTED_20M_CHANNELS >= m_radio_info.supported_channels.size()) {
        LOG(DEBUG) << "Increasing supported channels vector to: " 
                   << MAX_SUPPORTED_20M_CHANNELS;

        m_radio_info.supported_channels.resize(MAX_SUPPORTED_20M_CHANNELS);
    }

    // Clear the supported channels vector
    for (auto chan : m_radio_info.supported_channels) {
        memset(&chan, 0, sizeof(chan));
    }
    
    int channel_idx = 0;
    int64_t tmp_int = 0;

    for (auto element:reply) {

        if (!wav_obj_read_int("BW", element, tmp_int)) {
            LOG(ERROR) << "Failed reading BW parameter!";
            return false;
        }

        if (tmp_int == 20) {

            m_radio_info.supported_channels[channel_idx].bandwidth = 20;

            if (!wav_obj_read_int("Ch", element, tmp_int)) {
                LOG(ERROR) << "Failed reading Ch parameter!";
                return false;
            }
            m_radio_info.supported_channels[channel_idx].channel = tmp_int;
            // Check if channel is 5GHz
            if (son::wireless_utils::which_freq(m_radio_info.supported_channels[channel_idx].channel) == beerocks::eFreqType::FREQ_5G) {
                m_radio_info.is_5ghz = true;
            }

            if (!wav_obj_read_int("NF", element, tmp_int)) {
                LOG(ERROR) << "Failed reading NF parameter!";
                return false;
            }
            m_radio_info.supported_channels[channel_idx].noise = tmp_int;

            if (!wav_obj_read_int("bss", element, tmp_int)) {
                LOG(ERROR) << "Failed reading bss parameter!";
                return false;
            }
            m_radio_info.supported_channels[channel_idx].bss_overlap = tmp_int;

            if (!wav_obj_read_int("DFS", element, tmp_int)) {
                LOG(ERROR) << "Failed reading DFS parameter!";
                return false;
            }
            m_radio_info.supported_channels[channel_idx].is_dfs = tmp_int;

            if (!wav_obj_read_int("pow", element, tmp_int)) {
                LOG(ERROR) << "Failed reading pow parameter!";
                return false;
            }
            m_radio_info.supported_channels[channel_idx].tx_pow = tmp_int;

            channel_idx++;
            if (channel_idx == MAX_SUPPORTED_20M_CHANNELS) {
                break;
            }
        }

    }
    
    return true;
}

bool ap_wlan_hal_wav::set_vap_enable(const std::string& iface_name, const bool enable)
{
    LOG(DEBUG) << "TODO: missing function implementation";
    return true;
}

bool ap_wlan_hal_wav::get_vap_enable(const std::string& iface_name, bool& enable)
{
    LOG(DEBUG) << "TODO: missing function implementation";
    return true;
}

std::string ap_wlan_hal_wav::get_radio_driver_version()
{
    return beerocks::version::get_wave_version();
}

bool ap_wlan_hal_wav::process_wav_event(parsed_obj_map_t& parsed_obj)
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

    switch (event) {
        
        // TODO: ???
        case Event::AP_Enabled: {
        } break;

        // ACS/CSA Completed
        case Event::ACS_Completed: 
        case Event::CSA_Finished: {

            std::string channel_param_name;
            if (event == Event::CSA_Finished) {
                channel_param_name = "Channel";
                if (std::chrono::steady_clock::now() < (m_csa_event_filtering_timestamp + 
                    std::chrono::milliseconds(CSA_EVENT_FILTERING_TIMEOUT_MS)) && m_drop_csa) {
                    LOG(DEBUG) << "CSA_Finished - dump event - arrive on csa filtering timeout window";
                    return true;
                }
            } else {
                channel_param_name = "channel";
                m_csa_event_filtering_timestamp = std::chrono::steady_clock::now();
            }
   
            int64_t chan, bw, channel_ext_above_primary, vht_center_freq, is_dfs_channel;
            ChanSwReason chanSwReason = ChanSwReason::Unknown;

        
            if (!wav_obj_read_int(channel_param_name, parsed_obj, chan)) {
                LOG(ERROR) << "Failed reading Channel parameter!";
                return false;
            }

            if (!wav_obj_read_int("OperatingChannelBandwidt", parsed_obj, bw)) {
                LOG(ERROR) << "Failed reading OperatingChannelBandwidt parameter!";
                return false;
            }

            if (!wav_obj_read_int("ExtensionChannel", parsed_obj, channel_ext_above_primary)) {
                LOG(ERROR) << "Failed reading ExtensionChannel parameter!";
                return false;
            }

            // TODO: Should be read on CSA event as well?
            if (!wav_obj_read_int("cf1", parsed_obj, vht_center_freq)) {
                LOG(ERROR) << "Failed reading cf1 parameter!";
                return false;
            }
            
            if (!wav_obj_read_int("dfs_chan", parsed_obj, is_dfs_channel)) {
                LOG(ERROR) << "Failed reading dfs_chan parameter!";
                return false;
            }

            if (!wav_obj_read_str("reason", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading reason parameter!";
                return false;
            }
            
            // Channel switch reason
            auto tmpStr = std::string(tmp_str);
            if (tmpStr.find("RADAR") != std::string::npos) {
                chanSwReason = ChanSwReason::Radar;
            } else if (tmpStr.find("20_COEXISTANCE") != std::string::npos) {
                chanSwReason = ChanSwReason::CoEx_20;
            } else if(tmpStr.find("40_COEXISTANCE") != std::string::npos) {
                chanSwReason = ChanSwReason::CoEx_40;
            }
        
            // Update the radio information structure
            m_radio_info.channel = chan;
            if (son::wireless_utils::which_freq(chan) == beerocks::eFreqType::FREQ_5G) { 
                m_radio_info.is_5ghz = true; 
            }
            m_radio_info.bandwidth = bw;
            m_radio_info.channel_ext_above = channel_ext_above_primary;
            m_radio_info.vht_center_freq = vht_center_freq;
            m_radio_info.is_dfs_channel = bool(is_dfs_channel);
            m_radio_info.last_csa_sw_reason = chanSwReason;

            event_queue_push(event);
            
            // TODO: WHY?!
            if (event == Event::ACS_Completed) {
                LOG(DEBUG) << "ACS_COMPLETED >>> adding CSA_FINISHED event as well";
                event_queue_push(Event::CSA_Finished);
            }
            
        } break;

        // STA Connected
        case Event::STA_Connected: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
            
            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_IFACE, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading iface parameter!";
                return false;
            }
            
            msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(tmp_str).vap_id;
            LOG(DEBUG) << "iface name = " << tmp_str << ", vap_id = " << int(msg->params.vap_id);

            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading mac parameter!";
                return false;
            }
            
            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmp_str);

            bool caps_valid = true;
            SRadioCapabilitiesStrings caps_strings;
            if (!wav_obj_read_str("SupportedRates", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading SupportedRates parameter!";
                caps_valid = false;
            } else {
                caps_strings.supported_rates.assign(tmp_str);
            }

            if (!wav_obj_read_str("HT_CAP", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading HT_CAP parameter!";
                caps_valid = false;
            } else {
                caps_strings.ht_cap.assign(tmp_str);
            }

            if (!wav_obj_read_str("HT_MCS", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading HT_MCS parameter!";
                caps_valid = false;
            } else {
                caps_strings.ht_mcs.assign(tmp_str);
            }

            if (!wav_obj_read_str("VHT_CAP", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading VHT_CAP parameter!";
                caps_valid = false;
            } else {
                caps_strings.vht_cap.assign(tmp_str);
            }

            if (!wav_obj_read_str("VHT_MCS", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading VHT_CAP parameter!";
                caps_valid = false;
            } else {
                caps_strings.vht_mcs.assign(tmp_str);
            }

            if (!wav_obj_read_str("btm_supported", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading btm_supported parameter!";
                caps_valid = false;
            } else {
                caps_strings.btm_supported.assign(tmp_str);
            }

            if (!wav_obj_read_str("nr_enabled", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading nr_enabled parameter!";
                caps_valid = false;
            } else {
                caps_strings.nr_enabled.assign(tmp_str);
            }

            if (!wav_obj_read_str("cell_capa", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading cell_capa parameter!";
                caps_valid = false;
            } else {
                caps_strings.cell_capa.assign(tmp_str);
            }
            
            if (caps_valid) {
                get_sta_caps(caps_strings, msg->params.capabilities, get_radio_info().is_5ghz);
            } else {
                LOG(ERROR) << "One or more of required capability strings is missing!";

                // Setting minimum default values
                msg->params.capabilities.ant_num          = 1;
                msg->params.capabilities.wifi_standard    = STANDARD_N;
                msg->params.capabilities.default_mcs      = MCS_6;
            }
            
            // Add the message to the queue
            event_queue_push(Event::STA_Connected, msg_buff);
            
        } break;

        // STA Disconnected
        case Event::STA_Disconnected: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));

            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_IFACE, parsed_obj, &tmp_str) ||
                strncmp("wlan", tmp_str, 4)) {
                LOG(ERROR) << "Failed reading iface parameter: " << tmp_str; ;
                return false;
            }

            msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(tmp_str).vap_id;
            LOG(DEBUG) << "iface name = " << tmp_str << ", vap_id = " << int(msg->params.vap_id);

            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading mac parameter!";
                return false;
            }
            
            // Store the MAC address of the disconnected STA
            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmp_str);
            
            // Add the message to the queue
            event_queue_push(Event::STA_Disconnected, msg_buff);
            
        } break;

        // Unassociated STA Measurement
        case Event::STA_Unassoc_RSSI: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
            auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
            msg->params.rx_rssi = beerocks::RSSI_INVALID;
    
            // Read and store the MAC address
            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading mac parameter!";
                return false;
            }
            
            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmp_str);

            // Read the RSSI value
            if (!wav_obj_read_str("rssi", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading rssi parameter!";
                return false;
            }

            // Split the RSSI values
            auto rssiVec = beerocks::string_utils::str_split(tmp_str, ' ');

            // -128 -128 -128 -128
            int   rssi_cnt  = 0;
            float rssi_watt = 0;
            float rssi_tmp  = 0;
            for (auto v : rssiVec) {
                rssi_tmp = float(beerocks::string_utils::stoi(v));
                if(rssi_tmp > beerocks::RSSI_MIN) {
                    rssi_watt += pow(10, rssi_tmp / float(10));
                    rssi_cnt++;
                }
            }

            // Read the number of received packets
            if (!wav_obj_read_int("rx_packets", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading rx_packets parameter!";
                return false;
            }
            
            msg->params.rx_packets = (tmp_int >= 127) ? 127 : tmp_int;
    
            // Measurement succeeded
            if ((rssi_cnt > 0) && (msg->params.rx_packets > 1)) {
                rssi_watt = (rssi_watt / float(rssi_cnt)); 
                msg->params.rx_rssi = int(10 * log10(rssi_watt));

            // Measurement failed
            } else {
                auto now   = std::chrono::steady_clock::now();
                auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_unassoc_measure_start).count();
                auto temp_sum_delay = (delta - m_unassoc_measure_delay);

                if (temp_sum_delay > m_unassoc_measure_window_size) {
                    LOG(ERROR) << "event_obj->params.rx_packets = -2 , delay = " << int(temp_sum_delay);
                    msg->params.rx_packets = -2; // means the actual measurement started later then aspected 
                }
            }

            // Add the message to the queue
            event_queue_push(Event::STA_Unassoc_RSSI, msg_buff);

        } break;

        // BSS Transition (802.11v)
        case Event::BSS_TM_Response: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));
            auto msg = reinterpret_cast<sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE));

            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading mac parameter!";
                return false;
            }

            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmp_str);

            
            if (!wav_obj_read_int("status_code",parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading status_code parameter!";
                return false;
            }

            msg->params.status_code = tmp_int;

            // Add the message to the queue
            event_queue_push(Event::BSS_TM_Response, msg_buff);            
            
        } break;

        // DFS CAC Completed
        case Event::DFS_CAC_Completed: {

            if (!get_radio_info().is_5ghz) {
                LOG(WARNING) << "DFS event on 2.4Ghz radio. Ignoring...";
                return true;
            }
    
            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION));
            
            if (!wav_obj_read_int("success", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading success parameter!";
                return false;
            }

            msg->params.success = tmp_int;
            
            if (!wav_obj_read_int("freq", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading freq parameter!";
                return false;
            }

            msg->params.frequency = tmp_int;
            
            msg->params.channel = beerocks::utils::wifi_freq_to_channel(msg->params.frequency);
            
            if (!wav_obj_read_int("timeout", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading timeout parameter!";
                return false;
            }

            msg->params.timeout = tmp_int;

            if (!wav_obj_read_str("chan_width", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading chan_width parameter!";
                return false;
            }

            msg->params.bandwidth = wav_bw_to_beerocks_bw(tmp_str);

            // Add the message to the queue
            event_queue_push(Event::DFS_CAC_Completed, msg_buff);            
                
        } break;

        // DFS NOP Finished
        case Event::DFS_NOP_Finished: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";

            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION));
            
            if (!wav_obj_read_int("freq", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading freq parameter!";
                return false;
            }

            msg->params.frequency = tmp_int;
            
            msg->params.channel = beerocks::utils::wifi_freq_to_channel(msg->params.frequency);
            
            if (!wav_obj_read_str("chan_width", parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading chan_width parameter!";
                return false;
            }

            msg->params.bandwidth = wav_bw_to_beerocks_bw(tmp_str);
            
            if (!wav_obj_read_int("cf1", parsed_obj, tmp_int)) {
                LOG(ERROR) << "Failed reading cf1 parameter!";
                return false;
            }

            msg->params.vht_center_frequency = tmp_int;

            // Add the message to the queue
            event_queue_push(Event::DFS_NOP_Finished, msg_buff);            
            
        } break;
        case Event::AP_Disabled:
        {
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
            auto msg = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";

            memset(msg_buff.get(), 0, sizeof(sHOSTAP_DISABLED_NOTIFICATION));
            
            if (!wav_obj_read_str(WAV_EVENT_KEYLESS_PARAM_IFACE, parsed_obj, &tmp_str)) {
                LOG(ERROR) << "Failed reading iface parameter!";
                return false;
            }

            auto iface_ids = beerocks::utils::get_ids_from_iface_string(tmp_str);
            msg->vap_id = iface_ids.vap_id;

            event_queue_push(Event::AP_Disabled, msg_buff);

        } break;

        case Event::Interface_Disabled:
        case Event::ACS_Failed: {
            // Forward to the AP manager
            event_queue_push(event);            
        } break;

        // Gracefully ignore unhandled events
        // TODO: Probably should be changed to an error once WAV will stop
        //       sending empty or irrelevant events...
        default: {
            LOG(WARNING) << "Unhandled event received: " << opcode;
            return true;

        } break;
    }

    return true;
}

} // namespace wav
} // namespace bwl

// AP WAV HAL Factory Functions
extern "C" {

bwl::ap_wlan_hal* ap_wlan_hal_create(
    std::string iface_name, bool acs_enabled, bwl::base_wlan_hal::hal_event_cb_t callback)
{ return new bwl::wav::ap_wlan_hal_wav(iface_name, acs_enabled, callback); }

void ap_wlan_hal_destroy(bwl::ap_wlan_hal* obj)
{ delete obj; }

}
