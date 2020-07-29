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

void TopologyTask::handle_event(uint8_t event_enum_value)
{
    switch (eEvent(event_enum_value)) {
    case AGENT_RADIO_STATE_CHANGED: {
        send_topology_notification();
        break;
    }
    case AGENT_DEVICE_INITIALIZED: {
        send_topology_discovery();
        break;
    }
    default: {
        LOG(DEBUG) << "Message handler doesn't exists for event type " << event_enum_value;
        break;
    }
    }
}

bool TopologyTask::handle_cmdu(ieee1905_1::CmduMessageRx &cmdu_rx, const sMacAddr &src_mac,
                               std::shared_ptr<beerocks_header> beerocks_header)
{
    switch (cmdu_rx.getMessageType()) {
    case ieee1905_1::eMessageType::TOPOLOGY_DISCOVERY_MESSAGE: {
        handle_topology_discovery(cmdu_rx, src_mac);
        break;
    }
    case ieee1905_1::eMessageType::TOPOLOGY_QUERY_MESSAGE: {
        handle_topology_query(cmdu_rx, src_mac);
        break;
    }
    case ieee1905_1::eMessageType::VENDOR_SPECIFIC_MESSAGE: {
        return handle_vendor_specific(cmdu_rx, src_mac, beerocks_header);
    }
    default: {
        // Message was not handled, therefore return false.
        return false;
    }
    }
    return true;
}

void TopologyTask::handle_topology_discovery(ieee1905_1::CmduMessageRx &cmdu_rx,
                                             const sMacAddr &src_mac)
{
    auto tlvAlMac = cmdu_rx.getClass<ieee1905_1::tlvAlMacAddress>();
    if (!tlvAlMac) {
        LOG(ERROR) << "getClass tlvAlMacAddress failed";
        return;
    }

    auto db = AgentDB::get();

    // Filter out the messages we have sent.
    if (tlvAlMac->mac() == db->bridge.mac) {
        return;
    }

    auto mid = cmdu_rx.getMessageId();
    LOG(INFO) << "Received TOPOLOGY_DISCOVERY_MESSAGE from AL MAC=" << tlvAlMac->mac()
              << ", mid=" << std::hex << mid;

    auto tlvMac = cmdu_rx.getClass<ieee1905_1::tlvMacAddress>();
    if (!tlvMac) {
        LOG(ERROR) << "getClass tlvMacAddress failed";
        return;
    }

    uint32_t if_index                      = message_com::get_uds_header(cmdu_rx)->if_index;
    std::string local_receiving_iface_name = network_utils::linux_get_iface_name(if_index);
    if (local_receiving_iface_name.empty()) {
        LOG(ERROR) << "Failed getting interface name for index: " << if_index;
        return;
    }

    std::string local_receiving_iface_mac_str;
    if (!network_utils::linux_iface_get_mac(local_receiving_iface_name,
                                            local_receiving_iface_mac_str)) {
        LOG(ERROR) << "Failed getting MAC address for interface: " << local_receiving_iface_name;
        return;
    }

    LOG(DEBUG) << "sender iface_mac=" << tlvMac->mac()
               << ", local_receiving_iface=" << local_receiving_iface_name
               << ", local_receiving_iface_mac=" << local_receiving_iface_mac_str;

    // Check if it is a new device so if it does, we will send a Topology Notification.
    bool new_device = false;
    for (auto &neighbors_on_local_iface_entry : db->neighbor_devices) {
        auto &neighbors_on_local_iface = neighbors_on_local_iface_entry.second;
        new_device =
            neighbors_on_local_iface.find(tlvAlMac->mac()) == neighbors_on_local_iface.end();
        if (new_device) {
            break;
        }
    }

    // Add/Update the device on our list.
    AgentDB::sNeighborDevice neighbor_device;
    neighbor_device.transmitting_iface_mac = tlvMac->mac();
    neighbor_device.timestamp              = std::chrono::steady_clock::now();

    auto &neighbor_devices_by_al_mac =
        db->neighbor_devices[tlvf::mac_from_string(local_receiving_iface_mac_str)];
    neighbor_devices_by_al_mac[tlvAlMac->mac()] = neighbor_device;

    // If it is a new device, then our 1905.1 neighbors list has changed and we are required to send
    // Topology Notification Message.
    if (new_device) {
        LOG(INFO) << "Sending Topology Notification on newly discovered 1905.1 device";
        auto cmdu_header =
            m_cmdu_tx.create(0, ieee1905_1::eMessageType::TOPOLOGY_NOTIFICATION_MESSAGE);
        if (!cmdu_header) {
            LOG(ERROR) << "cmdu creation of type TOPOLOGY_NOTIFICATION_MESSAGE, has failed";
            return;
        }
        m_btl_ctx.send_cmdu_to_broker(m_cmdu_tx, network_utils::MULTICAST_1905_MAC_ADDR,
                                      tlvf::mac_to_string(db->bridge.mac));
    }
}

void TopologyTask::handle_topology_query(ieee1905_1::CmduMessageRx &cmdu_rx,
                                         const sMacAddr &src_mac)
{
    const auto mid = cmdu_rx.getMessageId();
    LOG(DEBUG) << "Received TOPOLOGY_QUERY_MESSAGE, mid=" << std::hex << mid;
    auto cmdu_tx_header =
        m_cmdu_tx.create(mid, ieee1905_1::eMessageType::TOPOLOGY_RESPONSE_MESSAGE);
    if (!cmdu_tx_header) {
        LOG(ERROR) << "Failed creating topology response header";
        return;
    }

    if (!add_device_information_tlv()) {
        return;
    }

    if (!add_1905_neighbor_device_tlv()) {
        return;
    }

    if (!add_supported_service_tlv()) {
        return;
    }

    if (!add_ap_operational_bss_tlv()) {
        return;
    }

    if (!add_associated_clients_tlv()) {
        return;
    }

    auto db = AgentDB::get();

    LOG(DEBUG) << "Sending topology response message, mid=" << std::hex << mid;
    m_btl_ctx.send_cmdu_to_broker(m_cmdu_tx, tlvf::mac_to_string(src_mac),
                                  tlvf::mac_to_string(db->bridge.mac));
}

bool TopologyTask::handle_vendor_specific(ieee1905_1::CmduMessageRx &cmdu_rx,
                                          const sMacAddr &src_mac,
                                          std::shared_ptr<beerocks_header> beerocks_header)
{
    if (!beerocks_header) {
        LOG(ERROR) << "beerocks_header is nullptr";
        return false;
    }

    // Since currently we handle only action_ops of action type "ACTION_BACKHAUL", use a single
    // switch-case on "ACTION_BACKHAUL" only.
    switch (beerocks_header->action_op()) {
    case beerocks_message::ACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION: {
        handle_vs_client_associated(cmdu_rx, beerocks_header);
        break;
    }
    case beerocks_message::ACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION: {
        handle_vs_client_disassociated(cmdu_rx, beerocks_header);
        break;
    }
    default: {
        // Message was not handled, therfore return false.
        return false;
    }
    }
    return true;
}

void TopologyTask::handle_vs_client_associated(ieee1905_1::CmduMessageRx &cmdu_rx,
                                               std::shared_ptr<beerocks_header> beerocks_header)
{
    // TODO: Move handling of "ACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION" to here when
    // moving this task to unified agent context.
}

void TopologyTask::handle_vs_client_disassociated(ieee1905_1::CmduMessageRx &cmdu_rx,
                                                  std::shared_ptr<beerocks_header> beerocks_header)
{
    // TODO: Move handling of "ACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION" to here when
    // moving this task to unified agent context.
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

bool TopologyTask::add_device_information_tlv()
{
    auto tlvDeviceInformation = m_cmdu_tx.addClass<ieee1905_1::tlvDeviceInformation>();
    if (!tlvDeviceInformation) {
        LOG(ERROR) << "addClass ieee1905_1::tlvDeviceInformation failed";
        return false;
    }

    auto db = AgentDB::get();

    /**
     * 1905.1 AL MAC address of the device.
     */
    tlvDeviceInformation->mac() = db->bridge.mac;

    /**
     * Set the number of local interfaces and fill info of each of the local interfaces, according
     * to IEEE_1905 section 6.4.5
     */

    /**
     * Add a LocalInterfaceInfo field for the wired interface, if any.
     */
    std::string &local_eth_iface_name = db->ethernet.iface_name;
    if (!local_eth_iface_name.empty() &&
        network_utils::linux_iface_is_up_and_running(local_eth_iface_name)) {
        ieee1905_1::eMediaType media_type = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
        if (!MediaType::get_media_type(local_eth_iface_name,
                                       ieee1905_1::eMediaTypeGroup::IEEE_802_3, media_type)) {
            LOG(ERROR) << "Unable to compute media type for interface " << local_eth_iface_name;
            return false;
        }

        std::shared_ptr<ieee1905_1::cLocalInterfaceInfo> localInterfaceInfo =
            tlvDeviceInformation->create_local_interface_list();

        // default to zero mac if get_mac fails.
        std::string eth_iface_mac = network_utils::ZERO_MAC_STRING;
        network_utils::linux_iface_get_mac(local_eth_iface_name, eth_iface_mac);
        localInterfaceInfo->mac()               = tlvf::mac_from_string(eth_iface_mac);
        localInterfaceInfo->media_type()        = media_type;
        localInterfaceInfo->media_info_length() = 0;

        tlvDeviceInformation->add_local_interface_list(localInterfaceInfo);
    }

    /**
     * Add a LocalInterfaceInfo field for each wireless interface.
     */
    for (const auto radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }

        // Iterate on front radio iface and then switch to back radio iface
        auto fill_radio_iface_info = [&](ieee1905_1::eMediaType media_type, bool front_iface) {
            LOG(DEBUG) << "filling interface information on radio="
                       << (front_iface ? radio->front.iface_name : radio->back.iface_name);

            // Skip Backhaul iteration iface when STA BWL is not allocated (Eth connection or GW).
            if (!front_iface && (db->device_conf.local_gw ||
                                 radio->back.iface_name != db->backhaul.selected_iface_name)) {
                LOG(TRACE) << "Skip radio interface with no active STA BWL, front_radio="
                           << radio->front.iface_name << ", back_radio=" << radio->back.iface_name;
                return true;
            }

            auto localInterfaceInfo = tlvDeviceInformation->create_local_interface_list();

            localInterfaceInfo->mac() =
                front_iface ? radio->front.iface_mac : radio->back.iface_mac;

            LOG(DEBUG) << "Added radio interface to tlvDeviceInformation: "
                       << localInterfaceInfo->mac();

            localInterfaceInfo->media_type() = media_type;

            ieee1905_1::s802_11SpecificInformation media_info = {};
            localInterfaceInfo->alloc_media_info(sizeof(media_info));

            // BSSID field is not defined well for interface. The common definition is in simple
            // words "the AP/ETH mac that we are connected to".
            // For fronthaul radio interface or unused backhaul interface put zero mac.
            if (db->device_conf.local_gw ||
                db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wired ||
                front_iface ||
                (db->backhaul.connection_type == AgentDB::sBackhaul::eConnectionType::Wireless &&
                 radio->back.iface_name != db->backhaul.selected_iface_name)) {
                media_info.network_membership = network_utils::ZERO_MAC;
            } else {
                media_info.network_membership = radio->back.iface_mac;
            }

            media_info.role =
                front_iface ? ieee1905_1::eRole::AP : ieee1905_1::eRole::NON_AP_NON_PCP_STA;

            // TODO: The Backhaul manager does not hold the information on the front radios.
            // For now, put zeros and when the Agent management will be move to unified Agent thread
            // this field will be filled. #435
            media_info.ap_channel_bandwidth               = 0;
            media_info.ap_channel_center_frequency_index1 = 0;
            media_info.ap_channel_center_frequency_index2 = 0;

            auto *media_info_ptr = localInterfaceInfo->media_info(0);
            if (media_info_ptr == nullptr) {
                LOG(ERROR) << "media_info is nullptr";
                return false;
            }

            std::copy_n(reinterpret_cast<uint8_t *>(&media_info), sizeof(media_info),
                        media_info_ptr);

            tlvDeviceInformation->add_local_interface_list(localInterfaceInfo);

            return true;
        };

        std::string &local_radio_iface_name = radio->front.iface_name;

        ieee1905_1::eMediaTypeGroup media_type_group = ieee1905_1::eMediaTypeGroup::IEEE_802_11;
        ieee1905_1::eMediaType media_type            = ieee1905_1::eMediaType::UNKNOWN_MEDIA;
        if (!MediaType::get_media_type(local_radio_iface_name, media_type_group, media_type)) {
            LOG(ERROR) << "Unable to compute media type for interface " << local_radio_iface_name;
            return false;
        }

        if (!fill_radio_iface_info(media_type, true)) {
            LOG(DEBUG) << "filling interface information on radio=" << radio->front.iface_name
                       << " has failed!";
            return false;
        }

        if (!fill_radio_iface_info(media_type, false)) {
            LOG(DEBUG) << "filling interface information on radio=" << radio->back.iface_name
                       << " backhaul has failed!";
            return false;
        }
    }
    return true;
}

bool TopologyTask::add_1905_neighbor_device_tlv()
{
    auto db = AgentDB::get();

    /**
     * Add a 1905.1 neighbor device TLV for each local interface for which this management entity
     * has inferred the presence of a 1905.1 neighbor device. Include each discovered neighbor
     * device in its corresponding 1905.1 neighbor device TLV.
     *
     * First, group known 1905 neighbor devices by the local interface that links to them. Create
     * a map which key is the name of the local interface and the value is the list of neighbor
     * devices inferred from that interface.
     */
    for (auto &neighbors_on_local_iface_entry : db->neighbor_devices) {
        auto tlv1905NeighborDevice = m_cmdu_tx.addClass<ieee1905_1::tlv1905NeighborDevice>();
        if (!tlv1905NeighborDevice) {
            LOG(ERROR) << "addClass ieee1905_1::tlv1905NeighborDevice failed";
            return false;
        }

        tlv1905NeighborDevice->mac_local_iface() = neighbors_on_local_iface_entry.first;
        auto &neighbors_on_local_iface           = neighbors_on_local_iface_entry.second;

        if (!tlv1905NeighborDevice->alloc_mac_al_1905_device(neighbors_on_local_iface.size())) {
            LOG(ERROR) << "alloc_mac_al_1905_device() has failed";
            return false;
        }

        size_t index = 0;
        for (const auto &neighbor_on_local_iface_entry : neighbors_on_local_iface) {
            auto &neighbor_al_mac = neighbor_on_local_iface_entry.first;

            auto mac_al_1905_device_tuple = tlv1905NeighborDevice->mac_al_1905_device(index);
            if (!std::get<0>(mac_al_1905_device_tuple)) {
                LOG(ERROR) << "getting mac_al_1905_device element has failed";
                return false;
            }

            auto &mac_al_1905_device = std::get<1>(mac_al_1905_device_tuple);
            mac_al_1905_device.mac   = neighbor_al_mac;
            mac_al_1905_device.bridges_exist =
                ieee1905_1::tlv1905NeighborDevice::eBridgesExist::AT_LEAST_ONE_BRIDGES_EXIST;
            index++;
        }
    }
    return true;
}

bool TopologyTask::add_supported_service_tlv()
{
    auto tlvSupportedService = m_cmdu_tx.addClass<wfa_map::tlvSupportedService>();
    if (!tlvSupportedService) {
        LOG(ERROR) << "addClass wfa_map::tlvSupportedService failed";
        return false;
    }

    auto db = AgentDB::get();

    size_t number_of_supported_services = 1;
    if (db->device_conf.local_controller) {
        number_of_supported_services++;
    }

    if (!tlvSupportedService->alloc_supported_service_list(number_of_supported_services)) {
        LOG(ERROR) << "alloc_supported_service_list failed";
        return false;
    }

    auto supportedServiceTuple = tlvSupportedService->supported_service_list(0);
    if (!std::get<0>(supportedServiceTuple)) {
        LOG(ERROR) << "Failed accessing supported_service_list(0)";
        return false;
    }

    std::get<1>(supportedServiceTuple) =
        wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_AGENT;

    if (db->device_conf.local_controller) {
        supportedServiceTuple = tlvSupportedService->supported_service_list(1);
        if (!std::get<0>(supportedServiceTuple)) {
            LOG(ERROR) << "Failed accessing supported_service_list(1)";
            return false;
        }

        std::get<1>(supportedServiceTuple) =
            wfa_map::tlvSupportedService::eSupportedService::MULTI_AP_CONTROLLER;
    }
    return true;
}

bool TopologyTask::add_ap_operational_bss_tlv()
{
    auto tlvApOperationalBSS = m_cmdu_tx.addClass<wfa_map::tlvApOperationalBSS>();
    if (!tlvApOperationalBSS) {
        LOG(ERROR) << "addClass wfa_map::tlvApOperationalBSS failed";
        return false;
    }

    auto db = AgentDB::get();

    for (const auto &radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }

        auto radio_list         = tlvApOperationalBSS->create_radio_list();
        radio_list->radio_uid() = radio->front.iface_mac;
        for (const auto &bssid : radio->front.bssids) {
            if (bssid.mac == network_utils::ZERO_MAC) {
                continue;
            }
            if (bssid.ssid.empty()) {
                continue;
            }
            auto radio_bss_list           = radio_list->create_radio_bss_list();
            radio_bss_list->radio_bssid() = bssid.mac;
            radio_bss_list->set_ssid(bssid.ssid);

            radio_list->add_radio_bss_list(radio_bss_list);
        }
        tlvApOperationalBSS->add_radio_list(radio_list);
    }

    return true;
}

bool TopologyTask::add_associated_clients_tlv()
{
    auto db = AgentDB::get();

    // The Multi-AP Agent shall include an Associated Clients TLV in the message if there is at
    // least one 802.11 client directly associated with any of the BSS(s) that is operated by the
    // Multi-AP Agent.
    bool include_associated_clients_tlv = false;
    for (const auto &radio : db->get_radios_list()) {
        if (!radio) {
            continue;
        }
        if (radio->associated_clients.size() > 0) {
            include_associated_clients_tlv = true;
            break;
        }
    }

    if (include_associated_clients_tlv) {
        auto tlvAssociatedClients = m_cmdu_tx.addClass<wfa_map::tlvAssociatedClients>();
        if (!tlvAssociatedClients) {
            LOG(ERROR) << "addClass wfa_map::tlvAssociatedClients failed";
            return false;
        }

        // Get current time to compute elapsed time since last client association
        auto now = std::chrono::steady_clock::now();

        // Fill in Associated Clients TLV
        for (const auto &radio : db->get_radios_list()) {
            if (!radio) {
                continue;
            }

            for (const auto &bssid : radio->front.bssids) {
                auto bss_list     = tlvAssociatedClients->create_bss_list();
                bss_list->bssid() = bssid.mac;

                for (const auto &associated_client_entry : radio->associated_clients) {
                    if (associated_client_entry.second.bssid != bssid.mac) {
                        continue;
                    }

                    auto client_info = bss_list->create_clients_associated_list();

                    auto &association_time = associated_client_entry.second.association_time;
                    auto elapsed =
                        std::chrono::duration_cast<std::chrono::seconds>(now - association_time)
                            .count();
                    if ((elapsed < 0) || (elapsed > UINT16_MAX)) {
                        elapsed = UINT16_MAX;
                    }

                    client_info->mac()                             = associated_client_entry.first;
                    client_info->time_since_last_association_sec() = elapsed;

                    bss_list->add_clients_associated_list(client_info);
                }
                tlvAssociatedClients->add_bss_list(bss_list);
            }
        }
    }
    return true;
}
