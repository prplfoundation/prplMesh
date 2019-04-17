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

#include "base_wlan_hal_fapi.h"

#include <easylogging++.h>

#include "ugw_helper.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// FAPI ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define LOGGING_ID base_wlan_hal_fapi
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

extern ObjList *mapperObjPtr;

namespace bwl {
namespace wav_fapi {

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

base_wlan_hal_fapi::base_wlan_hal_fapi(
    HALType type, std::string iface_name, 
    IfaceType iface_type, bool acs_enabled, hal_event_cb_t callback) :
    base_wlan_hal(type, iface_name, iface_type, acs_enabled, callback)
{
    // Initialize the FAPI
    if (mapperObjPtr == nullptr) {
        fapiWlanFailSafeLoad();
    }

    using namespace std::placeholders; // for "_1" arguments
    
    // Create a new instance of the FAPI attach state machine
    m_fapi_attach_fsm = std::make_shared<fapi_attach_fsm>
        (type, m_radio_info, 
         std::bind(&base_wlan_hal_fapi::parse_fapi_event, this, _1, _2));

    // Initialize the FSM
    m_fapi_attach_fsm->setup();
}

base_wlan_hal_fapi::~base_wlan_hal_fapi()
{
    // detach();
}

HALState base_wlan_hal_fapi::attach(bool block)
{
    while (true) {

        auto attach_state = m_fapi_attach_fsm->attach();
        if (m_last_attach_state != attach_state) {
            LOG(DEBUG) << "FAPI FSM " << m_radio_info.iface_name << " Attach State: " << m_fapi_attach_fsm->state_enum_to_string(attach_state);
            m_last_attach_state = attach_state;
        }

        switch (attach_state)
        {
            // Initializing
            case fapi_fsm_state::Delay:
            case fapi_fsm_state::Init:
            case fapi_fsm_state::GetRadioInfo: 
            case fapi_fsm_state::Attach: {
                if (block) {
                    // TODO: Delay?
                    continue;
                } else {
                    return (m_hal_state = HALState::Initializing);
                }
            }
            
            // Initialization completed
            case fapi_fsm_state::Operational: {
                
                // Store the FD for external (FAPI) events
#ifdef USE_FAPI_DAEMON
                m_fd_ext_events = m_fapi_attach_fsm->get_wpa_ctrl_events_fd();
#else
                auto wpa_ctrl = m_fapi_attach_fsm->get_wpa_ctrl_socket();
                m_fd_ext_events = (wpa_ctrl) ? wpa_ctrl_get_fd(wpa_ctrl) : -1;
#endif

                return (m_hal_state = HALState::Operational);
            }

            // Initialization failed
            case fapi_fsm_state::Detach: {
                return (m_hal_state = HALState::Failed);
            }

            // Invalid state
            default: {
                LOG(ERROR) << "Invalid FAPI Attach State: " << int(attach_state);
            }
        }
    };
}

bool base_wlan_hal_fapi::detach()
{
    return m_fapi_attach_fsm->detach();
}

bool base_wlan_hal_fapi::refresh_radio_info()
{
    return m_fapi_attach_fsm->refresh_radio_info();
}

bool base_wlan_hal_fapi::refresh_vaps_info(int id)
{
    return m_fapi_attach_fsm->refresh_vaps_info();
}

bool base_wlan_hal_fapi::process_ext_events()
{
    
#ifdef USE_FAPI_DAEMON

    if (m_fd_ext_events == -1) return true;

    int ret = fapi_wlan_report_process(get_radio_info().iface_name.c_str());    
    if (ret == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_report_process failed: " << ret;
        return false;
    }

#else 
    
    auto wpa_ctrl = m_fapi_attach_fsm->get_wpa_ctrl_socket();    

    if (!wpa_ctrl) {
        LOG(ERROR) << "Invalid WPA Control socket (wpa_ctrl == nullptr)";
        return false;
    }

    // Check if there are pending events
    int status = wpa_ctrl_pending(wpa_ctrl);

    // No pending messages
    if (status == 0) {
        LOG(WARNING) << "base_wlan_hal_fapi::process_ext_events() called but there are no pending messages...";
        return false;
    } else if (status < 0) {
        LOG(ERROR) << "Invalid WPA Control socket status: " << status << " --> detaching!";
        detach();
        return false;
    }

    // Create a new object
    UGW_OBJLIST_CREATE(wlObj);
    LOG_IF(!wlObj, FATAL) << "Memory allocation failed!";
    
    // Process the event
    char opcode[128] = {0};
    int ret = fapi_wlan_hostapd_report_process(get_radio_info().iface_name.c_str(), wlObj.get(), opcode);    
    if (ret == UGW_FAILURE) {
        LOG(ERROR) << "fapi_wlan_hostapd_report_process failed: " << ret;
        return false;
    } else if (ret == WPA_CTRL_DISCONNECT) {
        LOG(INFO) << "Ignoring HOSTAPD-DISCONNECTED or CTRL-EVENT-TERMINATING event...";
    } else {
        
        // Process the event
        if (!parse_fapi_event(opcode, wlObj)) {
            LOG(ERROR) << "Failed parsing FAPI event: " << opcode;
            return false;
        }
    }

#endif

    return true;
}

std::string base_wlan_hal_fapi::get_radio_mac()
{
    // Check if the main VAP exists
    if (m_radio_info.available_vaps.find(beerocks::IFACE_MAIN_VAP_ID) !=
        m_radio_info.available_vaps.end()) {

        // Return the MAC address of the main VAP
        return (m_radio_info.available_vaps[beerocks::IFACE_MAIN_VAP_ID].mac);
    } else {
        return std::string();
    }
}

} // namespace wav_fapi
} // namespace bwl