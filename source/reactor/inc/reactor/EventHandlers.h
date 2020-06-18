#ifndef Reactor_EventHandlers_H
#define Reactor_EventHandlers_H

#include <functional>

namespace u {
namespace reactor {
class EventLoop;

/**
 * @brief Event handler function definition.
 *
 * Parameters to the event handler function are:
 * - The file descriptor of the OS resource where the event was originated at. Event handler can
 * perform, for example, accept, read, write and close operations on the file descriptor.
 * - The event loop where it was caught. Event handler can install new handlers, remove existing
 * handlers and even ask for loop termination.
 */
using EventHandler = std::function<void(int, EventLoop *)>;

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
     * Read events are dispatched when the file descriptor is ready for a read operation (a read
     * operation will not block).
     * @param fd File descriptor the event was originated at.
     * @param loop Event loop where the event was caught on.
     */
    EventHandler onRead = nullptr;

    /**
     * Hook method that is called back by the event loop to handle write events.
     * Write events are dispatched when the file descriptor is ready for a write operation (a write
     * operation will not block).
     * @param fd File descriptor the event was originated at.
     * @param loop Event loop where the event was caught on.
     */
    EventHandler onWrite = nullptr;

    /**
     * Hook method that is called back by the event loop to handle error events.
     * Error events are dispatched when an error occurs while waiting for the file descriptor to
     * be ready for a read or write operation.
     * @param fd File descriptor the event was originated at.
     * @param loop Event loop where the event was caught on.
     */
    EventHandler onError = nullptr;

    /**
     * Hook method that is called back by the event loop to handle timeout events.
     * Timeout events are dispatched when timeout expires while waiting for the file descriptor to
     * be ready for a read or write operation.
     * @param fd File descriptor the event was originated at.
     * @param loop Event loop where the event was caught on.
     */
    EventHandler onTimeout = nullptr;
};
} // namespace reactor
} // namespace u

#endif
