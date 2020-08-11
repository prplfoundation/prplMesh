/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_event_loop_impl.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <sys/socket.h>

#include <easylogging++.h>

// Initialize easylogging++
INITIALIZE_EASYLOGGINGPP

using namespace beerocks;

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::StrictMock;

/**
 * @brief Mockable event handlers class
 */
class EventHandlersMock : public EventLoop::EventHandlers {

public:
    EventHandlersMock()
    {
        on_read       = [&](int fd, EventLoop &loop) { return handle_read(fd, &loop); };
        on_write      = [&](int fd, EventLoop &loop) { return handle_write(fd, &loop); };
        on_disconnect = [&](int fd, EventLoop &loop) { return handle_disconnect(fd, &loop); };
        on_error      = [&](int fd, EventLoop &loop) { return handle_error(fd, &loop); };
    }

    MOCK_METHOD(bool, handle_read, (int fd, EventLoop *));
    MOCK_METHOD(bool, handle_write, (int fd, EventLoop *));
    MOCK_METHOD(bool, handle_disconnect, (int fd, EventLoop *));
    MOCK_METHOD(bool, handle_error, (int fd, EventLoop *));
};

TEST(beerocks_event_loop_impl, setup)
{
    // Configure easylogging++ formatting
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Format,
                            "%level %datetime{%H:%m:%s:%g} <%thread> %fbase[%line] --> %msg");

    // el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::reconfigureLogger("default", defaultConf);
}

TEST(beerocks_event_loop_impl, simple_read_write)
{
    EventLoopImpl loop;
    StrictMock<EventHandlersMock> reader;
    StrictMock<EventHandlersMock> writer;

    // Disable the on_write handler (to prevent the loop from firing "write ready" events)
    reader.on_write = nullptr;

    // Create a socketpair for reader/writer simulation
    int sv[2];
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    ASSERT_NE(-1, rc);

    int writer_fd = sv[0];
    int reader_fd = sv[1];

    {
        InSequence sequence;

        EXPECT_CALL(writer, handle_write(writer_fd, &loop))
            .WillOnce(Invoke([](int fd, EventLoop *loop) -> bool {
                // Send a dummy byte and remove the socket from the loop to prevent additional events
                EXPECT_EQ(1, send(fd, "X", 1, 0));
                EXPECT_TRUE(loop->remove_handlers(fd));
                return true;
            }));

        EXPECT_CALL(reader, handle_read(reader_fd, &loop))
            .WillOnce(Invoke([](int fd, EventLoop *loop) -> bool {
                char dummy;
                EXPECT_EQ(1, read(fd, &dummy, 1));
                return true;
            }));
    };

    // Add the reader/writer sockets and handlers to the loop
    ASSERT_TRUE(loop.register_handlers(writer_fd, writer));
    ASSERT_TRUE(loop.register_handlers(reader_fd, reader));

    // Run the loop
    ASSERT_EQ(1, loop.run());
    ASSERT_EQ(1, loop.run());

    // Delete the reader socket
    ASSERT_TRUE(loop.remove_handlers(reader_fd));

    // Close the file descriptors
    close(writer_fd);
    close(reader_fd);
}
