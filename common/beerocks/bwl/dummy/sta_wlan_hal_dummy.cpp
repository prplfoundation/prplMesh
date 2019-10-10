/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "sta_wlan_hal_dummy.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <easylogging++.h>

#define LOGGING_ID sta_wlan_hal_dummy
#define PACKAGE_ID "0"

namespace bwl {
namespace dummy {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

struct localScanResults {
    uint frequency;
    char bssid[MAC_ADDR_SIZE];
    int8_t rssi;
    char ssid[SSID_MAX_SIZE];
    char vsie[49];
};

static sta_wlan_hal::Event dummy_to_bwl_event(const std::string &opcode)
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

// Convert BWL to dummy security string
static std::string dummy_security_val(WiFiSec sec)
{
    switch (sec) {
    case WiFiSec::None:
        return "NONE";
    case WiFiSec::WEP_64:
        return "WEP-64";
    case WiFiSec::WEP_128:
        return "WEP-128";
    case WiFiSec::WPA_PSK:
        return "WPA-PSK";
    case WiFiSec::WPA2_PSK:
        return "WPA-PSK";
    case WiFiSec::WPA_WPA2_PSK:
        return "WPA-PSK";

    default:
        return "INVALID";
    }
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

sta_wlan_hal_dummy::sta_wlan_hal_dummy(std::string iface_name, hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::Station, iface_name, IfaceType::Intel, callback),
      base_wlan_hal_dummy(bwl::HALType::Station, iface_name, callback)
{
}

sta_wlan_hal_dummy::~sta_wlan_hal_dummy() {}

bool sta_wlan_hal_dummy::initiate_scan() { return true; }

int sta_wlan_hal_dummy::get_scan_results(const std::string &ssid, std::vector<SScanResult> &list,
                                         bool parse_vsie)
{
    return 0;
}

bool sta_wlan_hal_dummy::connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                                 bool mem_only_psk, const std::string &bssid, uint8_t channel,
                                 bool hidden_ssid)
{
    return true;
}

bool sta_wlan_hal_dummy::disconnect() { return true; }

bool sta_wlan_hal_dummy::roam(const std::string &bssid, uint8_t channel) { return false; }

bool sta_wlan_hal_dummy::get_4addr_mode() { return true; }

bool sta_wlan_hal_dummy::set_4addr_mode(bool enable) { return true; }

bool sta_wlan_hal_dummy::unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                                  int vht_center_frequency, int delay,
                                                  int window_size)
{
    return false;
}

bool sta_wlan_hal_dummy::is_connected() { return true; }

int sta_wlan_hal_dummy::get_channel() { return m_active_channel; }

bool sta_wlan_hal_dummy::update_status() { return true; }

std::string sta_wlan_hal_dummy::get_ssid() { return m_active_ssid; }

std::string sta_wlan_hal_dummy::get_bssid() { return m_active_bssid; }

bool sta_wlan_hal_dummy::process_dummy_event(char *buffer, int bufLen, const std::string &opcode)
{
    return true;
}

// Add a new network and return the ID
int sta_wlan_hal_dummy::add_network() { return 0; }

bool sta_wlan_hal_dummy::set_network(int network_id, const std::string &param,
                                     const std::string &value)
{
    return true;
}

bool sta_wlan_hal_dummy::set_network_params(int network_id, const std::string &ssid,
                                            const std::string &bssid, WiFiSec sec,
                                            bool mem_only_psk, const std::string &pass,
                                            bool hidden_ssid, int freq)
{
    return true;
}

bool sta_wlan_hal_dummy::enable_network(int network_id) { return true; }

bool sta_wlan_hal_dummy::is_connected(const std::string &wpa_state) { return true; }

bool sta_wlan_hal_dummy::read_status(ConnectionStatus &connection_status) { return true; }

void sta_wlan_hal_dummy::update_status(const ConnectionStatus &connection_status) {}

} // namespace dummy
} // namespace bwl

// AP dummy HAL Factory Functions
extern "C" {

bwl::sta_wlan_hal *sta_wlan_hal_create(std::string iface_name,
                                       bwl::base_wlan_hal::hal_event_cb_t callback)
{
    return new bwl::dummy::sta_wlan_hal_dummy(iface_name, callback);
}

void sta_wlan_hal_destroy(bwl::sta_wlan_hal *obj) { delete obj; }
}
