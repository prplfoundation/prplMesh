/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/interface_state_manager_impl.h>

namespace beerocks {
namespace net {

InterfaceStateManagerImpl::InterfaceStateManagerImpl(
    std::unique_ptr<InterfaceStateMonitor> interface_state_monitor,
    std::unique_ptr<InterfaceStateReader> interface_state_reader)
    : m_interface_state_monitor(std::move(interface_state_monitor)),
      m_interface_state_reader(std::move(interface_state_reader))
{
}

bool InterfaceStateManagerImpl::start()
{
    m_interface_state_monitor->set_handler([&](const std::string &iface_name, bool iface_state) {
        bool last_iface_state;
        if (get_state(iface_name, last_iface_state)) {
            if (last_iface_state == iface_state) {
                return;
            }
        }

        set_state(iface_name, iface_state);
        notify_state_changed(iface_name, iface_state);
    });

    return m_interface_state_monitor->start();
}

bool InterfaceStateManagerImpl::stop()
{
    m_interface_state_monitor->clear_handler();

    return m_interface_state_monitor->stop();
}

bool InterfaceStateManagerImpl::read_state(const std::string &iface_name, bool &iface_state)
{
    if (get_state(iface_name, iface_state)) {
        return true;
    }

    if (!m_interface_state_reader->read_state(iface_name, iface_state)) {
        return false;
    }

    set_state(iface_name, iface_state);

    return true;
}

bool InterfaceStateManagerImpl::get_state(const std::string &iface_name, bool &iface_state)
{
    const auto &it = m_interface_states.find(iface_name);
    if (m_interface_states.end() == it) {
        return false;
    }

    iface_state = it->second;

    return true;
}

void InterfaceStateManagerImpl::set_state(const std::string &iface_name, bool iface_state)
{
    m_interface_states[iface_name] = iface_state;
}

} // namespace net
} // namespace beerocks
