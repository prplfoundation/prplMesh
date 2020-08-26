/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_socket_event_loop.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <easylogging++.h>

// Initialize easylogging++
INITIALIZE_EASYLOGGINGPP

using namespace beerocks;

using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::StrictMock;

static constexpr std::chrono::milliseconds s_test_timeout(100);

// Define Event and Loop types for the use of the unit tests
using EventType = std::shared_ptr<Socket>;
using LoopType  = EventLoop<EventType>;

/**
 * @brief Mockable event handlers class
 */
class EventHandlersMock : public LoopType::EventHandlers {

public:
    EventHandlersMock()
    {
        on_read  = [&](EventType socket, LoopType &loop) { return handle_read(socket, &loop); };
        on_write = [&](EventType socket, LoopType &loop) { return handle_write(socket, &loop); };
        on_disconnect = [&](EventType socket, LoopType &loop) {
            return handle_disconnect(socket, &loop);
        };
        on_error = [&](EventType socket, LoopType &loop) { return handle_error(socket, &loop); };
    }

    MOCK_METHOD(bool, handle_read, ((EventType &), LoopType *));
    MOCK_METHOD(bool, handle_write, ((EventType &), LoopType *));
    MOCK_METHOD(bool, handle_disconnect, ((EventType &), LoopType *));
    MOCK_METHOD(bool, handle_error, ((EventType &), LoopType *));
};

TEST(beerocks_socket_event_loop, setup)
{
    // Configure easylogging++ formatting
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Format,
                            "%level %datetime{%H:%m:%s:%g} <%thread> %fbase[%line] --> %msg");

    // el::Loggers::addFlag(el::LoggingFlag::ColoredTerminalOutput);
    el::Loggers::reconfigureLogger("default", defaultConf);
}

TEST(beerocks_socket_event_loop, simple_read_write)
{
    SocketEventLoop loop(s_test_timeout);
    StrictMock<EventHandlersMock> reader;
    StrictMock<EventHandlersMock> writer;

    // Disable the on_write handler (to prevent the loop from firing "write ready" events)
    reader.on_write = nullptr;

    // Create a socketpair for reader/writer simulation
    int sv[2];
    int rc = socketpair(AF_UNIX, SOCK_STREAM, 0, sv);
    ASSERT_NE(-1, rc);

    auto writer_socket = std::make_shared<Socket>(sv[0]);
    auto reader_socket = std::make_shared<Socket>(sv[1]);

    ASSERT_NE(writer_socket, nullptr);
    ASSERT_NE(reader_socket, nullptr);

    {
        InSequence sequence;

        EXPECT_CALL(writer, handle_write(writer_socket, &loop))
            .WillOnce(Invoke([](EventType socket, LoopType *loop) -> bool {
                // Send a dummy byte and remove the socket from the loop to prevent additional timeouts
                EXPECT_EQ(1, send(socket->getSocketFd(), "X", 1, 0));
                EXPECT_TRUE(loop->del_event(socket));
                return true;
            }));

        EXPECT_CALL(reader, handle_read(reader_socket, &loop))
            .WillOnce(Invoke([](EventType socket, LoopType *loop) -> bool {
                char dummy;
                EXPECT_EQ(1, read(socket->getSocketFd(), &dummy, 1));
                return true;
            }));
    };

    // Add the reader/writer sockets and handlers to the loop
    ASSERT_TRUE(loop.add_event(writer_socket, writer));
    ASSERT_TRUE(loop.add_event(reader_socket, reader));

    // Run the loop
    ASSERT_EQ(1, loop.run());
    ASSERT_EQ(1, loop.run());

    // Delete the reader socket
    ASSERT_TRUE(loop.del_event(reader_socket));

    // Close the file descriptors
    close(sv[0]);
    close(sv[1]);
}
