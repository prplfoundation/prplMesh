/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_MANAGER_IMPL_H_
#define BCL_NETWORK_INTERFACE_STATE_MANAGER_IMPL_H_

#include "interface_state_manager.h"

#include <memory>
#include <unordered_map>

namespace beerocks {
namespace net {

/**
 * This class implements the InterfaceStateManager facade interface in terms of (by delegating to)
 * the InterfaceStateMonitor and InterfaceStateReader interfaces and performs additional
 * functionality before/after forwarding requests.
 */
class InterfaceStateManagerImpl : public InterfaceStateManager {
public:
    /**
     * @brief Class constructor
     *
     * This implementation delegates InterfaceStateMonitor and InterfaceStateReader requests to
     * given reader and monitor instances respectively.
     *
     * The interface state monitor is used to monitor changes in the state of the network
     * interfaces in an event-driven way, that is, without polling (which is very CPU expensive).
     *
     * The interface state reader is used to read the state of an interface when it is not already
     * known (i.e. the first time it is queried and no state-changed event has occurred yet).
     *
     * @param interface_state_monitor Interface state monitor.
     * @param interface_state_reader Interface state reader.
     */
    InterfaceStateManagerImpl(std::unique_ptr<InterfaceStateMonitor> interface_state_monitor,
                              std::unique_ptr<InterfaceStateReader> interface_state_reader);

    /**
     * @brief Starts the interface state manager.
     *
     * Installs a state-changed event handler on the monitor and then delegates to
     * InterfaceStateMonitor::start.
     *
     * The handler function stores the interface state into the list of current states for each
     * known interface. This way, when read_state() is called, the cached state can be quickly
     * returned instead of having to query the network interface with the reader.
     *
     * @see InterfaceStateMonitor::start
     */
    bool start() override;

    /**
     * @brief Stops the interface state manager.
     *
     * Removes the state-changed event handler on the monitor and then delegates to
     * InterfaceStateMonitor::stop.
     *
     * @see InterfaceStateMonitor::stop
     */
    bool stop() override;

    /**
     * @brief Reads interface up-and-running state.
     *
     * If the interface state is known (either because a state-changed event has occurred or
     * because it has been explicitly read), the cached state is returned. Otherwise delegates to
     * InterfaceStateReader::read_state and stores obtained state.
     *
     * @see InterfaceStateReader::read_state
     */
    bool read_state(const std::string &iface_name, bool &iface_state) override;

private:
    /**
     * Interface state monitor used to monitor changes in the state of the network interfaces.
     */
    std::unique_ptr<InterfaceStateMonitor> m_interface_state_monitor;

    /**
     * Interface state reader used to read the state of an interface when it is not already known
     * (i.e. the first time it is queried and no state-changed event has been occurred yet).
     */
    std::unique_ptr<InterfaceStateReader> m_interface_state_reader;

    /**
     * Map containing the current state of each known interface.
     * The map key is the interface name and the map value is the interface state (true if
     * up-and-running and false otherwise).
     */
    std::unordered_map<std::string, bool> m_interface_states;

    /**
     * @brief Gets last known interface state.
     *
     * @param[in] iface_name Interface name.
     * @param[out] iface_state Interface state (true if it is up-and-running).
     * @return True on success and false otherwise (i.e.: interface state is not known yet).
     */
    bool get_state(const std::string &iface_name, bool &iface_state);

    /**
     * @brief Sets last known interface state.
     *
     * @param[in] iface_name Interface name.
     * @param[in] iface_state Interface state (true if it is up and running).
     */
    void set_state(const std::string &iface_name, bool iface_state);
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_MANAGER_IMPL_H_ */
