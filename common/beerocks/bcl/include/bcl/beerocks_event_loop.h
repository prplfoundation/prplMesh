/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_EVENT_LOOP_H_
#define _BEEROCKS_EVENT_LOOP_H_

#include "beerocks_backport.h"

#include <chrono>
#include <functional>

namespace beerocks {

/**
 * @brief The event loop is a programming construct that waits for and dispatches events 
 * in a program.
 *
 * It works by making a request to some "event provider" (which generally blocks the request
 * until an event has arrived), and then it calls the relevant event handler (i.e.: dispatches
 * the event).
 *
 * The event provider used by the event loop follows the file interface, which can be selected
 * or 'polled' (the Unix system call, not actual polling). The event loop operates
 * asynchronously with the event originator.
 *
 * When the event loop forms the central control flow construct of a program, as it often does,
 * it may be termed the main loop or main event loop. This title is appropriate because such an
 * event loop is at the highest level of control within the program.
 */

template <typename E, typename T> class EventLoop {
    // Fail the build if T (timeout type) is not derived from std::chrono::duration
    static_assert(is_chrono_duration<T>::value, "T must be derived from std::chrono::duration");

public:
    /**
     * The type of the event source (e.g. file descriptor).
     */
    using EventType = E;

    /**
     * The type of the event loop.
     */
    using EventLoopType = EventLoop<E, T>;

    /**
     * The type of the timeout units for the event loop.
     */
    using TimeoutType = T;

    /**
     * @brief Event handler function definition.
     *
     * Parameters to the event handler function are:
     * @param[in] event The resource where the event was originated at.
     * @param[in] loop The event loop where the event was caught. Event handler can install new handlers, 
     * remove existing handlers and even ask for loop termination.
     * 
     * @returns True on success or false otherwise
     */
    using EventHandler = std::function<bool(EventType event, EventLoopType &loop)>;

    /**
     * Set of event handler functions, one function to handle each possible event happened.
     * Handlers are grouped into a struct to facilitate passing them as a single parameter to the install and
     * remove handlers functions of the event loop.
     * Event handlers are optional and if not set for a given event, that event will be silently
     * ignored.
     */
    struct EventHandlers {
        /**
         * Hook method that is called back by the event loop to handle read events.
         * Read events are dispatched when the socket is ready for a read operation (a read
         * operation will not block).
         * @param socket Socket the event was originated at.
         * @param loop Event loop where the event was caught on.
         */
        EventHandler on_read;

        /**
         * Hook method that is called back by the event loop to handle write events.
         * Write events are dispatched when the socket is ready for a write operation (a write
         * operation will not block).
         * @param socket Socket the event was originated at.
         * @param loop Event loop where the event was caught on.
         */
        EventHandler on_write;

        /**
         * Hook method that is called back by the event loop to handle timeout events.
         * Timeout events are dispatched when timeout expires while waiting for the socket to
         * be ready for a read or write operation.
         * @param socket Socket the event was originated at.
         * @param loop Event loop where the event was caught on.
         */
        EventHandler on_timeout;

        /**
         * Hook method that is called back by the event loop to handle disconnect events.
         * Disconnect events are dispatched when the remote socket is closed.
         * @param socket Socket the event was originated at.
         * @param loop Event loop where the event was caught on.
         */
        EventHandler on_disconnect;

        /**
         * Hook method that is called back by the event loop to handle error events.
         * Error events are dispatched when an error occurs while waiting for the socket to
         * be ready for a read or write operation.
         * @param socket Socket the event was originated at.
         * @param loop Event loop where the event was caught on.
         */
        EventHandler on_error;
    };

    /**
     * Default destructor.
     */
    virtual ~EventLoop() = default;

    /**
     * @brief Registers a set of event handlers for the given event source (e.g. socket).
     *
     * Event handler for the event that occurred will be called back when the event source is
     * ready for a read/write operation, when a disconnect/error occurs or when given timeout expires.
     *
     * @param event Event source object.
     * @param handlers Set of event handlers: class with the methods to be called back when an
     * event occurs.
     * @param timeout Time to wait in milliseconds (-1 to wait indefinitely) for a read/write
     * event to occur.
     * @return True on success and false otherwise.
     */
    virtual bool add_event(EventType event, EventHandlers handlers,
                           TimeoutType timeout = TimeoutType::min()) = 0;

    /**
     * @brief Removes previously registered event handlers for the given event source.
     *
     * @param event Event source object.
     * @return True on success and false otherwise.
     */
    virtual bool del_event(EventType socket) = 0;

    /**
     * @brief Runs message loop.
     *
     * Method returns when loop is explicitly terminated, when an error or timeout occurs 
     * or when there is no event to wait for (i.e.: no handler installed).
     *
     * @return -1 on critical errors
     * @return 0 on timeout without any socket events
     * @return >0 number of socket events processed during the class to this method.
     */
    virtual int run() = 0;

    /**
     * @brief Terminates a running event loop.
     *
     * This method asks for loop termination and returns immediately. It is not thread-safe and
     * should only be called from an event handler (which runs on the same thread as the event
     * loop).
     */
    virtual void die() = 0;
};

} // namespace beerocks

#endif // _BEEROCKS_EVENT_LOOP_H_
