/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ap_wlan_hal_dummy.h"

#include <beerocks/bcl/beerocks_defines.h>
#include <beerocks/bcl/beerocks_os_utils.h>
#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/beerocks_version.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>
#include <easylogging++.h>
#include <math.h>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace dummy {

#define CSA_EVENT_FILTERING_TIMEOUT_MS 1000

// As defined on "ieee802_11_defs.h":
#define WLAN_FC_STYPE_PROBE_REQ 4
#define WLAN_FC_STYPE_AUTH 11

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

// Temporary storage for station capabilities
struct SRadioCapabilitiesStrings {
    std::string supported_rates;
    std::string ht_cap;
    std::string ht_mcs;
    std::string vht_cap;
    std::string vht_mcs;
    std::string rrm_caps;
};

// Temporary storage for parsed ACS report
struct DUMMY_acs_report_get {
    int Ch;
    int BW;
    int DFS;
    int bss;
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static ap_wlan_hal::Event dummy_to_bwl_event(const std::string &opcode)
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

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// NOTE: Since *base_wlan_hal_dummy* inherits *base_wlan_hal* virtually, we
//       need to explicitly call it's from any deriving class
ap_wlan_hal_dummy::ap_wlan_hal_dummy(std::string iface_name, bool acs_enabled,
                                     hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::AccessPoint, iface_name, IfaceType::Intel, acs_enabled, callback),
      base_wlan_hal_dummy(bwl::HALType::AccessPoint, iface_name, acs_enabled, callback, 0)
{
}

ap_wlan_hal_dummy::~ap_wlan_hal_dummy() {}

HALState ap_wlan_hal_dummy::attach(bool block)
{
    auto state = base_wlan_hal_dummy::attach(block);

    // On Operational send the AP_Attached event to the AP Manager
    if (state == HALState::Operational) {
        event_queue_push(Event::AP_Attached);
    }

    return state;
}

bool ap_wlan_hal_dummy::sta_allow(const std::string &mac) { return true; }

bool ap_wlan_hal_dummy::sta_deny(const std::string &mac, int reject_sta) { return true; }

bool ap_wlan_hal_dummy::sta_disassoc(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_deauth(int8_t vap_id, const std::string &mac, uint32_t reason)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_bss_steer(const std::string &mac, const std::string &bssid, int chan,
                                      int disassoc_timer, int valid_int)
{
    LOG(DEBUG) << "Got steer request for " << mac << " steer to " << bssid;
    return true;
}

bool ap_wlan_hal_dummy::sta_unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                     int vht_center_frequency, int delay,
                                                     int window_size)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_softblock_add(const std::string &vap_name,
                                          const std::string &client_mac, uint8_t reject_error_code,
                                          uint8_t probe_snr_threshold_hi,
                                          uint8_t probe_snr_threshold_lo,
                                          uint8_t authetication_snr_threshold_hi,
                                          uint8_t authetication_snr_threshold_lo)
{
    return true;
}

bool ap_wlan_hal_dummy::sta_softblock_remove(const std::string &vap_name,
                                             const std::string &client_mac)
{
    return true;
}

bool ap_wlan_hal_dummy::switch_channel(int chan, int bw, int vht_center_frequency)
{
    LOG(DEBUG) << "Got channel switch, simulate ACS-STARTED;ACS-COMPLETED";
    event_queue_push(Event::ACS_Started);
    event_queue_push(Event::ACS_Completed);
    event_queue_push(Event::CSA_Finished);

    return true;
}

bool ap_wlan_hal_dummy::set_antenna_mode(AntMode mode) { return true; }

bool ap_wlan_hal_dummy::wds_set_mode(WDSMode mode) { return true; }

bool ap_wlan_hal_dummy::wds_add_sta(const std::string &mac) { return true; }

bool ap_wlan_hal_dummy::wds_del_sta(const std::string &mac) { return true; }

bool ap_wlan_hal_dummy::wds_clear_list() { return true; }

bool ap_wlan_hal_dummy::failsafe_channel_set(int chan, int bw, int vht_center_frequency)
{
    return true;
}

bool ap_wlan_hal_dummy::failsafe_channel_get(int &chan, int &bw) { return false; }

bool ap_wlan_hal_dummy::restricted_channels_set(char *channel_list) { return true; }

bool ap_wlan_hal_dummy::restricted_channels_get(char *channel_list) { return false; }

bool ap_wlan_hal_dummy::read_acs_report()
{
    uint8_t idx = 0;
    if (m_radio_info.is_5ghz == false) {
        m_radio_info.channel = 1;
        // 2.4G simulated report
        for (uint16_t ch = 1; ch <= 11; ch++) {
            m_radio_info.supported_channels[idx].channel     = ch;
            m_radio_info.supported_channels[idx].bandwidth   = 20;
            m_radio_info.supported_channels[idx].bss_overlap = 10;
            m_radio_info.supported_channels[idx].is_dfs      = 0;
            idx++;
        }
    } else {
        // 5G simulated report
        m_radio_info.channel = 149;
        for (uint16_t ch = 36; ch <= 64; ch += 4) {
            for (uint16_t step = 0; step < 3; step++) {
                m_radio_info.supported_channels[idx].channel     = ch;
                m_radio_info.supported_channels[idx].bandwidth   = 20 + step * 20;
                m_radio_info.supported_channels[idx].bss_overlap = 10 + step * 10;
                m_radio_info.supported_channels[idx].is_dfs      = (ch > 48) ? 1 : 0;
                idx++;
            }
        }
        for (uint16_t ch = 100; ch <= 144; ch += 4) {
            for (uint16_t step = 0; step < 3; step++) {
                m_radio_info.supported_channels[idx].channel     = ch;
                m_radio_info.supported_channels[idx].bandwidth   = 20 + step * 20;
                m_radio_info.supported_channels[idx].bss_overlap = 10 + step * 10;
                m_radio_info.supported_channels[idx].is_dfs      = 1;
                idx++;
            }
        }
        for (uint16_t ch = 149; ch <= 165; ch += 4) {
            for (uint16_t step = 0; step < 3; step++) {
                m_radio_info.supported_channels[idx].channel     = ch;
                m_radio_info.supported_channels[idx].bandwidth   = 20 + step * 20;
                m_radio_info.supported_channels[idx].bss_overlap = 10 + step * 10;
                m_radio_info.supported_channels[idx].is_dfs      = 0;
                idx++;
            }
        }
    }

    return true;
}

bool ap_wlan_hal_dummy::set_vap_enable(const std::string &iface_name, const bool enable)
{
    return true;
}

bool ap_wlan_hal_dummy::get_vap_enable(const std::string &iface_name, bool &enable) { return true; }

std::string ap_wlan_hal_dummy::get_radio_driver_version() { return std::string("dummy"); }

bool ap_wlan_hal_dummy::process_dummy_event(parsed_obj_map_t &parsed_obj)
{
    char *tmp_str;

    // Filter out empty events
    std::string opcode;
    if (!(parsed_obj.find(DUMMY_EVENT_KEYLESS_PARAM_OPCODE) != parsed_obj.end() &&
          !(opcode = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE]).empty())) {
        return true;
    }

    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = dummy_to_bwl_event(opcode);

    switch (event) {
    // STA Connected
    case Event::STA_Connected: {
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION));

        msg->params.vap_id = beerocks::IFACE_VAP_ID_MIN;
        LOG(DEBUG) << "iface name = " << get_iface_name()
                   << ", vap_id = " << int(msg->params.vap_id);

        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }

        msg->params.mac = beerocks::net::network_utils::mac_from_string(tmp_str);

        bool caps_valid = true;
        SRadioCapabilitiesStrings caps_strings;
        if (!dummy_obj_read_str("SupportedRates", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading SupportedRates parameter!";
            caps_valid = false;
        } else {
            caps_strings.supported_rates.assign(tmp_str);
        }

        if (!dummy_obj_read_str("HT_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.ht_cap.assign(tmp_str);
        }

        if (!dummy_obj_read_str("HT_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading HT_MCS parameter!";
            caps_valid = false;
        } else {
            caps_strings.ht_mcs.assign(tmp_str);
        }

        if (!dummy_obj_read_str("VHT_CAP", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading VHT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.vht_cap.assign(tmp_str);
        }

        if (!dummy_obj_read_str("VHT_MCS", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading VHT_CAP parameter!";
            caps_valid = false;
        } else {
            caps_strings.vht_mcs.assign(tmp_str);
        }

        if (caps_valid) {
            //get_sta_caps(caps_strings, msg->params.capabilities, get_radio_info().is_5ghz);
        } else {
            LOG(ERROR) << "One or more of required capability strings is missing!";

            // Setting minimum default values
            msg->params.capabilities.ant_num       = 1;
            msg->params.capabilities.wifi_standard = STANDARD_N;
            msg->params.capabilities.default_mcs   = MCS_6;
        }

        // Add the message to the queue
        event_queue_push(Event::STA_Connected, msg_buff);
    } break;
    case Event::STA_Disconnected: {
        auto msg_buff =
            ALLOC_SMART_BUFFER(sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));
        auto msg =
            reinterpret_cast<sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION *>(msg_buff.get());
        LOG_IF(!msg, FATAL) << "Memory allocation failed!";

        // Initialize the message
        memset(msg_buff.get(), 0, sizeof(sACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION));

        msg->params.vap_id = beerocks::IFACE_VAP_ID_MIN;
        LOG(DEBUG) << "iface name = " << get_iface_name()
                   << ", vap_id = " << int(msg->params.vap_id);

        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }

        // Store the MAC address of the disconnected STA
        msg->params.mac = beerocks::net::network_utils::mac_from_string(tmp_str);

        // Add the message to the queue
        event_queue_push(Event::STA_Disconnected, msg_buff);
    } break;
    // Gracefully ignore unhandled events
    default: {
        LOG(WARNING) << "Unhandled event received: " << opcode;
        return true;

    } break;
    }

    return true;
}

} // namespace dummy
} // namespace bwl

// AP dummy HAL Factory Functions
extern "C" {

bwl::ap_wlan_hal *ap_wlan_hal_create(std::string iface_name, bool acs_enabled,
                                     bwl::base_wlan_hal::hal_event_cb_t callback)
{
    return new bwl::dummy::ap_wlan_hal_dummy(iface_name, acs_enabled, callback);
}

void ap_wlan_hal_destroy(bwl::ap_wlan_hal *obj) { delete obj; }
}
