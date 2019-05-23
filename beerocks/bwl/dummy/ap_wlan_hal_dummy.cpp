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

bool ap_wlan_hal_dummy::read_acs_report() { return true; }

bool ap_wlan_hal_dummy::set_vap_enable(const std::string &iface_name, const bool enable)
{
    return true;
}

bool ap_wlan_hal_dummy::get_vap_enable(const std::string &iface_name, bool &enable) { return true; }

std::string ap_wlan_hal_dummy::get_radio_driver_version() { return std::string("dummy"); }

bool ap_wlan_hal_dummy::process_dummy_event(char *buffer, int bufLen, const std::string &opcode)
{
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
