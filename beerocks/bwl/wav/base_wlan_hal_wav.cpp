/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "base_wlan_hal_wav.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>

#include <easylogging++.h>

extern "C" {
    #include <wpa_ctrl.h>
}

static const std::string BASE_CTRL_PATH = "/var/run/";
#define UNHANDLED_EVENTS_LOGS 20

namespace bwl {
namespace wav {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Functions ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& out, const wav_fsm_state& value) {
	switch (value) {
		case wav_fsm_state::Delay:         out << "Delay"; break;
		case wav_fsm_state::Init:          out << "Init"; break;
		case wav_fsm_state::GetRadioInfo:  out << "GetRadioInfo"; break;
		case wav_fsm_state::Attach:        out << "Attach"; break;
		case wav_fsm_state::Operational:   out << "Operational"; break;
		case wav_fsm_state::Detach:        out << "Detach"; break;
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, const wav_fsm_event& value) {
	switch (value) {
		case wav_fsm_event::Attach: out << "Attach"; break;
		case wav_fsm_event::Detach: out << "Detach"; break;
	}
	return out;
}

static void map_obj_parser(std::stringstream& ss_in, std::list<char> delimiter_list, parsed_obj_map_t& map_obj)
{
	if(delimiter_list.empty()) return;
	
	std::string str_storage, key;
	bool kv = true; // '1'=key, '0'=val;
	while (std::getline(ss_in, str_storage, delimiter_list.front())) {
		if (delimiter_list.size() == 1) {
			if(kv){ 
				key = str_storage;          // save key
			} else {
				map_obj[key] = str_storage; // save val 
				
			}
			kv = !kv;
			
		} else {
			auto delimiter_list_out(delimiter_list);
			delimiter_list_out.erase(delimiter_list_out.begin()); // delete first delimiter
			std::stringstream ss_out(str_storage);
			map_obj_parser(ss_out, delimiter_list_out, map_obj);
		}
	}
}

static void listed_map_obj_parser(std::stringstream& ss_in, std::list<char> delimiter_list, parsed_obj_listed_map_t& listed_map_obj)
{
	if(delimiter_list.empty()) return;
	
	std::string str_storage;
	while (std::getline(ss_in, str_storage, delimiter_list.front())) {
		auto delimiter_list_out(delimiter_list);
		delimiter_list_out.erase(delimiter_list_out.begin()); // delete first delimiter
		std::stringstream ss_out(str_storage);
		parsed_obj_map_t map_obj;
		map_obj_parser(ss_out, delimiter_list_out, map_obj);
		listed_map_obj.push_back(map_obj);
	}
}

static std::string::size_type find_first_of_delimiter_pair(std::string &str, std::string::size_type pos, char delim_near, char delim_far)
{
	// finds first occurrence on string 'str' of <'delim_near'><any_characters except delimiters><'delim_far'>
	auto idx = str.find_first_of(delim_far, pos);
	if (idx == std::string::npos){
		return idx;
	}
	idx = str.rfind(delim_near, idx);
	return idx;
}

static void map_event_obj_parser(std::string event_str, parsed_obj_map_t& map_obj)
{
	// eliminate event log level from the begining of the event string : "<3>"
	auto idx_start = event_str.find_first_of(">");
	if (idx_start != std::string::npos) {
		idx_start++;
    } else {
		LOG(WARNING) << "empty event! event_string: " << event_str;
	}
	
	// find params without key end index
	auto idx = find_first_of_delimiter_pair(event_str, idx_start, ' ', '=');
	
	// put null terminator at the end of our key=val, for ss construction
    if (idx != std::string::npos) {
		event_str[idx] = '\0';
	}

    // insert to map known prams without key
	std::stringstream ss(event_str.c_str() + idx_start);
	std::string str_storage;
	bool opcode = true;
	while (std::getline(ss, str_storage, ' ')) {
		if(opcode) {
			// assume that the first param is event name
			map_obj[WAV_EVENT_KEYLESS_PARAM_OPCODE] = str_storage;
            opcode = false;
		} else if (beerocks::net::network_utils::is_valid_mac(str_storage)) {
			map_obj[WAV_EVENT_KEYLESS_PARAM_MAC] = str_storage;
		} else if (!strncmp(str_storage.c_str(), "wlan", 4)) {
			map_obj[WAV_EVENT_KEYLESS_PARAM_IFACE] = str_storage;
		}
	}
	
	// fill the map with the rest of event data
	while(idx != std::string::npos) {
		
		idx_start = ++idx;
		
		// find first '=' to skip on it
		idx = event_str.find_first_of('=', idx_start);
		
		// find the next pair of delimiters index
		idx = find_first_of_delimiter_pair(event_str, ++idx, ' ', '=');
		
		if (idx != std::string::npos) {
			// put null terminator at the end of our key=val, for ss_in construction
			event_str[idx] = '\0';
		}
		
		// parse key=val
		std::stringstream ss_in(event_str.c_str() + idx_start);
		map_obj_parser(ss_in, {'='}, map_obj);
	}
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

base_wlan_hal_wav::base_wlan_hal_wav(
    HALType type, std::string iface_name, bool acs_enabled,
    hal_event_cb_t callback, int wpa_ctrl_buffer_size) :
    base_wlan_hal(type, iface_name, IfaceType::Intel, acs_enabled, callback),
    beerocks::beerocks_fsm<wav_fsm_state, wav_fsm_event>(wav_fsm_state::Delay),
    m_wpa_ctrl_buffer_size(wpa_ctrl_buffer_size)
{

    // Allocate wpa_ctrl buffer 
    if(m_wpa_ctrl_buffer_size) {
        m_wpa_ctrl_buffer = std::shared_ptr<char>(new char[m_wpa_ctrl_buffer_size],
            [](char* obj) { if(obj) delete [] obj; });
    }

    ctrl_path = BASE_CTRL_PATH;
    if (get_type() == HALType::AccessPoint || get_type() == HALType::Monitor) {
        ctrl_path += "hostapd/";
    } else if (get_type() == HALType::Station) {
        ctrl_path += "wpa_supplicant/";
    } else {
        LOG(ERROR) << "hal type is Invalid";
        return; // HACK TODO what should we do in that case?
    }

    ctrl_path += m_radio_info.iface_name;
    
    // Initialize the FSM
    fsm_setup();
}

bool base_wlan_hal_wav::wav_obj_read_int(const std::string& key, parsed_obj_map_t& obj, int64_t& value, bool ignore_unknown)
{
    auto val_iter = obj.find(key);
    if (val_iter == obj.end()) {
        LOG(ERROR) << "param :" << key << " is not exist";
        return false;
    }

    static const std::string unknown_string = "UNKNOWN";

    if (ignore_unknown && !unknown_string.compare(val_iter->second)) {
        value = 0;
        return true;
    }

    value = beerocks::string_utils::stoi(val_iter->second);

    return (true);
}

bool base_wlan_hal_wav::wav_obj_read_str(const std::string& key, parsed_obj_map_t& obj, char** value)
{
    auto val_iter = obj.find(key);
    if (val_iter == obj.end()) {
        LOG(ERROR) << "param :" << key << " does not exist";
        return false;
    }

    *value = (char*)((val_iter->second).c_str());
    return (true);
}

base_wlan_hal_wav::~base_wlan_hal_wav()
{
    detach();
}

bool base_wlan_hal_wav::fsm_setup()
{   
    config()
    
    // Setup states:

    //////////////////////////////////////////////////////////////////////////
    ////////////////////////////// State: Delay //////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    .state(wav_fsm_state::Delay)   

    // On Entry
    .entry(
        [&](const void* args) -> bool { 
            m_state_timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
            return true;
        })

    // EVENT -> Attach
    .on(wav_fsm_event::Attach, wav_fsm_state::Init, 
        [&](TTransition& transition, const void* args) -> bool {
            return (std::chrono::steady_clock::now() >= m_state_timeout);
        })

    //////////////////////////////////////////////////////////////////////////
    ////////////////////////////// State: Init ///////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    .state(wav_fsm_state::Init)
    
    .entry(
        [&](const void* args) -> bool { 
            m_state_timeout = std::chrono::steady_clock::now() + std::chrono::seconds(200);
            return true;
        })

    // Handle "Detach" event
    .on(wav_fsm_event::Detach, wav_fsm_state::Detach)

    // Handle "Attach" event
    .on(wav_fsm_event::Attach, { wav_fsm_state::Attach, wav_fsm_state::Detach, wav_fsm_state::GetRadioInfo },
        [&](TTransition& transition, const void* args) -> bool {

            // Open a control interface to wpa_supplicant/hostapd.
            if ((m_wpa_ctrl_cmd = wpa_ctrl_open(ctrl_path.c_str()))) {
                if (get_type() != HALType::Station) {
                    transition.change_destination(wav_fsm_state::GetRadioInfo);                
                }
                return true;
            } else {
                LOG(DEBUG) << "wpa_ctrl_open() failed, ctrl_iface_path: " << ctrl_path;
            }

            // False if timeout not reached yet, and True otherwise (switch state)
            if (std::chrono::steady_clock::now() >= m_state_timeout) {
                LOG(ERROR) << "Failed attaching to the hostapd control interface of " << m_radio_info.iface_name;
                return (transition.change_destination(wav_fsm_state::Detach));
            } 

            // Stay in the current state
            return false;
        })
    
    //////////////////////////////////////////////////////////////////////////
    ////////////////////////// State: GetRadioInfo ///////////////////////////
    //////////////////////////////////////////////////////////////////////////

    .state(wav_fsm_state::GetRadioInfo)
    
    .entry(
        [&](const void* args) -> bool { 
            m_state_timeout = std::chrono::steady_clock::now() + std::chrono::seconds(AP_ENABELED_TIMEOUT_SEC);
            return true;
        })

    // Handle "Detach" event
    .on(wav_fsm_event::Detach, wav_fsm_state::Detach)
    
    .on(wav_fsm_event::Attach, { wav_fsm_state::Attach, wav_fsm_state::Detach },
        [&](TTransition& transition, const void* args) -> bool {

            // Attempt to read radio info
            if (!refresh_radio_info()) {
                return (transition.change_destination(wav_fsm_state::Detach));
            }

            // If Non-AP HAL, continue to the next state
            if (get_type() != HALType::AccessPoint) { return true; }

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
                    return true;
                } else {
                    LOG(ERROR) << "ap_enabled with tx OFF!";
                    error = true;
                }
            }
            
            if (error || (std::chrono::steady_clock::now() >= m_state_timeout)) {
                return (transition.change_destination(wav_fsm_state::Detach));
            }

            // Remain in the current state
            return false;
        })

    //////////////////////////////////////////////////////////////////////////
    ///////////////////////////// State: Attach //////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    .state(wav_fsm_state::Attach)
    
    // Handle "Detach" event
    .on(wav_fsm_event::Detach, wav_fsm_state::Detach)
    
    .on(wav_fsm_event::Attach, { wav_fsm_state::Operational, wav_fsm_state::Delay },
        [&](TTransition& transition, const void* args) -> bool {

            // Open a event interface to wpa_supplicant/hostapd.
            if (!(m_wpa_ctrl_event = wpa_ctrl_open(ctrl_path.c_str()))) {
                LOG(DEBUG) << "wpa_ctrl_open() failed, ctrl_iface_path: " << ctrl_path;
                return (transition.change_destination(wav_fsm_state::Delay));
            }
            
            // Get the wpa_supplicant/hostapd event interface file descriptor
            m_fd_ext_events = wpa_ctrl_get_fd((wpa_ctrl*)m_wpa_ctrl_event);

            // Attach to the control interface for events receiving
            int result;
            int try_cnt = 0;
            do {
                result = wpa_ctrl_attach(m_wpa_ctrl_event); // return values: 0 on success, -1 on failure, -2 on timeout
            } while (result == -2 && ++try_cnt < 3);
            
            if (result < 0) {
                // Return with error
                LOG(ERROR) << "Failed attaching to control interface of " << m_radio_info.iface_name;
                return (transition.change_destination(wav_fsm_state::Delay));
            }

            // Success
            LOG(DEBUG) << "Open and attach an event interface to wpa_supplicant/hostapd - SUCCESS!";
            return true;
        })

    //////////////////////////////////////////////////////////////////////////
    /////////////////////////// State: Operational ///////////////////////////
    //////////////////////////////////////////////////////////////////////////

    .state(wav_fsm_state::Operational)
    
    // Handle "Detach" event
    .on(wav_fsm_event::Detach, wav_fsm_state::Detach)

    //////////////////////////////////////////////////////////////////////////
    ////////////////////////////// State: Detach /////////////////////////////
    //////////////////////////////////////////////////////////////////////////

    .state(wav_fsm_state::Detach)
    
    .entry(
        [&](const void* args) -> bool { 
            LOG(DEBUG) << "wav_attach_fsm - Detaching...";

            if(m_wpa_ctrl_event) {
                int result;
                int try_cnt = 0;
                do {
                    result = wpa_ctrl_detach(m_wpa_ctrl_event); // return values: 0 on success, -1 on failure, -2 on timeout
                } while (result == -2 && ++try_cnt < 3);
                
                if (result < 0) {
                    LOG(WARNING) << "can't detach wpa_ctrl_event";
                }
            }

            // Close the events control interface
            wpa_ctrl_close(m_wpa_ctrl_event);
            wpa_ctrl_close(m_wpa_ctrl_cmd);

            m_wpa_ctrl_event = nullptr;
            m_wpa_ctrl_cmd = nullptr;

            m_fd_ext_events = -1;

            return true;
        })

    // Handle "Attach" event
    .on(wav_fsm_event::Attach, wav_fsm_state::Delay);

    // Start the FSM
    return (start());
}

HALState base_wlan_hal_wav::attach(bool block)
{
    while (true) {

        fire(wav_fsm_event::Attach);
        auto attach_state = state();
        if (m_last_attach_state != attach_state) {
            LOG(DEBUG) << "WAV FSM " << m_radio_info.iface_name << " Attach State: " << attach_state;
            m_last_attach_state = attach_state;
        }

        switch (attach_state)
        {
            // Initializing
            case wav_fsm_state::Delay:
            case wav_fsm_state::Init:
            case wav_fsm_state::GetRadioInfo: 
            case wav_fsm_state::Attach: {
                if (block) {
                    // TODO: Delay?
                    continue;
                } else {
                    return (m_hal_state = HALState::Initializing);
                }
            }
            
            // Initialization completed
            case wav_fsm_state::Operational: {
                return (m_hal_state = HALState::Operational);
            }

            // Initialization failed
            case wav_fsm_state::Detach: {
                return (m_hal_state = HALState::Failed);
            }

            // Invalid state
            default: {
                LOG(ERROR) << "Invalid WAV Attach State: " << int(attach_state);
            }
        }
    };
}

bool base_wlan_hal_wav::detach()
{
    fire(wav_fsm_event::Detach);
    return (state() == wav_fsm_state::Detach);
}

bool base_wlan_hal_wav::wpa_ctrl_send_msg(const std::string& cmd)
{
    int result;
    int try_cnt = 0;

    if (!m_wpa_ctrl_cmd) {
        LOG(WARNING) << "m_wpa_ctrl_cmd=nullptr";
        return false;
    }

    auto buffer = m_wpa_ctrl_buffer.get();

    auto buff_size_copy = m_wpa_ctrl_buffer_size;
    do {
        result = wpa_ctrl_request(m_wpa_ctrl_cmd, cmd.c_str(), cmd.size(), buffer, &buff_size_copy, NULL);
    } while (result == -2 && ++try_cnt < 3);
    
    if (result < 0) {
        LOG(ERROR) << "can't send wpa_ctrl_request";
        LOG(ERROR) << "failed cmd: " << cmd;
        return false;
    }

    if (buff_size_copy >= m_wpa_ctrl_buffer_size) {
        LOG(ERROR) << "wpa_ctrl_request returned reply of size " << buff_size_copy;
        return false;
    }
    
    buffer[buff_size_copy] = 0; // the wpa_ctrl does not put null terminator at the and of the string

	if ((!strncmp(buffer, "FAIL", 4)) || (!strncmp(buffer, "UNKNOWN", 7))) {
        LOG(DEBUG) << std::endl << "cmd failed: " << cmd;
        LOG(WARNING) << std::endl << "reply: " << buffer;
        return false;
	}

    return true;
}

bool base_wlan_hal_wav::wpa_ctrl_send_msg(const std::string& cmd, parsed_obj_map_t& reply)
{
    if (!wpa_ctrl_send_msg(cmd)) {
        return false;
    }

	if (strncmp(m_wpa_ctrl_buffer.get(), "", 1)) {
        // if reply is not empty
        std::stringstream ss_in(m_wpa_ctrl_buffer.get());
        map_obj_parser(ss_in, {'\n', '='}, reply);
    }

    return true;
}

bool base_wlan_hal_wav::wpa_ctrl_send_msg(const std::string& cmd, parsed_obj_listed_map_t& reply)
{
    if (!wpa_ctrl_send_msg(cmd)) {
        return false;
    }

	if (strncmp(m_wpa_ctrl_buffer.get(), "", 1)) {
        // if reply is not empty
        std::stringstream ss_in(m_wpa_ctrl_buffer.get());
        listed_map_obj_parser(ss_in, {'\n', ' ', '='}, reply);
    }

    return true;
}

bool base_wlan_hal_wav::wpa_ctrl_send_msg(const std::string& cmd, char** reply)
{
    if (!reply || !(*reply)) {
        LOG(ERROR) << "Invalid reply pointer!";
        return false;
    }
    
    if (!wpa_ctrl_send_msg(cmd)) {
        return false;
    }

    *reply = m_wpa_ctrl_buffer.get();

    return true;
}

void base_wlan_hal_wav::parsed_obj_debug(parsed_obj_map_t& obj)
{
    LOG(TRACE) << "parsed_obj_debug:";
    std::stringstream ss_obj;
    ss_obj << std::endl << "parsed_obj_debug: " << std::endl;
    for(auto element:obj) {
        LOG(TRACE) << "key: " << element.first << ", value: " << element.second;
		ss_obj << "key: " << element.first << ", value: " << element.second << std::endl;
	}

    LOG(DEBUG) << ss_obj.str();
}

void base_wlan_hal_wav::parsed_obj_debug(parsed_obj_listed_map_t& obj)
{
    LOG(TRACE) << "parsed_obj_debug:";
    std::stringstream ss_obj;
    ss_obj << std::endl << "parsed_obj_debug: " << std::endl;
	int element_num = 0;
	for(auto list_element:obj) {
        LOG(TRACE) << "vector element: " << element_num;
		ss_obj << "vector element: " << element_num << std::endl;
		for(auto map_element:list_element) {
            LOG(TRACE) << "key: " << map_element.first << ", value: " << map_element.second;
			ss_obj << "key: " << map_element.first << ", value: " << map_element.second << std::endl;
		}
		element_num++;
	}
    LOG(DEBUG) << ss_obj.str();
}

bool base_wlan_hal_wav::refresh_radio_info()
{ 
    parsed_obj_map_t reply;

    if(!wpa_ctrl_send_msg("GET_RADIO_INFO", reply)){
        LOG(ERROR) << __func__ <<  " failed";
        return false;
    }

    // Station HAL
    if (get_type() == HALType::Station) {
        // update radio info struct
        // TODO test the the output
        // m_radio_info.wifi_ctrl_enabled = beerocks::string_utils::stoi(reply["HostapdEnabled"]);
        // m_radio_info.tx_enabled        = beerocks::string_utils::stoi(reply["TxPower"]);
        // m_radio_info.channel           = beerocks::string_utils::stoi(reply["Channel"]);
        if (m_radio_info.channel <= 0) {
            //LOG(ERROR) << "X_LANTIQ_COM_Vendor_Channel not valid: " << radio_info.channel;
            return false;
        } else if (m_radio_info.channel > 14) {
            m_radio_info.is_5ghz = true;
        }

    } else {
        // update radio info struct
        int64_t tmp_int;

        if (!wav_obj_read_int("TxAntennas", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading TxAntennas parameter!";
            return false;
        }
        m_radio_info.ant_num = tmp_int;

        if (!wav_obj_read_int("TxPower", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading TxPower parameter!";
            return false;
        }
        m_radio_info.conducted_power = tmp_int;
        
        if (!wav_obj_read_int("OperatingChannelBandwidt", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading OperatingChannelBandwidt parameter!";
            return false;
        }
        m_radio_info.bandwidth = tmp_int;

        if (!wav_obj_read_int("Cf1", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading Cf1 parameter!";
            return false;
        }
        m_radio_info.vht_center_freq = tmp_int;

        if (!wav_obj_read_int("HostapdEnabled", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading HostapdEnabled parameter!";
            return false;
        }
        m_radio_info.wifi_ctrl_enabled = tmp_int;

        if (!wav_obj_read_int("TxEnabled", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading TxEnabled parameter!";
            return false;
        }
        m_radio_info.tx_enabled = tmp_int;

        if (!wav_obj_read_int("Channel", reply, tmp_int)) {
            LOG(ERROR) << "Failed reading Channel parameter!";
            return false;
        }
        m_radio_info.channel = tmp_int;

        m_radio_info.is_5ghz = (son::wireless_utils::which_freq(m_radio_info.channel) == beerocks::eFreqType::FREQ_5G);

        // If the VAPs map is empty, refresh it as well
        // TODO: update on every refresh?
        if (!m_radio_info.available_vaps.size()) {
            if (!refresh_vaps_info(beerocks::IFACE_RADIO_ID)) {
                return false;
            }
        }
    }   

    return true;
}

bool base_wlan_hal_wav::refresh_vaps_info()
{
    LOG(TRACE) << __func__;

    char* tmp_str;

    for (int vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id <= beerocks::IFACE_VAP_ID_MAX; vap_id++) {

        // Get the VAP interface
        auto vap_iface = beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
        
        std::string cmd = "GET_VAP_MEASUREMENTS " + vap_iface;
        parsed_obj_map_t reply;
        
        // Read the VAP information
        if (!wpa_ctrl_send_msg(cmd, reply) || reply.empty()) {
            
            // On any other VAP assume that it doesn't exist, and continue
            if (m_radio_info.available_vaps.find(vap_id) != m_radio_info.available_vaps.end()) {
                m_radio_info.available_vaps.erase(vap_id);
            }
            continue;
        }

        // New VAP Element
        VAPElement vapElement;

        // Read and Store the BSSID
        if (!wav_obj_read_str("BSSID", reply, &tmp_str)) {
            LOG(ERROR) << "Failed reading BSSID parameter!";
            return false;
        }
        vapElement.mac = reply["BSSID"];

        // Read and Store the SSID
        if (!wav_obj_read_str("SSID", reply, &tmp_str)) {
            LOG(ERROR) << "Failed reading SSID parameter!";
            return false;
        }
        vapElement.ssid = reply["SSID"];

        // Store the VAP element
        LOG(DEBUG) << "Detected VAP ID (" << vap_id << ") - MAC: " << vapElement.mac
                   << ", SSID: " << vapElement.ssid;

        m_radio_info.available_vaps[vap_id] = vapElement;
    }

    return true;    
}

bool base_wlan_hal_wav::process_ext_events()
{
    if (!m_wpa_ctrl_event) {
        LOG(ERROR) << "Invalid WPA Control socket (m_wpa_ctrl_event == nullptr)";
        return false;
    }

    // Check if there are pending events
    int status = wpa_ctrl_pending(m_wpa_ctrl_event);

    // No pending messages
    if (status == 0) {
        LOG(WARNING) << "Process external events called but there are no pending messages...";
        return false;
    } else if (status < 0) {
        LOG(ERROR) << "Invalid WPA Control socket status: " << status << " --> detaching!";
        detach();
        return false;
    }

    auto buffer = m_wpa_ctrl_buffer.get();
    auto buff_size_copy = m_wpa_ctrl_buffer_size;

    if (wpa_ctrl_recv(m_wpa_ctrl_event, buffer, &buff_size_copy) < 0) {
        LOG(ERROR) << "wpa_ctrl_recv() failed!";
        return false;
    }

    buffer[buff_size_copy] = 0; // the wpa_ctrl does not put null termintaor at the and of the string
    
    /* Silencing unhandled multiple events */
    if (strstr(buffer, "WPS-ENROLLEE-SEEN")) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG) << "DWPAL unhandled event WPS-ENROLLEE-SEEN recieved";
        return true;
    } else if (strstr(buffer, "AP-PROBE-REQ-RECEIVED")) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG) << "DWPAL unhandled event AP-PROBE-REQ-RECEIVED recieved";
        return true;
    } else if ( strstr(buffer, "RRM-BEACON-REP-RECEIVED")) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG) << "DWPAL unhandled event RRM-BEACON-REP-RECEIVED recieved";
        return true;
    }

    LOG(DEBUG) << "event received:" << std::endl << buffer;

    parsed_obj_map_t event_obj;
    map_event_obj_parser(buffer, event_obj);

    base_wlan_hal_wav::parsed_obj_debug(event_obj);

    // Process the event
    if (!process_wav_event(event_obj)) {
        LOG(ERROR) << "Failed processing WAV event: " << event_obj[WAV_EVENT_KEYLESS_PARAM_OPCODE];
        return false;
    }

    return true;
}

std::string base_wlan_hal_wav::get_radio_mac()
{
	std::string mac;
	if (!beerocks::net::network_utils::linux_iface_get_mac(m_radio_info.iface_name, mac)) {
		LOG(ERROR) << "Failed to get radio mac from ifname " << m_radio_info.iface_name;
	}
	return mac;
}

void base_wlan_hal_wav::send_ctrl_iface_cmd(std::string cmd)
{
    parsed_obj_map_t obj1;
    parsed_obj_listed_map_t obj2;

    auto last_char = cmd.back();

    LOG(DEBUG) << "last char: '" << last_char << "'";

    if(last_char == '1' || last_char == '2') {
        cmd.pop_back();
    }
    cmd = beerocks::string_utils::trimmed_substr(cmd);

    if(last_char == '1') {
        wpa_ctrl_send_msg(cmd, obj1);
    } else if (last_char == '2') {
        wpa_ctrl_send_msg(cmd, obj2);
    } else {
        wpa_ctrl_send_msg(cmd);
    }
}

} // namespace wav
} // namespace bwl