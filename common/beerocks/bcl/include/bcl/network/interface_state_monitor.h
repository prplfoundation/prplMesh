/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_MONITOR_H_
#define BCL_NETWORK_INTERFACE_STATE_MONITOR_H_

#include <functional>
#include <string>

namespace beerocks {
namespace net {

class InterfaceStateMonitor {
public:
    /**
     * Network interface state-change handler function.
     *
     * @param iface_name Interface name.
     * @param iface_state Interface state (true if it is up and running).
     */
    using StateChangeHandler = std::function<void(const std::string &iface_name, bool iface_state)>;

    /**
     * @brief Class destructor
     */
    virtual ~InterfaceStateMonitor() = default;

    /**
     * @brief Starts the interface state monitor.
     *
     * Starts monitoring the state of all the network interfaces and calls back the installed
     * handler (if any) whenever any of the interfaces changes its state to or from the
     * up-and-running value.
     *
     * @return True on success and false otherwise.
     */
    virtual bool start() = 0;

    /**
     * @brief Stops the interface state monitor.
     *
     * @return True on success and false otherwise.
     */
    virtual bool stop() = 0;

    /**
     * @brief Sets the state-changed event handler function.
     *
     * Sets the callback function to handle network interface state changes.
     * Use nullptr to remove previously installed callback function.
     *
     * @param handler State change handler function (or nullptr).
     */
    void set_handler(const StateChangeHandler &handler) { m_handler = handler; }

    /**
     * @brief Clears previously set state-changed event handler function.
     *
     * Clears callback function previously set.
     * Behaves like set_handler(nullptr)
     */
    void clear_handler() { m_handler = nullptr; }

protected:
    /**
     * @brief Notifies a network interface state-changed event.
     *
     * @param iface_name Name of the network interface that changed state.
     * @param iface_state New state of the network interface (true means up-and-running).
     */
    void notify_state_changed(const std::string &iface_name, bool iface_state) const
    {
        if (m_handler) {
            m_handler(iface_name, iface_state);
        }
    }

private:
    /**
     * Network interface state-change handler function that is called back whenever any network
     * interface changes its state.
     */
    StateChangeHandler m_handler;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_MONITOR_H_ */
