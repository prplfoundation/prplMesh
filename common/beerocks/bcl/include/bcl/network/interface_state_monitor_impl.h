/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_MONITOR_IMPL_H_
#define BCL_NETWORK_INTERFACE_STATE_MONITOR_IMPL_H_

#include "interface_state_monitor.h"
#include "sockets_impl.h"

#include <memory>

namespace beerocks {

class EventLoop;

namespace net {

class InterfaceStateMonitorImpl : public InterfaceStateMonitor {
    static constexpr size_t netlink_buffer_size = 8192;

public:
    /**
     * @brief Class constructor
     *
     * @param connection Netlink socket connection for kernel/user-space communication.
     * @param event_loop Event loop to wait for I/O events.
     */
    InterfaceStateMonitorImpl(const std::shared_ptr<Socket::Connection> &connection,
                              const std::shared_ptr<EventLoop> &event_loop);

    /**
     * @brief Starts the interface state monitor.
     *
     * @see InterfaceStateMonitor::start
     */
    bool start() override;

    /**
     * @brief Stops the interface state monitor.
     *
     * @see InterfaceStateMonitor::stop
     */
    bool stop() override;

private:
    /**
     * Buffer to hold data received through socket connection
     */
    BufferImpl<netlink_buffer_size> m_buffer;

    /**
     * Socket connection through which interface state information is received.
     */
    std::shared_ptr<Socket::Connection> m_connection;

    /**
     * Application event loop used by the monitor to wait for I/O events.
     */
    std::shared_ptr<EventLoop> m_event_loop;

    /**
     * @brief Parses data received through the Netlink socket connection.
     *
     * The array of bytes contains a list of Netlink messages.
     *
     * @param data Pointer to array of bytes to parse.
     * @param length Number of bytes to parse.
     */
    void parse(const uint8_t *data, size_t length) const;

    /**
     * @brief Parses message received through the Netlink socket connection.
     *
     * If the type of the Netlink message is RTM_NEWLINK or RTM_DELLINK then reads the interface
     * index and state and notifies a change in the interface state.
     *
     * @param msg_hdr Netlink message to parse.
     */
    void parse(const nlmsghdr *msg_hdr) const;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_MONITOR_IMPL_H_ */
