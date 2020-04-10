/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_econet.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

#include <easylogging++.h>

#include <cmath>

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////// DUMMY////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

namespace bwl {
namespace dummy {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

#define GET_OP_CLASS(channel) ((channel < 14) ? 4 : 5)
#define BUFFER_SIZE 4096

// Allocate a char array wrapped in a shared_ptr
#define ALLOC_SMART_BUFFER(size)                                                                   \
    std::shared_ptr<char>(new char[size], [](char *obj) {                                          \
        if (obj)                                                                                   \
            delete[] obj;                                                                          \
    })

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static mon_wlan_hal::Event dummy_to_bwl_event(const std::string &opcode)
{
    if (opcode == "RRM-BEACON-REP-RECEIVED")
        return mon_wlan_hal::Event::RRM_Beacon_Response;

    return mon_wlan_hal::Event::Invalid;
}

static mon_wlan_hal_dummy::Data dummy_to_bwl_data(const std::string &opcode)
{
    if (opcode == "STA-UPDATE-STATS")
        return mon_wlan_hal_dummy::Data::STA_Update_Stats;
    else if (opcode == "RRM-BEACON-REP-RECEIVED")
        return mon_wlan_hal_dummy::Data::RRM_Update_Beacon_Measurements;

    return mon_wlan_hal_dummy::Data::Invalid;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

mon_wlan_hal_dummy::mon_wlan_hal_dummy(std::string iface_name, hal_event_cb_t callback)
    : base_wlan_hal(bwl::HALType::Monitor, iface_name, IfaceType::Intel, callback),
      base_wlan_hal_dummy(bwl::HALType::Monitor, iface_name, callback)
{
}

mon_wlan_hal_dummy::~mon_wlan_hal_dummy() {}

bool mon_wlan_hal_dummy::update_radio_stats(SRadioStats &radio_stats)
{
    radio_stats = {};
    return true;
}

bool mon_wlan_hal_dummy::update_vap_stats(const std::string vap_iface_name, SVapStats &vap_stats)
{
    vap_stats = {};
    return true;
}

bool mon_wlan_hal_dummy::update_stations_stats(const std::string vap_iface_name,
                                               const std::string sta_mac, SStaStats &sta_stats)
{
    SStaStats dummy_sta_stats;
    auto dummy_sta = m_dummy_stas_map.find(sta_mac);

    if (dummy_sta == m_dummy_stas_map.end()) {
        LOG(WARNING) << "No stats for sta " << sta_mac;
        return false;
    }

    dummy_sta_stats = dummy_sta->second.sta_stats;

    sta_stats.rx_rssi_watt             = dummy_sta_stats.rx_rssi_watt;
    sta_stats.rx_rssi_watt_samples_cnt = dummy_sta_stats.rx_rssi_watt_samples_cnt;
    sta_stats.rx_snr_watt              = dummy_sta_stats.rx_snr_watt;
    sta_stats.rx_snr_watt_samples_cnt  = dummy_sta_stats.rx_snr_watt_samples_cnt;
    sta_stats.tx_phy_rate_100kb        = dummy_sta_stats.tx_phy_rate_100kb;
    sta_stats.rx_phy_rate_100kb        = dummy_sta_stats.rx_phy_rate_100kb;

    return true;
}

bool mon_wlan_hal_dummy::sta_channel_load_11k_request(const SStaChannelLoadRequest11k &req)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_dummy::sta_beacon_11k_request(const SBeaconRequest11k &req, int &dialog_token)
{
    LOG(TRACE) << __func__;

    auto sta_mac = beerocks::net::network_utils::mac_to_string(req.sta_mac.oct);
    auto bssid   = beerocks::net::network_utils::mac_to_string(req.bssid.oct);

    LOG(DEBUG) << "Beacon 11k request to sta " << sta_mac << " on bssid " << bssid << " channel "
               << std::to_string(req.channel);

    auto dummy_sta = m_dummy_stas_map.find(sta_mac);
    if (dummy_sta != m_dummy_stas_map.end()) {
        auto dummy_bssid_event = dummy_sta->second.beacon_measurment_events.find(bssid);
        if (dummy_bssid_event == dummy_sta->second.beacon_measurment_events.end()) {
            LOG(WARNING) << "No RRM beacon response for bssid " << bssid;
        } else {
            //TODO: remove sleep. Sleep fixes missing requests sent by controller but not
            //received by agent
            sleep(1);
            process_dummy_event(dummy_bssid_event->second);
        }
    } else
        LOG(WARNING) << "No RRM beacon response for sta " << sta_mac;

    return true;
}

bool mon_wlan_hal_dummy::sta_statistics_11k_request(const SStatisticsRequest11k &req)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_dummy::sta_link_measurements_11k_request(const std::string &sta_mac)
{
    LOG(TRACE) << __func__;
    return true;
}

bool mon_wlan_hal_dummy::channel_scan_trigger(int dwell_time_msec,
                                              const std::vector<unsigned int> &channel_pool)
{
    LOG(TRACE) << __func__ << " - NOT IMPLEMENTED";
    return false;
}

bool mon_wlan_hal_dummy::process_dummy_data(parsed_obj_map_t &parsed_obj)
{
    char *tmp_str;
    char *sta_mac;
    char *bssid;
    int64_t tmp_int;
    // Filter out empty data
    std::string opcode;
    if (!(parsed_obj.find(DUMMY_EVENT_KEYLESS_PARAM_OPCODE) != parsed_obj.end() &&
          !(opcode = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE]).empty())) {
        return true;
    }
    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto data = dummy_to_bwl_data(opcode);

    switch (data) {
    case Data::STA_Update_Stats: {
        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &sta_mac)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }

        if (!dummy_obj_read_str("rssi", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading rssi!";
            return false;
        }

        SStaStats dummy_sta_stats = {};

        // Format rssi= %d,%d,%d,%d
        auto samples = beerocks::string_utils::str_split(tmp_str, ',');
        for (auto &s : samples) {
            float s_float = float(beerocks::string_utils::stoi(s));
            if (s_float > beerocks::RSSI_MIN) {
                dummy_sta_stats.rx_rssi_watt += std::pow(10, s_float / float(10));
                dummy_sta_stats.rx_rssi_watt_samples_cnt++;
            }
        }

        if (!dummy_obj_read_str("snr", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading snr!";
            return false;
        }

        // Format snr=%d,%d,%d,%d
        auto samples_snr = beerocks::string_utils::str_split(tmp_str, ',');
        for (auto &s : samples_snr) {
            float s_float = float(beerocks::string_utils::stoi(s));
            if (s_float >= beerocks::SNR_MIN) {
                dummy_sta_stats.rx_snr_watt += std::pow(10, s_float / float(10));
                dummy_sta_stats.rx_snr_watt_samples_cnt++;
            }
        }

        if (!dummy_obj_read_int("uplink", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading rsni parameter!";
            return false;
        }
        dummy_sta_stats.tx_phy_rate_100kb = (tmp_int / 100);

        if (!dummy_obj_read_int("downlink", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading rsni parameter!";
            return false;
        }
        dummy_sta_stats.rx_phy_rate_100kb = (tmp_int / 100);

        m_dummy_stas_map[sta_mac].sta_stats = dummy_sta_stats;
        break;
    }
    case Data::RRM_Update_Beacon_Measurements: {
        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &sta_mac)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }

        if (!dummy_obj_read_str("bssid", parsed_obj, &bssid)) {
            LOG(ERROR) << "Failed reading bssid!";
            return false;
        }

        m_dummy_stas_map[sta_mac].beacon_measurment_events[bssid] = parsed_obj;
        break;
    }
    default:
        LOG(WARNING) << "Unhandled event received: " << opcode;
        break;
    }

    return true;
}

bool mon_wlan_hal_dummy::process_dummy_event(parsed_obj_map_t &parsed_obj)
{
    char *tmp_str;
    int64_t tmp_int;
    // Filter out empty events
    std::string opcode;
    if (!(parsed_obj.find(DUMMY_EVENT_KEYLESS_PARAM_OPCODE) != parsed_obj.end() &&
          !(opcode = parsed_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE]).empty())) {
        return true;
    }
    LOG(TRACE) << __func__ << " - opcode: |" << opcode << "|";

    auto event = dummy_to_bwl_event(opcode);

    switch (event) {
    case Event::RRM_Beacon_Response: {
        // Allocate response object
        auto resp_buff = ALLOC_SMART_BUFFER(sizeof(SBeaconResponse11k));
        auto resp      = reinterpret_cast<SBeaconResponse11k *>(resp_buff.get());

        if (!resp) {
            LOG(FATAL) << "Memory allocation failed!";
            return false;
        }

        // Initialize the message
        memset(resp_buff.get(), 0, sizeof(SBeaconResponse11k));

        // STA MAC
        if (!dummy_obj_read_str(DUMMY_EVENT_KEYLESS_PARAM_MAC, parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }

        beerocks::net::network_utils::mac_from_string(resp->sta_mac.oct, tmp_str);

        // Channel
        if (!dummy_obj_read_int("channel", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading channel parameter!";
            return false;
        }
        resp->channel = tmp_int;

        // Dialog Token
        if (!dummy_obj_read_int("dialog_token", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading dialog_token parameter!";
            return false;
        }
        resp->dialog_token = tmp_int;

        // Measurement Response Mode
        if (!dummy_obj_read_int("measurement_rep_mode", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading measurement_rep_mode parameter!";
            return false;
        }
        resp->rep_mode = tmp_int;

        // Operating Class
        if (!dummy_obj_read_int("op_class", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading op_class parameter!";
            return false;
        }
        resp->op_class = tmp_int;

        // Measurement Duration
        if (!dummy_obj_read_int("duration", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading duration parameter!";
            return false;
        }
        resp->duration = tmp_int;

        // RCPI
        if (!dummy_obj_read_int("rcpi", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading rcpi parameter!";
            return false;
        }
        resp->rcpi = tmp_int;

        // RSNI
        if (!dummy_obj_read_int("rsni", parsed_obj, tmp_int)) {
            LOG(ERROR) << "Failed reading rsni parameter!";
            return false;
        }
        resp->rsni = tmp_int;

        // BSSID
        if (!dummy_obj_read_str("bssid", parsed_obj, &tmp_str)) {
            LOG(ERROR) << "Failed reading mac parameter!";
            return false;
        }
        beerocks::net::network_utils::mac_from_string(resp->bssid.oct, tmp_str);

        // Add the message to the queue
        event_queue_push(event, resp_buff);
        break;
    }
    default:
        LOG(WARNING) << "Unhandled event received: " << opcode;
        break;
    }
    return true;
}

bool mon_wlan_hal_dummy::set(const std::string &param, const std::string &value, int vap_id)
{
    LOG(TRACE) << __func__;
    return true;
}

} // namespace dummy

std::shared_ptr<mon_wlan_hal> mon_wlan_hal_create(std::string iface_name,
                                                  base_wlan_hal::hal_event_cb_t callback)
{
    return std::make_shared<dummy::mon_wlan_hal_dummy>(iface_name, callback);
}

} // namespace bwl
