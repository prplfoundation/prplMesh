/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee1905_transport.h"
#include <mapf/common/utils.h>

#include <arpa/inet.h>
#include <net/if.h>

namespace beerocks {

void Ieee1905Transport::handle_local_bus_pollin_event()
{
    auto msg = local_bus_->subscriber().Receive();

    if (auto *cmdu_tx_msg = dynamic_cast<CmduTxMessage *>(msg.get())) {
        MAPF_DBG("received CmduTxMessage message:" << std::endl << *cmdu_tx_msg);
        handle_local_bus_cmdu_tx_message(*cmdu_tx_msg);
    } else if (auto *interface_configuration_request_msg =
                   dynamic_cast<InterfaceConfigurationRequestMessage *>(msg.get())) {
        MAPF_DBG("received InterfaceConfigurationRequestMessage message:"
                 << std::endl
                 << *interface_configuration_request_msg);
        handle_local_bus_interface_configuration_request_message(
            *interface_configuration_request_msg);
    } else if (auto *interface_configuration_query_msg =
                   dynamic_cast<InterfaceConfigurationQueryMessage *>(msg.get())) {
        MAPF_DBG("received InterfaceConfigurationQueryMessage message:"
                 << std::endl
                 << *interface_configuration_query_msg);
        publish_interface_configuration_indication();
    } else {
        // should never receive messages which we are not subscribed to
        MAPF_WARN("received un-expected message:" << std::endl << *msg);
    }
}

void Ieee1905Transport::handle_local_bus_cmdu_tx_message(CmduTxMessage &msg)
{
    // parse Local Bus CmduTxMessage message
    struct Packet packet;
    packet.dst_if_type  = msg.metadata()->if_type;
    packet.dst_if_index = msg.metadata()->if_index;
    packet.src_if_type  = CmduRxMessage::IF_TYPE_LOCAL_BUS;
    packet.src_if_index = 0;
    std::copy_n(msg.metadata()->dst, ETH_ALEN, packet.dst.oct);
    std::copy_n(msg.metadata()->src, ETH_ALEN, packet.src.oct);
    packet.ether_type       = msg.metadata()->ether_type;
    packet.header.iov_base  = 0;
    packet.header.iov_len   = 0;
    packet.payload.iov_base = msg.data();
    packet.payload.iov_len  = msg.metadata()->length;

    if (packet.ether_type == ETH_P_1905_1) {
        mapf_assert(packet.payload.iov_len >= sizeof(Ieee1905CmduHeader));

        Ieee1905CmduHeader *ch = (Ieee1905CmduHeader *)packet.payload.iov_base;

        // update messageId field (a.k.a "MID") unless is was pre-set by the originator
        if (!msg.metadata()->preset_message_id) {
            uint16_t messageId = get_next_message_id();
            ch->messageId      = htons(messageId);
        }

        // send confirmation (with messageId value)
        if (msg.metadata()->cookie != 0) {
            CmduTxConfirmationMessage confMsg;

            confMsg.metadata()->cookie     = msg.metadata()->cookie;
            confMsg.metadata()->ether_type = msg.metadata()->ether_type;
            confMsg.metadata()->msg_type   = msg.metadata()->msg_type;
            confMsg.metadata()->msg_id     = ntohs(ch->messageId);

            MAPF_DBG("publishing CmduTxConfirmationMessage:" << std::endl << confMsg);
            if (local_bus_->publisher().Send(confMsg) == false) {
                MAPF_ERR("cannot publish message on local bus.");
            }
        }

        if (msg.metadata()->msg_type != ntohs(ch->messageType)) {
            MAPF_WARN("CmduTxMessage messageType mismatch: cmdu: "
                      << std::hex << msg.metadata()->msg_type
                      << " packet: " << ntohs(ch->messageType));
        }
    }

    counters_[CounterId::OUTGOING_LOCAL_BUS_PACKETS]++;
    handle_packet(packet);
}

void Ieee1905Transport::handle_local_bus_interface_configuration_request_message(
    InterfaceConfigurationRequestMessage &msg)
{
    std::map<std::string, NetworkInterface> updated_network_interfaces;

    // fill an interfaces std::map with the specified interfaces
    for (int i = 0; i < int(msg.metadata()->numInterfaces); i++) {
        using Flags = InterfaceConfigurationRequestMessage::Flags;

        // only consider interfaces marked with IEEE1905 transport enabled
        if (!(msg.metadata()->interfaces[i].flags &
              (Flags::ENABLE_IEEE1905_TRANSPORT | Flags::IS_BRIDGE))) {
            continue;
        }

        std::string ifname        = std::string(msg.metadata()->interfaces[i].ifname);
        std::string bridge_ifname = std::string(msg.metadata()->interfaces[i].bridge_ifname);
        unsigned int if_index     = if_nametoindex(ifname.c_str());

        if (updated_network_interfaces.count(ifname) > 0) {
            MAPF_ERR("ignoring duplicate entry for interface " << if_index << ".");
            continue;
        }

        updated_network_interfaces[ifname].ifname      = ifname;
        updated_network_interfaces[ifname].bridge_name = bridge_ifname;
        updated_network_interfaces[ifname].is_bridge =
            msg.metadata()->interfaces[i].flags & Flags::IS_BRIDGE;
    }

    update_network_interfaces(updated_network_interfaces);

    publish_interface_configuration_indication();
}

bool Ieee1905Transport::send_packet_to_local_bus(Packet &packet)
{
    // create and fill an CmduRxMessage to be sent on the local bus
    CmduRxMessage msg;

    std::copy_n(packet.src.oct, ETH_ALEN, msg.metadata()->src);
    std::copy_n(packet.dst.oct, ETH_ALEN, msg.metadata()->dst);

    msg.metadata()->ether_type = packet.ether_type;
    msg.metadata()->if_type    = packet.src_if_type;
    msg.metadata()->if_index   = packet.src_if_index;
    msg.metadata()->length     = packet.payload.iov_len;
    std::copy_n((uint8_t *)packet.payload.iov_base, packet.payload.iov_len, msg.data());

    if (packet.ether_type == ETH_P_1905_1) {
        Ieee1905CmduHeader *ch   = (Ieee1905CmduHeader *)packet.payload.iov_base;
        msg.metadata()->msg_type = ntohs(ch->messageType);
        msg.metadata()->relay    = ch->GetRelayIndicator() ? 1 : 0;
    } else {
        msg.metadata()->msg_type = 0;
        msg.metadata()->relay    = 0;
    }

    counters_[CounterId::INCOMMING_LOCAL_BUS_PACKETS]++;

    MAPF_DBG("publishing CmduRxMessage:" << std::endl << msg);
    if (local_bus_->publisher().Send(msg) == false) {
        MAPF_ERR("failed to publish message on local bus.");
        return false;
    }

    return true;
}

void Ieee1905Transport::publish_interface_configuration_indication()
{
    // publish an updated interface status indication
    InterfaceConfigurationIndicationMessage indication_msg;
    uint32_t n = 0;
    for (auto it = network_interfaces_.begin(); it != network_interfaces_.end(); ++it) {
        using Flags = InterfaceConfigurationRequestMessage::Flags;
        auto &iface = it->second;

        mapf::utils::copy_string(indication_msg.metadata()->interfaces[n].ifname,
                                 iface.ifname.c_str(), IF_NAMESIZE);
        mapf::utils::copy_string(indication_msg.metadata()->interfaces[n].bridge_ifname,
                                 iface.bridge_name.c_str(), IF_NAMESIZE);

        if (iface.is_bridge) {
            indication_msg.metadata()->interfaces[n].flags |= Flags::IS_BRIDGE;
        } else {
            indication_msg.metadata()->interfaces[n].flags |= Flags::ENABLE_IEEE1905_TRANSPORT;
        }

        n++;
    }
    indication_msg.metadata()->numInterfaces = n;

    MAPF_DBG("publishing InterfaceConfigurationIndicationMessage:" << std::endl << indication_msg);
    if (local_bus_->publisher().Send(indication_msg) == false) {
        MAPF_ERR("failed to publish message on local bus.");
    }
}

uint16_t Ieee1905Transport::get_next_message_id()
{
    message_id_++;

    if (message_id_ == 0) {
        MAPF_DBG("messageId wrap-around occurred.");
        counters_[CounterId::MESSAGE_ID_WRAPAROUND]++;
    }

    return message_id_;
}

} // namespace beerocks
