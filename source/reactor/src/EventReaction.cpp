#include "EventReaction.h"
#include "TimeTools.h"

namespace u {
namespace reactor {
EventReaction::EventReaction(int fd, long timeout, const EventHandlers &handlers)
{
    m_fd       = fd;
    m_timeout  = timeout;
    m_handlers = handlers;

    setStartTime();
}

int EventReaction::getFd(void) const { return m_fd; }

long EventReaction::getTimeout(void) const { return m_timeout; }

long EventReaction::getElapsedTime(struct timespec *current) const
{
    struct timespec end;

    if (current) {
        end = *current;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &end);
    }

    return TimeTools::getElapsedTime(&m_start, &end);
}

long EventReaction::getRemainingTime(struct timespec *current) const
{
    long remaining = 0;

    long elapsed = getElapsedTime(current);
    if (m_timeout > elapsed) {
        remaining = m_timeout - elapsed;
    }

    return remaining;
}

void EventReaction::setStartTime(struct timespec *current)
{
    if (current) {
        m_start = *current;
    } else {
        clock_gettime(CLOCK_MONOTONIC, &m_start);
    }
}

EventHandlers EventReaction::getHandlers(void) const { return m_handlers; }
} // namespace reactor
} // namespace u
