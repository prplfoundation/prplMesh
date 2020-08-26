/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <gtest/gtest.h>

#include "../ieee1905_transport_broker.h"

#include <sys/uio.h>

#include <bcl/beerocks_event_loop_impl.h>
#include <bcl/beerocks_os_utils.h>
#include <bcl/beerocks_thread_base.h>

#include <beerocks/tlvf/beerocks_message.h>

#include <tlvf/CmduMessageTx.h>

#include <mapf/transport/ieee1905_transport_messages.h>

#include <easylogging++.h>

// Initialize easylogging++
INITIALIZE_EASYLOGGINGPP

namespace beerocks {
namespace transport {
namespace broker {
namespace tests {

using namespace beerocks::transport::messages;

//////////////////////////////////////////////////////////////////////////////
////////////////////////////// Global Variables //////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// UDS file for the tests
static const std::string broker_uds_file = "beerocks_broker_test_uds";

// Broker SocketServer listen buffer depth
static constexpr int broker_listen_buffer = 1;

// Default broker server timeout
static constexpr auto broker_timeout = std::chrono::milliseconds(100);

// Invalid message magic value
static constexpr uint32_t INVALID_MAGIC = 0x12345678;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Helper Classes ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Wrapper class for the BrokerServer for capturing errors
class BrokerServerWrapper : public BrokerServer {
public:
    // Inherit BrokerServer's constructor
    using BrokerServer::BrokerServer;

    bool error() { return m_error_occurred; }

protected:
    bool m_error_occurred = false;

    bool handle_msg(const std::shared_ptr<Socket> &sd) override
    {
        if (BrokerServer::handle_msg(sd) == false) {
            m_error_occurred = true;
            return false;
        }

        m_error_occurred = false;
        return true;
    }
};

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////// Tests ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Suppress cppcheck syntax error for gtest TEST macro
// cppcheck-suppress syntaxError
TEST(broker_server, setup)
{
    // Configure easylogging++ formatting
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Format,
                            "%level %datetime{%H:%m:%s:%g} <%thread> %fbase[%line] --> %msg");

    // el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::reconfigureLogger("default", defaultConf);
}

TEST(broker_server, invalid_message_magic)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Register a dummy internal message handler
    broker_wrapper.register_internal_message_handler(
        [](std::unique_ptr<messages::Message> &msg, BrokerServer &broker) -> bool { return true; });

    // Create a random message
    messages::Message dummy;

    // Invalid header
    messages::Message::Header header;
    header.magic = INVALID_MAGIC;

    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    LOG(DEBUG) << "Sending INVALID magic...";
    ASSERT_TRUE(messages::send_transport_message(sock1, dummy, &header));
    ASSERT_EQ(1, event_loop->run()); // Process
    ASSERT_TRUE(broker_wrapper.error());

    LOG(DEBUG) << "Sending VALID magic...";
    ASSERT_TRUE(messages::send_transport_message(sock1, dummy));
    ASSERT_EQ(1, event_loop->run());
    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, subscribe_empty_message)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type = SubscribeMessage::ReqType::SUBSCRIBE;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process

    ASSERT_TRUE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, subscribe_single_type)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type               = SubscribeMessage::ReqType::SUBSCRIBE;
    subscribe.metadata()->msg_types_count    = 1;
    subscribe.metadata()->msg_types[0].value = 5;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process

    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, subscribe_multiple_types)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type               = SubscribeMessage::ReqType::SUBSCRIBE;
    subscribe.metadata()->msg_types_count    = 3;
    subscribe.metadata()->msg_types[0].value = 1;
    subscribe.metadata()->msg_types[1].value = 2;
    subscribe.metadata()->msg_types[2].value = 3;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process

    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, unsubscribe_empty_message)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type = SubscribeMessage::ReqType::UNSUBSCRIBE;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process

    ASSERT_TRUE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, unsubscribe_single_type)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type               = SubscribeMessage::ReqType::UNSUBSCRIBE;
    subscribe.metadata()->msg_types_count    = 1;
    subscribe.metadata()->msg_types[0].value = 5;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process

    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, unsubscribe_multiple_types)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type               = SubscribeMessage::ReqType::UNSUBSCRIBE;
    subscribe.metadata()->msg_types_count    = 3;
    subscribe.metadata()->msg_types[0].value = 1;
    subscribe.metadata()->msg_types[1].value = 2;
    subscribe.metadata()->msg_types[2].value = 3;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process

    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, subscribe_unsubscribe)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Create a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type               = SubscribeMessage::ReqType::SUBSCRIBE;
    subscribe.metadata()->msg_types_count    = 3;
    subscribe.metadata()->msg_types[0].value = 1;
    subscribe.metadata()->msg_types[1].value = 2;
    subscribe.metadata()->msg_types[2].value = 3;

    // Connect to the broker and send the message
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection
    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process
    ASSERT_FALSE(broker_wrapper.error());

    // Unsubscribe
    subscribe.metadata()->type = SubscribeMessage::ReqType::UNSUBSCRIBE;
    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process
    ASSERT_FALSE(broker_wrapper.error());

    ASSERT_TRUE(broker_wrapper.stop());
}

TEST(broker_server, publish_internal_message)
{
    auto server_socket = std::make_shared<SocketServer>(broker_uds_file, broker_listen_buffer);
    auto event_loop    = std::make_shared<EventLoopImpl>(broker_timeout);
    BrokerServerWrapper broker_wrapper(server_socket, event_loop);

    ASSERT_TRUE(broker_wrapper.start());

    // Connect to the broker
    SocketClient sock1(broker_uds_file);
    ASSERT_EQ(1, event_loop->run()); // Accept the connection
    ASSERT_FALSE(broker_wrapper.error());

    // Build a subscribe message
    SubscribeMessage subscribe;
    subscribe.metadata()->type              = SubscribeMessage::ReqType::SUBSCRIBE;
    subscribe.metadata()->msg_types_count   = 1;
    subscribe.metadata()->msg_types[0].bits = {
        .internal        = 1,
        .vendor_specific = 0,
        .reserved        = 0,
        .type            = uint32_t(Type::InterfaceConfigurationIndicationMessage),
    };

    // Subscribe to the InterfaceConfigurationIndicationMessage message
    ASSERT_TRUE(messages::send_transport_message(sock1, subscribe));
    ASSERT_EQ(1, event_loop->run()); // Process
    ASSERT_FALSE(broker_wrapper.error());

    // Publish an InterfaceConfigurationIndicationMessage message to subscribers
    constexpr int NUM_OF_IFACES = 17;
    InterfaceConfigurationIndicationMessage iface_indication_msg_tx;
    iface_indication_msg_tx.metadata()->numInterfaces = NUM_OF_IFACES;
    ASSERT_TRUE(broker_wrapper.publish(iface_indication_msg_tx));

    // Verify the data is received on the subscribed socket
    ASSERT_TRUE(size_t(sock1.getBytesReady()) > sizeof(messages::Message::Header));

    // Read, parse and validate the message
    auto iface_indication_msg_rx_ptr = messages::read_transport_message(sock1);
    ASSERT_TRUE(iface_indication_msg_rx_ptr);
    ASSERT_TRUE(iface_indication_msg_rx_ptr->type() ==
                Type::InterfaceConfigurationIndicationMessage);

    auto &iface_indication_msg_rx =
        dynamic_cast<InterfaceConfigurationIndicationMessage &>(*iface_indication_msg_rx_ptr);

    // Validate the number of interfaces matches the sent message
    ASSERT_TRUE(iface_indication_msg_rx.metadata()->numInterfaces == NUM_OF_IFACES);

    ASSERT_TRUE(broker_wrapper.stop());
}

} // namespace tests
} // namespace broker
} // namespace transport
} // namespace beerocks
