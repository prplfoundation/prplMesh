#include "reactor/EventLoopImpl.h"

#include "EventReaction.h"
#include "TimeTools.h"

#include <algorithm>

namespace u {
namespace reactor {

bool EventLoopImpl::registerHandlers(int fd, long timeout, const EventHandlers &handlers)
{
    if (m_eventReactions.find(fd) == m_eventReactions.end()) {
        m_eventReactions[fd] = std::make_shared<EventReaction>(fd, timeout, handlers);
        return true;
    }

    return false;
}

bool EventLoopImpl::removeHandlers(int fd) { return m_eventReactions.erase(fd) > 0; }

bool EventLoopImpl::run(void)
{
    bool result  = true;
    m_terminated = false;

    while (!m_terminated) {
        // File descriptor sets to select()
        fd_set read_fds;
        fd_set write_fds;
        fd_set exception_fds;

        // Initialize the file descriptor sets
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&exception_fds);

        // This is the first argument to select() and should be set to the highest-numbered file
        // descriptor in any of the three sets, plus 1.
        int max_fd = 0;

        // The timeout argument for select() is computed as the minimum time to wait (-1 if not
        // set yet) of all file descriptors
        long min_timeout = -1;

        // Get current time (time at this loop iteration)
        struct timespec current;
        clock_gettime(CLOCK_MONOTONIC, &current);

        for (const auto &entry : m_eventReactions) {
            auto reaction = entry.second;
            int fd        = reaction->getFd();

            if (fd > max_fd) {
                max_fd = fd;
            }

            // Fill the file descriptor sets only with the file descriptors for which a handler
            // function has been provided.
            EventHandlers handlers = reaction->getHandlers();
            if (handlers.onRead) {
                FD_SET(fd, &read_fds);
            }
            if (handlers.onWrite) {
                FD_SET(fd, &write_fds);
            }
            if (handlers.onError) {
                FD_SET(fd, &exception_fds);
            }

            // Compute time to wait as the minimum remaining time to wait
            if (reaction->getTimeout() >= 0) {
                long timeout = reaction->getRemainingTime(&current);
                if ((-1 == min_timeout) || (timeout < min_timeout)) {
                    min_timeout = timeout;
                }
            }
        }

        // Exit event loop if there is nothing to wait for
        if (0 == max_fd) {
            break;
        }

        // Initialize the timeout data structure with computed time to wait
        struct timeval timeout;
        timeout.tv_sec  = min_timeout / MSEC_PER_SEC;
        timeout.tv_usec = min_timeout % MSEC_PER_SEC * MSEC_PER_SEC;

        // Wait for events to occur.
        // Common events include connection acceptance events, data input and output events, and
        // timeout events.
        // The select() call indicates which handles can have operations invoked on them
        // synchronously without blocking the application process.
        // select() returns 0 if timeout, 1 if input/output available or exception, -1 if error.
        int rc = select(max_fd + 1, &read_fds, &write_fds, &exception_fds,
                        (-1 == min_timeout) ? NULL : &timeout);

        // Get current time to compute elapsed time for each handler and determine if its timeout
        // has elapsed for.
        clock_gettime(CLOCK_MONOTONIC, &current);

        // If error ...
        if (rc < 0) {
            result       = false;
            m_terminated = true;
        } else {
            for (const auto &entry : m_eventReactions) {
                auto reaction = entry.second;
                int fd        = reaction->getFd();

                EventHandlers handlers = reaction->getHandlers();

                // If timeout ...
                if (rc == 0) {
                    if (handlers.onTimeout && (reaction->getTimeout() >= 0)) {
                        if (reaction->getElapsedTime(&current) >= reaction->getTimeout()) {
                            handlers.onTimeout(fd, this);
                        }
                    }
                }
                // Ready to perform input / output operation
                else {
                    // Demultiplex service requests by synchronously calling back to the event handler
                    // associated to the event
                    if (handlers.onRead && FD_ISSET(fd, &read_fds)) {
                        handlers.onRead(fd, this);
                    } else if (handlers.onWrite && FD_ISSET(fd, &write_fds)) {
                        handlers.onWrite(fd, this);
                    } else if (handlers.onError && FD_ISSET(fd, &exception_fds)) {
                        handlers.onError(fd, this);
                    }
                }
            }
        }
    }

    return result;
}

void EventLoopImpl::terminate(void) { m_terminated = true; }
} // namespace reactor
} // namespace u
