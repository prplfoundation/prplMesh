/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "monitor_rdkb_hal.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_monitor.h>

using namespace beerocks;
using namespace net;
using namespace son;

monitor_rdkb_hal::monitor_rdkb_hal(ieee1905_1::CmduMessageTx &cmdu_tx_) : cmdu_tx(cmdu_tx_)
{
    mon_db       = nullptr;
    slave_socket = nullptr;
}

void monitor_rdkb_hal::stop()
{
    mon_db       = nullptr;
    slave_socket = nullptr;
}

bool monitor_rdkb_hal::start(monitor_db *mon_db_, Socket *slave_socket_)
{
    if (!mon_db_ || !slave_socket_) {
        LOG(ERROR) << "invalid parameters";
        return false;
    }

    stop();

    mon_db       = mon_db_;
    slave_socket = slave_socket_;

    return true;
}

monitor_rdkb_hal::snr_change_t monitor_rdkb_hal::get_snr_change_type(int8_t prev, int8_t cur,
                                                                     int8_t th)
{
    if (prev == SNR_INVALID || cur == SNR_INVALID) {
        return WIFI_STEERING_SNR_UNCHANGED;
    }

    if (prev <= th && cur > th) {
        return WIFI_STEERING_SNR_HIGHER;
    } else if (prev > th && cur <= th) {
        return WIFI_STEERING_SNR_LOWER;
    } else {
        return WIFI_STEERING_SNR_UNCHANGED;
    }
}

void monitor_rdkb_hal::print_debug_info(mon_rdkb_debug_info_t &mdi, bool pkts_count_en)
{
    std::stringstream ss;
    ss << mdi.sample << ": " << mdi.sta_mac << " prev_snr:" << int(mdi.prev_snr)
       << " curr_snr:" << int(mdi.cur_snr) << " snrInact:" << int(mdi.snrInactXing)
       << " snrLow:" << int(mdi.snrLowXing) << " snrHigh:" << int(mdi.snrHighXing)
       << " act_state:" << mdi.state;

    if (pkts_count_en) {
        ss << " tx_pkts:" << int(mdi.tx_packets) << " rx_pkts:" << int(mdi.rx_packets)
           << " accum_tx_pkts:" << int(mdi.accumulatedPackets);
    } else {
        ss << " {tx_pkts|rx_pkts|accum_pkts}: N/A";
    }

    LOG(DEBUG) << ss.str();
}

void monitor_rdkb_hal::process()
{

    if (mon_db == nullptr) {
        LOG(ERROR) << "mon_db not initialized";
        return;
    }
    //snr didn't update yet.
    if (mon_db->is_last_poll() == false) {
        return;
    }

    for (auto it : conf_stas) {

        auto sta_mac     = it.first;
        auto conf_client = it.second;

        auto sta_node = mon_db->sta_find(sta_mac);
        //client not connected.
        if (sta_node == nullptr) {
            continue;
        }

        auto sta_stats  = sta_node->get_stats();
        int8_t prev_snr = conf_client->getRxPrevSnr();
        int8_t cur_snr  = sta_stats.rx_snr_curr;

        int8_t snrInactXing = conf_client->getSnrInactXing();
        int8_t snrHighXing  = conf_client->getSnrHighXing();
        int8_t snrLowXing   = conf_client->getSnrLowXing();

        conf_client->setRxPrevSnr(sta_stats.rx_snr_curr);

        monitor_rdkb_hal::crossing_status_t ths;

        ths.high     = get_snr_change_type(prev_snr, cur_snr, snrHighXing);
        ths.low      = get_snr_change_type(prev_snr, cur_snr, snrLowXing);
        ths.inactive = get_snr_change_type(prev_snr, cur_snr, snrInactXing);

        if (ths.high != WIFI_STEERING_SNR_UNCHANGED || ths.low != WIFI_STEERING_SNR_UNCHANGED ||
            ths.inactive != WIFI_STEERING_SNR_UNCHANGED) {
            send_snr_crossing_event(sta_mac, sta_stats, ths, conf_client->getVapIndex());
            LOG(DEBUG) << "snr cross event " << sta_mac << " prev snr:" << int(prev_snr)
                       << " snr:" << int(cur_snr) << " inactive:" << ths.inactive
                       << " low:" << ths.low << " high:" << ths.high;
        }

        const auto conf_ap = conf_find_ap(conf_client->getVapIndex());
        if (!conf_ap) {
            LOG(ERROR) << "configuration for sta " << sta_mac
                       << " ap: " << int(conf_client->getVapIndex())
                       << " not found, aborting inactivity process";
            continue;
        }

        mon_rdkb_debug_info_t debug_info;
        debug_info.sample       = conf_client->getSample();
        debug_info.sta_mac      = sta_mac;
        debug_info.prev_snr     = prev_snr;
        debug_info.cur_snr      = cur_snr;
        debug_info.snrInactXing = snrInactXing;
        debug_info.snrLowXing   = snrLowXing;
        debug_info.snrHighXing  = snrHighXing;
        debug_info.state        = conf_client->getStateAbbreviation();

        bool isSampleIntervalPassed =
            conf_client->getLastSampleTime() +
                std::chrono::milliseconds(1000 * conf_ap->getInactCheckIntervalSec()) <
            std::chrono::steady_clock::now();

        if (isSampleIntervalPassed) {
            conf_client->setLastSampleTime(std::chrono::steady_clock::now());
            debug_info.tx_packets = sta_node->get_tx_packets();
            debug_info.rx_packets = sta_node->get_rx_packets();

            uint32_t nPacketsSample = sta_node->get_tx_packets() + sta_node->get_rx_packets();
            conf_client->setSamplePackets(nPacketsSample);

            debug_info.accumulatedPackets = conf_client->getAccumulatedPackets() + nPacketsSample;
            conf_client->setAccumulatedPackets(conf_client->getAccumulatedPackets() +
                                               nPacketsSample);

            conf_client->setSample(debug_info.sample + 1);
        }

        print_debug_info(debug_info, isSampleIntervalPassed);

        bool thresholdTimeElapsed =
            conf_client->getStartTime() +
                std::chrono::milliseconds(1000 * conf_ap->getInactCheckThresholdSec()) <
            std::chrono::steady_clock::now();
        if (thresholdTimeElapsed == false) {
            continue;
        }

        switch (conf_client->getState()) {
        case rdkb_hal_sta_config::HAL_CLIENT_START_STATE: {
            if (conf_client->getAccumulatedPackets() > conf_ap->getInactCheckThresholdPackets()) {
                conf_client->setState(rdkb_hal_sta_config::HAL_CLIENT_ACTIVE_STATE);
                send_activity_event(sta_mac, true, conf_client->getVapIndex());
            } else {
                conf_client->setState(rdkb_hal_sta_config::HAL_CLIENT_INACTIVE_STATE);
                send_activity_event(sta_mac, false, conf_client->getVapIndex());
                LOG(DEBUG) << "goto  HAL_CLIENT_INACTIVE_STATE from START mac: " << sta_mac
                           << " s:" << debug_info.sample;
            }
            break;
        }
        case rdkb_hal_sta_config::HAL_CLIENT_INACTIVE_STATE: {
            if (conf_client->getAccumulatedPackets() > conf_ap->getInactCheckThresholdPackets()) {
                conf_client->setState(rdkb_hal_sta_config::HAL_CLIENT_ACTIVE_STATE);
                send_activity_event(sta_mac, true, conf_client->getVapIndex());
                LOG(DEBUG) << "goto  HAL_CLIENT_ACTIVE_STATE mac: " << sta_mac << " s:" << debug_info.sample;
            }
            break;
        }
        case rdkb_hal_sta_config::HAL_CLIENT_ACTIVE_STATE: {
            if (conf_client->getAccumulatedPackets() <= conf_ap->getInactCheckThresholdPackets()) {
                conf_client->setState(rdkb_hal_sta_config::HAL_CLIENT_INACTIVE_STATE);
                send_activity_event(sta_mac, false, conf_client->getVapIndex());
                LOG(DEBUG) << "goto  HAL_CLIENT_INACTIVE_STATE mac: " << sta_mac << " s:" << debug_info.sample;
            }
            break;
        }
        default:
            LOG(ERROR) << "underfined state for client mac:" << sta_mac;
            break;
        }

        conf_client->setStartTime(std::chrono::steady_clock::now());
        conf_client->setAccumulatedPackets(0);
        conf_client->setSample(0);
    }
}

void monitor_rdkb_hal::send_activity_event(const std::string &sta_mac, bool active, int8_t vap_id)
{
    LOG(DEBUG) << "activity event active: " << active << " mac " << sta_mac;

    auto response = message_com::create_vs_message<
        beerocks_message::cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION "
                      "message!";
        return;
    }

    if (mon_db == nullptr) {
        LOG(ERROR) << "mon_db not initialized";
        return;
    }

    auto vap_node = mon_db->vap_get_by_id(vap_id);
    if (!vap_node) {
        LOG(ERROR) << "Failed getting vap_node for id " << int(vap_id);
    }

    response->params().active     = active;
    response->params().client_mac = network_utils::mac_from_string(sta_mac);
    response->params().bssid      = network_utils::mac_from_string(vap_node->get_mac());

    message_com::send_cmdu(slave_socket, cmdu_tx);
}

void monitor_rdkb_hal::send_snr_crossing_event(const std::string &sta_mac,
                                               monitor_sta_node::SStaStats &sta_stats,
                                               crossing_status_t thrs, int8_t vap_id)
{
    LOG(DEBUG) << "crossing event"
               << " mac " << sta_mac;

    auto response = message_com::create_vs_message<
        beerocks_message::cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR)
            << "Failed building cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION message!";
        return;
    }

    if (mon_db == nullptr) {
        LOG(ERROR) << "mon_db not initialized";
        return;
    }

    auto vap_node = mon_db->vap_get_by_id(vap_id);
    if (!vap_node) {
        LOG(ERROR) << "Failed getting vap_node for id " << int(vap_id);
    }

    response->params().client_mac  = network_utils::mac_from_string(sta_mac);
    response->params().bssid       = network_utils::mac_from_string(vap_node->get_mac());
    response->params().snr         = unsigned(abs(sta_stats.rx_snr_curr));
    response->params().inactveXing = beerocks_message::eSteeringSnrChange(thrs.inactive);
    response->params().highXing    = beerocks_message::eSteeringSnrChange(thrs.high);
    response->params().lowXing     = beerocks_message::eSteeringSnrChange(thrs.low);

    message_com::send_cmdu(slave_socket, cmdu_tx);
}

std::shared_ptr<rdkb_hal_sta_config> monitor_rdkb_hal::conf_add_client(const std::string &sta_mac)
{

    auto cp = conf_find_client(sta_mac);
    if (cp == nullptr) {
        auto node = std::make_shared<son::rdkb_hal_sta_config>(sta_mac);
        auto ret  = conf_stas.insert(std::make_pair(sta_mac, node));
        if (ret.second == true) {
            node->setLastSampleTime(std::chrono::steady_clock::now());
        } else {
            LOG(WARNING) << "existing client configuration not found";
        }
        return ret.first->second;
    } else {
        return cp;
    }
}

std::shared_ptr<rdkb_hal_sta_config> monitor_rdkb_hal::conf_get_client(const std::string &sta_mac)
{
    return conf_find_client(sta_mac);
}

std::shared_ptr<rdkb_hal_sta_config> monitor_rdkb_hal::conf_find_client(const std::string sta_mac)
{

    auto it = conf_stas.find(sta_mac);
    if (it != conf_stas.end()) {
        return (it->second);
    }
    return nullptr;
}

std::shared_ptr<rdkb_hal_ap_config> monitor_rdkb_hal::conf_add_ap(const int8_t vap_id)
{

    auto cp = conf_find_ap(vap_id);
    if (cp == nullptr) {
        auto ap  = std::make_shared<son::rdkb_hal_ap_config>(vap_id);
        auto ret = conf_aps.insert(std::make_pair(vap_id, ap));
        if (ret.second == false) {
            LOG(WARNING) << "existing client configuration not found";
        }
        return ret.first->second;
    } else {
        return cp;
    }
}

std::shared_ptr<rdkb_hal_ap_config> monitor_rdkb_hal::conf_find_ap(const int8_t vap_id)
{

    auto it = conf_aps.find(vap_id);
    if (it != conf_aps.end()) {
        return (it->second);
    }
    return nullptr;
}

template <typename T, typename K> bool monitor_rdkb_hal::conf_erase(T &conf, K k)
{

    auto n = conf.erase(k);
    if (n == 0) {
        LOG(WARNING) << "trying to erase not existing configuration  !";
        return false;
    }

    return true;
}

bool monitor_rdkb_hal::conf_erase_client(const std::string &sta_mac)
{
    return conf_erase(conf_stas, sta_mac);
}

bool monitor_rdkb_hal::conf_erase_ap(int8_t ap_index)
{
    if (!conf_erase(conf_aps, ap_index)) {
        LOG(ERROR) << "Failed to erase AP from configuration";
        return false;
    }

    for (auto it = conf_stas.begin(); it != conf_stas.end();) {
        const auto &sta_conf = it->second;
        if (sta_conf->getVapIndex() == ap_index) {
            it = conf_stas.erase(it);
        } else {
            ++it;
        }
    }

    return true;
}

std::string rdkb_hal_sta_config::getStateAbbreviation()
{
    switch (getState()) {
    case HAL_CLIENT_START_STATE: {
        return "ST";
        break;
    }
    case HAL_CLIENT_INACTIVE_STATE: {
        return "IN";
        break;
    }
    case HAL_CLIENT_ACTIVE_STATE: {
        return "AC";
        break;
    }
    default:
        LOG(ERROR) << "unexpected state" << getState();
        return "UN";
        break;
    };
}

std::string rdkb_hal_sta_config::getStateString(client_state_t state)
{
    switch (state) {
    case HAL_CLIENT_START_STATE: {
        return "HAL_CLIENT_START_STATE";
        break;
    }
    case HAL_CLIENT_INACTIVE_STATE: {
        return "HAL_CLIENT_INACTIVE_STATE";
        break;
    }
    case HAL_CLIENT_ACTIVE_STATE: {
        return "HAL_CLIENT_ACTIVE_STATE";
        break;
    }
    default:
        LOG(ERROR) << "unexpected state" << state;
        return "UNEXPECTED STATE";
        break;
    };
}

void rdkb_hal_sta_config::setState(client_state_t new_state)
{
    LOG(DEBUG) << printStats() << " go to state: " << getStateString(new_state)
               << " from state: " << getStateString(rdkb_hal_client_state);
    rdkb_hal_client_state = new_state;
}

std::string rdkb_hal_sta_config::printStats()
{
    return " total packets: " + std::to_string(getAccumulatedPackets()) +
           " sample_packets: " + std::to_string(getSamplePackets()) + " mac: " + getMac();
}

std::ostream &operator<<(std::ostream &ost, rdkb_hal_sta_config &sta)
{

    ost << std::endl << sta.printStats() << std::endl;

    return ost;
}

void rdkb_hal_sta_config::clearData()
{
    setState(rdkb_hal_sta_config::HAL_CLIENT_START_STATE);
    setRxPrevSnr(SNR_INVALID);
    setSample(0);
}