/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_event_loop.h>
#include <bcl/network/interface_state_monitor_impl.h>
#include <bcl/network/interface_state_reader.h>

#include <net/if.h>

using namespace beerocks;

namespace beerocks {
namespace net {

InterfaceStateMonitorImpl::InterfaceStateMonitorImpl(
    const std::shared_ptr<Socket::Connection> &connection,
    const std::shared_ptr<EventLoop> &event_loop)
    : m_connection(connection), m_event_loop(event_loop)
{
}

bool InterfaceStateMonitorImpl::start()
{
    EventLoop::EventHandlers handlers;
    handlers.on_read = [&](int fd, EventLoop &loop) -> bool {
        int length = m_connection->receive(m_buffer);
        if (length > 0) {
            parse(m_buffer.data(), length);
        }

        return true;
    };

    return m_event_loop->register_handlers(m_connection->socket()->fd(), handlers);
}

bool InterfaceStateMonitorImpl::stop()
{
    return m_event_loop->remove_handlers(m_connection->socket()->fd());
}

void InterfaceStateMonitorImpl::parse(const uint8_t *data, size_t length) const
{
    for (const nlmsghdr *msg_hdr            = reinterpret_cast<const nlmsghdr *>(data);
         NLMSG_OK(msg_hdr, length); msg_hdr = NLMSG_NEXT(msg_hdr, length)) {
        parse(msg_hdr);
    }
}

void InterfaceStateMonitorImpl::parse(const nlmsghdr *msg_hdr) const
{
    switch (msg_hdr->nlmsg_type) {
    case RTM_NEWLINK:
    case RTM_DELLINK:
        const ifinfomsg *ifi = static_cast<const ifinfomsg *>(NLMSG_DATA(msg_hdr));

        uint32_t iface_index = ifi->ifi_index;
        bool iface_state     = (ifi->ifi_flags & IFF_UP) && (ifi->ifi_flags & IFF_RUNNING);

        char iface_name[IFNAMSIZ]{};
        if (0 != if_indextoname(iface_index, iface_name)) {
            notify_state_changed(iface_name, iface_state);
        }

        break;
    }
}

} // namespace net
} // namespace beerocks
