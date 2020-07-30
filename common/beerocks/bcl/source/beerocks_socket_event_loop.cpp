/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_socket_event_loop.h>

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <easylogging++.h>

namespace beerocks {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local module definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Maximal number of events to process in a single epoll_wait call
static constexpr int MAX_POLL_EVENTS = 17;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

SocketEventLoop::SocketEventLoop(TimeoutType timeout) : m_timeout(timeout)
{
    m_epoll_fd = epoll_create1(0);
    LOG_IF(m_epoll_fd == -1, FATAL) << "Failed creating epoll: " << strerror(errno);
}

SocketEventLoop::~SocketEventLoop()
{
    // Delete all the sockets in the poll
    LOG(DEBUG) << "Removing " << m_fd_to_event_data.size()
               << " FDs from the event loop: " << m_fd_to_event_data;

    while (!m_fd_to_event_data.empty()) {
        auto event_data = m_fd_to_event_data.begin()->second;
        del_event(event_data->socket);
    }

    // Close the poll fd
    LOG_IF(close(m_epoll_fd) == -1, ERROR)
        << "Failed closing epoll file descriptor: " << strerror(errno);
}

bool SocketEventLoop::add_event(SocketEventLoop::EventType socket,
                                EventLoop::EventHandlers handlers, TimeoutType timeout)
{
    if (!socket) {
        LOG(ERROR) << "Invalid socket pointer!";
        return false;
    }

    // Make sure that the FD is not already part of the poll
    if (m_fd_to_event_data.find(socket->getSocketFd()) != m_fd_to_event_data.end()) {
        LOG(WARNING) << "Requested to add FD (" << socket->getSocketFd()
                     << ") to the poll, but it's already there...";

        return false;
    }

    // Build EventData structure
    auto event_data      = std::make_shared<EventData>();
    event_data->socket   = socket;
    event_data->handlers = handlers;

    // If timeout is set and a handler is defined, initialize a timerfd
    if (timeout > TimeoutType::zero()) {
        if (handlers.on_timeout) {

            // TODO: timerfd may not be supported on all kernels.
            //       Implement a check at creation, and in case it's not supported
            //       base the timeouts mechanism on the main timeout of the loop.

            // Create a new timerfd file descriptor and initialize the timeout values
            event_data->timeout_value = timeout;
            event_data->timerfd       = timerfd_create(CLOCK_MONOTONIC, 0);

            if (event_data->timerfd == -1) {
                LOG(ERROR) << "Failed creating timerfd: " << strerror(errno);
                return false;
            }

            // Convert the timeout into seconds and nano-seconds
            auto timeout_sec = std::chrono::duration_cast<std::chrono::seconds>(timeout);
            auto timeout_nanosec =
                std::chrono::duration_cast<std::chrono::nanoseconds>(timeout - timeout_sec);

            timespec timeout_spec({.tv_sec  = static_cast<time_t>(timeout_sec.count()),
                                   .tv_nsec = static_cast<long>(timeout_nanosec.count())});
            itimerspec timer_val({.it_interval = timeout_spec, .it_value = timeout_spec});

            // Set the timerfd timeout value
            if (timerfd_settime(event_data->timerfd, 0, &timer_val, nullptr) == -1) {
                LOG(ERROR) << "Failed setting timerfd value: " << strerror(errno);
                return false;
            }
        } else {
            LOG(WARNING) << "Timeout was requested for '" << timeout.count()
                         << "' milliseconds, but not handler is provided. Ignoring.";
        }
    }

    // Helper lambda function for adding a fd to the poll, and register for the following events:
    // EPOLLIN: The associated fd is available for read operations.
    // EPOLLOUT: The associated fd is available for write operations.
    // EPOLLRDHUP: Socket peer closed connection, or shut down writing half of connection.
    // EPOLLERR: Error condition happened on the associated fd.
    // EPOLLHUP: Hang up happened on the associated fd.
    auto add_fd_to_epoll = [&](int fd) -> bool {
        epoll_event event = {};
        event.data.fd     = fd;
        event.events      = EPOLLRDHUP | EPOLLERR | EPOLLHUP;

        // If read or timeout handlers were set, also listen for POLL-IN events
        if (event_data->handlers.on_read || event_data->handlers.on_timeout) {
            event.events |= EPOLLIN;
        }

        // If write handler was set, also listen for POLL-OUT events
        if (event_data->handlers.on_write) {
            event.events |= EPOLLOUT;
        }

        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event) == -1) {
            LOG(ERROR) << "Failed adding FD (" << fd << ") to the poll: " << strerror(errno);
            return false;
        }

        // Map the fd to the event data structure
        m_fd_to_event_data[fd] = event_data;

        return true;
    };

    // Add the socket fd to the poll
    if (!add_fd_to_epoll(event_data->socket->getSocketFd())) {
        return false;
    }

    // Add the timeout fd to the poll
    if (event_data->timerfd != -1) {
        if (!add_fd_to_epoll(event_data->timerfd)) {
            // Remove the socket from the poll and fail
            del_event(socket);
            return false;
        }
    }

    return true;
}

bool SocketEventLoop::del_event(SocketEventLoop::EventType socket)
{
    if (!socket) {
        LOG(ERROR) << "Invalid socket pointer!";
        return false;
    }

    // Make sure that the fd was previously added to the poll
    auto event_data_itr = m_fd_to_event_data.find(socket->getSocketFd());
    if (event_data_itr == m_fd_to_event_data.end()) {
        LOG(WARNING) << "Requested to delete FD (" << socket->getSocketFd()
                     << ") from the poll, but it wasn't previously added.";

        return false;
    }

    // Store a copy of the shared_ptr to prevent loosing the reference
    // when removing the instance from the map
    auto event_data = event_data_itr->second;

    // Delete the socket fd from the poll
    auto error = false;
    if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, event_data->socket->getSocketFd(), nullptr) == -1) {
        LOG(ERROR) << "Failed deleting socket FD (" << event_data->socket->getSocketFd()
                   << ") from the poll: " << strerror(errno);

        error = true;
    }

    // Erase the fd from the map
    m_fd_to_event_data.erase(event_data->socket->getSocketFd());

    // Delete the timeout fd from the poll
    if (event_data->timerfd != -1) {
        if (epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, event_data->timerfd, nullptr) == -1) {
            LOG(ERROR) << "Failed deleting timeout FD (" << event_data->timerfd
                       << ") from the poll: " << strerror(errno);

            error = true;
        }

        // Remove the fd from the map. Since timeout sockets are created
        // automatically by the event pool, also close the fd.
        m_fd_to_event_data.erase(event_data->timerfd);
        close(event_data->timerfd);
    }

    return !error;
}

int SocketEventLoop::run()
{
    // Poll events
    epoll_event events[MAX_POLL_EVENTS] = {0};

    // Convert the global event loop timeout (if set) to milliseconds
    int timeout_millis =
        (m_timeout > TimeoutType::zero())
            ? static_cast<int>(
                  std::chrono::duration_cast<std::chrono::milliseconds>(m_timeout).count())
            : -1;

    // Poll the sockets
    auto num_events = epoll_wait(m_epoll_fd, events, sizeof(events), timeout_millis);

    if (num_events == -1) {
        LOG(ERROR) << "Error during epoll_wait: " << strerror(errno);
        return -1;
    } else if (num_events == 0) {
        // Timeout... Do nothing
        return 0;
    }

    // Trigger event handlers
    for (int i = 0; i < num_events; i++) {
        int fd              = events[i].data.fd;
        auto event_data_itr = m_fd_to_event_data.find(fd);

        if (event_data_itr == m_fd_to_event_data.end()) {
            LOG(ERROR) << "Event on unknown FD: " << fd;
            continue;
        }

        // Store a copy of the shared_ptr to prevent loosing the reference
        // when removing the instance from the map
        auto event_data = event_data_itr->second;
        auto &socket    = event_data->socket;

        // Handle errors
        if (events[i].events & EPOLLERR) {

            // Read the error from the socket
            int error        = 0;
            socklen_t errlen = sizeof(error);
            getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&error, &errlen);
            LOG(ERROR) << "Error on FD (" << fd << "): " << strerror(error);

            // Remove the socket from the poll
            del_event(socket);

            // Call the on_error handler of this socket
            if (event_data->handlers.on_error) {
                if (!event_data->handlers.on_error(socket, *this)) {
                    return -1;
                }
            }

            // Handle Disconnected Sockets
        } else if ((events[i].events & EPOLLRDHUP) || (events[i].events & EPOLLHUP)) {
            LOG(DEBUG) << "Socket with FD (" << fd << ") disconnected";

            // Remove the socket from the poll
            del_event(socket);

            // Call the on_disconnect handler of this socket
            if (event_data->handlers.on_disconnect) {
                if (!event_data->handlers.on_disconnect(socket, *this)) {
                    return -1;
                }
            }

            // Handle Data & Timeouts
        } else if (events[i].events & EPOLLIN) {

            // Handle incoming data
            if (event_data->socket->getSocketFd() == fd) {
                if (!event_data->handlers.on_read) {
                    LOG(WARNING) << "Incoming data on socket FD (" << fd
                                 << ") without handler. Removing.";
                    del_event(socket);
                } else {
                    if (!event_data->handlers.on_read(socket, *this)) {
                        return -1;
                    }
                }
                // Handle timeouts
            } else if (event_data->timerfd == fd) {
                if (!event_data->handlers.on_timeout) {
                    // This shouldn't happen as the timeout socket is created only if
                    // a handler was provided... If somehow if does happen, remove the
                    // associated socket and the timeout socket.
                    LOG(ERROR) << "Event on timeout FD (" << fd << ") without handler. Removing.";
                    del_event(socket);
                } else {
                    // Read the number of expirations occurred on this timerfd
                    uint64_t num_exp;
                    if (read(event_data->timerfd, &num_exp, sizeof(num_exp)) != sizeof(num_exp)) {
                        LOG(ERROR)
                            << "Failed reading timerfd number of expirations: " << strerror(errno);
                        return -1;
                    }

                    // If a timer has expired more than once, it means that there is some
                    // delay in processing events, so print a warning
                    if (num_exp > 1) {
                        LOG(WARNING) << "Timer on FD (" << fd << ") expired " << num_exp
                                     << " times since it was previously handled";
                    }

                    if (!event_data->handlers.on_timeout(socket, *this)) {
                        return -1;
                    }
                }
            }

            // Handle socket is ready for write operations
        } else if (events[i].events & EPOLLOUT) {
            if (!event_data->handlers.on_write) {
                LOG(WARNING) << "Socket FD (" << fd
                             << ") is ready for write, but there's no handler. Removing.";
                del_event(socket);
            } else {
                if (!event_data->handlers.on_write(socket, *this)) {
                    return -1;
                }
            }

        } else {
            LOG(ERROR) << "FD (" << fd << ") generated unknown event: " << events[i].events;
        }
    }

    return num_events;
}

} // namespace beerocks
