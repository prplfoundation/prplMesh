#include "reactor/EventHandlers.h"
#include "reactor/EventLoopImpl.h"

#include "TimeTools.h"

#include <sys/timerfd.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace u::reactor;

using ::testing::AtLeast;
using ::testing::Invoke;

namespace {

TEST(EventLoopImplTest, registerHandler_ShouldSucceed)
{
    EventLoopImpl loop;
    EventHandlers handlers;

    bool ok = loop.registerHandlers(0, 0, handlers);
    loop.removeHandlers(0);

    ASSERT_TRUE(ok);
}

TEST(EventLoopImplTest, removeHandler_ShouldSucceed)
{
    EventLoopImpl loop;
    EventHandlers handlers;

    loop.registerHandlers(0, 0, handlers);
    bool ok = loop.removeHandlers(0);

    ASSERT_TRUE(ok);
}

TEST(EventLoopImplTest, removeHandler_ShouldFailIfAlreadyRemoved)
{
    EventLoopImpl loop;
    EventHandlers handlers;

    loop.registerHandlers(0, 0, handlers);
    loop.removeHandlers(0);
    bool ok = loop.removeHandlers(0);

    ASSERT_FALSE(ok);
}

TEST(EventLoopImplTest, removeHandler_ShouldFailIfNotAdded)
{
    EventLoopImpl loop;
    EventHandlers handlers;

    bool ok = loop.removeHandlers(0);

    ASSERT_FALSE(ok);
}

TEST(EventLoopImplTest, runWithNoHandler_ShouldSucceed)
{
    EventLoopImpl loop;

    bool ok = loop.run();

    ASSERT_TRUE(ok);
}

TEST(EventLoopImplTest, terminate_ShouldSucceed)
{
    EventLoopImpl loop;
    EventHandlers handlers;

    handlers.onWrite = [&](int fd, EventLoop *loop) { loop->terminate(); };

    loop.registerHandlers(1, 0, handlers);
    bool ok = loop.run();
    loop.removeHandlers(1);

    ASSERT_TRUE(ok);
}

TEST(EventLoopImplTest, timer_ShouldSucceed)
{
    EventLoopImpl loop;
    EventHandlers handlers;

    handlers.onRead = [&](int fd, EventLoop *loop) {
        loop->terminate();
        close(fd);
    };

    int fd = timerfd_create(CLOCK_MONOTONIC, 0);

    itimerspec value{{0, 0}, {0, 100 * NSEC_PER_MSEC}};

    int error = timerfd_settime(fd, 0, &value, NULL);
    ASSERT_EQ(0, error);

    loop.registerHandlers(fd, 0, handlers);
    bool ok = loop.run();
    loop.removeHandlers(fd);

    ASSERT_TRUE(ok);
}

} // namespace
