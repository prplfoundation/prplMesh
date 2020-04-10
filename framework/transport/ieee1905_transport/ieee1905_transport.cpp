/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport.h>

#include <unistd.h>

namespace mapf {

void Ieee1905Transport::run()
{
    MAPF_INFO("starting 1905 transport.");

    // init Local Bus interface, subscribe to specific topics
    local_bus_ = new LocalBusInterface(Context::Instance());
    local_bus_->Init();
    if (local_bus_->subscriber().Subscribe<CmduTxMessage>() < 0) {
        MAPF_ERR("cannot subscribe to local bus.");
        return;
    }
    if (local_bus_->subscriber().Subscribe<InterfaceConfigurationRequestMessage>() < 0) {
        MAPF_ERR("cannot subscribe to local bus.");
        return;
    }
    local_bus_->Sync();
    poller_.Add(local_bus_->subscriber());

    // init netlink socket
    if (!open_netlink_socket()) {
        MAPF_ERR("cannot open netlink socket.");
        return;
    }
    poller_.Add(netlink_fd_);

    // ---------------------------------
    // Poller Items         Type
    // ---------------------------------
    // local bus            mapf::Socket
    // netlink socket       fd
    // interface socket(s)  fd
    // ---------------------------------
    while (1) {
        MAPF_DBG("polling...");
        int nready = poller_.Poll();
        MAPF_DBG("poll returned " << nready << ".");

        // check for events on the Local Bus socket
        MAPF_DBG("check for events on local bus subscriber socket.");
        auto revents = poller_.CheckEvent(local_bus_->subscriber());
        if (revents & MAPF_POLLIN) {
            handle_local_bus_pollin_event();
        }
        if (revents & MAPF_POLLERR) {
            MAPF_ERR("poll error on local bus subscriber socket.");
            // TODO: handle this
        }

        // check for events on the netlink socket
        MAPF_DBG("check for events on netlink socket.");
        revents = poller_.CheckEvent(netlink_fd_);
        if (revents & MAPF_POLLIN) {
            MAPF_DBG("got MAPF_POLLIN event on netlink socket.");
            handle_netlink_pollin_event();
        }
        if (revents & MAPF_POLLERR) {
            MAPF_ERR("got MAPF_POLLERR event on netlink socket.");
            // TODO: handle this
        }

        // check for events on all active network interface sockets
        for (auto it = network_interfaces_.begin(); it != network_interfaces_.end(); ++it) {
            unsigned int if_index   = it->first;
            auto &network_interface = it->second;

            if (network_interface.fd >= 0 && !network_interface.is_bridge) {
                MAPF_DBG("check for events on interface " << if_index << ".");
                auto revents = poller_.CheckEvent(network_interface.fd);
                if (revents & MAPF_POLLIN) {
                    MAPF_DBG("got MAPF_POLLIN event on interface " << if_index << ".");
                    handle_interface_pollin_event(network_interface.fd);
                }
                if (revents & MAPF_POLLERR) {
                    // this could happen whenever an interface comes down
                    MAPF_DBG("got MAPF_POLLERR event on interface " << if_index
                                                                    << " (disabling it).");
                    handle_interface_status_change(if_index, false);
                }
            }
        }
    }
}

} // namespace mapf
