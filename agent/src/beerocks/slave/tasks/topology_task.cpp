/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "topology_task.h"
#include "../agent_db.h"
#include "../backhaul_manager/backhaul_manager_thread.h"
#include "../helpers/media_type.h"

#include <bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message_backhaul.h>

#include <tlvf/ieee_1905_1/s802_11SpecificInformation.h>
#include <tlvf/ieee_1905_1/tlv1905NeighborDevice.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddress.h>
#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/ieee_1905_1/tlvMacAddress.h>

#include <tlvf/wfa_map/tlvApOperationalBSS.h>
#include <tlvf/wfa_map/tlvAssociatedClients.h>
#include <tlvf/wfa_map/tlvSupportedService.h>

#include <easylogging++.h>

using namespace beerocks;
using namespace net;
using namespace son;

TopologyTask::TopologyTask(backhaul_manager &btl_ctx, ieee1905_1::CmduMessageTx &cmdu_tx)
    : Task(eTaskType::TOPOLOGY), m_btl_ctx(btl_ctx), m_cmdu_tx(cmdu_tx)
{
}

void TopologyTask::work()
{
    auto now = std::chrono::steady_clock::now();

    // Send topology discovery every 60 seconds according to IEEE_Std_1905.1-2013 specification
    constexpr uint8_t TOPOLOGY_DISCOVERY_TX_CYCLE_SEC = 60;
    static std::chrono::steady_clock::time_point discovery_timestamp =
        std::chrono::steady_clock::now();

    if (now - discovery_timestamp > std::chrono::seconds(TOPOLOGY_DISCOVERY_TX_CYCLE_SEC)) {
        discovery_timestamp = now;
        send_topology_discovery();
    }

    // Each platform must send Topology Discovery message every 60 seconds to its first circle
    // 1905.1 neighbors.
    // Iterate on each of known 1905.1 neighbors and check if we have received in the last
    // 60 seconds a Topology Discovery message from it. If not, remove this neighbor from our list
    // and send a Topology Notification message.
    static constexpr uint8_t DISCOVERY_NEIGHBOUR_REMOVAL_TIMEOUT_SEC =
        ieee1905_1_consts::DISCOVERY_NOTIFICATION_TIMEOUT_SEC + 3; // 3 seconds grace period.

    auto db = AgentDB::get();

    bool neighbors_list_changed = false;
    for (auto &neighbors_on_local_iface_entry : db->neighbor_devices) {
        auto &neighbors_on_local_iface = neighbors_on_local_iface_entry.second;
        for (auto it = neighbors_on_local_iface.begin(); it != neighbors_on_local_iface.end();) {
            auto &last_topology_discovery = it->second.timestamp;
            if (now - last_topology_discovery >
                std::chrono::seconds(DISCOVERY_NEIGHBOUR_REMOVAL_TIMEOUT_SEC)) {
                auto &device_al_mac = it->first;
                LOG(INFO) << "Removed 1905.1 device " << device_al_mac << " from neighbors list";
                it                     = neighbors_on_local_iface.erase(it);
                neighbors_list_changed = true;
                continue;
            }
            it++;
        }
    }

    if (neighbors_list_changed) {
        LOG(INFO) << "Sending topology notification on removeing of 1905.1 neighbors";
        send_topology_notification();
    }
}

void TopologyTask::send_topology_discovery()
{
    // TODO: get the list of interfaces that are up_and_running using the event-driven mechanism
    // to be implemented in #866

    /**
     * Transmission type of Topology Discovery message is 'neighbor multicast'.
     * That is, the CMDU must be transmitted once on each and every of its 1905.1 interfaces.
     * Also, according to IEEE1905.1, the message should include a MAC Address TLV which contains
     * the address of the interface on which the message is sent. Thus, a different message should
     * be sent on each interface.
     */
    auto db = AgentDB::get();

    // Make list of ifaces Macs to send on the message.
    auto ifaces = network_utils::linux_get_iface_list_from_bridge(db->bridge.iface_name);
    for (const auto &iface_name : ifaces) {
        if (!network_utils::linux_iface_is_up_and_running(iface_name)) {
            continue;
        }

        std::string iface_mac_str;
        if (!network_utils::linux_iface_get_mac(iface_name, iface_mac_str)) {
            LOG(ERROR) << "Failed getting MAC address for interface: " << iface_name;
            return;
        }

        auto iface_mac = tlvf::mac_from_string(iface_mac_str);

        auto cmdu_header =
            m_cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_DISCOVERY_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "Failed to create TOPOLOGY_DISCOVERY_MESSAGE cmdu";
            return;
        }
        auto tlvAlMacAddress = m_cmdu_tx.addClass<ieee1905_1::tlvAlMacAddress>();
        if (!tlvAlMacAddress) {
            LOG(ERROR) << "Failed to create tlvAlMacAddress tlv";
            return;
        }
        tlvAlMacAddress->mac() = db->bridge.mac;

        auto tlvMacAddress = m_cmdu_tx.addClass<ieee1905_1::tlvMacAddress>();
        if (!tlvMacAddress) {
            LOG(ERROR) << "Failed to create tlvMacAddress tlv";
            return;
        }
        tlvMacAddress->mac() = iface_mac;

        LOG(DEBUG) << "send_1905_topology_discovery_message, bridge_mac=" << db->bridge.mac
                   << ", iface=" << iface_name;
        m_btl_ctx.send_cmdu_to_broker(m_cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                      tlvf::mac_to_string(db->bridge.mac), iface_name);
    }
}

void TopologyTask::send_topology_notification()
{
    auto cmdu_header = m_cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
    if (!cmdu_header) {
        LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
        return;
    }
    auto db = AgentDB::get();
    m_btl_ctx.send_cmdu_to_broker(m_cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                  tlvf::mac_to_string(db->bridge.mac));
}
