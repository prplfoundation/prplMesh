/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_EVENT_LOOP_IMPL_H_
#define _BEEROCKS_EVENT_LOOP_IMPL_H_

#include "beerocks_event_loop.h"

#include <chrono>
#include <unordered_map>

namespace beerocks {

/**
 * @brief ePoll based implementation of the EventLoop interface.
 * @see EventLoop
 * 
 * This class uses the Linux epoll APIs for monitoring the provided file descriptors for I/O
 * operations.
 */
class EventLoopImpl : public EventLoop {
public:
    /**
     * @brief Class constructor.
     * 
     * Initializes an epoll file descriptor.
     * 
     * @param [in] timeout Sets the master timeout (in milliseconds) for the event loop.
     */
    explicit EventLoopImpl(std::chrono::milliseconds timeout = std::chrono::milliseconds::min());

    /**
     * @brief Class destructor.
     */
    ~EventLoopImpl() override;

    /**
     * @see EventLoop::register_handlers
     */
    bool register_handlers(int fd, const EventHandlers &handlers) override;

    /**
     * @see EventLoop::remove_handlers
     */
    bool remove_handlers(int fd) override;

    /**
     * @brief Main event loop method.
     * @see EventLoop::run
     * 
     * Executes the epoll_wait() function and processes occurred events.
     */
    int run() override;

private:
    /**
     * epoll file descriptor.
     */
    int m_epoll_fd = -1;

    /**
     * Event loop master timeout (used for the epoll_wait function).
     */
    std::chrono::milliseconds m_timeout = std::chrono::milliseconds::min();

    /**
     * Map of registered event handlers.
     * Key value is the file descriptor and value is the EventHandlers structure containing the
     * event handlers to deall with events occurred on that file descriptor.
     */
    std::unordered_map<int, EventHandlers> m_fd_to_event_handlers;
};

} // namespace beerocks

#endif // _BEEROCKS_EVENT_LOOP_IMPL_H_
