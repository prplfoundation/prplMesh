/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "network_map.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <bml_defs.h>

#include <unordered_set>

using namespace beerocks;
using namespace net;
using namespace son;

void network_map::send_bml_network_map_message(db &database, Socket *sd,
                                               ieee1905_1::CmduMessageTx &cmdu_tx, uint16_t id)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_NW_MAP_RESPONSE>(cmdu_tx, id);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_NW_MAP_BRIDGE_RESPONSE message!";
        return;
    }

    auto beerocks_header = message_com::get_beerocks_header(cmdu_tx);

    if (!beerocks_header) {
        LOG(ERROR) << "Failed getting beerocks_header!";
        return;
    }

    beerocks_header->actionhdr()->last() = 0;

    uint8_t *data_start = nullptr;
    // std::ptrdiff_t size=0, size_left=0, node_len=0;
    const size_t gwIreNodeSize  = sizeof(BML_NODE);
    const size_t clientNodeSize = sizeof(BML_NODE) - sizeof(BML_NODE::N_DATA::N_GW_IRE);

    std::ptrdiff_t size = 0, size_left = 0, node_len = 0;
    uint32_t &num_of_nodes = response->node_num();
    num_of_nodes           = 0;

    database.rewind();
    bool last          = false;
    bool get_next_node = true;
    beerocks::eType n_type;
    std::shared_ptr<node> n;

    // because of virtual nodes (vap nodes) are poiting to the radio node,
    // we want to save the in a list in order to not count them multiple times as the same mac.
    std::unordered_set<std::string> ap_list;

    while (!last || !get_next_node) {
        if (get_next_node) {
            n    = nullptr;
            last = database.get_next_node(n);
        }
        // LOG(DEBUG) << "last = " << last << " get_next_node=" << get_next_node;

        if (n == nullptr) {
            // LOG(DEBUG) << "n == nullptr";
            continue;
        }

        // skip virtual vap nodes
        if (n->get_type() == beerocks::TYPE_SLAVE) {
            if (ap_list.find(n->mac) == ap_list.end()) {
                // radio/main vap
                ap_list.insert(n->mac);
            } else {
                // vap node
                continue;
            }
        }
        size_left = beerocks_header->getMessageBuffLength() - beerocks_header->getMessageLength();

        // LOG(DEBUG) << "num_of_nodes = " << num_of_nodes << ", size = " << int(size) << ", size_left = " << int(size_left);

        n_type = n->get_type();
        if (n->state == beerocks::STATE_CONNECTED &&
            (n_type == beerocks::TYPE_CLIENT || n_type == beerocks::TYPE_IRE ||
             n_type == beerocks::TYPE_GW)) {

            if (n_type == beerocks::TYPE_CLIENT) {
                node_len = clientNodeSize;
            } else {
                node_len = gwIreNodeSize;
            }

            if (node_len > size_left && num_of_nodes == 0) {
                LOG(ERROR) << "node size is bigger than buffer size";
                return;
            } else if (node_len > size_left) {
                message_com::send_cmdu(sd, cmdu_tx);

                get_next_node = false;
                response =
                    message_com::create_vs_message<beerocks_message::cACTION_BML_NW_MAP_RESPONSE>(
                        cmdu_tx, id);

                if (response == nullptr) {
                    LOG(ERROR) << "Failed building message!";
                    return;
                }

                beerocks_header                      = message_com::get_beerocks_header(cmdu_tx);
                beerocks_header->actionhdr()->last() = 0;
                num_of_nodes = response->node_num(); // prepare for next message
                num_of_nodes = 0;
                data_start   = nullptr;
                size         = 0;
            } else {
                if (!response->alloc_buffer(node_len)) {
                    LOG(ERROR) << "Failed allocating buffer!";
                    return;
                }

                if (data_start == nullptr) {
                    data_start = (uint8_t *)response->buffer(0);
                }

                fill_bml_node_data(database, n, data_start + size, size_left);

                num_of_nodes++;
                size += node_len;
                get_next_node = true;
            }
        }
    }

    beerocks_header->actionhdr()->last() = 1;
    message_com::send_cmdu(sd, cmdu_tx);
    //LOG(DEBUG) << "sending message, last=1";
}

std::ptrdiff_t network_map::fill_bml_node_data(db &database, std::string node_mac,
                                               uint8_t *tx_buffer, std::ptrdiff_t &buffer_size,
                                               bool force_client_disconnect)
{
    auto n = database.get_node(node_mac);
    if (n == nullptr) {
        LOG(ERROR) << "get_node(), node_mac=" << node_mac << " , n == nullptr !!!";
        return 0;
    } else {
        return fill_bml_node_data(database, n, tx_buffer, buffer_size, force_client_disconnect);
    }
}

std::ptrdiff_t network_map::fill_bml_node_data(db &database, std::shared_ptr<node> n,
                                               uint8_t *tx_buffer, std::ptrdiff_t &buffer_size,
                                               bool force_client_disconnect)
{
    auto node = (BML_NODE *)tx_buffer;

    if (n == nullptr) {
        LOG(ERROR) << " n == nullptr !!!";
        return 0;
    }

    uint8_t node_type;
    auto n_type = n->get_type();
    std::ptrdiff_t node_len;

    if (n_type == beerocks::TYPE_GW) {
        node_len  = sizeof(BML_NODE);
        node_type = BML_NODE_TYPE_GW;
    } else if (n_type == beerocks::TYPE_IRE) {
        node_len  = sizeof(BML_NODE);
        node_type = BML_NODE_TYPE_IRE;
    } else {
        node_len  = sizeof(BML_NODE) - sizeof(BML_NODE::N_DATA::N_GW_IRE);
        node_type = BML_NODE_TYPE_CLIENT;
    }

    if (node_len > buffer_size) {
        //LOG(DEBUG) << "buffer overflow!";
        return 0;
    }

    memset(node, 0, node_len);

    // Fill the common fields
    node->type = node_type;

    if (force_client_disconnect) {
        node->state = BML_NODE_STATE_DISCONNECTED;
    } else {
        switch (n->state) {
        case beerocks::STATE_DISCONNECTED:
            node->state = BML_NODE_STATE_DISCONNECTED;
            break;

        case beerocks::STATE_CONNECTING:
            node->state = BML_NODE_STATE_CONNECTING;
            break;

        case beerocks::STATE_CONNECTED:
            node->state = BML_NODE_STATE_CONNECTED;
            break;

        default:
            node->state = BML_NODE_STATE_UNKNOWN;
        }
    }

    if (n_type == beerocks::TYPE_IRE &&
        database.is_node_wireless(database.get_node_parent(n->mac))) {
        auto parent_backhaul_mac = database.get_node_parent_backhaul(n->mac);
        node->channel            = database.get_node_channel(parent_backhaul_mac);
        node->bw                 = database.get_node_bw(parent_backhaul_mac);
        node->channel_ext_above_secondary =
            database.get_node_channel_ext_above_secondary(parent_backhaul_mac);
    } else {
        node->channel                     = n->channel;
        node->bw                          = n->bandwidth;
        node->channel_ext_above_secondary = n->channel_ext_above_secondary;
    }

    network_utils::mac_from_string(node->mac, n->mac); // if IRE->bridge, else if STA->sta mac
    network_utils::mac_from_string(node->parent_bridge,
                                   database.get_node_parent_ire(n->mac)); // remote bridge
    if (n_type == beerocks::TYPE_CLIENT) {
        network_utils::mac_from_string(node->parent_bssid, n->parent_mac); // remote radio(ap)
        node->rx_rssi = database.get_load_rx_rssi(n->mac);
    } else {
        if (n->parent_mac != std::string()) {
            auto n_parent = database.get_node(n->parent_mac);
            if (n_parent) {
                network_utils::mac_from_string(node->parent_bssid,
                                               n_parent->parent_mac); // remote radio(ap)
            }
        }
    }

    network_utils::ipv4_from_string(node->ip_v4, n->ipv4);
    string_utils::copy_string(node->name, n->name.c_str(), sizeof(node->name));

    // GW/IRE specific parameters
    if (n_type != beerocks::TYPE_CLIENT) {
        network_utils::mac_from_string(
            node->data.gw_ire.backhaul_mac,
            database.get_node_parent_backhaul(n->mac)); // local parent backhaul
        auto children = database.get_node_children(n, beerocks::TYPE_SLAVE);
        int i         = 0;
        for (auto c : children) {
            if (c->state == beerocks::STATE_CONNECTED) {
                if (i >= int(beerocks::utils::array_length(node->data.gw_ire.radio))) {
                    LOG(ERROR) << "exceeded size of data.gw_ire.radio[]";
                    break;
                }

                network_utils::mac_from_string(node->data.gw_ire.radio[i].radio_mac,
                                               c->mac); // local parent backhaul

                // Copy the interface name
                string_utils::copy_string(node->data.gw_ire.radio[i].iface_name,
                                          database.get_hostap_iface_name(c->mac).c_str(),
                                          BML_NODE_IFACE_NAME_LEN);

                // Radio Vendor
                switch (database.get_hostap_iface_type(c->mac)) {
                case beerocks::eIfaceType::IFACE_TYPE_WIFI_INTEL:
                    node->data.gw_ire.radio[i].vendor = BML_WLAN_VENDOR_INTEL;
                    break;
                default:
                    node->data.gw_ire.radio[i].vendor = BML_WLAN_VENDOR_UNKNOWN;
                }

                // Copy the driver version string
                string_utils::copy_string(node->data.gw_ire.radio[i].driver_version,
                                          database.get_hostap_driver_version(c->mac).c_str(),
                                          BML_WLAN_DRIVER_VERSION_LEN);

                node->data.gw_ire.radio[i].channel       = !c->channel ? 255 : c->channel;
                node->data.gw_ire.radio[i].cac_completed = c->hostap->cac_completed;
                node->data.gw_ire.radio[i].bw            = c->bandwidth;
                node->data.gw_ire.radio[i].channel_ext_above_secondary =
                    c->channel_ext_above_secondary;
                node->data.gw_ire.radio[i].ap_active = c->hostap->active;

                // Copy the radio identifier string
                network_utils::mac_from_string(node->data.gw_ire.radio[i].radio_identifier,
                                               c->radio_identifier);

                for (int8_t vap_id = beerocks::IFACE_VAP_ID_MIN;
                     vap_id < int8_t(c->hostap->vaps_info.size()); vap_id++) {
                    const auto &vap = (c->hostap->vaps_info[vap_id]);
                    network_utils::mac_from_string(node->data.gw_ire.radio[i].vap[vap_id].bssid,
                                                   vap.mac);
                    string_utils::copy_string(node->data.gw_ire.radio[i].vap[vap_id].ssid,
                                              vap.ssid.c_str(),
                                              sizeof(node->data.gw_ire.radio[i].vap[0].ssid));
                    node->data.gw_ire.radio[i].vap[vap_id].backhaul_vap = vap.backhaul_vap;
                }
                ++i;
            }
        }
    }
    return node_len;
}

void network_map::send_bml_nodes_statistics_message_to_listeners(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::vector<Socket *> bml_listeners,
    std::set<std::string> valid_hostaps)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_STATS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    auto beerocks_header = message_com::get_beerocks_header(cmdu_tx);
    if (!beerocks_header) {
        LOG(ERROR) << "Failed getting beerocks_header!";
        return;
    }
    beerocks_header->actionhdr()->last() = 0;

    // nodes iterating
    //LOG(DEBUG) << "send_bml_nodes_statistics_message, buf_size " << int(tx_buffer_size);
    uint8_t *data_start = nullptr;
    std::ptrdiff_t size = 0, size_left = 0, node_len = 0;

    const auto reserved_size =
        message_com::get_vs_cmdu_size_on_buffer<beerocks_message::cACTION_BML_STATS_UPDATE>();

    // Save room in the output buffer for the number of transmitted nodes
    uint32_t &num_of_stats_bulks = response->num_of_stats_bulks();
    num_of_stats_bulks           = 0;

    //LOG(DEBUG) << "tx_buffer = " << int(tx_buffer) << ", data_start = " << int(data_start);

    int get_next_node = 1;
    beerocks::eType n_type;
    std::shared_ptr<node> n;

    // common function definition ///////////////////////////////////////////////
    auto fill_and_send = [&](std::ptrdiff_t &size, std::ptrdiff_t &size_left,
                             std::ptrdiff_t &node_len, const uint16_t reserved_size,
                             uint8_t *&data_start) -> int {
        size_left = cmdu_tx.getMessageBuffLength() - reserved_size - size;
        //LOG(DEBUG) << "num_of_stats_bulks = " << num_of_stats_bulks << ", size = " << int(size) << ", size_left = " << int(size_left);

        size_t node_size = get_bml_node_statistics_len(n);
        response->alloc_buffer(node_size);

        if (data_start == nullptr) {
            data_start = (uint8_t *)response->buffer(0);
        }

        if (((reserved_size + size + node_size) >
             cmdu_tx.getMessageBuffLength()) || // No more size on the buffer
            !(node_len = fill_bml_node_statistics(database, n, data_start + size, size_left))) {
            if (num_of_stats_bulks == 0) {
                LOG(ERROR) << "node size is bigger than buffer size";
                return -1;
            }

            //LOG(DEBUG) << "sending message, last=0";
            // sending to all listeners
            for (auto it = bml_listeners.begin(); it != bml_listeners.end(); ++it) {
                message_com::send_cmdu(*it, cmdu_tx);
            }

            // prepare for next message
            response =
                message_com::create_vs_message<beerocks_message::cACTION_BML_STATS_UPDATE>(cmdu_tx);

            if (response == nullptr) {
                LOG(ERROR) << "Failed building message!";
                return -1;
            }

            beerocks_header                      = message_com::get_beerocks_header(cmdu_tx);
            beerocks_header->actionhdr()->last() = 0;
            num_of_stats_bulks                   = response->num_of_stats_bulks();
            num_of_stats_bulks                   = 0;
            get_next_node                        = 0;
            size                                 = 0;
        } else {
            //LOG(DEBUG) << "node_len = " << int(node_len);
            num_of_stats_bulks++;
            size += node_len;
            get_next_node = 1;
        }
        return get_next_node;
    };
    /////////////////////////////////////////////////////////////////////////

    // nodes iterating
    for (auto radio = valid_hostaps.begin(); radio != valid_hostaps.end();) {
        std::string hostap_mac = *radio;
        // LOG(DEBUG) << "radio mac = " << hostap_mac;
        n = database.get_node(hostap_mac);
        if (!n) {
            LOG(ERROR) << "n == nullptr";
            continue;
        }
        n_type = n->get_type();
        if ((n != nullptr && n->state == beerocks::STATE_CONNECTED) &&
            (n_type == beerocks::TYPE_SLAVE)) {

            get_next_node = fill_and_send(size, size_left, node_len, reserved_size, data_start);
            if (get_next_node == 0) {
                continue;
            } else if (get_next_node < 0) {
                return;
            }

            // sta's
            auto sta_clients = database.get_node_children(hostap_mac);
            for (auto sta = sta_clients.begin(); sta != sta_clients.end();) {
                std::string sta_mac = *sta;
                // LOG(DEBUG) << "sta mac = " << sta_mac;
                n = database.get_node(sta_mac);
                if (n == nullptr) {
                    LOG(ERROR) << "n == nullptr";
                    continue;
                }
                if (n != nullptr && n->state == beerocks::STATE_CONNECTED) {
                    get_next_node =
                        fill_and_send(size, size_left, node_len, reserved_size, data_start);
                    if (get_next_node == 0) {
                        continue;
                    } else if (get_next_node < 0) {
                        return;
                    }
                }

                if (get_next_node) {
                    ++sta;
                }
            }
        }

        if (get_next_node) {
            ++radio;
        }
    }

    beerocks_header->actionhdr()->last() = 1;

    //LOG(DEBUG) << "sending message, last=0";
    // sending to all listeners
    for (auto it = bml_listeners.begin(); it != bml_listeners.end(); ++it) {
        message_com::send_cmdu(*it, cmdu_tx);
    }
    //LOG(DEBUG) << "sending message, last=1";
}

void network_map::send_bml_event_to_listeners(ieee1905_1::CmduMessageTx &cmdu_tx,
                                              std::vector<Socket *> &bml_listeners)
{
    for (auto it = bml_listeners.begin(); it != bml_listeners.end(); ++it) {
        message_com::send_cmdu(*it, cmdu_tx);
    }
}

void network_map::send_bml_bss_tm_req_message_to_listeners(db &database,
                                                           ieee1905_1::CmduMessageTx &cmdu_tx,
                                                           std::vector<Socket *> bml_listeners,
                                                           std::string target_bssid,
                                                           uint8_t disassoc_imminent)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_BSS_TM_REQ))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_BSS_TM_REQ;
    auto size   = sizeof(BML_EVENT);
    event->data = GET_MESSAGE_POINTER(BML_EVENT_BSS_TM_REQ, response->buffer(0), size);

    auto event_bss_tm_req = (BML_EVENT_BSS_TM_REQ *)event->data;
    network_utils::mac_from_string(event_bss_tm_req->target_bssid, target_bssid);
    event_bss_tm_req->disassoc_imminent = disassoc_imminent;

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

void network_map::send_bml_bh_roam_req_message_to_listeners(db &database,
                                                            ieee1905_1::CmduMessageTx &cmdu_tx,
                                                            std::vector<Socket *> bml_listeners,
                                                            std::string bssid, uint8_t channel)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_BH_ROAM_REQ))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_BH_ROAM_REQ;
    auto size   = sizeof(BML_EVENT);
    event->data = GET_MESSAGE_POINTER(BML_EVENT_BH_ROAM_REQ, response->buffer(0), size);

    auto event_bh_roam_req = (BML_EVENT_BH_ROAM_REQ *)event->data;
    network_utils::mac_from_string(event_bh_roam_req->bssid, bssid);
    event_bh_roam_req->channel = channel;

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

void network_map::send_bml_client_allow_req_message_to_listeners(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::vector<Socket *> bml_listeners,
    std::string sta_mac, std::string hostap_mac, std::string ip)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_CLIENT_ALLOW_REQ))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_CLIENT_ALLOW_REQ;

    auto size   = sizeof(BML_EVENT);
    event->data = GET_MESSAGE_POINTER(BML_EVENT_CLIENT_ALLOW_REQ, response->buffer(0), size);
    auto event_client_allow_req = (BML_EVENT_CLIENT_ALLOW_REQ *)event->data;
    network_utils::mac_from_string(event_client_allow_req->sta_mac, sta_mac);
    network_utils::mac_from_string(event_client_allow_req->hostap_mac, hostap_mac);
    network_utils::ipv4_from_string(event_client_allow_req->ip, ip);

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

void network_map::send_bml_client_disallow_req_message_to_listeners(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::vector<Socket *> bml_listeners,
    std::string sta_mac, std::string hostap_mac)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_CLIENT_DISALLOW_REQ))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_CLIENT_DISALLOW_REQ;
    auto size   = sizeof(BML_EVENT);
    event->data = GET_MESSAGE_POINTER(BML_EVENT_CLIENT_DISALLOW_REQ, response->buffer(0), size);

    auto event_client_disallow_req = (BML_EVENT_CLIENT_DISALLOW_REQ *)event->data;
    network_utils::mac_from_string(event_client_disallow_req->sta_mac, sta_mac);
    network_utils::mac_from_string(event_client_disallow_req->hostap_mac, hostap_mac);

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

void network_map::send_bml_acs_start_message_to_listeners(db &database,
                                                          ieee1905_1::CmduMessageTx &cmdu_tx,
                                                          std::vector<Socket *> bml_listeners,
                                                          std::string hostap_mac)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_ACS_START))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_ACS_START;
    auto size   = sizeof(BML_EVENT);
    event->data = GET_MESSAGE_POINTER(BML_EVENT_ACS_START, response->buffer(0), size);

    auto event_acs_start = (BML_EVENT_ACS_START *)event->data;

    network_utils::mac_from_string(event_acs_start->hostap_mac, hostap_mac);

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

void network_map::send_bml_csa_notification_message_to_listeners(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::vector<Socket *> bml_listeners,
    std::string hostap_mac, uint8_t bandwidth, uint8_t channel, uint8_t channel_ext_above_primary,
    uint16_t vht_center_frequency)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) + sizeof(BML_EVENT_CSA_NOTIFICATION))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_CSA_NOTIFICATION;
    auto size   = sizeof(BML_EVENT);
    event->data = GET_MESSAGE_POINTER(BML_EVENT_CSA_NOTIFICATION, response->buffer(0), size);

    auto event_csa_notification = (BML_EVENT_CSA_NOTIFICATION *)event->data;
    network_utils::mac_from_string(event_csa_notification->hostap_mac, hostap_mac);
    event_csa_notification->bandwidth                 = bandwidth;
    event_csa_notification->channel                   = channel;
    event_csa_notification->channel_ext_above_primary = channel_ext_above_primary;
    event_csa_notification->vht_center_frequency      = vht_center_frequency;

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

void network_map::send_bml_cac_status_changed_notification_message_to_listeners(
    db &database, ieee1905_1::CmduMessageTx &cmdu_tx, std::vector<Socket *> bml_listeners,
    std::string hostap_mac, uint8_t cac_completed)
{
    auto response =
        message_com::create_vs_message<beerocks_message::cACTION_BML_EVENTS_UPDATE>(cmdu_tx);
    if (response == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_STATS_UPDATE message!";
        return;
    }

    if (!response->alloc_buffer(sizeof(BML_EVENT) +
                                sizeof(BML_EVENT_CAC_STATUS_CHANGED_NOTIFICATION))) {
        LOG(ERROR) << "Failed to alloc buffer";
        return;
    }

    auto event = reinterpret_cast<BML_EVENT *>(response->buffer(0));
    if (event == nullptr) {
        LOG(ERROR) << "event is nullptr";
        return;
    }

    event->type = BML_EVENT_TYPE_CAC_STATUS_CHANGED_NOTIFICATION;
    auto size   = sizeof(BML_EVENT);
    event->data =
        GET_MESSAGE_POINTER(BML_EVENT_CAC_STATUS_CHANGED_NOTIFICATION, response->buffer(0), size);

    auto event_cac_status_changed = (BML_EVENT_CAC_STATUS_CHANGED_NOTIFICATION *)event->data;
    network_utils::mac_from_string(event_cac_status_changed->hostap_mac, hostap_mac);
    event_cac_status_changed->cac_completed = cac_completed;

    send_bml_event_to_listeners(cmdu_tx, bml_listeners);
}

std::ptrdiff_t network_map::get_bml_node_statistics_len(std::shared_ptr<node> n)
{
    auto n_type                   = n->get_type();
    std::ptrdiff_t stats_bulk_len = 0;

    switch (n_type) {
    case beerocks::TYPE_SLAVE: {
        stats_bulk_len =
            sizeof(BML_STATS) - sizeof(BML_STATS::S_TYPE) + sizeof(BML_STATS::S_TYPE::S_RADIO);
    } break;
    case beerocks::TYPE_CLIENT: {
        stats_bulk_len =
            sizeof(BML_STATS) - sizeof(BML_STATS::S_TYPE) + sizeof(BML_STATS::S_TYPE::S_CLIENT);
    } break;

    default:
        break;
    }

    return stats_bulk_len;
}

std::ptrdiff_t network_map::fill_bml_node_statistics(db &database, std::shared_ptr<node> n,
                                                     uint8_t *tx_buffer, std::ptrdiff_t buf_size)
{
    if (n == nullptr) {
        LOG(ERROR) << "n == nullptr";
        return 0;
    }
    auto n_type                   = n->get_type();
    std::ptrdiff_t stats_bulk_len = 0;

    stats_bulk_len = get_bml_node_statistics_len(n);
    if (stats_bulk_len > buf_size) {
        // LOG(DEBUG) << "buffer overflow!";
        return 0;
    }

    switch (n_type) {
    case beerocks::TYPE_SLAVE: {
        //LOG(DEBUG) << "fill TYPE_SLAVE";
        //prepearing buffer and calc size
        auto radio_stats_bulk = (BML_STATS *)tx_buffer;

        //fill radio stats
        //memset(radio_stats_bulk, 0, stats_bulk_len);
        network_utils::mac_from_string(radio_stats_bulk->mac, n->mac);
        radio_stats_bulk->type = BML_STAT_TYPE_RADIO;

        radio_stats_bulk->bytes_sent              = n->hostap->stats_info->tx_bytes;
        radio_stats_bulk->bytes_received          = n->hostap->stats_info->rx_bytes;
        radio_stats_bulk->packets_sent            = n->hostap->stats_info->tx_packets;
        radio_stats_bulk->packets_received        = n->hostap->stats_info->rx_packets;
        radio_stats_bulk->measurement_window_msec = n->hostap->stats_info->stats_delta_ms;

        radio_stats_bulk->errors_sent       = n->hostap->stats_info->errors_sent;
        radio_stats_bulk->errors_received   = n->hostap->stats_info->errors_received;
        radio_stats_bulk->retrans_count     = n->hostap->stats_info->retrans_count;
        radio_stats_bulk->uType.radio.noise = n->hostap->stats_info->noise;

        radio_stats_bulk->uType.radio.bss_load = n->hostap->stats_info->channel_load_percent;
        break;
    }
    case beerocks::TYPE_IRE_BACKHAUL: {
        if (database.is_node_wireless(n->mac)) {
            n_type = beerocks::TYPE_CLIENT;
        } else {
            break;
        }
    }
        FALLTHROUGH;
    case beerocks::TYPE_CLIENT: {
        //LOG(DEBUG) << "fill TYPE_CLIENT";

        // filter client which have not been measured yet
        if (n->stats_info->rx_rssi == beerocks::RSSI_INVALID) {
            //LOG(DEBUG) << "sta_mac=" << n->mac << ", signal_strength=INVALID!";
            return buf_size;
        }

        //prepearing buffer and calc size
        auto sta_stats_bulk = (BML_STATS *)tx_buffer;

        //fill sta stats
        //memset(sta_stats_bulk, 0, stats_bulk_len);
        network_utils::mac_from_string(sta_stats_bulk->mac, n->mac);
        sta_stats_bulk->type = BML_STAT_TYPE_CLIENT;

        sta_stats_bulk->bytes_sent              = n->stats_info->tx_bytes;
        sta_stats_bulk->bytes_received          = n->stats_info->rx_bytes;
        sta_stats_bulk->packets_sent            = n->stats_info->tx_packets;
        sta_stats_bulk->packets_received        = n->stats_info->rx_packets;
        sta_stats_bulk->measurement_window_msec = n->stats_info->stats_delta_ms;
        sta_stats_bulk->retrans_count           = n->stats_info->retrans_count;

        // These COMMON params are not available for station from bwl
        sta_stats_bulk->errors_sent     = 0;
        sta_stats_bulk->errors_received = 0;

        // LOG(DEBUG) << "sta_mac=" << n->mac << ", signal_strength=" << int(n->stats_info->rx_rssi);
        sta_stats_bulk->uType.client.signal_strength = n->stats_info->rx_rssi;
        sta_stats_bulk->uType.client.last_data_downlink_rate =
            n->stats_info->tx_phy_rate_100kb * 100000;
        sta_stats_bulk->uType.client.last_data_uplink_rate =
            n->stats_info->rx_phy_rate_100kb * 100000;

        //These CLIENT SPECIFIC params are missing in DB:
        sta_stats_bulk->uType.client.retransmissions = 0;

        break;
    }
    default:
        break;
    }

    return stats_bulk_len;
}
