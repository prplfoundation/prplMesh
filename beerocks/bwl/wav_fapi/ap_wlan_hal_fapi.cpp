/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_fapi.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/beerocks_version.h>
#include <beerocks/bcl/beerocks_os_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

#include "ugw_helper.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// FAPI ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define LOGGING_ID ap_wlan_hal_fapi
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

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace wav_fapi {
    
#define CSA_EVENT_FILTERING_TIMEOUT_MS 1000

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

static ap_wlan_hal::Event fapi_to_bwl_event(const std::string& opcode)
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
    } else if (opcode == "DFS-RADAR-DETECTED") {
        return ap_wlan_hal::Event::DFS_RADAR_Detected;
    }

    return ap_wlan_hal::Event::Invalid;
}

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

static bool fapi_read_int(const std::shared_ptr<ObjList> wlObj, 
    const char* objName, const char* paramName, 
    const char* tmpBuff, int& value)
{
    if (UGW_READ_VALUE(wlObj, objName, 0, paramName, tmpBuff) != UGW_SUCCESS) {
        return false;
    }

    value = beerocks::string_utils::stoi(tmpBuff);
    return (true);
}

static uint8_t fapi_bw_to_beerocks_bw(const char* tmpBuff) {

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

        getline(str_stream, item, ' ');
        vht_mcs_rx = (uint16_t)std::stoul(item, nullptr, 16);

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
    while(str_stream == getline(str_stream, item, ' ')){
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

// NOTE: Since *base_wlan_hal_fapi* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_fapi::ap_wlan_hal_fapi(
    std::string iface_name, bool acs_enabled, hal_event_cb_t callback) :
    base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, acs_enabled, callback),
    base_wlan_hal_fapi(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, acs_enabled, callback)
{
    // Pointer for parsing FAPI object values
    m_temp_fapi_value = std::shared_ptr<char>(new char[MAX_LEN_PARAM_VALUE],
        [](char* obj) { if(obj) delete [] obj; });
}

ap_wlan_hal_fapi::~ap_wlan_hal_fapi()
{
}

HALState ap_wlan_hal_fapi::attach(bool block)
{
    auto state = base_wlan_hal_fapi::attach(block);

    // On Operational send the AP_Attached event to the AP Manager
    if (state == HALState::Operational) {
        event_queue_push(Event::AP_Attached);
    }

    return state;
}

bool ap_wlan_hal_fapi::sta_allow(const std::string& mac) 
{ 
    LOG(TRACE) << __func__ << " mac: " << mac 
    << " iface_name = " << get_radio_info().iface_name;

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Build the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor",
                                 "MACAddressControlList", mac.c_str(), 0, 0);

    // Call the FAPI
    wav_fapi_debug_obj(wlObj, "fapi_wlan_sta_allow");
    if (fapi_wlan_sta_allow(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_sta_allow failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_fapi::sta_deny(const std::string& mac, int reject_sta) 
{ 
    LOG(TRACE) << __func__ << " mac: " << mac << ", reject_sta=" << reject_sta;
    
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Build the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor",
                                 "MACAddressControlList", mac.c_str(), 0, 0);

    if (reject_sta) {
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor", "rejectSta", 
            std::to_string(reject_sta).c_str(), 0, 0);
    }
    
    if (fapi_wlan_sta_deny(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_sta_deny failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_fapi::sta_disassoc(int8_t vap_id, const std::string& mac, uint32_t reason) 
{ 
    LOG(TRACE) << __func__ << " mac: " << mac;
    
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Build the object
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor",
                                 "MACAddress", mac.c_str(), 0, 0);
    
    const std::string ifname = beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    if (fapi_wlan_sta_disassociate(ifname.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_sta_disassociate failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_fapi::sta_bss_steer(const std::string& mac, const std::string& bssid, int chan, int disassoc_timer, int valid_int)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", BSS: " << bssid 
               << ", channel: " << chan << ", disassoc: " << disassoc_timer << ", valid_int: " << valid_int;

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Build the object
    // Set the STA MAC address
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.AssociatedDevice");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice",
                                 "MACAddress", mac.c_str(), 0, 0);

    // Target BSSID
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.SSID");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID",
                                 "BSSID", bssid.c_str(), 0, 0);

    // Transition management parameters
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.SSID.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "dialog_token", "0", 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "Mode",         "0", 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "abridged",     "1", 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "pref",         "1", 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "0_neighbor",
        std::string(bssid + ",0,0," + std::to_string(chan) + ",0,255").c_str(), 0, 0);
    
    if (valid_int) {
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "valid_int", 
        std::to_string(valid_int).c_str(), 0, 0);
    }

    if (disassoc_timer) {
        LOG(DEBUG) << __func__ << " mac = " << mac << ", adding disassoc_timer = " << disassoc_timer;
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "disassoc_imminent", "1", 0, 0);
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.SSID.X_LANTIQ_COM_Vendor", "disassoc_timer", 
            std::to_string(disassoc_timer).c_str(), 0, 0);
    }
    wav_fapi_debug_obj(wlObj, "fapi_wlan_bss_transition_management_req");
    // Execute the command
    if (fapi_wlan_bss_transition_management_req(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_bss_transition_management_req failed!";
        return false;
    }  
 
    return true;
}

bool ap_wlan_hal_fapi::sta_unassoc_rssi_measurement(const std::string& mac, int chan, int bw, int vht_center_frequency, int delay, int window_size)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", channel: " << chan 
               << ", bw: " << bw << ", delay: " << delay;

    // Convert values to strings
    std::string chanBandwidth       = std::to_string(bw);
    std::string centerFreq          = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
    std::string waveVhtCenterFreq   = std::to_string((unsigned int)vht_center_frequency);

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.AccessPoint.AssociatedDevice");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", "MACAddress", mac.c_str(), 0, 0);
    
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "CenterFrequency", centerFreq.c_str(), 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "WaveVhtCenterFrequency", waveVhtCenterFreq.c_str(), 0, 0);

    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "OperatingChannelBandwidth", chanBandwidth.c_str(), 0, 0);

    // Delay the first measurement...
    UTILS_SLEEP_MSEC(delay);

    m_unassoc_measure_start         = std::chrono::steady_clock::now();
    m_unassoc_measure_delay         = delay;
    m_unassoc_measure_window_size   = window_size;

    // Trigger a measurement
    if (fapi_wlan_unassociated_devices_info_req(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_unassociated_devices_info_req failed!";
        return false;
    }
    
    return true;
}

bool ap_wlan_hal_fapi::switch_channel(int chan, int bw, int vht_center_frequency) 
{ 
    LOG(TRACE) << __func__ << " channel: " << chan << ", bw: " << bw 
               << ", vht_center_frequency: " << vht_center_frequency;

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Add a new list to the object
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");
    
    // ACS
    if (chan == 0) {
        m_drop_csa = true;
        
         UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "CenterFrequency", "0", 0, 0);    

    // Specific Channel
    } else {
        m_drop_csa = false;
        
        // int bandwidth_int = utils::convert_bandwidth_to_int(bandwidth);
        std::string bandwidth_str = std::to_string(bw);
        std::string ant_num_str = std::to_string(get_radio_info().ant_num);
        int freq = beerocks::utils::wifi_channel_to_freq(chan);
        std::string freq_str = std::to_string(freq);
        std::string wave_vht_center_frequency = std::to_string((unsigned int)vht_center_frequency);

        LOG(DEBUG) << __func__ << ": channel = " << int(chan) 
                   << ", bandwidth = " << bandwidth_str 
                   << ", vht_center_frequency = " << vht_center_frequency;

        // Center Freq
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "CenterFrequency", freq_str.c_str(), 0, 0);

        // Additional List
        UGW_OBJLIST_ADD(wlObj, "Device.WiFi.Radio");
        
        // Extension Channel
        if (bw != 20) {
            UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "ExtensionChannel", 
                ((freq < vht_center_frequency) ? "AboveControlChannel" : "BelowControlChannel"), 0, 0);
        }
        
        // Supported Standard n/ac
        if (bw == 20 || bw == 40) {
            UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "SupportedStandards", "n", 0, 0);
        } else if (bw == 80) {
            UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "SupportedStandards", "ac", 0, 0);
        }

        // Channel bandwidth
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio", "OperatingChannelBandwidth", bandwidth_str.c_str(), 0, 0);
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "WaveVhtCenterFrequency", 
            wave_vht_center_frequency.c_str(), 0, 0);
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "NumOfAntennas", 
            ant_num_str.c_str(), 0, 0);
    }
    
    // Execute the command
    if (fapi_wlan_channel_switch(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_switch_channel failed!";
        wav_fapi_debug_obj(wlObj, "fapi_wlan_channel_switch");
        return false;
    }

    return true;
}

bool ap_wlan_hal_fapi::set_antenna_mode(AntMode mode) 
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

bool ap_wlan_hal_fapi::wds_set_mode(WDSMode mode)
{
    // TODO: Move to FAPI
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " sFourAddrMode " + std::to_string(int(mode));

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;    
}

bool ap_wlan_hal_fapi::wds_add_sta(const std::string& mac)
{
    // TODO: Move to FAPI
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " sAddFourAddrSta " + mac;

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;    
}

bool ap_wlan_hal_fapi::wds_del_sta(const std::string& mac)
{
    // TODO: Move to FAPI
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " sDelFourAddrSta " + mac;

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_fapi::wds_clear_list()
{
    // TODO: Move to FAPI
    // TODO: Check if works... Line extracted from beerocks_utils.sh script
    std::string cmd = "iwpriv " + get_radio_info().iface_name + 
                      " gFourAddrStas | while read -r line; do iwpriv " + 
                      get_radio_info().iface_name + 
                      " sDelFourAddrSta \"$line\"; done";

    // Execute the call
    beerocks::os_utils::system_call(cmd, 2);
    return true;
}

bool ap_wlan_hal_fapi::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    // Channel number of the new channel or ‘0’ to trigger low level channel selection algorithm.
    // 0 is triggeres the same behavior as when the failsafe channel is NOT set.
   
    LOG(TRACE) << __func__;

    std::string bw_str   = std::to_string(bw);
    std::string chan_str = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
    std::string freq_str = std::to_string(vht_center_frequency);
    LOG(DEBUG) << "chan_str = " << chan_str << " bw_str = " << bw_str << " vht_freq_str = " <<  freq_str;
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Transition management parameters
    UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "Channel",                    chan_str.c_str(), 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "CenterFrequency",            freq_str.c_str(), 0, 0);
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "OperatingChannelBandwidth",  bw_str.c_str(), 0, 0);
    
    if (fapi_wlan_failsafe_channel_set(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_failsafe_channel_set failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_fapi::failsafe_channel_get(int& chan, int& bw)
{
    LOG(TRACE) << __func__;

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Read the value from the Radio
    if (fapi_wlan_failsafe_channel_get(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_failsafe_channel_get failed!";
        return false;
    }

    // FAPI_DEBUG_OBJ("get_failsafe_channel()", wlObj);

    // Read the frequency
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "freq", m_temp_fapi_value.get()) != UGW_SUCCESS) {
        LOG(ERROR) << "Failed reading the frequency of the failsafe channel!";
        return false;
    }

    if (!strncmp(m_temp_fapi_value.get(), "ACS", 3)) {
        chan = bw = 0;
        return true;
    }

    // Convert the frequency to channel
    chan = beerocks::utils::wifi_freq_to_channel(beerocks::string_utils::stoi(m_temp_fapi_value.get()));

    // Read the bandwidth
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "bandwidth", m_temp_fapi_value.get()) != UGW_SUCCESS) {
        LOG(ERROR) << "Failed reading the bandwidth of the failsafe channel!";
        return false;
    }

    // Store the bandwidth value
    bw = beerocks::string_utils::stoi(m_temp_fapi_value.get());

    return true;
}

bool ap_wlan_hal_fapi::restricted_channels_set(char* channel_list)
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
    
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    if (!temp_channel_list.empty()) {
        UGW_OBJLIST_ADD(wlObj,       "Device.WiFi.Radio.X_LANTIQ_COM_Vendor");
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "ChannelList", temp_channel_list.c_str(), 0, 0);
    }

    if (fapi_wlan_restricted_channels_set(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE){
        LOG(ERROR) << "fapi_wlan_restricted_channels_set failed!";
        return false;
    }

    return true;
}

bool ap_wlan_hal_fapi::restricted_channels_get(char* channel_list)
{
    LOG(TRACE) << __func__;
    
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    memset(channel_list, 0, beerocks::message::RESTRICTED_CHANNEL_LENGTH);

    if (fapi_wlan_restricted_channels_get(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_restricted_channels_get failed!";
        return false;
    }

    // FAPI_DEBUG_OBJ("get_restricted_channels", wlObj);

    auto tmpBuff = m_temp_fapi_value.get();
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "restricted_channels", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "Failed reading restricted_channels parameter!";
        return false;
    }

    // Convert the string to an array of bytes (int values)
    std::stringstream channel_list_stream(tmpBuff);
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

bool ap_wlan_hal_fapi::read_acs_report() 
{ 
    LOG(TRACE) << __func__ << " for interface: " << get_radio_info().iface_name;

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Get the ACS report object
    if (fapi_wlan_acs_report_get(get_radio_info().iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_acs_report_get failed!";
        return false;
    }

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

    size_t channel_idx=0;
    std::string param, value;
    bool save_info=false;
    int channel=0, next_channel=0, bw=0, tx_pow=0, dfs=0, bss_overlap=0, noise=0;
    ObjList* currentObj;
    ParamList* currentParam;

    // wav_fapi_debug_obj(wlObj, "read_acs_report() ");

    FOR_EACH_OBJ(wlObj.get(), currentObj) {
        FOR_EACH_PARAM(currentObj, currentParam) {
            
            // LOG(DEBUG) << "param->" << std::string(GET_PARAM_NAME(currentParam)) 
            //            <<" , value->" << std::string(GET_PARAM_VALUE(currentParam));

            param.assign(GET_PARAM_NAME(currentParam));
            if (param.find("ChannelLoad") != std::string::npos) {
                if (bw == 20) {
                    save_info = true;
                }
            } else if (param.find("IsDfsChannel") != std::string::npos) {
                value.assign(GET_PARAM_VALUE(currentParam));
                dfs = beerocks::string_utils::stoi(value);
            } else if (param.find("TxPowerLevel") != std::string::npos) {
                value.assign(GET_PARAM_VALUE(currentParam));
                tx_pow = beerocks::string_utils::stoi(value);
            } else if(param.find("Noisessloor") != std::string::npos) {
                value.assign(GET_PARAM_VALUE(currentParam));
                noise = beerocks::string_utils::stoi(value);
            } else if(param.find("OverlapCh") !=  std::string::npos) {
                value.assign(GET_PARAM_VALUE(currentParam));
                bss_overlap = beerocks::string_utils::stoi(value);
            } else if(param.find("OperatingChannelBandwidth") != std::string::npos) {
                value.assign(GET_PARAM_VALUE(currentParam));
                bw = beerocks::string_utils::stoi(value);
                channel = next_channel;
            } else if(param.find("Channel") != std::string::npos) {
                value.assign(GET_PARAM_VALUE(currentParam));
                next_channel = beerocks::string_utils::stoi(value);
            }

            // Store the information
            if (save_info && (channel_idx < MAX_SUPPORTED_20M_CHANNELS)) {
                save_info = false;

                if (channel > 14) { 
                    m_radio_info.is_5ghz = true; 
                }

                m_radio_info.supported_channels[channel_idx].channel        = channel;
                m_radio_info.supported_channels[channel_idx].noise          = noise;
                m_radio_info.supported_channels[channel_idx].bss_overlap    = bss_overlap;
                m_radio_info.supported_channels[channel_idx].is_dfs         = dfs;
                m_radio_info.supported_channels[channel_idx].bandwidth      = bw;
                m_radio_info.supported_channels[channel_idx].tx_pow         = tx_pow;

                channel_idx++;
            }
        }
    }

    return true;
}

std::string ap_wlan_hal_fapi::get_radio_driver_version()
{
    return beerocks::version::get_wave_version();
}

bool ap_wlan_hal_fapi::set_vap_enable(const std::string& iface_name, const bool enable)
{
    LOG(TRACE) << __func__ << " iface: " << iface_name << ", enable=" << int(enable);

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    LOG(DEBUG) << "TODO: fill command object params before sending command";
    
    // if (fapi_wlan_vap_enable_set(iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
    //     LOG(ERROR) << "fapi_wlan_acs_report_get failed!";
    //     return false;
    // }

    return true;
}

bool ap_wlan_hal_fapi::get_vap_enable(const std::string& iface_name, bool& enable)
{
    LOG(TRACE) << __func__ << " iface: " << iface_name << ", enable=" << int(enable);

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // if (fapi_wlan_vap_enable_get(iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
    //     LOG(ERROR) << "fapi_wlan_acs_report_get failed!";
    //     return false;
    // }

    LOG(DEBUG) << "TODO: extract object params";
    return true;
}


bool ap_wlan_hal_fapi::parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj)
{
    // Filter out empty events
    if (opcode.empty()) { return true; }

    auto event = fapi_to_bwl_event(opcode);

    if (event == Event::Invalid) { return true; }
    
    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";
    auto wlObj = std::static_pointer_cast<ObjList>(obj);
    auto tmpBuff = m_temp_fapi_value.get();

    if (!wlObj) {
        LOG(ERROR) << "FAPI Object is NULL!";
        return false;
    }

    switch (event) {
        
        // TODO: ???
        case Event::AP_Enabled: {
        } break;

        // ACS/CSA Completed
        case Event::ACS_Completed: 
        case Event::CSA_Finished: {

            if (event == Event::CSA_Finished) {
                if (std::chrono::steady_clock::now() < (m_csa_event_filtering_timestamp + 
                    std::chrono::milliseconds(CSA_EVENT_FILTERING_TIMEOUT_MS)) && m_drop_csa) {
                    LOG(DEBUG) << "CSA_Finished - dump event - arrive on csa filtering timeout window";
                    return true;
                }
            } else {
                m_csa_event_filtering_timestamp = std::chrono::steady_clock::now();
            }
            
            int chan, bw, channel_ext_above_primary, vht_center_freq, is_dfs_channel;
            ChanSwReason chanSwReason = ChanSwReason::Unknown;

            if (!fapi_read_int(wlObj, "Device.WiFi.Radio", "Channel", tmpBuff, chan)) {
                LOG(ERROR) << "Failed reading Channel parameter!";
                return false;
            }

            if (chan <= 0) {
                LOG(ERROR) << "channel is not valid!! channel = " << int(chan);
                // TODO - Remove once fapi bug is fixed!!!
                //HACK fixing fapi string mismatch return "channel=value " suppose to be just "value"
                auto vec = beerocks::string_utils::str_split(tmpBuff , '=');
                for(auto str : vec) {
                LOG(DEBUG) << "str = " << str;
                chan = beerocks::string_utils::stoi(str);
                }
                if (chan <= 0) {
                        LOG(ERROR) << "channel is not valid!! channel = " << int(chan);
                        return false;
                }
            }

            if (!fapi_read_int(wlObj, "Device.WiFi.Radio", "OperatingChannelBandwidt", tmpBuff, bw)) {
                LOG(ERROR) << "Failed reading OperatingChannelBandwidt parameter!";
                return false;
            }

            if (!fapi_read_int(wlObj, "Device.WiFi.Radio", "ExtensionChannel", tmpBuff, channel_ext_above_primary)) {
                LOG(ERROR) << "Failed reading ExtensionChannel parameter!";
                return false;
            }

            // TODO: Should be read on CSA event as well?
            if (!fapi_read_int(wlObj, "Device.WiFi.Radio", "cf1", tmpBuff, vht_center_freq)) {
                LOG(ERROR) << "Failed reading cf1 parameter!";
                return false;
            }
            
            if (!fapi_read_int(wlObj, "Device.WiFi.Radio", "dfs_chan", tmpBuff, is_dfs_channel)) {
                LOG(ERROR) << "Failed reading dfs_chan parameter!";
                return false;
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio", 0, "reason", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed reading reason parameter!";
                return false;
            }

            
            wav_fapi_debug_obj(wlObj, "CSA_Finished");

            // Channel switch reason
            auto tmpStr = std::string(tmpBuff);
            if (tmpStr.find("RADAR") != std::string::npos) {
                chanSwReason = ChanSwReason::Radar;
            } else if (tmpStr.find("20_COEXISTANCE") != std::string::npos) {
                chanSwReason = ChanSwReason::CoEx_20;
            } else if(tmpStr.find("40_COEXISTANCE") != std::string::npos) {
                chanSwReason = ChanSwReason::CoEx_40;
            } 

            //CSA for the same channel and bw ignore.
            if( m_radio_info.channel == chan && 
            m_radio_info.bandwidth == bw && 
            event == Event::CSA_Finished &&
            chanSwReason != ChanSwReason::Radar
            ) {
                LOG(DEBUG) << "CSA_Finished for the same channel and bw ignore" << std::endl
                           << " m_radio_info.channel = " << int(m_radio_info.channel) << " chan = " << int(chan)
                           << "m_radio_info.bandwidth = " << int(m_radio_info.bandwidth) << " bw = " << int(bw); 
                return false;
            }
            
            // Update the radio information structure
            m_radio_info.channel = chan;
            if (chan > 14) { m_radio_info.is_5ghz = true; }
            m_radio_info.bandwidth = bw;
            m_radio_info.channel_ext_above = channel_ext_above_primary;
            m_radio_info.vht_center_freq = vht_center_freq;
            m_radio_info.is_dfs_channel = bool(is_dfs_channel);
            m_radio_info.last_csa_sw_reason = chanSwReason;

            event_queue_push(event);
            
            // TODO: Revise this
            if (event == Event::ACS_Completed) {
                LOG(DEBUG) << "ACS_COMPLETED >>> adding CSA_FINISHED event as well";
                event_queue_push(Event::CSA_Finished);
            }
            
        } break;

        // TODO: Need to replace to permanent solution
        // WORKAROUND!!!
        // driver send CSA in an Inconsistent way , so for the first cac iterupted by radar
        // beerocks generate CSA event (driver do not send this event).
        case Event::DFS_RADAR_Detected: {
          
            LOG(DEBUG) << "DFS-RADAR-Detected " << std::endl << " wifi_ctrl_enabled = " << int(m_radio_info.wifi_ctrl_enabled) 
                           << " channel = " << int(m_radio_info.channel) 
                           << " bandwidth = " << int(m_radio_info.bandwidth)
                           << " wireless_utils::is_dfs_channel(m_radio_info.channel) " << int(son::wireless_utils::is_dfs_channel(m_radio_info.channel));

            if (m_radio_info.wifi_ctrl_enabled == 0 && //ap value on first cac interupted by radar.
               son::wireless_utils::is_dfs_channel(m_radio_info.channel)) {
                int ch,bw;
                //double event protection.
                m_drop_csa=true;
                m_csa_event_filtering_timestamp = std::chrono::steady_clock::now();
                //
                if (!failsafe_channel_get(ch,bw)) {
                	LOG(ERROR) << "failsafe_channel_get failed!";
                	return false;
                }
                m_radio_info.channel = ch;
                m_radio_info.bandwidth = bw;
                m_radio_info.vht_center_freq = beerocks::utils::wifi_channel_to_vht_center_freq(ch,bw,true);
                m_radio_info.last_csa_sw_reason = ChanSwReason::Radar; 
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
            
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "Name", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }
            
            msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(tmpBuff).vap_id;
            LOG(DEBUG) << "iface name = " << tmpBuff << ", vap_id = " << int(msg->params.vap_id);

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }
            
            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmpBuff);

            bool caps_valid = true;
            SRadioCapabilitiesStrings caps_strings;
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "SupportedRates", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "SupportedRates not valid";
                caps_valid = false;
            } else {            
                caps_strings.supported_rates.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "HT_CAP", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "HT_CAP not valid";
                caps_valid = false;
            } else {            
                caps_strings.ht_cap.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "HT_MCS", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "HT_MCS not valid";
                caps_valid = false;
            } else {            
                caps_strings.ht_mcs.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "VHT_CAP", tmpBuff) != UGW_SUCCESS) {
                // optional, not an error 
            } else {            
                caps_strings.vht_cap.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "VHT_MCS", tmpBuff) != UGW_SUCCESS) {
                // optional, not an error 
            } else {            
                caps_strings.vht_mcs.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "btm_supported", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "btm_supported not valid";
                caps_valid = false;
            } else {            
                caps_strings.btm_supported.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "nr_enabled", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "nr_enabled not valid";
                caps_valid = false;
            } else {            
                caps_strings.nr_enabled.assign(tmpBuff);
            }

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "cell_capa", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "cell_capa not valid";
                caps_valid = false;
            } else {            
                caps_strings.cell_capa.assign(tmpBuff);
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

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "Name", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }

            msg->params.vap_id = beerocks::utils::get_ids_from_iface_string(tmpBuff).vap_id;
            LOG(DEBUG) << "iface name = " << tmpBuff << ", vap_id = " << int(msg->params.vap_id);

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Failed parsing FAPI object!";
                return false;
            }
            
            // Store the MAC address of the disconnected STA
            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmpBuff);
            
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
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }
            
            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmpBuff);

            // Read the RSSI value
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "SignalStrength", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "SignalStrength field is missing from the FAPI object!";
                return false;
            }
            
            // Split the RSSI values
            auto rssiVec = beerocks::string_utils::str_split(tmpBuff, ' ');

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
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice.Stats", 0, "PacketsReceived", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "PacketsReceived field is missing from the FAPI object!";
                return false;
            }
            
            int val = beerocks::string_utils::stoi(tmpBuff);
            msg->params.rx_packets = (val >= 127) ? 127 : val;
    
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

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.AssociatedDevice", 0, "MACAddress", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "MACAddress field is missing from the FAPI object!";
                return false;
            }

            msg->params.mac = beerocks::net::network_utils::mac_from_string(tmpBuff);

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.AccessPoint.X_LANTIQ_COM_Vendor.AssociatedDevice", 0, "status_code", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "status_code field is missing from the FAPI object!";
                return false;
            }

            msg->params.status_code = beerocks::string_utils::stoi(tmpBuff);
            
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
            
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "success", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "success field is missing from the FAPI object!";
                return false;
            }
            
            msg->params.success = beerocks::string_utils::stoi(tmpBuff);

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "freq", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "freq field is missing from the FAPI object!";
                return false;
            }
            
            uint32_t freq = beerocks::string_utils::stoi(tmpBuff);
            msg->params.channel = beerocks::utils::wifi_freq_to_channel(freq);
            msg->params.frequency = freq;

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "timeout", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "timeout field is missing from the FAPI object!";
                return false;
            }

            msg->params.timeout = beerocks::string_utils::stoi(tmpBuff);

            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "chan_width", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "chan_width field is missing from the FAPI object!";
                return false;
            }

            msg->params.bandwidth = fapi_bw_to_beerocks_bw(tmpBuff);

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
            
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "freq", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "freq field is missing from the FAPI object!";
                return false;
            }
            
            uint32_t freq = beerocks::string_utils::stoi(tmpBuff);
            msg->params.channel = beerocks::utils::wifi_freq_to_channel(freq);
            msg->params.frequency = freq;
            
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "chan_width", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "chan_width field is missing from the FAPI object!";
                return false;
            }

            msg->params.bandwidth = fapi_bw_to_beerocks_bw(tmpBuff);
            
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", 0, "cf1", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "cf1 field is missing from the FAPI object!";
                return false;
            }
            
            msg->params.vht_center_frequency = beerocks::string_utils::stoi(tmpBuff);

            // Add the message to the queue
            event_queue_push(Event::DFS_NOP_Finished, msg_buff);            
            
        } break;

        case Event::AP_Disabled:
        {
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sHOSTAP_DISABLED_NOTIFICATION));
            auto msg = reinterpret_cast<sHOSTAP_DISABLED_NOTIFICATION*>(msg_buff.get());
            LOG_IF(!msg, FATAL) << "Memory allocation failed!";

            // Read and store the MAC address
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.Radio", 0, "Name", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "Iface Name field is missing from the FAPI object!";
                return false;
            }

            auto iface_ids = beerocks::utils::get_ids_from_iface_string(tmpBuff);

            msg->vap_id = iface_ids.vap_id;
            LOG(DEBUG) << "AP_Disabled for interface " << tmpBuff << " VAP_id = " << int(msg->vap_id);

            event_queue_push(Event::AP_Disabled, msg_buff);  

        } break;
        case Event::Interface_Disabled:
        case Event::ACS_Failed: {
            // Forward to the AP manager
            event_queue_push(event);            
        } break;

        // Gracefully ignore unhandled events
        // TODO: Probably should be changed to an error once FAPI will stop
        //       sending empty or irrelevant events...
        default: {
            // LOG(WARNING) << "Unhandled event received: " << opcode;
            return true;

        } break;
    }

    return true;
}

} // namespace wav_fapi
} // namespace bwl

// AP FAPI HAL Factory Functions
extern "C" {
bwl::ap_wlan_hal* ap_wlan_hal_create(
    std::string iface_name, bool acs_enabled, bwl::base_wlan_hal::hal_event_cb_t callback)
{ return new bwl::wav_fapi::ap_wlan_hal_fapi(iface_name, acs_enabled, callback); }

void ap_wlan_hal_destroy(bwl::ap_wlan_hal* obj)
{ delete obj; }

}
