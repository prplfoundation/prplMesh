/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "mon_wlan_hal_dummy.h"

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
    sta_stats = {};
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

    std::string measurement_mode;
    switch ((SBeaconRequest11k::MeasurementMode)(req.measurement_mode)) {
    case SBeaconRequest11k::MeasurementMode::Passive:
        measurement_mode = "passive";
        break;
    case SBeaconRequest11k::MeasurementMode::Active:
        measurement_mode = "active";
        break;
    case SBeaconRequest11k::MeasurementMode::Table:
        measurement_mode = "table";
        break;
    default:
        measurement_mode = "passive";
    }

    auto request = (!req.enable) ? 0 : req.request;
    auto report  = (!req.enable) ? 0 : req.report;

    uint8_t req_mode = (req.parallel | (req.enable ? 0x02 : 0) | (request ? 0x04 : 0) |
                        (report ? 0x08 : 0) | (req.mandatory_duration ? 0x10 : 0));

    auto op_class = req.op_class < 0 ? GET_OP_CLASS(get_radio_info().channel) : req.op_class;

    std::string cmd = beerocks::net::network_utils::mac_to_string(req.sta_mac.oct) +
                      " " + // Destination MAC Address
                      beerocks::net::network_utils::mac_to_string(req.bssid.oct) +
                      " " +                                 // Target BSSID
                      std::to_string(req.channel) + " " +   // Channel
                      std::to_string(req.repeats) + " " +   // Number of repitions
                      std::to_string(req_mode) + " " +      // Measurements Request Mode
                      std::to_string(op_class) + " " +      // Operating Class
                      std::to_string(req.rand_ival) + " " + // Random Interval
                      std::to_string(req.duration) + " " +  // Duration
                      measurement_mode;                     // Measurement Mode

    if (req.use_optional_ssid) {
        std::string req_ssid = '"' + std::string((char *)req.ssid) + '"';

        cmd += " ssid=" + req_ssid;
    }

    LOG(DEBUG) << "Beacon 11k request " << cmd;
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

bool mon_wlan_hal_dummy::process_dummy_data(parsed_obj_map_t &parsed_obj) { return true; }

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
