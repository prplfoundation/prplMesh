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

#include "fapi_attach_fsm.h"

#include "../common/base_wlan_hal_types.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <easylogging++.h>

#include "ugw_helper.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// FAPI ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define LOGGING_ID fapi_attach_fsm
#define PACKAGE_ID "0"

extern "C" {
#ifdef USE_FAPI_DAEMON

    #include "fapi_wlan_daemon.h"
    #include "fapi_wlan_public.h"
    
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

namespace bwl {
namespace wav_fapi {

//////////////////////////////////////////////////////////////////////////////
///////////////////////// Local Module Definitions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define AP_ENABELED_TIMEOUT_SEC             15
#define AP_ENABELED_FIXED_DFS_TIMEOUT_SEC   660

#define FAPI_DEBUG_OBJ(LABLE, OBJ) { \
    LOG(DEBUG) << "FAPI_debug_obj, " << LABLE << ": ";  fapi_debug_obj(OBJ); \
}

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Functions ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const fapi_fsm_state& value) {
	switch (value) {
		case fapi_fsm_state::Delay:         out << "Delay"; break;
		case fapi_fsm_state::Init:          out << "Init"; break;
		case fapi_fsm_state::GetRadioInfo:  out << "GetRadioInfo"; break;
		case fapi_fsm_state::Attach:        out << "Attach"; break;
		case fapi_fsm_state::Operational:   out << "Operational"; break;
		case fapi_fsm_state::Detach:        out << "Detach"; break;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const fapi_fsm_event& value) {
	switch (value) {
		case fapi_fsm_event::Attach: out << "Attach"; break;
		case fapi_fsm_event::Detach: out << "Detach"; break;
	}
	return out;
}

static void fapi_debug_obj(ObjList *debug_obj) {
    ObjList * currentObj;
    ParamList * currentParam;

    LOG(DEBUG) << "============";
    FOR_EACH_OBJ(debug_obj, currentObj) {
        LOG(DEBUG) << "OBJECT NAME: " << std::string( GET_OBJ_NAME(currentObj));
        FOR_EACH_PARAM(currentObj, currentParam) {
            LOG(DEBUG) << "PARAM: " << std::string( GET_PARAM_NAME(currentParam))
                       << "=" << std::string( GET_PARAM_VALUE(currentParam));
        }
    }
    LOG(DEBUG) << "============";
}

static int64_t read_fapi_int(std::shared_ptr<ObjList> wlObj, 
    char* tmpBuff, const char* objName, const char* objProp)
{
    if (HELP_SL_GET_NAME_BASED(wlObj.get(), (char*)objName, 0, 
            (char*)objProp, tmpBuff) != 0) {

        LOG(ERROR) << "Failed reading " << objName << "." << objProp;
        return -1;
    }

    return beerocks::string_utils::stoi(tmpBuff);
}

static std::string read_fapi_string(std::shared_ptr<ObjList> wlObj, 
    char* tmpBuff, const char* objName, const char* objProp)
{
    if (HELP_SL_GET_NAME_BASED(wlObj.get(), (char*)objName, 0, 
            (char*)objProp, tmpBuff) != 0) {

        LOG(ERROR) << "Failed reading " << objName << "." << objProp;
        return std::string();
    }
    LOG(DEBUG) << "tmpBuff = " << std::string(tmpBuff) ;
    return std::string(tmpBuff);
}

static bool read_radio_info_ap(char* tmpBuff, RadioInfo& radio_info)
{
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    if (fapi_wlan_radio_info_get(radio_info.iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE){
        LOG(ERROR) << "fapi_wlan_radio_info_get failed for interface: " << radio_info.iface_name;
        return false;
    }

    // FAPI_DEBUG_OBJ("refresh_radio_info()", wlObj.get());

    // Device.WiFi.Radio.Stats.TxAntennas
    radio_info.ant_num = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio.Stats", "TxAntennas");
    if (radio_info.ant_num <= 0) {
        LOG(ERROR) << "TxAntennas not valid: " << radio_info.ant_num;
        return false;
    }

    // Device.WiFi.Radio.Stats.TxPower
    radio_info.conducted_power = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio.Stats", "TxPower");
    if (radio_info.conducted_power <= 0) {
        LOG(ERROR) << "TxPower not valid: " << radio_info.conducted_power;
        return false;
    }
    
    // NOTE: "OperatingChannelBandwidt" is NOT a typo
    radio_info.bandwidth = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio.Stats", "OperatingChannelBandwidt");
    if (radio_info.bandwidth <= 0) {
        LOG(ERROR) << "OperatingChannelBandwidt not valid: " << radio_info.bandwidth;
        return false;
    }
   
    auto cf1 = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio.Stats", "Cf1");
    if (cf1 <= 0) {
        LOG(ERROR) << "Cf1 not valid: " << cf1;
        return false;
    }
    radio_info.vht_center_freq = cf1;
        
    auto tmp_int = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio", "HostapdEnabled");
    if (tmp_int < 0) {
        LOG(ERROR) << "HostapdEnabled not valid: " << tmp_int;
        return false;
    }
    radio_info.wifi_ctrl_enabled = int(tmp_int);
    
    tmp_int = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio", "TxEnabled");
    if (tmp_int < 0) {
        LOG(ERROR) << "TxEnabled not valid: " << tmp_int;
        return false;
    }
    radio_info.tx_enabled = bool(tmp_int);
    
    radio_info.channel = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio", "Channel");
    if (radio_info.channel <= 0) {
        LOG(DEBUG) << "Channel not valid: " << radio_info.channel;
        //return false;
    } else {
        m_radio_info.is_5ghz = (son::wireless_utils::which_freq(m_radio_info.channel) == beerocks::eFreqType::FREQ_5G);
    }

    tmp_int = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.Radio", "Dfs_chan");
    if (tmp_int < 0) {
        LOG(ERROR) << "Dfs_chan: " << tmp_int;
        return false;
    }
    radio_info.is_dfs_channel = int(tmp_int);

#ifndef USE_FAPI_DAEMON
    if (radio_info.wifi_ctrl_enabled == 0) {
        radio_info.wifi_ctrl_enabled = 1;
    } else {
        
        radio_info.wifi_ctrl_enabled = 0;
    }
#endif
    return true;
}

static bool read_radio_info_sta(char* tmpBuff, RadioInfo& radio_info)
{
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    if (fapi_wlan_client_mode_radio_info_get(radio_info.iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_client_mode_radio_info_get failed for interface: " << radio_info.iface_name;
        return false;
    }

    auto tmp_int = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_WpaSupplicantEnabled");
    if (tmp_int < 0) {
        LOG(ERROR) << "X_LANTIQ_COM_Vendor_WpaSupplicantEnabled not valid: " << tmp_int;
        return false;
    }
    radio_info.wifi_ctrl_enabled = int(tmp_int);
    
    tmp_int = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_TxEnabled");
    if (tmp_int < 0) {
        LOG(ERROR) << "X_LANTIQ_COM_Vendor_TxEnabled not valid: " << tmp_int;
        return false;
    }
    radio_info.tx_enabled = bool(tmp_int);
    
    radio_info.channel = read_fapi_int(wlObj, tmpBuff, "Device.WiFi.EndPoint.Profile", "X_LANTIQ_COM_Vendor_Channel");
    if (radio_info.channel <= 0) {
        LOG(ERROR) << "X_LANTIQ_COM_Vendor_Channel not valid: " << radio_info.channel;
        return false;
    } else if (radio_info.channel > 14) {
        radio_info.is_5ghz = true;
    }
    
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

fapi_attach_fsm::fapi_attach_fsm(
    HALType type, RadioInfo& radio_info, fapi_event_cb_t cb) :
    beerocks::beerocks_fsm<fapi_fsm_state, fapi_fsm_event>(fapi_fsm_state::Delay),
    m_hal_type(type),
    m_radio_info(radio_info),
    m_fapi_event_cb(cb)
{
    // Pointer for parsing FAPI object values
    m_temp_fapi_value = std::shared_ptr<char>(new char[MAX_LEN_PARAM_VALUE],
        [](char* obj) { if(obj) delete [] obj; });
}

bool fapi_attach_fsm::setup()
{   
    config()
    
    /////////////////////////////////////////////////////////
    ///////////////////// STATE - Delay /////////////////////
    /////////////////////////////////////////////////////////

    .state(fapi_fsm_state::Delay)   

    // On Entry
    .entry(
        [&](const void* args) -> bool { 
            m_state_timeout = std::chrono::steady_clock::now() + 
                std::chrono::seconds(5);
            return true;
        })

    // EVENT -> Attach
    .on(fapi_fsm_event::Attach, fapi_fsm_state::Init, 
        [&](TTransition& transition, const void* args) -> bool {
            return (std::chrono::steady_clock::now() >= m_state_timeout);
        })
    
    ////////////////////////////////////////////////////////
    ///////////////////// STATE - Init /////////////////////
    ////////////////////////////////////////////////////////

    .state(fapi_fsm_state::Init)
    
    .entry(
        [&](const void* args) -> bool { 
            m_state_timeout = std::chrono::steady_clock::now() + std::chrono::seconds(200);
            return true;
        })

    // Handle "Detach" event
    .on(fapi_fsm_event::Detach, fapi_fsm_state::Detach)

    // Handle "Attach" event
    .on(fapi_fsm_event::Attach, { fapi_fsm_state::Attach, fapi_fsm_state::Detach, fapi_fsm_state::GetRadioInfo },
        [&](TTransition& transition, const void* args) -> bool {

#ifdef USE_FAPI_DAEMON
            if (get_interface_info(m_radio_info.iface_name))
#else
            if (fapi_wlan_start_monitoring(m_radio_info.iface_name.c_str(), NULL, 0) == UGW_SUCCESS)
#endif
            {
                if (m_hal_type != HALType::Station) {
                    transition.change_destination(fapi_fsm_state::GetRadioInfo);
                }
                return true;
            } else {
                LOG(DEBUG) << "interface/hostapd " << m_radio_info.iface_name << " are not ready";
            }

            // False if timeout not reached yet, and True otherwise (switch state)
            if (std::chrono::steady_clock::now() >= m_state_timeout) {
                LOG(ERROR) << "Failed attaching to the hostapd control interface of " << m_radio_info.iface_name;
                return (transition.change_destination(fapi_fsm_state::Detach));
            } 

            // Stay in the current state
            return false;
        })

    ////////////////////////////////////////////////////////////////
    ///////////////////// STATE - GetRadioInfo /////////////////////
    ////////////////////////////////////////////////////////////////

    .state(fapi_fsm_state::GetRadioInfo)
    
    .entry(
        [&](const void* args) -> bool { 
            m_state_timeout = std::chrono::steady_clock::now() + 
                std::chrono::seconds(AP_ENABELED_TIMEOUT_SEC);
            return true;
        })

    // Handle "Detach" event
    .on(fapi_fsm_event::Detach, fapi_fsm_state::Detach)
    
    .on(fapi_fsm_event::Attach, { fapi_fsm_state::Attach, fapi_fsm_state::Detach },
        [&](TTransition& transition, const void* args) -> bool {

            // Attempt to read radio info
            if (!refresh_radio_info()) {
                return (transition.change_destination(fapi_fsm_state::Detach));
            }

            // If Non-AP HAL, continue to the next state
            if (m_hal_type != HALType::AccessPoint) { return true; }

            auto fixed_channel     = (!m_radio_info.acs_enabled && !m_radio_info.is_dfs_channel);
            auto fixed_dfs_channel = (!m_radio_info.acs_enabled && m_radio_info.is_dfs_channel);
            LOG(DEBUG) << "m_radio_info.acs_enabled  = " << int(m_radio_info.acs_enabled) 
                << " m_radio_info.is_dfs_channel = " << int(m_radio_info.is_dfs_channel)
                << " fixed_dfs_channel = " << int(fixed_dfs_channel)
                << " fixed_channel = " << int(fixed_channel);

            // AP is Enabled
            LOG(DEBUG) << "wifi_ctrl_enabled  = " << int(m_radio_info.wifi_ctrl_enabled);

            bool error = false;
            if (fixed_dfs_channel) {
                if (m_radio_info.tx_enabled) {
                    LOG(DEBUG) << "ap_enabled tx = " << int(m_radio_info.tx_enabled);
                    return true;
                } else {
                    LOG(ERROR) << "ap_enabled with tx OFF!";
                    error = true;
                }
            } else if (fixed_channel) {
                if (m_radio_info.wifi_ctrl_enabled == 1) {
                    if (m_radio_info.tx_enabled) {
                        LOG(DEBUG) << "ap_enabled tx = " << int(m_radio_info.tx_enabled);
                        return true;
                    } else {
                        LOG(ERROR) << "ap_enabled with tx OFF!";
                        error = true;
                    }
                }
            } else if (m_radio_info.wifi_ctrl_enabled == 2) {                
                if (m_radio_info.tx_enabled) {
                    LOG(DEBUG) << "ap_enabled tx = " << int(m_radio_info.tx_enabled);
                    return (true);
                } else {
                    LOG(ERROR) << "ap_enabled with tx OFF!";
                    error = true;
                }
            }
            
            if (error || (std::chrono::steady_clock::now() >= m_state_timeout)) {
                return (transition.change_destination(fapi_fsm_state::Detach));
            }

            // Remain in the current state
            return false;
        })

    //////////////////////////////////////////////////////////
    ///////////////////// STATE - Attach /////////////////////
    //////////////////////////////////////////////////////////

    .state(fapi_fsm_state::Attach)
    
    // Handle "Detach" event
    .on(fapi_fsm_event::Detach, fapi_fsm_state::Detach)
    
    .on(fapi_fsm_event::Attach, { fapi_fsm_state::Operational, fapi_fsm_state::Delay },
        [&](TTransition& transition, const void* args) -> bool {

#ifdef USE_FAPI_DAEMON 
       
            // Attach to the control interface for receiving events
            if (fapi_wlan_hostapd_event_attach(m_radio_info.iface_name.c_str()) == UGW_SUCCESS) {   
                // Get the hostapd control interface socket
                if ((fapi_wlan_event_fd_get(m_radio_info.iface_name.c_str(), &m_fapi_event_fd) == UGW_FAILURE) ){
                    // Return with error
                    m_fapi_event_fd = -1;
                    LOG(ERROR) << "fapi_wlan_event_fd_get failed on interface " << m_radio_info.iface_name;
                    return (transition.change_destination(fapi_fsm_state::Delay));
                }
            }

#else   

            // Get the hostapd control interface socket
            if ((fapi_wlan_hostapd_socket_get(m_radio_info.iface_name.c_str(),
                    (void**)&m_wpa_ctrl) == UGW_FAILURE) || (m_wpa_ctrl == nullptr)) {
                        
                // Return with error
                LOG(ERROR) << "fapi_wlan_wave_hostapd_socket_get failed!";
                return (transition.change_destination(fapi_fsm_state::Delay));
            }

            // Attach to the control interface for receiving events
            if (wpa_ctrl_attach(m_wpa_ctrl) != 0) {
                // Return with error
                LOG(ERROR) << "Failed attaching to the hostapd control interface of " << m_radio_info.iface_name;
                return (transition.change_destination(fapi_fsm_state::Delay));
            }

#endif
        
            // Register FAPI callback
            if (fapi_wlan_hostapd_callback_register(m_radio_info.iface_name.c_str(), 
                [](char* opcode, const char* ifname, ObjList* wlObj, unsigned int flags, void* context) {

                if (!strncmp(opcode, "OP_CODE_NOT_SUPPORTED", 21)) {
                    return UGW_SUCCESS;
                }
                
                LOG(DEBUG) << "FAPI Event Callback: " << opcode;

                    auto pThis = static_cast<fapi_attach_fsm*>(context);
                    LOG_IF(!pThis, FATAL) << "Invalid context pointer!";

                    // Pass the event to the HAL
                    pThis->inject_fapi_event(opcode, wlObj);
                    return UGW_SUCCESS;

                }, this) == UGW_FAILURE){

                    // Return with error
                    LOG(ERROR) <<"fapi_wlan_hostapd_callback_register failed";
                    return (transition.change_destination(fapi_fsm_state::Delay));
            }
        
            // Success
            return true;
        })

    ///////////////////////////////////////////////////////////////
    ///////////////////// STATE - Operational /////////////////////
    ///////////////////////////////////////////////////////////////

    .state(fapi_fsm_state::Operational)
    
    // Handle "Detach" event
    .on(fapi_fsm_event::Detach, fapi_fsm_state::Detach)

    //////////////////////////////////////////////////////////
    ///////////////////// STATE - Detach /////////////////////
    //////////////////////////////////////////////////////////

    .state(fapi_fsm_state::Detach)
    
    .entry(
        [&](const void* args) -> bool { 
            LOG(DEBUG) << "fapi_attach_fsm - Detaching...";

#ifdef USE_FAPI_DAEMON

            if (fapi_wlan_hostapd_event_detach(m_radio_info.iface_name.c_str()) == UGW_FAILURE) {
                LOG(ERROR) << "attach_fapi - fapi_wlan_hostapd_event_detach failed -goto INIT";
            }


            if (fapi_wlan_hostapd_callback_register(m_radio_info.iface_name.c_str(), NULL, NULL) == UGW_FAILURE) {
                LOG(ERROR) << "attach_fapi - fapi_wlan_hostapd_callback_register failed -goto INIT";
            }

            // Close the events control interface
            if (m_fapi_event_fd != -1) {
                m_fapi_event_fd = -1;
            }

#else

            // Close the events control interface
            if (m_wpa_ctrl) 
            {
                int count = 0;
                int result;
                do {
                    result = wpa_ctrl_detach(m_wpa_ctrl);
                } while (result == -2 && ++count < 3);

                if (result < 0) {
                    LOG(ERROR) << "Failed to detach event control interface";
                }

                m_wpa_ctrl = nullptr;
            }

            // Stop monitoring
            if (fapi_wlan_stop_monitoring(m_radio_info.iface_name.c_str(), NULL, 0) == UGW_FAILURE) {
                LOG(INFO) << "fapi_wlan_stop_monitoring failed! iface=" << m_radio_info.iface_name;
            }

#endif

            return true;
        })

    // Handle "Attach" event
    .on(fapi_fsm_event::Attach, fapi_fsm_state::Delay);

    // Start the FSM
    return (start());
}

fapi_fsm_state fapi_attach_fsm::attach()
{
    fire(fapi_fsm_event::Attach);
    return state();
}

bool fapi_attach_fsm::detach()
{
    fire(fapi_fsm_event::Detach);    
    return (state() == fapi_fsm_state::Detach);
}

bool fapi_attach_fsm::refresh_radio_info()
{
    // Buffer for temporary FAPI variables
    auto tmpBuff = m_temp_fapi_value.get();

    // Station HAL
    if (m_hal_type == HALType::Station) {
        return read_radio_info_sta(tmpBuff, m_radio_info);
    // AP/Monitor HAL
    } else {
        if (read_radio_info_ap(tmpBuff, m_radio_info)) {
            // If the VAPs map is empty, refresh it as well
            // TODO: update on every refresh?

            if (!m_radio_info.available_vaps.size()) {
                return refresh_vaps_info();
            }            
        } else {
            return false;
        }
    }

    return true;
}

bool fapi_attach_fsm::get_interface_info( std::string iface_name )
{
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    // Buffer for temporary FAPI variables
    auto tmpBuff = m_temp_fapi_value.get();

    if (fapi_wlan_interface_info_get( iface_name.c_str(), wlObj.get(), 0) == UGW_FAILURE) { 
        LOG(ERROR) << "fapi_wlan_interface_info_get call failed" ;  
        return (false);
    } 

    FAPI_DEBUG_OBJ("fapi_wlan_interface_info_get()", wlObj.get());

    auto OperationMode = read_fapi_string(wlObj, tmpBuff, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "OperationMode");

    auto SupportedFrequencyBands = read_fapi_string(wlObj, tmpBuff, "Device.WiFi.Radio.X_LANTIQ_COM_Vendor", "SupportedFrequencyBands");

    LOG(DEBUG) << "OperationMode = " << OperationMode << " SupportedFrequencyBands = " << SupportedFrequencyBands; 

    if ( OperationMode.empty() || OperationMode == "NONE" ) {
        LOG(DEBUG) << "fapi_wlan_interface_info_get OperationMode empty or NONE" ;
        return false;
    }

    if( SupportedFrequencyBands.empty() || OperationMode == "NONE" ) {
        LOG(DEBUG) << "fapi_wlan_interface_info_get SupportedFrequencyBands empty or NONE" ;
        return false;
    }

    return true;

}

bool fapi_attach_fsm::refresh_vaps_info()
{
    // LOG(TRACE) << __func__;

    // Buffer for temporary FAPI variables
    auto tmpBuff = m_temp_fapi_value.get();

    std::string main_vap_mac;
    
    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    for (int vap_id = beerocks::IFACE_MAIN_VAP_ID; vap_id <= beerocks::IFACE_VAP_ID_MAX; vap_id++) {

        // quit if wifi_ctrl_enabled != 1 on non main vap, to avoid vap mac of zeros
        if (m_radio_info.wifi_ctrl_enabled != 1 && vap_id != beerocks::IFACE_MAIN_VAP_ID) break;
        
        // Get the VAP interface
        auto vap_iface = beerocks::utils::get_iface_string_from_iface_vap_ids(
                                m_radio_info.iface_name, vap_id);

        // Read the VAP information
        if (fapi_wlan_vap_measurement_get(vap_iface.c_str(), wlObj.get(), 0) == UGW_FAILURE) {
            
            // If the command failed on the main vap it's an error
            if (vap_id == beerocks::IFACE_MAIN_VAP_ID) {
                LOG(ERROR) << "Failed reading the information of the Main VAP: " << vap_iface;
                return false;
            } else {
                // On any other VAP assume that it doesn't exist, and exit
                if (m_radio_info.available_vaps.find(vap_id) != m_radio_info.available_vaps.end()) {
					m_radio_info.available_vaps.erase(vap_id);
				}
                continue;
            }
        }

        // New VAP Element
        VAPElement vapElement;

        // Read and Store the BSSID
        if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "BSSID", tmpBuff) != UGW_SUCCESS) {
            LOG(ERROR) << "Failed reading BSSID of VAP: " << vap_iface;
            return false;
        }

        vapElement.mac.assign(tmpBuff);

        // HACK because of empty vap mac for non main vap iface
        if (vap_id == beerocks::IFACE_MAIN_VAP_ID) {
            main_vap_mac.assign(tmpBuff);
        }

        // Read and Store the SSID
        if (UGW_READ_VALUE(wlObj, "Device.WiFi.SSID", 0, "SSID", tmpBuff) != UGW_SUCCESS) {
            LOG(ERROR) << "Failed reading SSID of VAP: " << vap_iface;
            return false;
        }

        vapElement.ssid.assign(tmpBuff);

        //FIXME hack for now
        if (vap_id >= 0) {
            vapElement.mac = main_vap_mac;
            vapElement.mac.back() +=  (vap_id+2);
        }

        // Store the VAP element
        // LOG(DEBUG) << "Detected VAP ID (" << vap_id << ") - MAC: " << vapElement.mac
        //            << ", SSID: " << vapElement.ssid;

        m_radio_info.available_vaps[vap_id] = vapElement;
    }

    return true;    
}

void fapi_attach_fsm::inject_fapi_event(const std::string opcode, void* obj)
{
    if (!m_fapi_event_cb) {
        LOG(WARNING) << "FAPI event arrived but no callback was registered!";
        return;        
    }
    
    // Wrap the object
    UGW_OBJLIST_WRAP_EXTERNAL(wlObj, static_cast<ObjList*>(obj));
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";

    m_fapi_event_cb(opcode, wlObj);
}

std::string fapi_attach_fsm::state_enum_to_string(fapi_fsm_state state)
{
    std::ostringstream os;
    os << state;
    return os.str();
}

} // namespace bwl
} // namespace wav_fapi
    
