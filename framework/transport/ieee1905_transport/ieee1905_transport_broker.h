/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BROKER_SERVER_H
#define BROKER_SERVER_H

#include <bcl/beerocks_event_loop.h>

#include <mapf/transport/ieee1905_transport_messages.h>

#include <unordered_map>
#include <unordered_set>

namespace beerocks {
namespace transport {
namespace broker {

/**
 * @brief prplMesh internal message broker server.
 * 
 * Implements a broker server that is responsible for the internal communication
 * between the different prplMesh components (transport, controller, agent etc.)
 * 
 * The broker accepts connection over a SocketServer.
 * Once connected to the server, a client can subscribe to CMDU types.
 * Message filtering is implemented inside the server, so that clients receive only
 * the message types they subscribed to.
 */
class BrokerServer {
public:
    /**
     * @brief Transport messages (@see Message) handler function definition.
     *
     * Parameters to the event handler function are:
     * @param[in] msg Pointer to the message to handle.
     * @param[in] broker Reference to the BrokerServer instance.
     * 
     * @returns true on success or false otherwise
     */
    using MessageHandler =
        std::function<bool(std::unique_ptr<messages::Message> &msg, BrokerServer &broker)>;

    /**
     * Constructor.
     * 
     * @param [in] server_socket Server socket listening for incoming connections.
     * @param [in] Application event loop.
     */
    BrokerServer(const std::shared_ptr<SocketServer> &server_socket,
                 const std::shared_ptr<EventLoop> &event_loop);

    /**
     * Destructor.
     */
    virtual ~BrokerServer() = default;

    /**
     * @brief Start the Broker's server socket.
     */
    bool start();

    /**
     * @brief Stop the Broker's server socket.
     */
    bool stop();

    /**
     * @brief Publishes the message with the broker subscribers.
     * Sends the message to all the clients that subscribed for the type of the message.
     * 
     * @param [in] msg Reference to the message to publish.
     * 
     * @returns true on success or false otherwise.
     */
    virtual bool publish(const messages::Message &msg);

    /**
     * @brief Register a handler function for internal (non-CMDU) messages
     * 
     * The registered handler will be called for processing non-CMDU messages,
     * with the exception of the Subscribe/Unsubscribe messages that will be
     * processed internally by the broker.
     * 
     * @param [in] handler Handler function.
     */
    virtual void register_internal_message_handler(MessageHandler handler);

    /**
     * @brief Register a handler function for external (CMDU_TX/CMDU_RX) messages
     * 
     * The registered handler will be called for processing CMDU messages.
     * 
     * @param [in] handler Handler function.
     */
    virtual void register_external_message_handler(MessageHandler handler);

protected:
    /**
     * @brief Handle incoming message.
     * 
     * @param [in] sd The socket interface on which the incoming data event originated.
     * 
     * @return true on success and false otherwise.
     */
    virtual bool handle_msg(const std::shared_ptr<Socket> &sd);

private:
    /**
     * @brief Handle broker subscribe/unsubscribe messages.
     * 
     * @param [in] sd The socket interface on which the incoming data event originated.
     * @param [in] msg The internal SubscribeMessage message.
     * 
     * @return true on success and false otherwise.
     */
    bool handle_subscribe(const std::shared_ptr<Socket> &sd, const messages::SubscribeMessage &msg);

    /**
     * @brief Handler method to accept incoming socket connections.
     * 
     * @return true on success and false otherwise.
     */
    bool socket_connected();

    /**
     * @brief Handler method for socket disconnections.
     * 
     * @param [in] sd The socket interface on which the disconnection event originated.
     * 
     * @return true on success and false otherwise.
     */
    bool socket_disconnected(const std::shared_ptr<Socket> &sd);

private:
    /**
     * Shared pointer to the broker server socket.
     */
    std::shared_ptr<SocketServer> m_server_socket;

    /**
     * Application event loop to use by the broker to wait for I/O events.
     */
    std::shared_ptr<EventLoop> m_event_loop;

    /**
     * Map for storing Socket->CMDU Type subscriptions.
     */
    std::unordered_map<std::shared_ptr<Socket>, std::unordered_set<uint32_t>> m_soc_to_type;

    /**
     * Map for storing CMDU Type->Socket subscriptions.
     */
    std::unordered_map<uint32_t, std::unordered_set<std::shared_ptr<Socket>>> m_type_to_soc;

    /**
     * Handler for internal (non-CMDU) messages.
     */
    MessageHandler m_internal_message_handler = nullptr;

    /**
     * Handler for external (CMDU) messages.
     */
    MessageHandler m_external_message_handler = nullptr;
};

} // namespace broker
} // namespace transport
} // namespace beerocks

#endif // BROKER_SERVER_H
