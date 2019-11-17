/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_utils.h"

#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <iomanip> // std::setprecision
#include <sstream>

using namespace beerocks::net;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

static std::string node_type_to_string(uint8_t type)
{
    std::string ret;

    switch (type) {
    case BML_NODE_TYPE_GW:
        ret = "GW";
        break;
    case BML_NODE_TYPE_IRE:
        ret = "IRE";
        break;
    case BML_NODE_TYPE_CLIENT:
        ret = "Client";
        break;

    default:
        ret = "N/A";
    }

    return ret;
}

static std::string node_platform_to_string(uint8_t platform)
{
    std::string ret;

    switch (platform) {
    case BML_PLATFORM_GRX_350:
        ret = "GRX_350";
        break;
    case BML_PLATFORM_IRE_220:
        ret = "IRE_220";
        break;
    default:
        ret = "Unknown";
    }

    return ret;
}

static std::string node_state_to_string(uint8_t state)
{
    std::string ret;

    switch (state) {
    case BML_NODE_STATE_DISCONNECTED:
        ret = "Disconnected";
        break;

    case BML_NODE_STATE_CONNECTING:
        ret = "Connecting";
        break;

    case BML_NODE_STATE_CONNECTED:
        ret = "Connected";
        break;

    default:
        ret = "Unknown";
    }

    return ret;
}

static std::string radio_vendor_to_string(uint8_t vendor)
{
    switch (vendor) {
    case BML_WLAN_VENDOR_INTEL:
        return ("Intel");
    case BML_WLAN_VENDOR_BROADCOM:
        return ("Broadcom");
    default:
        return ("Unknown");
    }
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

int bml_utils_node_to_string(const struct BML_NODE *node, char *buffer, int buffer_len)
{
    std::stringstream ss;

    ss << "Name: " << ((node->name[0]) ? node->name : "N/A");

    ss << ", Type: " << node_type_to_string(node->type) << " (" << std::to_string(node->type) << ")"
       << ", Platform: " << node_platform_to_string(node->platform) << " ("
       << std::to_string(node->platform) << ")"
       << ", State: " << node_state_to_string(node->state) << " (" << std::to_string(node->state)
       << ")"
       << ", MAC: " << network_utils::mac_to_string(node->mac)
       << ", IP: " << network_utils::ipv4_to_string(node->ip_v4);

    if (node->type != BML_NODE_TYPE_CLIENT) {
        ss << ", Backhaul: " << network_utils::mac_to_string(node->data.gw_ire.backhaul_mac);
    }

    if (node->channel) {
        ss << ", Channel: " << std::to_string(node->channel);
    }

    if (node->parent_bridge[0]) {
        ss << ", Parent Bridge: " << network_utils::mac_to_string(node->parent_bridge)
           << ", Parent BSSID: " << network_utils::mac_to_string(node->parent_bssid);
    }

    // New line
    ss << std::endl;

    if (node->type != BML_NODE_TYPE_CLIENT) {
        int radio_length = sizeof(node->data.gw_ire.radio) / sizeof(node->data.gw_ire.radio[0]);
        for (int i = 0; i < radio_length; i++) {
            if (node->data.gw_ire.radio[i].channel != 0) {
                ss << "  Radio[" << std::to_string(i) << "]:"
                   << " Interface: " << node->data.gw_ire.radio[i].iface_name
                   << ", Vendor: " << radio_vendor_to_string(node->data.gw_ire.radio[i].vendor)
                   << ", Channel: "
                   << (node->data.gw_ire.radio[i].channel != 255
                           ? std::to_string(node->data.gw_ire.radio[i].channel)
                           : std::string("N/A"))
                   << ", Bandwidth: "
                   << beerocks::utils::convert_bandwidth_to_int(
                          beerocks::eWiFiBandwidth(node->data.gw_ire.radio[i].bw))
                   << ", CAC Completed: "
                   << std::to_string(node->data.gw_ire.radio[i].cac_completed)
                   << ", AP Active: " << (node->data.gw_ire.radio[i].ap_active ? "true" : "false")
                   << ", Driver Version: " << node->data.gw_ire.radio[i].driver_version
                   << std::endl;

                int vap_length = sizeof(node->data.gw_ire.radio[i].vap) /
                                 sizeof(node->data.gw_ire.radio[i].vap[0]);
                for (int j = 0; j < vap_length; j++) {
                    std::string bssid =
                        network_utils::mac_to_string(node->data.gw_ire.radio[i].vap[j].bssid);
                    if (bssid != network_utils::ZERO_MAC_STRING) {
                        ss << "    VAP[" << std::to_string(j) << "]: "
                           << "BSSID: " << bssid << ", SSID: "
                           << ((node->data.gw_ire.radio[i].vap[j].ssid[0])
                                   ? node->data.gw_ire.radio[i].vap[j].ssid
                                   : std::string("N/A"));

                        // New line
                        ss << std::endl;
                    }
                }
            }
        }
    }

    ss.seekp(0, std::ios::end);
    std::stringstream::pos_type offset = ss.tellp();
    if (offset > buffer_len) {
        std::cout << "ERROR: bml_node length > given buffer length" << std::endl;
        return 0;
    } else {
        beerocks::string_utils::copy_string(buffer, ss.str().c_str(), buffer_len);
        return offset;
    }
}

int bml_utils_stats_to_string(const struct BML_STATS *stats, char *buffer, int buffer_len)
{
    std::stringstream ss;

    ss << "Type: " << node_type_to_string(stats->type) << " (" << int(stats->type)
       << ")"
          ", MAC: "
       << network_utils::mac_to_string(stats->mac)
       << ", measurement_window_msec: " << std::to_string(stats->measurement_window_msec);

    if (stats->type == BML_STAT_TYPE_RADIO) {
        ss << ", bss_load: " << std::to_string(stats->uType.radio.bss_load)
           << ", noise: " << std::to_string(stats->uType.radio.noise);
    } else if (stats->type == BML_STAT_TYPE_CLIENT) {
        ss << ", signal_strength: " << std::to_string(stats->uType.client.signal_strength)
           << " dBm";
    }

    ss << std::endl
       << "  TX - "
       << "bytes_sent: " << std::to_string(stats->bytes_sent)
       << ", packets_sent: " << std::to_string(stats->packets_sent)
       << ", errors_sent: " << std::to_string(stats->errors_sent)
       << ", retrans_count: " << std::to_string(stats->retrans_count);

    if (stats->type == BML_STAT_TYPE_VAP) {
        ss << std::endl
           << "       unicast_packets_sent: "
           << std::to_string(stats->uType.vap.unicast_packets_sent) << ", multicast_packets_sent: "
           << std::to_string(stats->uType.vap.multicast_packets_sent)
           << ", broadcast_packets_sent: "
           << std::to_string(stats->uType.vap.broadcast_packets_sent);
    } else if (stats->type == BML_STAT_TYPE_CLIENT) {
        ss << std::endl
           << "       last_data_uplink_rate: " << std::fixed << std::setprecision(1)
           << (double(stats->uType.client.last_data_uplink_rate) / 1000000)
           << " MBit/s"
              ", retransmissions: "
           << std::to_string(stats->uType.client.retransmissions);
    }

    ss << std::endl
       << "  RX - "
       << "bytes_received: " << std::to_string(stats->bytes_received)
       << ", packets_received: " << std::to_string(stats->packets_received)
       << ", errors_received: " << std::to_string(stats->errors_received);

    if (stats->type == BML_STAT_TYPE_VAP) {
        ss << std::endl
           << "       unicast_packets_received: "
           << std::to_string(stats->uType.vap.unicast_packets_received)
           << ", multicast_packets_received: "
           << std::to_string(stats->uType.vap.multicast_packets_received)
           << ", broadcast_packets_received: "
           << std::to_string(stats->uType.vap.broadcast_packets_received);
    } else if (stats->type == BML_STAT_TYPE_CLIENT) {
        ss << std::endl
           << "       last_data_downlink_rate: " << std::fixed << std::setprecision(1)
           << (double(stats->uType.client.last_data_downlink_rate) / 1000000) << " MBit/s";
    }

    // switch (stats->type) {
    //     case BML_STAT_TYPE_RADIO: {
    //         ss <<   ", bss_load: " << std::to_string(stats->uType.radio.bss_load) <<
    //                 ", noise: " << std::to_string(stats->uType.radio.noise);

    //     } break;

    //     case BML_STAT_TYPE_VAP: {
    //         ss <<   ", unicast_packets_sent: " << std::to_string(stats->uType.vap.unicast_packets_sent)  << ", " <<
    //                 "unicast_packets_received: " << std::to_string(stats->uType.vap.unicast_packets_received)  << ", " <<
    //                 "multicast_packets_sent: " << std::to_string(stats->uType.vap.multicast_packets_sent)  << ", " <<
    //                 "multicast_packets_received: " << std::to_string(stats->uType.vap.multicast_packets_received)  <<
    //                 "broadcast_packets_sent: " << std::to_string(stats->uType.vap.broadcast_packets_sent)  << ", " <<
    //                 "broadcast_packets_received: " << std::to_string(stats->uType.vap.broadcast_packets_received)  << ", " << std::endl;

    //     } break;

    //     case BML_STAT_TYPE_CLIENT: {
    //         ss <<   ", signal_strength: " << std::to_string(stats->uType.client.signal_strength)  << ", " <<
    //                 "last_data_downlink_rate: " << std::to_string(stats->uType.client.last_data_downlink_rate)  << ", " <<
    //                 "last_data_uplink_rate: " << std::to_string(stats->uType.client.last_data_uplink_rate)  << ", " <<
    //                 "retransmissions: " << std::to_string(stats->uType.client.retransmissions)  << std::endl;

    //     } break;

    //     default: {}
    // }

    // Add new line
    ss << std::endl;

    ss.seekp(0, std::ios::end);
    std::stringstream::pos_type offset = ss.tellp();
    if (offset > buffer_len) {
        std::cout << "ERROR: bml_stats length > given buffer length" << std::endl;
        return 0;
    } else {
        beerocks::string_utils::copy_string(buffer, ss.str().c_str(), buffer_len);
        return offset;
    }
}

int bml_utils_stats_to_string_raw(const struct BML_STATS *stats, char *buffer, int buffer_len)
{
    std::stringstream ss;

    ss << "Type: " << int(stats->type) << ", mac: " << network_utils::mac_to_string(stats->mac)
       << ", measurement_window_msec: " << std::to_string(stats->measurement_window_msec);

    if (stats->type == BML_STAT_TYPE_RADIO) {
        ss << ", bss_load: " << std::to_string(stats->uType.radio.bss_load)
           << ", noise: " << std::to_string(stats->uType.radio.noise);
    } else if (stats->type == BML_STAT_TYPE_CLIENT) {
        ss << ", signal_strength: " << std::to_string(stats->uType.client.signal_strength);
    }

    ss << ", bytes_sent: " << std::to_string(stats->bytes_sent)
       << ", packets_sent: " << std::to_string(stats->packets_sent)
       << ", errors_sent: " << std::to_string(stats->errors_sent)
       << ", retrans_count: " << std::to_string(stats->retrans_count);

    if (stats->type == BML_STAT_TYPE_VAP) {
        ss << ", unicast_packets_sent: " << std::to_string(stats->uType.vap.unicast_packets_sent)
           << ", multicast_packets_sent: "
           << std::to_string(stats->uType.vap.multicast_packets_sent)
           << ", broadcast_packets_sent: "
           << std::to_string(stats->uType.vap.broadcast_packets_sent);
    } else if (stats->type == BML_STAT_TYPE_CLIENT) {
        ss << ", last_data_uplink_rate: "
           << std::to_string(stats->uType.client.last_data_uplink_rate)
           << ", retransmissions: " << std::to_string(stats->uType.client.retransmissions);
    }

    ss << ", bytes_received: " << std::to_string(stats->bytes_received)
       << ", packets_received: " << std::to_string(stats->packets_received)
       << ", errors_received: " << std::to_string(stats->errors_received);

    if (stats->type == BML_STAT_TYPE_VAP) {
        ss << ", unicast_packets_received: "
           << std::to_string(stats->uType.vap.unicast_packets_received)
           << ", multicast_packets_received: "
           << std::to_string(stats->uType.vap.multicast_packets_received)
           << ", broadcast_packets_received: "
           << std::to_string(stats->uType.vap.broadcast_packets_received);
    } else if (stats->type == BML_STAT_TYPE_CLIENT) {
        ss << ", last_data_downlink_rate: "
           << std::to_string(stats->uType.client.last_data_downlink_rate);
    }

    // Add new line
    ss << std::endl;

    ss.seekp(0, std::ios::end);
    std::stringstream::pos_type offset = ss.tellp();
    if (offset > buffer_len) {
        std::cout << "ERROR: bml_stats length > given buffer length" << std::endl;
        return 0;
    } else {
        beerocks::string_utils::copy_string(buffer, ss.str().c_str(), buffer_len);
        return offset;
    }
}

int bml_utils_event_to_string(const struct BML_EVENT *event, char *buffer, int buffer_len)
{
    std::stringstream ss;

    switch (event->type) {
    case BML_EVENT_TYPE_BSS_TM_REQ: {
        ss << "BML_EVENT_TYPE_BSS_TM_REQ";
        auto event_data = (BML_EVENT_BSS_TM_REQ *)(event->data);
        ss << ", target bssid: " << network_utils::mac_to_string(event_data->target_bssid) << ", "
           << "disassoc imminent: " << std::to_string(event_data->disassoc_imminent) << std::endl;
        break;
    }
    case BML_EVENT_TYPE_BH_ROAM_REQ: {
        ss << "BML_EVENT_TYPE_BH_ROAM_REQ";
        auto event_data = (BML_EVENT_BH_ROAM_REQ *)(event->data);
        ss << ", bssid: " << network_utils::mac_to_string(event_data->bssid) << ", "
           << "channel: " << std::to_string(event_data->channel) << std::endl;
        break;
    }
    case BML_EVENT_TYPE_CLIENT_ALLOW_REQ: {
        ss << "BML_EVENT_TYPE_CLIENT_ALLOW_REQ";
        auto event_data = (BML_EVENT_CLIENT_ALLOW_REQ *)(event->data);
        ss << ", hostap_mac: " << network_utils::mac_to_string(event_data->hostap_mac) << ", "
           << "sta_mac: " << network_utils::mac_to_string(event_data->sta_mac) << ", "
           << "ip: " << network_utils::ipv4_to_string(event_data->ip) << std::endl;
        break;
    }
    case BML_EVENT_TYPE_CLIENT_DISALLOW_REQ: {
        ss << "BML_EVENT_TYPE_CLIENT_DISALLOW_REQ";
        auto event_data = (BML_EVENT_CLIENT_ALLOW_REQ *)(event->data);
        ss << ", hostap_mac: " << network_utils::mac_to_string(event_data->hostap_mac)
           << ", sta_mac: " << network_utils::mac_to_string(event_data->sta_mac) << std::endl;
        break;
    }
    case BML_EVENT_TYPE_ACS_START: {
        ss << "BML_EVENT_TYPE_ACS_START";
        auto event_data = (BML_EVENT_ACS_START *)(event->data);
        ss << ", hostap_mac: " << network_utils::mac_to_string(event_data->hostap_mac) << std::endl;
        break;
    }
    case BML_EVENT_TYPE_CSA_NOTIFICATION: {
        ss << "BML_EVENT_TYPE_CSA_NOTIFICATION";
        auto event_data = (BML_EVENT_CSA_NOTIFICATION *)(event->data);
        ss << ", hostap_mac: " << network_utils::mac_to_string(event_data->hostap_mac)
           << ", bandwidth: " << std::to_string(event_data->bandwidth)
           << ", channel: " << std::to_string(event_data->channel)
           << ", channel_ext_above_primary: "
           << std::to_string(event_data->channel_ext_above_primary)
           << ", vht_center_frequency: " << std::to_string(event_data->vht_center_frequency)
           << std::endl;
        break;
    }
    case BML_EVENT_TYPE_CAC_STATUS_CHANGED_NOTIFICATION: {
        ss << "BML_EVENT_TYPE_CAC_STATUS_CHANGED_NOTIFICATION";
        auto event_data = (BML_EVENT_CAC_STATUS_CHANGED_NOTIFICATION *)(event->data);
        ss << ", hostap_mac: " << network_utils::mac_to_string(event_data->hostap_mac)
           << ", cac_completed: " << std::to_string(event_data->cac_completed) << std::endl;
        break;
    }
#ifdef BEEROCKS_RDKB
    case BML_EVENT_TYPE_STEERING: {
        auto event_data = (BML_EVENT_STEERING *)(event->data);
        switch (event_data->type) {
        case BML_STEERING_EVENT_PROBE_REQ: {
            ss << "BML_STEERING_EVENT_PROBE_REQ";
            break;
        }
        case BML_STEERING_EVENT_CLIENT_CONNECT: {
            ss << "BML_STEERING_EVENT_CLIENT_CONNECT";
            break;
        }
        case BML_STEERING_EVENT_CLIENT_DISCONNECT: {
            ss << "BML_STEERING_EVENT_CLIENT_DISCONNECT";
            break;
        }
        case BML_STEERING_EVENT_CLIENT_ACTIVITY: {
            ss << "BML_STEERING_EVENT_CLIENT_ACTIVITY";
            break;
        }
        case BML_STEERING_EVENT_SNR_XING: {
            ss << "BML_STEERING_EVENT_SNR_XING";
            break;
        }
        case BML_STEERING_EVENT_SNR: {
            ss << "BML_STEERING_EVENT_SNR";
            break;
        }
        case BML_STEERING_EVENT_AUTH_FAIL: {
            ss << "BML_STEERING_EVENT_AUTH_FAIL";
            break;
        }
        default: {
            break;
        }
        }
        break;
    }
#endif //BEEROCKS_RDKB
    default: {
        break;
    }
    }

    ss << std::endl;

    ss.seekp(0, std::ios::end);
    std::stringstream::pos_type offset = ss.tellp();
    if (offset > buffer_len) {
        std::cout << "ERROR: bml_stats length > given buffer length" << std::endl;
        return 0;
    } else {
        beerocks::string_utils::copy_string(buffer, ss.str().c_str(), buffer_len);
        return offset;
    }
}
