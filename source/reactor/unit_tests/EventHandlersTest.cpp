#include <sys/socket.h>

#include "EventReaderMock.h"
#include "EventWriterMock.h"
#include "reactor/EventLoopImpl.h"

#include "gtest/gtest.h"

using namespace u;
using namespace u::reactor;

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::StrictMock;

constexpr long test_timeout = 100;

namespace {

TEST(EventHandlersTest, read_write_ShouldSucceed)
{
    EventLoopImpl loop;
    StrictMock<EventReaderMock> reader;
    StrictMock<EventWriterMock> writer;

    int sv[2];
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    ASSERT_NE(-1, rc);

    {
        InSequence sequence;

        EXPECT_CALL(writer, handleWrite(sv[0], &loop)).WillOnce(Invoke([](int fd, EventLoop *loop) {
            send(fd, "b", 1, 0);

            loop->removeHandlers(fd);
            close(fd);
        }));

        EXPECT_CALL(reader, handleRead(sv[1], &loop)).WillOnce(Invoke([](int fd, EventLoop *loop) {
            loop->removeHandlers(fd);
            close(fd);
        }));
    };

    loop.registerHandlers(sv[0], test_timeout, writer);
    loop.registerHandlers(sv[1], test_timeout, reader);

    bool ok = loop.run();

    ASSERT_TRUE(ok);
}

TEST(EventHandlersTest, timeout_ShouldSucceed)
{
    EventLoopImpl loop;
    StrictMock<EventReaderMock> reader;
    StrictMock<EventWriterMock> writer;

    int sv[2];
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    ASSERT_NE(-1, rc);

    {
        InSequence sequence;

        EXPECT_CALL(writer, handleWrite(sv[0], &loop)).WillOnce(Invoke([](int fd, EventLoop *loop) {
            // Do not write anything so read timeout expires
            // Do not close socket so read handler is not invoked either

            loop->removeHandlers(fd);
        }));

        EXPECT_CALL(reader, handleTimeout(sv[1], &loop))
            .WillOnce(Invoke([](int fd, EventLoop *loop) {
                loop->removeHandlers(fd);
                close(fd);
            }));
    };

    loop.registerHandlers(sv[0], test_timeout, writer);
    loop.registerHandlers(sv[1], test_timeout, reader);

    bool ok = loop.run();

    close(sv[0]);

    ASSERT_TRUE(ok);
}

} // namespace
// namespace
