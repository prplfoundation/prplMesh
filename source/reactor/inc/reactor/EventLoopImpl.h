#ifndef Reactor_EventLoopImpl_H
#define Reactor_EventLoopImpl_H

#include "EventLoop.h"

#include <map>
#include <memory>

namespace u {
namespace reactor {
class EventReaction;

class EventLoopImpl : public EventLoop {
public:

    /**
     * @see EventLoop::registerHandlers
     */
    bool registerHandlers(int fd, long timeout, const EventHandlers &handlers) override;

    /**
     * @see EventLoop::removeHandlers
     */
    bool removeHandlers(int fd) override;

    /**
     * @see EventLoop::run
     */
    bool run(void) override;

    /**
     * @see EventLoop::terminate
     */
    void terminate(void) override;

protected:
    /**
     * Map of registered event reactions.
     * Map key is the file descriptor and the value is the event reaction, a structure made of a
     * file descriptor, a timeout for read/write events and the set of event handlers.
     */
    std::map<int, std::shared_ptr<EventReaction>> m_eventReactions;

    /**
     * Flag to signal that event loop must be terminated
     */
    bool m_terminated = false;
};
} // namespace reactor
} // namespace u

#endif
