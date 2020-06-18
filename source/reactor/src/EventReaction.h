#ifndef Reactor_EventReaction_H
#define Reactor_EventReaction_H

#include "reactor/EventHandlers.h"

#include <time.h>

namespace u {
namespace reactor {

class EventReaction {
public:
    /**
     * Class constructor.
     *
     * @param fd File descriptor.
     * @param timeout Timeout value (in milliseconds).
     * @param handlers Set of event handlers.
     */
    EventReaction(int fd, long timeout, const EventHandlers &handlers);

    /**
     * Returns file descriptor.
     * @return File descriptor.
     */
    int getFd(void) const;

    /**
     * Gets timeout (in milliseconds).
     * @return Timeout in milliseconds.
     */
    long getTimeout(void) const;

    /**
     * Gets elapsed time since start (in milliseconds).
     * @param current Reference time or current time if not specified.
     * @return Elapsed time in milliseconds.
     */
    long getElapsedTime(struct timespec *current = 0) const;

    /**
     * Gets remaining time since start (in milliseconds).
     * @param current Reference time or current time if not specified.
     * @return Remaining time in milliseconds.
     */
    long getRemainingTime(struct timespec *current = 0) const;

    /**
     * Sets start time to count down from.
     * @param current Start time or current time if not specified.
     */
    void setStartTime(struct timespec *current = nullptr);

    /**
     * Gets event handlers.
     * @return Event handlers.
     */
    EventHandlers getHandlers(void) const;

protected:
    /**
     * File descriptor.
     * Handle that identifies a resource managed within an OS: network connections, open files,
     * timers, synchronization objects, etc.
     */
    int m_fd;

    /**
     * Timeout value (in milliseconds) to wait for a read/write event to occur.
     */
    long m_timeout;

    /**
     * Set of event handlers: class with the methods to be called back when an event occurs.
     */
    EventHandlers m_handlers;

    /**
     * Start time.
     */
    struct timespec m_start;
};
} // namespace reactor
} // namespace u

#endif
