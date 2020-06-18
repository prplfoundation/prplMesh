#ifndef EventLoopMock_H
#define EventLoopMock_H

#include "gmock/gmock.h"
#include "reactor/EventLoop.h"

namespace u {
namespace reactor {
class EventLoopMock : public EventLoop {
public:
    MOCK_METHOD(bool, registerHandlers, (int fd, long timeout, const EventHandlers &handlers),
                override);
    MOCK_METHOD(bool, removeHandlers, (int fd), override);
    MOCK_METHOD(bool, run, (void), override);
    MOCK_METHOD(void, terminate, (void), override);
};
} // namespace reactor
} // namespace u

#endif
