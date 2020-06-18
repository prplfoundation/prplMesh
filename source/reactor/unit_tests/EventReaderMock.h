#ifndef EventReaderMock_H
#define EventReaderMock_H

#include "reactor/EventHandlers.h"

#include "gmock/gmock.h"

namespace u {
namespace reactor {
class EventLoop;

class EventReaderMock : public EventHandlers {

public:
    EventReaderMock()
    {
        onRead    = [&](int fd, EventLoop *loop) { handleRead(fd, loop); };
        onError   = [&](int fd, EventLoop *loop) { handleError(fd, loop); };
        onTimeout = [&](int fd, EventLoop *loop) { handleTimeout(fd, loop); };
    }

    MOCK_METHOD(void, handleRead, (int fd, EventLoop *loop));
    MOCK_METHOD(void, handleError, (int fd, EventLoop *loop));
    MOCK_METHOD(void, handleTimeout, (int fd, EventLoop *loop));
};
} // namespace reactor
} // namespace u

#endif
