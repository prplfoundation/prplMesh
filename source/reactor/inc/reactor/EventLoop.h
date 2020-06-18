#ifndef Reactor_EventLoop_H
#define Reactor_EventLoop_H

namespace u {
namespace reactor {
class EventHandlers;

/**
 * The event loop is a programming construct that waits for and dispatches events in a program.
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
class EventLoop {
public:
    virtual ~EventLoop() = default;

    /**
     * @brief Registers a set of event handlers for the given file descriptor.
     *
     * Event handler for the event that occurred will be called back when the file descriptor is
     * ready for a read/write O operation, when an error occurs or when given timeout expires.
     *
     * @param fd File descriptor. Handle that identifies a resource managed within an OS:
     * network connections, open files, timers, synchronization objects, etc.
     * @param timeout Time to wait in milliseconds (-1 to wait indefinitely) for a read/write
     * event to occur.
     * @param handlers Set of event handlers: class with the methods to be called back when an
     * event occurs.
     * @return True on success and false otherwise.
     */
    virtual bool registerHandlers(int fd, long timeout, const EventHandlers &handlers) = 0;

    /**
     * @brief Removes previously registered event handlers for the given file descriptor.
     *
     * @param fd File descriptor.
     * @return True on success and false otherwise.
     */
    virtual bool removeHandlers(int fd) = 0;

    /**
     * @brief Runs message loop.
     *
     * Method returns when loop is explicitly terminated, when an error occurs or when there is
     * no event to wait for (i.e.: no handler installed).
     *
     * @return True on success and false otherwise.
     */
    virtual bool run(void) = 0;

    /**
     * @brief Terminates a running event loop.
     *
     * This method asks for loop termination and returns immediately. It is not thread-safe and
     * should only be called from an event handler (which runs on the same thread as the event
     * loop).
     */
    virtual void terminate(void) = 0;
};
} // namespace reactor
} // namespace u

#endif
