/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "node.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <easylogging++.h>

using namespace beerocks;
using namespace son;

node::node(beerocks::eType type_, const std::string mac_)
    : mac(mac_), capabilities(m_sta_24ghz_capabilities) // deafult value
{
    type = type_;
    if ((type == beerocks::TYPE_CLIENT) || (type == beerocks::TYPE_IRE_BACKHAUL)) {
        stats_info = std::make_shared<sta_stats_params>();
    } else if (type == beerocks::TYPE_SLAVE) {
        hostap             = std::make_shared<radio>();
        hostap->stats_info = std::make_shared<radio::ap_stats_params>();
    }
    m_sta_5ghz_capabilities.cap_flag  = false;
    m_sta_24ghz_capabilities.cap_flag = false;
}

namespace son {
std::ostream &operator<<(std::ostream &os, const node &n)
{
    std::chrono::steady_clock::time_point tCurrTime = std::chrono::steady_clock::now();
    auto node_type                                  = const_cast<node *>(&n)->get_type();

    os << std::endl;
    if ((node_type == beerocks::TYPE_IRE_BACKHAUL) || (node_type == beerocks::TYPE_CLIENT)) {
        os << " Type: " << ((node_type == beerocks::TYPE_IRE_BACKHAUL) ? "IRE" : "CLIENT")
           << std::endl
           << " State: " << n.state << std::endl
           << " Name: " << n.name << std::endl
           << " Mac: " << n.mac << std::endl
           << " Ipv4: " << n.ipv4 << std::endl
           << " Manufacturer: " << n.manufacturer << std::endl
           << " ParentMac: " << n.parent_mac << std::endl
           << " PreviousParentMac: " << n.previous_parent_mac << std::endl
           << " Channel: " << int(n.channel) << std::endl
           << " Bandwidth : " << int(n.bandwidth) << std::endl
           << " Radio Identifier: " << n.radio_identifier << std::endl
           << " StaCapabilities: " << std::endl
           << "   ht_mcs=" << int(n.capabilities.ht_mcs) << " ht_ss=" << int(n.capabilities.ht_ss)
           << "  ht_bw= " << int(n.capabilities.ht_bw) << std::endl
           << "   vht_mcs=" << int(n.capabilities.vht_mcs)
           << " vht_ss=" << int(n.capabilities.vht_ss) << "  vht_bw= " << int(n.capabilities.vht_bw)
           << std::endl
           << "   ant_num=" << int(n.capabilities.ant_num) << " wifi_standard="
           << (n.capabilities.wifi_standard == beerocks::STANDARD_AC
                   ? "ac"
                   : (n.capabilities.wifi_standard == beerocks::STANDARD_N ? "n" : "none ac/n"))
           << std::endl
           << " Hierarchy: " << int(n.hierarchy) << std::endl
           << " State: " << int(n.state) << std::endl
           << " Supports5ghz: " << bool(n.supports_5ghz) << std::endl
           << " Supports24ghz: " << bool(n.supports_24ghz) << std::endl
           << " Statistics:" << std::endl
           << "   LastUpdate: "
           << float((std::chrono::duration_cast<std::chrono::duration<double>>(
                         tCurrTime - n.stats_info->timestamp))
                        .count())
           << "[sec]" << std::endl
           << "   StatsDelta: " << float(n.stats_info->stats_delta_ms) / 1000.0 << "[sec]"
           << std::endl
           << "   RSSI (RX): " << int(n.stats_info->rx_rssi) << " [dBm] " << std::endl
           << "   Packets (RX|TX): " << int(n.stats_info->rx_packets) << " | "
           << int(n.stats_info->tx_packets) << std::endl
           << "   Bytes (RX|TX): " << int(n.stats_info->rx_bytes) << " | "
           << int(n.stats_info->tx_bytes) << std::endl
           << "   PhyRate (RX|TX): " << int(n.stats_info->rx_phy_rate_100kb / 10.0) << " | "
           << int(n.stats_info->tx_phy_rate_100kb / 10.0) << " [Mbps]" << std::endl
           << "   Load (RX|TX): " << int(n.stats_info->rx_load_percent) << " | "
           << int(n.stats_info->tx_load_percent) << " [%]" << std::endl
           << "   RX Load: [";

        for (int i = 0; i < 10; ++i) {
            if (i < n.stats_info->rx_load_percent / 10) {
                os << "#";
            } else {
                os << " ";
            }
        }

        os << "] | TX Load: [";

        for (int i = 0; i < 10; ++i) {
            if (i < n.stats_info->tx_load_percent / 10) {
                os << "#";
            } else {
                os << " ";
            }
        }

        os << "]" << std::endl
           << "   LastSeen: "
           << float((std::chrono::duration_cast<std::chrono::duration<double>>(tCurrTime -
                                                                               n.last_seen))
                        .count())
           << "[sec]" << std::endl
           << "   LastStateChange: "
           << float((std::chrono::duration_cast<std::chrono::duration<double>>(tCurrTime -
                                                                               n.last_state_change))
                        .count())
           << "[sec]" << std::endl;

        if (node_type == beerocks::TYPE_IRE_BACKHAUL) {
            os << "   Platform: " << utils::get_platform_string(n.platform) << std::endl
               << "   IfaceType: " << utils::get_iface_type_string(n.iface_type) << std::endl
               << "   IreHandoff: " << bool(n.ire_handoff) << std::endl;
        } else if (node_type == beerocks::TYPE_CLIENT) {
            os << "   Handoff: " << bool(n.handoff) << std::endl
               << "   Confined: " << bool(n.confined) << std::endl
               << "   Failed5ghzSteerAttemps: " << int(n.failed_5ghz_steer_attemps) << std::endl
               << "   Failed24ghzSteerAttemps: " << int(n.failed_24ghz_steer_attemps) << std::endl;
        }
    } else if (node_type == beerocks::TYPE_SLAVE) {
        os << " Type: HOSTAP" << std::endl
           << " IfaceType: " << utils::get_iface_type_string(n.hostap->iface_type) << std::endl
           << " State: " << n.state << std::endl
           << " Active: " << bool(n.hostap->active) << std::endl
           << " Is backhual manager: " << n.hostap->is_backhaul_manager << std::endl
           << " Manufacturer: " << n.manufacturer << std::endl
           << " Channel: " << int(n.channel) << std::endl
           << " ChannelBandwidth: " << int(n.bandwidth) << std::endl
           << " ChannelExtAboveSecondary: " << bool(n.channel_ext_above_secondary) << std::endl
           << " cac_completed: " << bool(n.hostap->cac_completed) << std::endl
           << " on_fail_safe_channel: " << bool(n.hostap->on_fail_safe_channel) << std::endl
           << " on_sub_band_channel: " << bool(n.hostap->on_sub_band_channel) << std::endl
           << " on_dfs_reentry: " << bool(n.hostap->on_dfs_reentry) << std::endl
           << " ap_activity_mode: "
           << ((uint8_t(n.hostap->ap_activity_mode)) ? "AP_ACTIVE_MODE" : "AP_IDLE_MODE")
           << std::endl
           << " Radio Identifier: " << n.radio_identifier << std::endl
           << " SupportedChannels: " << std::endl;
        for (auto val : n.hostap->supported_channels) {
            if (val.channel > 0) {
                os << " ch=" << int(val.channel) << " | dfs=" << int(val.is_dfs_channel)
                   << " | tx_pow=" << int(val.tx_pow) << " | noise=" << int(val.noise)
                   << " [dbm] | bss_overlap=" << int(val.bss_overlap) << std::endl;
            }
        }
        os << " AntGain: " << int(n.hostap->ant_gain) << std::endl
           << " ConductedPower: " << int(n.hostap->conducted_power) << std::endl
           << " AntNum: " << int(n.capabilities.ant_num) << std::endl
           << " Statistics:" << std::endl
           << "   LastUpdate: "
           << float((std::chrono::duration_cast<std::chrono::duration<double>>(
                         tCurrTime - n.hostap->stats_info->timestamp))
                        .count())
           << "[sec]" << std::endl
           << "   StatsDelta: " << float(n.hostap->stats_info->stats_delta_ms) / 1000.0 << "[sec]"
           << std::endl
           << "   ActiveStaCount: " << int(n.hostap->stats_info->active_sta_count) << std::endl
           << "   Packets (RX|TX): " << int(n.hostap->stats_info->rx_packets) << " | "
           << int(n.hostap->stats_info->tx_packets) << std::endl
           << "   Bytes (RX|TX): " << int(n.hostap->stats_info->rx_bytes) << " | "
           << int(n.hostap->stats_info->tx_bytes) << std::endl
           << "   ChannelLoad: " << int(n.hostap->stats_info->channel_load_percent) << " [%]"
           << std::endl
           << "   TotalStaLoad (RX|TX): " << int(n.hostap->stats_info->total_client_rx_load_percent)
           << " | " << int(n.hostap->stats_info->total_client_tx_load_percent) << " [%] "
           << std::endl
           << "**radar statistics**" << std::endl;
        for_each(begin(n.hostap->Radar_stats), end(n.hostap->Radar_stats),
                 [&](sWifiChannelRadarStats radar_stat) {
                     //for(auto radar_stat : n.hostap->Radar_stats) {
                     auto delta_radar =
                         std::chrono::duration_cast<std::chrono::seconds>(
                             radar_stat.csa_exit_timestamp - radar_stat.csa_enter_timestamp)
                             .count();
                     // if(delta// _radar)
                     os << "channel = " << int(radar_stat.channel)
                        << " bw = " << int(radar_stat.bandwidth)
                        << " time_in_channel = " << int(delta_radar) << std::endl;
                     //}
                 });
        os << "   RX Load: [";

        for (int i = 0; i < 10; ++i) {
            if (i < n.hostap->stats_info->total_client_rx_load_percent / 10) {
                os << "#";
            } else {
                os << "_";
            }
        }

        os << "] | TX Load: [";

        for (int i = 0; i < 10; ++i) {
            if (i < n.hostap->stats_info->total_client_tx_load_percent / 10) {
                os << "#";
            } else {
                os << "_";
            }
        }

        os << "]";
    }

    return (os);
}

std::ostream &operator<<(std::ostream &os, const node *n) { return (os << (const node &)(*n)); }

} // namespace son

bool node::get_beacon_measurement(std::string ap_mac_, int8_t &rcpi, uint8_t &rsni)
{
    auto it = beacon_measurements.find(ap_mac_);
    if (it == beacon_measurements.end()) {
        LOG(ERROR) << "ap_mac " << ap_mac_ << " does not exist!";
        rcpi = beerocks::RSSI_INVALID;
        rsni = 0;
        return false;
    }
    rcpi = it->second->rcpi;
    rsni = it->second->rsni;
    return true;
}

void node::set_beacon_measurement(std::string ap_mac_, int8_t rcpi, uint8_t rsni)
{
    auto it = beacon_measurements.find(ap_mac_);
    if (it == beacon_measurements.end()) {
        std::shared_ptr<beacon_measurement> m =
            std::make_shared<beacon_measurement>(ap_mac_, rcpi, rsni);
        beacon_measurements.insert(std::make_pair(ap_mac_, m));
    } else {
        it->second->rcpi      = rcpi;
        it->second->rsni      = rsni;
        it->second->timestamp = std::chrono::steady_clock::now();
    }
}

bool node::get_cross_rx_rssi(std::string ap_mac_, int8_t &rssi, int8_t &packets)
{
    auto it = cross_rx_rssi.find(ap_mac_);
    if (it == cross_rx_rssi.end()) {
        rssi    = beerocks::RSSI_INVALID;
        packets = -1;
        return false;
    }
    rssi    = it->second->rssi;
    packets = it->second->packets;
    return true;
}

void node::set_cross_rx_rssi(std::string ap_mac_, int8_t rssi, int8_t packets)
{
    auto it = cross_rx_rssi.find(ap_mac_);
    if (it == cross_rx_rssi.end()) {
        std::shared_ptr<rssi_measurement> m =
            std::make_shared<rssi_measurement>(ap_mac_, rssi, packets);
        cross_rx_rssi.insert(std::make_pair(ap_mac_, m));
    } else {
        it->second->rssi      = rssi;
        it->second->timestamp = std::chrono::steady_clock::now();
        it->second->packets   = packets;
    }
}

void node::clear_cross_rssi()
{
    cross_rx_rssi.clear();
    beacon_measurements.clear();
}

void node::clear_node_stats_info() { stats_info = std::make_shared<sta_stats_params>(); }

void node::clear_hostap_stats_info()
{
    hostap->stats_info = std::make_shared<radio::ap_stats_params>();
}

beerocks::eType node::get_type() { return type; }

bool node::set_type(beerocks::eType type_)
{
    //only allow TYPE_CLIENT to TYPE_IRE_BACKHAUL change
    if (type_ == type) {
        return true;
    } else if ((type == beerocks::TYPE_CLIENT) && (type_ == beerocks::TYPE_IRE_BACKHAUL)) {
        type = type_;
        return true;
    } else {
        LOG(ERROR) << "Not expected to happen: node = " << mac << ", old type = " << int(type)
                   << ", new type = " << int(type_);
    }
    return false;
}

bool node::link_metrics_data::add_transmitter_link_metric(
    std::shared_ptr<ieee1905_1::tlvTransmitterLinkMetric> tx_link_metric_data)
{
    //  interface_pair_info_length() returns the length in bytes (number of elements * sizeof(sInterfacePairInfo).
    size_t info_size = tx_link_metric_data->interface_pair_info_length() /
                       sizeof(ieee1905_1::tlvTransmitterLinkMetric::sInterfacePairInfo);

    for (size_t i = 0; i < info_size; i++) {
        auto info_tuple = tx_link_metric_data->interface_pair_info(i);

        if (!std::get<0>(info_tuple)) {
            LOG(ERROR) << "add_transmitter_link_metric getting operating class entry has failed!";
            return false;
        }
        auto &InterfacePairInfo = std::get<1>(info_tuple);
        transmitterLinkMetrics.push_back(InterfacePairInfo);

        LOG(DEBUG) << "adding tlvTransmitterLinkMetric data to list"
                   << " phy_rate = " << int(InterfacePairInfo.link_metric_info.phy_rate);
    }
    return true;
}

bool node::link_metrics_data::add_receiver_link_metric(
    std::shared_ptr<ieee1905_1::tlvReceiverLinkMetric> RxLinkMetricData)
{
    //  interface_pair_info_length() returns the length in bytes (number of elements * sizeof(sInterfacePairInfo).
    size_t info_size = RxLinkMetricData->interface_pair_info_length() /
                       sizeof(ieee1905_1::tlvReceiverLinkMetric::sInterfacePairInfo);

    for (size_t i = 0; i < info_size; i++) {
        auto info_tuple = RxLinkMetricData->interface_pair_info(i);

        if (!std::get<0>(info_tuple)) {
            LOG(ERROR) << "add_receiver_link_metric getting operating class entry has failed!";
            return false;
        }
        auto &InterfacePairInfo = std::get<1>(info_tuple);
        receiverLinkMetrics.push_back(InterfacePairInfo);

        LOG(DEBUG) << "adding tlvReceiverLinkMetric data to list"
                   << " rssi_db = " << int(InterfacePairInfo.link_metric_info.rssi_db);
    }
    return true;
}

bool node::ap_metrics_data::add_ap_metric_data(std::shared_ptr<wfa_map::tlvApMetric> ApMetricData)
{
    bssid                               = ApMetricData->bssid();
    channel_utilization                 = ApMetricData->channel_utilization();
    number_of_stas_currently_associated = ApMetricData->number_of_stas_currently_associated();

    if (ApMetricData->estimated_service_parameters().include_ac_be) {
        std::copy(ApMetricData->estimated_service_info_field_ac_be(),
                  ApMetricData->estimated_service_info_field_ac_be() + 3,
                  estimated_service_info_field_ac_be);
    }

    if (ApMetricData->estimated_service_parameters().include_ac_bk) {
        std::copy(ApMetricData->estimated_service_info_field_ac_bk(),
                  ApMetricData->estimated_service_info_field_ac_bk() + 3,
                  estimated_service_info_field_ac_bk);
    }

    if (ApMetricData->estimated_service_parameters().include_ac_vo) {
        std::copy(ApMetricData->estimated_service_info_field_ac_vo(),
                  ApMetricData->estimated_service_info_field_ac_vo() + 3,
                  estimated_service_info_field_ac_vo);
    }

    if (ApMetricData->estimated_service_parameters().include_ac_vi) {
        std::copy(ApMetricData->estimated_service_info_field_ac_vi(),
                  ApMetricData->estimated_service_info_field_ac_vi() + 3,
                  estimated_service_info_field_ac_vi);
    }
    return true;
}