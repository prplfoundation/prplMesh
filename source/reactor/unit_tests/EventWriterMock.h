#ifndef EventWriterMock_H
#define EventWriterMock_H

#include "reactor/EventHandlers.h"

#include "gmock/gmock.h"

namespace u {
namespace reactor {
class EventLoop;

class EventWriterMock : public EventHandlers {
public:
    EventWriterMock()
    {
        onWrite   = [&](int fd, EventLoop *loop) { handleWrite(fd, loop); };
        onError   = [&](int fd, EventLoop *loop) { handleError(fd, loop); };
        onTimeout = [&](int fd, EventLoop *loop) { handleTimeout(fd, loop); };
    }

    MOCK_METHOD(void, handleWrite, (int fd, EventLoop *loop));
    MOCK_METHOD(void, handleError, (int fd, EventLoop *loop));
    MOCK_METHOD(void, handleTimeout, (int fd, EventLoop *loop));
};
} // namespace reactor
} // namespace u

#endif
