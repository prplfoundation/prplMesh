#ifndef InterfaceStateMonitorImpl_H
#define InterfaceStateMonitorImpl_H

#include "interface_state_monitor.h"

#include "EventLoop.h"
#include "socket_refactorings.h"

#include <iostream>
#include <memory>

namespace beerocks {
namespace sockets {

constexpr size_t netlink_buffer_size = 8192;

class InterfaceStateMonitorImpl : public InterfaceStateMonitor {
public:
    InterfaceStateMonitorImpl(const std::shared_ptr<u::reactor::EventLoop> &event_loop,
                              const std::shared_ptr<Connection> &connection)
        : m_event_loop(event_loop), m_connection(connection)
    {
    }

    bool start() override
    {
        u::reactor::EventHandlers handlers;
        handlers.onRead = [&](int fd, EventLoop *loop) {
            int length = m_connection->receive(m_buffer);
            if (length > 0) {
                parse(m_buffer.data(), length);
            }
        };

        return m_event_loop->registerHandlers(m_connection->socket()->fd(), -1, handlers);
    }

    bool stop() override { return m_event_loop->removeHandlers(m_connection->socket()->fd()); }

private:
    BufferImpl2<netlink_buffer_size> m_buffer;
    std::shared_ptr<u::reactor::EventLoop> m_event_loop;
    std::shared_ptr<Connection> m_connection;

    void parse(const uint8_t *data, size_t length)
    {
        for (const struct nlmsghdr *msg_hdr     = reinterpret_cast<const struct nlmsghdr *>(data);
             NLMSG_OK(msg_hdr, length); msg_hdr = NLMSG_NEXT(msg_hdr, length)) {
            parse(msg_hdr);
        }
    }

    void parse(const struct nlmsghdr *msg_hdr)
    {
        switch (msg_hdr->nlmsg_type) {
        case RTM_NEWLINK:
        case RTM_DELLINK:
            char iface_name[IFNAMSIZ] = {0};
            const struct ifinfomsg *ifi =
                static_cast<const struct ifinfomsg *>(NLMSG_DATA(msg_hdr));

            if (0 == if_indextoname(ifi->ifi_index, iface_name)) {
                std::cout << "Failed to get interface name for index " << ifi->ifi_index
                          << std::endl;
                break;
            }

            bool up_and_running = (ifi->ifi_flags & IFF_RUNNING) && (ifi->ifi_flags & IFF_UP);

            handle_state_changed(iface_name, up_and_running);

            break;
        }
    }
};

} // namespace sockets
} // namespace beerocks

#endif
