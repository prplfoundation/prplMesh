/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <algorithm>
#include <mapf/topology_discovery_agent/topology_discovery_agent.h>

namespace mapf {

/* Parameters assumed to change are, ieee802_1bridge : indicating bridge interface or not,
 * transport : indicating interface is transport
 */
int TopologyDiscoveryAgentLocalDB::HandleInterfaceEvent(struct interface interface_instance)
{
    int bridge_state_change = 0, return_value = 0;
    struct interface_list_item search_interface;
    std::copy_n((uint8_t *)&interface_instance, sizeof(interface_instance),
                (uint8_t *)&search_interface.instance);
    auto it = std::find(interface_list.begin(), interface_list.end(), search_interface);
    if (it != interface_list.end()) {
        if ((it->instance.status != search_interface.instance.status) &&
            (interface_instance.status == 0)) {
            // delete operation
            interface_list.erase(it);
        } else {
            // modify operation
            // parameters that can be updated are bridge and transport state
            if (it->instance.ieee802_1bridge != interface_instance.ieee802_1bridge) {
                it->instance.ieee802_1bridge = interface_instance.ieee802_1bridge;
                // update flag to indicate if bridge state changed and is 1/0
                if (interface_instance.ieee802_1bridge == 1)
                    bridge_state_change = 1;
                else
                    bridge_state_change = 0;
            }
            if (it->instance.transport != interface_instance.transport) {
                it->instance.transport = interface_instance.transport;
            }
        }
    } else {
        // add operation
        interface_list.push_back(search_interface);
        if (interface_instance.ieee802_1bridge == 1) {
            bridge_state_change = 1;
        }
        // bit2 to trigger discovery by caller
        return_value |= (1 << 2);
    }

    if (bridge_state_change == 1) {
        // bit1 to update AL MAC by caller
        return_value |= (1 << 1);
    }

    ShowInterfaceList();
    return return_value;
}

void TopologyDiscoveryAgentLocalDB::ShowInterfaceList()
{
    MAPF_DBG("Total number of interfaces: " << interface_list.size());
    MAPF_DBG("Index\tBridge index\tBridge\tTransport");
    for (auto it : interface_list) {
        MAPF_DBG(it.instance.index << "\t" << it.instance.parent_bridge_index << "\t"
                                   << ((it.instance.ieee802_1bridge) ? "yes" : "no") << "\t"
                                   << ((it.instance.transport) ? "yes" : "no"));
    }
}

/* traverse thru interface list and respective neighbor list to search for specific neighbor mac
 * if not found, add as new neighbor to appropriate interface's neighbor list with bridge exist flag as true by default
 */
int TopologyDiscoveryAgentLocalDB::UpdateNeighborEntry(int index, uint8_t *mac, uint8_t *neigh_mac,
                                                       int type, int oper)
{
    int return_value = 0;
    struct interface_list_item search_interface(index, mac);
    struct neighbor search_neighbor(neigh_mac);
    auto interface_entry =
        std::find(interface_list.begin(), interface_list.end(), search_interface);

    MAPF_DBG("search for interface entry with index " << index);
    if (interface_entry != interface_list.end()) {
        MAPF_DBG("interface entry found");
        auto neighbor_entry = std::find(interface_entry->neighbor_list.begin(),
                                        interface_entry->neighbor_list.end(), search_neighbor);
        MAPF_DBG("search for specific neighbor");
        if (neighbor_entry != interface_entry->neighbor_list.end()) {
            MAPF_DBG("specific neigbor found");
            if (oper == 1) {
                // add operation : neighbor already known
            } else if (oper == 2) {
                // update operation
                neighbor_entry->bridge_exists = 0;
            } else {
                // delete operation
                interface_entry->neighbor_list.erase(neighbor_entry);
            }
        } else {
            MAPF_DBG("specific neigbor not found");
            if (oper == 1) {
                // add operation
                MAPF_DBG("Neighbor added");
                search_neighbor.bridge_exists = 1;
                search_neighbor.type          = type;
                interface_entry->neighbor_list.push_back(search_neighbor);
            } else {
                MAPF_DBG("neighbor can't be updated/deleted");
                // unable to find requested entry for update/delete operation
                return_value = -1;
            }
        }
    } else {
        MAPF_DBG("if not found");
        return_value = -1;
    }

    if (return_value != -1) {
        ShowNeighborList(interface_entry);
    }
    return return_value;
}

void TopologyDiscoveryAgentLocalDB::ShowNeighborList(
    std::list<struct interface_list_item>::iterator it1)
{
    if (it1->neighbor_list.begin() == it1->neighbor_list.end()) {
        MAPF_ERR("No known neighbors for interface " << it1->instance.index);
        return;
    }
    MAPF_DBG("Known neighbors for interface " << it1->instance.index);
    for (auto it2 : it1->neighbor_list) {
        MAPF_DBG("Neighbor : MAC - "
                 << std::hex << std::setfill('0') << std::setw(2) << (unsigned)it2.mac[0] << ":"
                 << std::hex << std::setfill('0') << std::setw(2) << (unsigned)it2.mac[1] << ":"
                 << std::hex << std::setfill('0') << std::setw(2) << (unsigned)it2.mac[2] << ":"
                 << std::hex << std::setfill('0') << std::setw(2) << (unsigned)it2.mac[3] << ":"
                 << std::hex << std::setfill('0') << std::setw(2) << (unsigned)it2.mac[4] << ":"
                 << std::hex << std::setfill('0') << std::setw(2) << (unsigned)it2.mac[5]
                 << ", type - " << it2.type << ", bridge exists - " << it2.bridge_exists);
    }
}

} // namespace mapf
