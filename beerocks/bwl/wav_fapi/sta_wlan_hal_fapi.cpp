/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_fapi.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include "ugw_helper.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// FAPI ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define LOGGING_ID sta_wlan_hal_fapi
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

// Client connection status
struct ConnectionStatus {
    std::string bssid;
    int freq;
    std::string ssid;
    std::string id;
    std::string mode;
    std::string pairwise_cipher;
    std::string group_cipher;
    std::string key_mgmt;
    std::string wpa_state;
    std::string address;
    std::string uuid;
};
        
//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static sta_wlan_hal::Event fapi_to_bwl_event(const std::string& opcode)
{
    if (opcode == "CTRL-EVENT-CONNECTED") {
        return sta_wlan_hal::Event::Connected;
    } else if (opcode == "CTRL-EVENT-DISCONNECTED") {
        return sta_wlan_hal::Event::Disconnected;
    } else if (opcode == "CTRL-EVENT-TERMINATING") {
        return sta_wlan_hal::Event::Terminating;
    } else if (opcode == "CTRL-EVENT-SCAN-RESULTS") {
        return sta_wlan_hal::Event::ScanResults;
    } else if (opcode == "CTRL-EVENT-CHANNEL-SWITCH") {
        return sta_wlan_hal::Event::ChannelSwitch;
    } else if (opcode == "UNCONNECTED-STA-RSSI") {
        return sta_wlan_hal::Event::STA_Unassoc_RSSI;
    }

    return sta_wlan_hal::Event::Invalid;
}

// Convert BWL to FAPI security string
static std::string fapi_security_val(WiFiSec sec)
{
    switch (sec)
    {
        case WiFiSec::None:          return "None";
        case WiFiSec::WEP_64:        return "WEP-64";
        case WiFiSec::WEP_128:       return "WEP-128";
        case WiFiSec::WPA_PSK:       return "WPA-Personal";
        case WiFiSec::WPA2_PSK:      return "WPA2-Personal";
        case WiFiSec::WPA_WPA2_PSK:  return "WPA-WPA2-Personal";

        default: return "INVALID";
    }    
}

static int fapi_str_to_int(char* str, bool ignore_unknown = true)
{
    static const std::string unknown_string = "UNKNOWN";

    if (!unknown_string.compare(str) && ignore_unknown) {
        return 0;
    }

    return beerocks::string_utils::stoi(str);
}

// Add a new network and return the ID
static int add_network(const std::string& iface, char* tmpBuff)
{
    LOG(TRACE) << __func__ << " iface: " << iface;

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Add a new network
    if (fapi_wlan_client_mode_network_add(iface.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "add_network - fapi_wlan_client_mode_network_add failed";
        return -1;
    }

    // Extract the network ID
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_NetworkId", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "Failed parsing FAPI object!";
        return false;
    }

    // Return the newly added network ID
    return beerocks::string_utils::stoi(tmpBuff);
}

// Update network parameters
static bool set_network(const std::string& iface, const std::string& network_id, const std::string& ssid, 
    const std::string& bssid, WiFiSec sec, const std::string& pass, bool hidden_ssid)
{
    LOG(TRACE) << __func__ << ": iface = " << iface << ", network_id = " << network_id;

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Network ID
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint", "X_LANTIQ_COM_Vendor_NetworkId", network_id.c_str(), 0, 0);

    // SSID & BSSID
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint.Profile");
    if (ssid.length() > 0) {
        std::string fapi_ssid = "\"" + ssid + "\"";
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile", "SSID", 
            std::string("\"" + ssid + "\"").c_str(), 0, 0);
    }
    
    if (bssid.length() > 0) {
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_BSSID", bssid.c_str(), 0, 0);
    }

    if (hidden_ssid) {
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_scan_ssid", "1", 0, 0);
    }

    // Security and Password
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint.Profile.Security");
    if (sec != WiFiSec::Invalid) {
        UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile.Security", "ModeEnabled", fapi_security_val(sec).c_str(), 0, 0);
    }

    switch (sec)
    {
        // WEP
        case WiFiSec::WEP_64:
        case WiFiSec::WEP_128: {
            UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile.Security", "WEPKey", 
                std::string("\"" + pass + "\"").c_str(), 0, 0);
        } break;

        // WPA
        case WiFiSec::WPA_PSK:
        case WiFiSec::WPA2_PSK:
        case WiFiSec::WPA_WPA2_PSK: {
            UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile.Security", "KeyPassphrase", 
                std::string("\"" + pass + "\"").c_str(), 0, 0);
        } break;

        // Do nothing...
        default: break;
    }

    //FAPI_DEBUG_OBJ("set_network", wlObj);

    // Set network values
    if (fapi_wlan_client_mode_network_set(iface.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "set_network - fapi_wlan_client_mode_network_set failed";
        return false;
    }

    return true;
}

static bool select_network(const std::string& iface, const std::string& network_id, const std::string& freq)
{
    LOG(TRACE) << __func__ << " entered for " << iface;

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Network ID
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint", "X_LANTIQ_COM_Vendor_NetworkId", network_id.c_str(), 0, 0);

    // Frequency
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint.Profile");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_Frequency", freq.c_str(), 0, 0);

    if (fapi_wlan_client_mode_network_select(iface.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "select_network - fapi_wlan_client_mode_network_select failed";
        return false;
    }

    return true;
}

static bool read_status(const std::string& iface, ConnectionStatus& connection_status, char* tmpBuff)
{
    LOG(TRACE) << __func__ << " entered for " << iface;

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    if (fapi_wlan_client_mode_connection_status_get(iface.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - fapi_wlan_client_mode_connection_status_get failed for " << iface;
        return false;
    }

    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_bssid", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_bssid";
        return false;
    }
    connection_status.bssid = std::string(tmpBuff);
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_freq", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_freq";
        return false;
    }
    connection_status.freq = fapi_str_to_int(tmpBuff);
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_ssid", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_SSID";
        return false;
    }
    connection_status.ssid = tmpBuff;

    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_id", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_id";
        return false;
    }
    connection_status.id = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_mode", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_mode";
    }
    connection_status.mode = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_pairwise_cipher", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_PairwiseCipher";
    }
    connection_status.pairwise_cipher = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_group_cipher", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_GroupCipher";
    }
    connection_status.group_cipher = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_key_mgmt", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_KeyMgmt";
    }
    connection_status.key_mgmt = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_wpa_state", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_WpaState";
    }
    connection_status.wpa_state = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_address", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_address";
    }
    connection_status.address = tmpBuff;
    
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_uuid", tmpBuff) == UGW_FAILURE) {
        LOG(ERROR) << "read_status - results missing X_LANTIQ_COM_Vendor_uuid";
    }
    connection_status.uuid = tmpBuff;
    
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

sta_wlan_hal_fapi::sta_wlan_hal_fapi(std::string iface_name, hal_event_cb_t callback) :
    base_wlan_hal(bwl::HALType::Station, iface_name, IfaceType::Intel, false, callback),
    base_wlan_hal_fapi(bwl::HALType::Station, iface_name, IfaceType::Intel, false, callback)
{
    // Pointer for parsing FAPI object values
    m_temp_fapi_value = ALLOC_SMART_BUFFER(MAX_LEN_PARAM_VALUE);
}

sta_wlan_hal_fapi::~sta_wlan_hal_fapi()
{
    detach();
}

bool sta_wlan_hal_fapi::detach()
{
    LOG(TRACE) << __func__ << ": iface = " << get_iface_name();
    // Disconnect before detaching
    disconnect();

    return base_wlan_hal_fapi::detach();
}

bool sta_wlan_hal_fapi::initiate_scan()
{
    auto& iface_name = get_iface_name();
    LOG(DEBUG) << "Initiating scan on interface: " << iface_name;
    
    // Start the scan
    if (fapi_wlan_client_mode_scan_start(iface_name.c_str(), nullptr, 0) == UGW_FAILURE) {
        LOG(ERROR) << "initiate_scan - fapi_wlan_client_mode_scan_start failed for " << iface_name;
        return false;
    }

    return true;
}

int  sta_wlan_hal_fapi::get_scan_results(const std::string& ssid, SScanResult* list, int size)
{
    // Validate input parameters
    if (!list || !size) {
        LOG(ERROR) << "Invalid scan results list " << list << " (" << size << ")";
        return 0;
    }
   
    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Read the scan results
    if (fapi_wlan_client_mode_scan_results_get(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "get_scan_results - fapi_wlan_client_mode_scan_results_get failed for " << get_iface_name();
        return 0;
    }

    //FAPI_DEBUG_OBJ("scan_results", wlObj.get());

    // Iterate over the objects
    ObjList*    curObj = nullptr;
    int         num_of_results = 0;
    FOR_EACH_OBJ(wlObj.get(), curObj) {

        // object list contains two objects - each contain numbered parameters with a different index for each network.
        auto full_object_name = std::string(GET_OBJ_NAME(curObj));

        // Only handle EP objects
        if (full_object_name != "Device.WiFi.EndPoint.Profile") {
            LOG(DEBUG) << "Skipping object: " << full_object_name;
            continue;            
        }

        // Temporary variables
        std::string scan_ssid;
        int current_index = 0;
        int new_index = -1;

        ParamList* curParam = nullptr;
        FOR_EACH_PARAM(curObj, curParam) {

            // Read parameter name and value
            std::string full_param_name = std::string(GET_PARAM_NAME(curParam));
            std::string param_value     = std::string(GET_PARAM_VALUE(curParam));

            /*
                Sample values:

                0_X_LANTIQ_COM_Vendor_BSSID value = ac:9a:96:f4:3e:f0
                0_X_LANTIQ_COM_Vendor_Frequency value = 2437
                0_X_LANTIQ_COM_Vendor_SignalLevel value = -24
                0_SSID value = beerocks_omer freqband=2G netmode=bgn
                1_X_LANTIQ_COM_Vendor_BSSID value = ac:9a:96:f4:3e:c0
                1_X_LANTIQ_COM_Vendor_Frequency value = 2437
                1_X_LANTIQ_COM_Vendor_SignalLevel value = -27
                1_SSID value = beerocks_demo_vit freqband=2G netmode=bgn
                2_X_LANTIQ_COM_Vendor_BSSID value = 84:3d:c6:6b:03:a7
                2_X_LANTIQ_COM_Vendor_Frequency value = 2462
                2_X_LANTIQ_COM_Vendor_SignalLevel value = -36
                2_SSID value = LabWLAN freqband=2G netmode=bgn
                3_X_LANTIQ_COM_Vendor_BSSID value = ac:9a:96:f4:1c:50
                3_X_LANTIQ_COM_Vendor_Frequency value = 2437
                3_X_LANTIQ_COM_Vendor_SignalLevel value = -25
                3_SSID value = beerocks_rob freqband=2G netmode=bgn
            */

            std::string str_index = full_param_name.substr(0, full_param_name.find_first_of('_'));
            new_index = beerocks::string_utils::stoi(str_index);
            //LOG(TRACE) << "new_index = " << new_index << ", current_index = " << current_index;

            if (new_index > current_index) {               
                if (scan_ssid == ssid) {
                    if (++num_of_results >= size) {
                        return num_of_results;
                    }
                }

                current_index = new_index;
            }

            if (full_param_name.find("BSSID") != std::string::npos) {
                beerocks::net::network_utils::mac_from_string(list[num_of_results].bssid, param_value);
            } else if (full_param_name.find("Frequency") != std::string::npos) {
                list[num_of_results].channel = beerocks::utils::wifi_freq_to_channel(
                    beerocks::string_utils::stoi(param_value));
            } else if (full_param_name.find("SignalLevel") != std::string::npos) {
                list[num_of_results].rssi = beerocks::string_utils::stoi(param_value);
            } else if (full_param_name.find("SSID") != std::string::npos) {
                scan_ssid = param_value.substr(0, param_value.find(" freqband="));
            }

        } // FOR_EACH_PARAM(curObj, curParam)

        // need this so that we don't miss the last result in the list
        if (new_index >= 0) {
            if (scan_ssid == ssid) {
                ++num_of_results;
            }
        }

    } // FOR_EACH_OBJ(wlObj, curObj)

    return num_of_results;
}

bool sta_wlan_hal_fapi::connect(const std::string& ssid, const std::string& pass, 
    WiFiSec sec, const std::string& bssid, uint8_t channel, bool hidden_ssid)
{
    LOG(DEBUG) << __func__ << ": iface " << get_iface_name() << " to SSID = " << ssid 
               << ", BSSID = " << bssid << ", Channel = " << int(channel) 
               << ", Sec = " << fapi_security_val(sec);

    // First disconnect (or do nothing if not connected)
    if (!disconnect()) { 
        LOG(WARNING) << "connect - Failed disconnecting"; 
        return false;
    }

    // Add a new network
    auto network_id = add_network(get_iface_name(), m_temp_fapi_value.get());
    if (network_id < 0) {
        LOG(ERROR) << "Failed (" << network_id << ") adding new network to interface: " << get_iface_name();
        return false;
    }

    // Update network parameters
    if (!set_network(get_iface_name(), std::to_string(network_id), ssid, bssid, sec, pass, hidden_ssid)) {
        LOG(ERROR) << "Failed setting network id = " << network_id << " on interface: " << get_iface_name();
        return false;
    }

    // Enable the network
    auto freq = std::to_string(beerocks::utils::wifi_channel_to_freq(int(channel)));
    if (!select_network(get_iface_name(), std::to_string(network_id), freq)) {
        LOG(ERROR) << "Failed enabling network id = " << network_id << " on interface: " << get_iface_name();
        return false;
    }

    // Update active network parameters
    m_active_ssid.assign(ssid);
    m_active_bssid.assign(bssid);
    m_active_pass.assign(pass);
    m_active_channel    = channel;
    m_active_security   = sec;
    m_active_network_id = network_id;

    return true;
}

bool sta_wlan_hal_fapi::disconnect()
{
    LOG(TRACE) << __func__ << ": iface = " << get_iface_name() << ", m_active_network_id = " << m_active_network_id;

    // Return gracefully if no network is connected
    if (m_active_network_id < 0) {
        LOG(DEBUG) << "Active network does not exist"; 
        return true; 
    }

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Network ID
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint", "X_LANTIQ_COM_Vendor_NetworkId", 
        std::to_string(m_active_network_id).c_str(), 0, 0);

    //FAPI_DEBUG_OBJ("client_mode_network_remove", wlObj.get());

    // if (fapi_wlan_client_mode_network_remove(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
    //     LOG(ERROR) << "disconnect - fapi_wlan_client_mode_network_remove failed for network_id = " 
    //                << m_active_network_id << " on interface: " << get_iface_name();

    //     return false;
    // }

    bool res = fapi_wlan_client_mode_network_remove(get_iface_name().c_str(), wlObj.get(), 0);
        
    if (res == UGW_FAILURE) {
        LOG(ERROR) << "disconnect - fapi_wlan_client_mode_network_remove failed for network_id = " 
                   << m_active_network_id << " on interface: " << get_iface_name();
    }

    // Clear state
    m_active_ssid       = "";
    m_active_bssid      = "";
    m_active_pass       = "";
    m_active_channel    =  0;
    m_active_security   = WiFiSec::None;
    m_active_network_id = -1;
    
    return (res != UGW_FAILURE);
}

bool sta_wlan_hal_fapi::roam(const std::string& bssid, uint8_t channel)
{
    auto& iface = get_iface_name();
    LOG(TRACE) << __func__ << ": iface = " << iface << " to bssid " << bssid << " on channel " << int(channel);

    if (m_active_network_id < 0) {
        LOG(ERROR) << "Roam failed - not connected!";
        return false;
    }

    // Roam part 1 (send_wpa_sup_command on interface with "BSSID x yy:yy:yy:yy:yy:yy")
    // NOTE: Apparently don't need the ssid, ModeEnabled, KeyPassphrase, or WEPKey parameters here.
    if (!set_network(iface, std::to_string(m_active_network_id), "", bssid, WiFiSec::Invalid, "", false)) {
        LOG(ERROR) << "roam - set_network failed for " << iface;
        return false;
    }

    // Roam part 2 (send_wpa_sup_cmd with "ENABLE_NETWORK x")
    std::string freq = std::to_string(beerocks::utils::wifi_channel_to_freq(channel));
    if (!select_network(iface, std::to_string(m_active_network_id), freq)) {
        LOG(ERROR) << "roam - select_network failed for " << iface;
        return false;
    }

    // Update the active channel and bssid
    m_active_bssid.assign(bssid);
    m_active_channel = channel;

    return true;
}

bool sta_wlan_hal_fapi::get_4addr_mode()
{
    LOG(TRACE) << __func__ << ": iface = " << get_iface_name();

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Read the routing mode
    if (fapi_wlan_client_mode_data_routing_mode_get(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_client_mode_data_routing_mode_get failed";
        return false;
    }

    auto tmpBuff = m_temp_fapi_value.get();

    // Extract the network ID
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint.Profile", 0, "X_LANTIQ_COM_Vendor_RoutingMode", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "Failed parsing FAPI object!";
        return false;
    }
    
    return (!std::string(tmpBuff).compare("4_ADDRESS_MODE"));
}

bool sta_wlan_hal_fapi::set_4addr_mode(bool enable)
{
    LOG(TRACE) << __func__ << ": iface = " << get_iface_name() << ", enable: " << enable;

    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Network ID
    UGW_OBJLIST_ADD(wlObj, "Device.WiFi.EndPoint.Profile");
    UGW_OBJLIST_EDIT_NODE(wlObj, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_RoutingMode", 
        ((enable) ? "4_ADDRESS_MODE" : "3_ADDRESS_MODE"), 0, 0);

    // Set the routing mode
    if (fapi_wlan_client_mode_data_routing_mode_set(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_client_mode_data_routing_mode_set failed";
        return false;
    }
    
    return true;
}

bool sta_wlan_hal_fapi::unassoc_rssi_measurement(const std::string& mac, int chan, int bw, int vht_center_frequency, int delay, int window_size)
{
    LOG(TRACE) << __func__ << " mac: " << mac << ", channel: " << chan 
               << ", bw: " << bw << ", vht_center_frequency: " << vht_center_frequency 
               << ", delay: " << delay;

    // Convert values to strings
    std::string chanBandwidth       = std::to_string(bw);
    std::string centerFreq          = std::to_string(beerocks::utils::wifi_channel_to_freq(chan));
    std::string waveVhtCenterFreq   = std::to_string(vht_center_frequency);

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
    if (fapi_wlan_unassociated_devices_info_req(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_unassociated_devices_info_req failed!";
        return false;
    }
    
    return true;
}

bool sta_wlan_hal_fapi::is_connected()
{
    // Allocate a new FAPI object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    // Read connection status
    if (fapi_wlan_client_mode_connection_status_get(get_iface_name().c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_client_mode_connection_status_get failed for " << get_iface_name();
        return false;
    }

    auto tmpBuff = m_temp_fapi_value.get();

    // Read the WPA connection status
    if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_wpa_state", tmpBuff) != UGW_SUCCESS) {
        LOG(ERROR) << "Failed parsing FAPI object!";
        return false;
    }

    return (!std::string(tmpBuff).compare("COMPLETED"));
}

int  sta_wlan_hal_fapi::get_rssi()
{
    return beerocks::RSSI_INVALID;
}

int  sta_wlan_hal_fapi::get_channel()
{
    return m_active_channel;
}

std::string sta_wlan_hal_fapi::get_ssid()
{
    return m_active_ssid;
}

std::string sta_wlan_hal_fapi::get_bssid()
{
    return m_active_bssid;
}

bool sta_wlan_hal_fapi::parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj)
{
    // Filter out empty events
    if (opcode.empty()) { return true; }

    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";
    
    auto event = fapi_to_bwl_event(opcode);
    auto wlObj = std::static_pointer_cast<ObjList>(obj);
    auto tmpBuff = m_temp_fapi_value.get();

    if (!wlObj) {
        LOG(ERROR) << "FAPI Object is NULL!";
        return false;
    }

    switch (event)
    {
        // Client Connected
        case Event::Connected: {

            ConnectionStatus connection_status;
            if (!read_status(get_iface_name(), connection_status, tmpBuff)) {
                LOG(ERROR) << "Failed reading connection status for iface: " << get_iface_name();
                return false;
            }

            LOG(DEBUG) << get_iface_name() << " - Connected: bssid = " << connection_status.bssid 
                       << ", freq = " << connection_status.freq;

            m_active_bssid.assign(connection_status.bssid);
            m_active_channel = beerocks::utils::wifi_freq_to_channel(connection_status.freq);

            // Forward the event
            event_queue_push(event);
            
        } break;

        // Client disconnected
        case Event::Disconnected: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));
            auto msg = reinterpret_cast<sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION*>(msg_buff.get());
            if (!msg) {
                LOG(FATAL) << "Memory allocation failed!";
                return false;
            }
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));
    
            // Read the disconnect reason value
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.EndPoint", 0, "X_LANTIQ_COM_Vendor_Reason", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "X_LANTIQ_COM_Vendor_Reason field is missing from the FAPI object!";
                return false;
            }
            msg->disconnect_reason = beerocks::string_utils::stoi(tmpBuff);

            // Read the BSSID
            if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "BSSID", tmpBuff) != UGW_SUCCESS) {
                LOG(ERROR) << "X_LANTIQ_COM_Vendor_Reason field is missing from the FAPI object!";
                return false;
            }
            msg->bssid = beerocks::net::network_utils::mac_from_string(tmpBuff);

            // Forward the event
            event_queue_push(event);
            
        } break;

        // Do nothing for the following events
        case Event::Terminating:
        case Event::ScanResults:
        case Event::ChannelSwitch: {
            // Forward the event
            event_queue_push(event);

        } break;

        // Unassociated STA Measurement
        case Event::STA_Unassoc_RSSI: {

            // TODO: Change to HAL objects
            auto msg_buff = ALLOC_SMART_BUFFER(sizeof(sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
            auto msg = reinterpret_cast<sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE*>(msg_buff.get());
            if (!msg) {
                LOG(FATAL) << "Memory allocation failed!";
                return false;
            }
                
            // Initialize the message
            memset(msg_buff.get(), 0, sizeof(sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
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

                auto measurement_recv_timestamp = std::chrono::steady_clock::now();
                m_unassoc_measure_window_size   = std::chrono::duration_cast<std::chrono::milliseconds>
                    (measurement_recv_timestamp - m_unassoc_measure_start).count();
                
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
            event_queue_push(event, msg_buff);

        } break;
    
        // Gracefully ignore unhandled events
        // TODO: Probably should be changed to an error once FAPI will stop
        //       sending empty or irrelevant events...
        default: LOG(WARNING) << "Unhandled event received: " << opcode; break;
    }

    return true;
}

} // namespace wav_fapi
} // namespace bwl

// AP FAPI HAL Factory Functions
extern "C" {
    bwl::sta_wlan_hal* sta_wlan_hal_create(
        std::string iface_name, bwl::base_wlan_hal::hal_event_cb_t callback)
    { return new bwl::wav_fapi::sta_wlan_hal_fapi(iface_name, callback); }

    void sta_wlan_hal_destroy(bwl::sta_wlan_hal* obj)
    { delete obj; }
}
