/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_wav.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

#define LOGGING_ID sta_wlan_hal_wav
#define PACKAGE_ID "0"

extern "C" {
#include <wpa_ctrl.h>
}

namespace bwl {
namespace wav {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

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

/*
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
    */

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

sta_wlan_hal_wav::sta_wlan_hal_wav(std::string iface_name, hal_event_cb_t callback)
    : base_wlan_hal(), base_wlan_hal_wav(bwl::HALType::Station, iface_name, true, callback, 1024)
{
}

sta_wlan_hal_wav::~sta_wlan_hal_wav() { detach(); }

bool sta_wlan_hal_wav::detach()
{
    /*
        LOG(TRACE) << __func__ << ": iface = " << get_iface_name();
        // Disconnect before detaching
        disconnect();
        return base_wlan_hal_fapi::detach();
*/
    return true;
}

bool sta_wlan_hal_wav::initiate_scan()
{
    /*
        auto& iface_name = get_iface_name();
        LOG(DEBUG) << "Initiating scan on interface: " << iface_name;

        // Start the scan
        if (!wpa_ctrl_send_msg("SCAN")) {
            LOG(ERROR) << "initiate_scan - wpa_ctrl_send_msg failed for " << iface_name;
            return false;
        }
*/
    return true;
}

int sta_wlan_hal_wav::get_scan_results(const std::string &ssid, net::sScanResult *list, int size)
{
#if 0
        // Validate input parameters
        if (!list || !size) {
            LOG(ERROR) << "Invalid scan results list " << list << " (" << size << ")";
            return 0;
        }

        parsed_obj_listed_map_t scan_results_listed_map;
        if (!wpa_ctrl_send_msg("SCAN_RESULTS", scan_results_listed_map)) {
            LOG(ERROR) << "get_scan_results - wpa_ctrl_send_msg failed for " << get_iface_name();
            return 0;
        }

        /*
           bssid=54:b8:0a:aa:86:72 frequency=5180 signal_level=-34 flags=[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][WPS][ESS] ssid=WINS_5 freqband=5G netmode=anac
           bssid=84:3d:c6:6b:03:af frequency=5280 signal_level=-41 flags=[WPA2-EAP-CCMP][ESS] ssid=RSN2OfficeWLAN freqband=5G netmode=anac
       */

        // Iterate over the objects
        for (auto result : scan_results_listed_map) {
            auto it = result.find("bssid");
            if (it != result.end()) {
                std::string bssid = *it;
            } else {
                LOG(ERROR) << "couldn't find bssid field in scan result";
            }
        }

        return num_of_results;
#endif
    return 0;
}

bool sta_wlan_hal_wav::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                               const std::string &bssid, uint8_t channel, bool hidden_ssid)
{
    /*
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
*/
    return true;
}

bool sta_wlan_hal_wav::disconnect()
{
    /*
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
        */
    return true;
}

bool sta_wlan_hal_wav::roam(const std::string &bssid, uint8_t channel)
{
    /*
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
*/
    return true;
}

bool sta_wlan_hal_wav::get_4addr_mode()
{
    /*
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
        */
    return true;
}

bool sta_wlan_hal_wav::set_4addr_mode(bool enable)
{
    /*
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
*/
    return true;
}

bool sta_wlan_hal_wav::unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                int vht_center_frequency, int delay,
                                                int window_size)
{
    /*
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
*/
    return true;
}

bool sta_wlan_hal_wav::is_connected()
{
    /*
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
        */
    return true;
}

int sta_wlan_hal_wav::get_rssi() { return beerocks::RSSI_INVALID; }

int sta_wlan_hal_wav::get_channel() { return m_active_channel; }

std::string sta_wlan_hal_wav::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_wav::get_bssid() { return m_active_bssid; }

bool sta_wlan_hal_wav::process_wav_event(parsed_obj_map_t &parsed_obj) { return true; }

#if 0
    bool sta_wlan_hal_wav::parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj)
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
                auto msg_buff = ALLOC_SMART_BUFFER(sizeof(beerocks::message::sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));
                auto msg = reinterpret_cast<beerocks::message::sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION*>(msg_buff.get());
                LOG_IF(!msg, FATAL) << "Memory allocation failed!";

                // Initialize the message
                memset(msg_buff.get(), 0, sizeof(beerocks::message::sACTION_BACKHAUL_DISCONNECT_REASON_NOTIFICATION));

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
                auto msg_buff = ALLOC_SMART_BUFFER(sizeof(beerocks::message::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
                auto msg = reinterpret_cast<beerocks::message::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE*>(msg_buff.get());
                LOG_IF(!msg, FATAL) << "Memory allocation failed!";

                // Initialize the message
                memset(msg_buff.get(), 0, sizeof(beerocks::message::sACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE));
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

                    Measurement failed
                } else {
                    auto now   = std::chrono::steady_clock::now();
                    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_unassoc_measure_start).count();
                    auto temp_sum_delay = (delta - m_unassoc_measure_delay);

                    if (temp_sum_delay > m_unassoc_measure_window_size) {
                        LOG(ERROR) << "event_obj->params.rx_packets = -2 , delay = " << int(temp_sum_delay);
                        msg->params.rx_packets = -2; // means the actual measurement started later then aspected 
                    }
                }

                Add the message to the queue
                    event_queue_push(event, msg_buff);

            } break;

                // Gracefully ignore unhandled events
                // TODO: Probably should be changed to an error once FAPI will stop
                //       sending empty or irrelevant events...
            default: LOG(WARNING) << "Unhandled event received: " << opcode; break;
        }

        return true;
    }
#endif

} // namespace wav
} // namespace bwl

// AP FAPI HAL Factory Functions
extern "C" {

bwl::sta_wlan_hal *sta_wlan_hal_create(std::string iface_name,
                                       bwl::base_wlan_hal::hal_event_cb_t callback)
{
    return new bwl::wav::sta_wlan_hal_wav(iface_name, callback);
}

void sta_wlan_hal_destroy(bwl::sta_wlan_hal *obj) { delete obj; }
}
