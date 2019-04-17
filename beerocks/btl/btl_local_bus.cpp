/*
* INTEL CONFIDENTIAL
* Copyright 2019 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#include "btl.h"
#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>
#include <mapf/local_bus.h>
#include <mapf/transport/ieee1905_transport.h>

using namespace beerocks::btl;
using namespace beerocks::net;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////
static bool local_bus_read_ready(std::shared_ptr<mapf::LocalBusInterface> bus, std::shared_ptr<mapf::Poller> poller)
{
    LOG_IF(!poller, FATAL) << "Poller is not allocated!";

    int rc = poller->CheckEvent(bus->subscriber());
    if (rc & MAPF_POLLIN) {
        return true;
    } else if (rc & MAPF_POLLERR) {
        return false;
    } else if (rc == 0) {
        return false;
    }

    LOG(ERROR) << "check event error on local bus sub socket, rc=" << rc;
    return false;

}

static bool subscribe_topic_to_bus(std::shared_ptr<mapf::LocalBusInterface> bus,
                                   std::shared_ptr<mapf::Poller> poller, const ieee1905_1::eMessageType msg_type)
{
    LOG(INFO) << "subscribing topic=" << (int)msg_type;
    int rc = bus->subscriber().Subscribe<mapf::CmduRxMessage>(mapf::CmduRxMessage::ieee1905_topic((uint16_t)msg_type));
    if (rc) {
        LOG(ERROR) << "Subscribe error rc=" << rc << ", on topic=" << (int)msg_type;
        return false;
    }

    rc = poller->Add(bus->subscriber());
    if (rc && errno != EEXIST) {
        LOG(ERROR) << "Add to poller error rc=" << rc;
        return false;
    }
    else if (errno == EEXIST) {
        LOG(DEBUG) << "Adding subscriber already exist";
    }

    bus->Sync(); //TODO - check return value after mapf asserts refactor
    return true;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Implementation ///////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void transport_socket_thread::bus_init()
{
    bus = std::make_shared<mapf::LocalBusInterface>(mapf::Context::Instance());
    LOG_IF(!bus, FATAL) << "Failed allocating bus!";

    poller = std::make_shared<mapf::Poller>();
    LOG_IF(!poller, FATAL) << "Failed allocating Poller!";
}

bool transport_socket_thread::bus_subscribe(const std::vector<ieee1905_1::eMessageType>& msg_types)
{
    bool ret = true;

    LOG_IF(!bus, FATAL) << "Bus is not allocated!";
    LOG_IF(!poller, FATAL) << "Poller is not allocaed!";
    bus->Init();
    for_each (msg_types.begin(), msg_types.end(), [&](const ieee1905_1::eMessageType msg_type) {
        ret &= subscribe_topic_to_bus(bus, poller, msg_type);
    });

    return ret;
}

bool transport_socket_thread::bus_connect(const std::string& beerocks_temp_path, const bool local_master)
{
    return true;
}

void transport_socket_thread::bus_connected(Socket *sd)
{    
}

bool transport_socket_thread::configure_ieee1905_transport_interfaces(const std::string& bridge_iface, const std::vector<std::string>& ifaces)
{
    LOG_IF(!bus, FATAL) << "Bus is not allocated!";
    
    int bridge_if_index = network_utils::linux_get_iface_index(bridge_iface);

    mapf::InterfaceConfigurationRequestMessage interface_configuration_request_msg;
    using Flags = mapf::InterfaceConfigurationRequestMessage::Flags;

    uint32_t n = 0;
    if (bridge_if_index) {
        interface_configuration_request_msg.metadata()->interfaces[n].if_index = bridge_if_index;
        interface_configuration_request_msg.metadata()->interfaces[n].flags |= Flags::IS_BRIDGE;
        n++;
        THREAD_LOG(DEBUG) << "adding bridge " << bridge_iface << " to ieee1905 transport, bridge_if_index=" << bridge_if_index;
    }
    for (const auto& iface : ifaces) {
        int if_index = network_utils::linux_get_iface_index(iface);
        interface_configuration_request_msg.metadata()->interfaces[n].if_index = if_index;
        interface_configuration_request_msg.metadata()->interfaces[n].bridge_if_index = bridge_if_index;
        interface_configuration_request_msg.metadata()->interfaces[n].flags |= Flags::ENABLE_IEEE1905_TRANSPORT;
        n++;
        THREAD_LOG(DEBUG) << "adding interface " << iface << " to ieee1905 transport, if_index=" << if_index << " bridge_if_index=" << bridge_if_index;
    }
    interface_configuration_request_msg.metadata()->numInterfaces = n;
    THREAD_LOG(DEBUG) << "numInterfaces=" << n;

    return bus->publisher().Send(interface_configuration_request_msg);
}

void transport_socket_thread::add_socket(Socket *s, bool add_to_vector)
{
    LOG_IF(!poller, FATAL) << "Poller is not allocated!";

    poller->Add(s->getSocketFd(), MAPF_POLLIN | MAPF_POLLERR);
    if (add_to_vector && std::find(sockets.begin(), sockets.end(), s) == sockets.end()) {
        sockets.push_back(s);
    }
}

void transport_socket_thread::remove_socket(Socket *s) 
{
    LOG_IF(!poller, FATAL) << "Poller is not allocated!";

    poller->Remove(s->getSocketFd());
    sockets.erase(std::remove(sockets.begin(), sockets.end(), s), sockets.end()); 
}

bool transport_socket_thread::read_ready(Socket *s)
{
    LOG_IF(!poller, FATAL) << "Poller is not allocated!";

    int rc = poller->CheckEvent(s->getSocketFd());
    if (rc & MAPF_POLLIN) {
        return true;
	} else if (rc & MAPF_POLLERR) {
        THREAD_LOG(DEBUG) << "Received POLLER";
		return true;
    } else if (rc == 0) {
        return false;
    } else {
        THREAD_LOG(ERROR) << "check event error on local bus sub socket, rc=" << rc;
        return false;
    }
}

bool transport_socket_thread::handle_cmdu_message_bus()
{
    auto msg = bus->subscriber().Receive();
    if(msg == nullptr) {
        THREAD_LOG(ERROR) << "Received msg is null";
        return false;
    }

    auto cmdu_rx_msg = dynamic_cast<mapf::CmduRxMessage*>(msg.get());
    if (cmdu_rx_msg) {
    } else {
        THREAD_LOG(ERROR) << "received non CmduRxMessage:\n\tMessage: " << *msg << "\n\tFrame: " << msg->frame().str();
        return false;
    }

    // Copy the data to rx_buffer
    if (sizeof(message::sUdsHeader) + cmdu_rx_msg->metadata()->length > sizeof(rx_buffer)){
        THREAD_LOG(ERROR) << "sizeof(message::sUdsHeader) + cmdu_rx_msg->metadata()->length > sizeof(rx_buffer)";
        return false;
    }

    std::copy_n((uint8_t*)cmdu_rx_msg->data(), cmdu_rx_msg->metadata()->length, rx_buffer + sizeof(message::sUdsHeader));

    // fill UDS Header
    message::sUdsHeader* uds_header = (message::sUdsHeader*)rx_buffer;
    std::copy_n((uint8_t*)cmdu_rx_msg->metadata()->src, sizeof(mapf::CmduRxMessage::Metadata::src), uds_header->src_bridge_mac);
    std::copy_n((uint8_t*)cmdu_rx_msg->metadata()->dst, sizeof(mapf::CmduRxMessage::Metadata::dst), uds_header->dst_bridge_mac);
    uds_header->length = cmdu_rx_msg->metadata()->length;
    uds_header->swap_needed = true;

    if (!verify_cmdu(uds_header)) {
        THREAD_LOG(ERROR) << "Failed verifying cmdu header";
        return false;
    }

    if (!cmdu_rx.parse(rx_buffer + sizeof(message::sUdsHeader), uds_header->length, uds_header->swap_needed)) {
         THREAD_LOG(ERROR) << "parsing cmdu failure, rx_buffer" << std::hex << rx_buffer << std::dec 
                           << ", uds_header->length=" << int(uds_header->length) << ", uds_header->swap_needed=" << int(uds_header->swap_needed);
        return false;
    }

    if (!handle_cmdu(nullptr, cmdu_rx)) {
        return false;
    }

    return true;
}

bool transport_socket_thread::bus_send(ieee1905_1::CmduMessage& cmdu, const std::string& dst_mac, const std::string& src_mac, uint16_t length)
{
    mapf::CmduTxMessage msg;

    net::network_utils::mac_from_string(msg.metadata()->src, src_mac);
    net::network_utils::mac_from_string(msg.metadata()->dst, dst_mac);

    msg.metadata()->ether_type = ETH_P_1905_1;
    msg.metadata()->length = length;
    msg.metadata()->msg_type = static_cast<uint16_t>(cmdu.getMessageType());

    std::copy_n((uint8_t*)cmdu.getMessageBuff(), msg.metadata()->length, (uint8_t*)msg.data());
    return bus->publisher().Send(msg);
}

bool transport_socket_thread::from_bus(Socket *sd)
{
    return sd == nullptr;
}

bool transport_socket_thread::work()
{
    LOG_IF(!bus, FATAL) << "Bus is not allocated!";
    LOG_IF(!poller, FATAL) << "Poller is not allocated!";

    before_select();

    int num_events = poller->Poll(poll_timeout_ms);
    if (num_events < 0) {
        // Do not fail for the following "errors"
        if (errno == EINTR) {
            THREAD_LOG(DEBUG) << "Poll returned: " << strerror(errno);
            return true;
        }

        THREAD_LOG(ERROR) << "Poll error: " << strerror(errno);
        return false;
    }

    after_select(num_events == 0);

    if (num_events == 0) {
        //THREAD_LOG(DEBUG) << "poll timeout";
        return true;
    }

    //If something happened on the server socket , then its an incoming connection
    if (server_socket && read_ready(server_socket.get())) {
        THREAD_LOG(DEBUG) << "accept new connection on server socket sd=" << server_socket.get();
        Socket *sd = server_socket->acceptConnections();
        if (sd == nullptr || (!server_socket->getError().empty())) {
            THREAD_LOG(ERROR) << "acceptConnections == nullptr: " << server_socket->getError();
            return false;
        } else {
            if(unix_socket_path.empty()) {
                THREAD_LOG(DEBUG) << "new connection from ip=" << sd->getPeerIP()
                       << " port=" << sd->getPeerPort() << " sd=" << sd;
            } else {
                THREAD_LOG(DEBUG) << "new connection on " << unix_socket_path << " sd=" << sd;
            }
            socket_connected(sd);
        }

        num_events--;
    }

    if (!num_events) return true;
    // read from bus
    if (local_bus_read_ready(bus, poller)) {
        handle_cmdu_message_bus();
        num_events--;
    }

    if (!num_events) return true;

    // read from UDS
    do {
        for (unsigned int i = 0; i < sockets.size() && num_events > 0; i++) {
            if (read_ready(sockets.at(i))) {
                num_events--;
                Socket *sd = sockets.at(i);
                if (socket_disconnected_uds(sd)) {
					continue;
                }

                if (!handle_cmdu_message_uds(sd)) {
                    continue;
                }
            }
        }
    } while (num_events);

    return true;
}