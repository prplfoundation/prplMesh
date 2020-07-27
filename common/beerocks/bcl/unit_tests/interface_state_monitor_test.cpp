#include "bcl/interface_state_monitor_impl.h"
#include "bcl/interface_state_monitor_mock.h"
#include "bcl/network/socket_refactorings.h"

#include "bcl/event_loop_impl.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using namespace beerocks;
using namespace beerocks::sockets;

using ::testing::_;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;
using ::testing::StrictMock;

namespace {

constexpr size_t netlink_buffer_size = 8192;

/**
 * This is not actually a unit test, but an example of how to use the InterfaceStateMonitorImpl
 * class.
 */
TEST(InterfaceStateMonitorTest, InterfaceStateMonitor_ShouldSucceed)
{
    /**
     * Create NETLINK_ROUTE netlink socket for kernel/user-space communication.
     */
    auto socket = std::make_shared<NetlinkRouteSocket>();
    ClientSocketImpl<NetlinkRouteSocket> client(socket);

    /**
     * Bind to route netlink multicast group to listen for multicast packets sent from the kernel
     * containing network interface create/delete/up/down events.
     */
    NetlinkAddress address(RTMGRP_LINK);
    ASSERT_EQ(0, client.bind(address));

    /**
     * Create connection to send/receive data using this socket
     */
    auto connection = std::make_shared<ConnectionImpl>(socket);

    /**
     * Create event loop to block waiting for socket events
     */
    auto event_loop = std::make_shared<EventLoopImpl>();

    /**
     * Create the interface state monitor. Dependencies are passed in as parameters in constructor.
     */
    InterfaceStateMonitorImpl monitor(event_loop, connection);

    /**
     * Setup the handler to deal with events signaling a change in the interface state.
     */
    auto handler = [](const std::string &iface_name, bool up_and_running) {
        std::cout << "interface: " << iface_name << ", up_and_running: " << up_and_running
                  << std::endl;
    };
    monitor.set_handler(handler);

    /**
     * Setup a timer to run this example (pseudo-test) during 10s only
     */
    TimerImpl<std::chrono::seconds> timer;
    timer.schedule(std::chrono::seconds(10), std::chrono::seconds(0));

    EventHandlers handlers;
    handlers.onRead = [&](int fd, EventLoop *loop) {
        uint64_t number_of_expirations;
        if (!timer.read(number_of_expirations)) {
            std::cout << "Failed reading timer: " << strerror(errno) << std::endl;
            return;
        }

        timer.cancel();
        event_loop->terminate();
    };

    event_loop->registerHandlers(timer.fd(), -1, handlers);

    /**
     * Start the monitor before event loop runs
     */
    monitor.start();

    /**
     * Run event loop until timer expires. In the meantime, all interface state changed events
     * will be processed by the handler
     */
    ASSERT_TRUE(event_loop->run());

    /**
     * Stop the monitor to clean-up resources
     */
    monitor.stop();
}

TEST(InterfaceStateMonitorTest, InterfaceStateMonitor_ShouldSucceed_WithHandlerFunction)
{

    auto monitor = std::make_shared<InterfaceStateMonitorMock>();

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Invoke([&]() -> bool {
            monitor->handle_state_changed("eth1", true);
            return true;
        }));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    }

    class MyClass {
    public:
        explicit MyClass(const std::shared_ptr<InterfaceStateMonitor> &monitor) : m_monitor(monitor)
        {

            /**
             * Create a lambda function and install it.
             * Note that passing "this" in the capture clause we have access to all members of this
             * class (the so-called context)
             */
            auto handler = [this](const std::string &iface_name, bool up_and_running) {
                m_events_count++;
            };

            m_monitor->set_handler(handler);
        }

        ~MyClass() { m_monitor->set_handler(nullptr); }

        void work()
        {
            m_monitor->start();
            std::cout << "working ..." << std::endl;
            m_monitor->stop();
        }

        uint32_t events_count() const { return m_events_count; }

    private:
        std::shared_ptr<InterfaceStateMonitor> m_monitor;
        uint32_t m_events_count = 0;
    };

    MyClass instance(monitor);
    instance.work();

    ASSERT_EQ(1, instance.events_count());
}

TEST(InterfaceStateMonitorTest, InterfaceStateMonitor_ShouldSucceed_WithHandlerMember)
{

    auto monitor = std::make_shared<InterfaceStateMonitorMock>();

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Invoke([&]() -> bool {
            monitor->handle_state_changed("eth1", true);
            return true;
        }));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    }

    class MyClass {
    public:
        explicit MyClass(const std::shared_ptr<InterfaceStateMonitor> &monitor) : m_monitor(monitor)
        {

            /**
             * It's possible to create a wrapper std::function around a member method of this class
             * and install it.
             * According to Stephan T. Lavavej - "Avoid using bind(), ..., use lambdas".
             * https://www.youtube.com/watch?v=zt7ThwVfap0&t=32m20s
             */
            auto handler = [this](const std::string &iface_name, bool up_and_running) {
                handle_state_changed(iface_name, up_and_running);
            };

            m_monitor->set_handler(handler);
        }

        ~MyClass() { m_monitor->set_handler(nullptr); }

        void work()
        {
            m_monitor->start();
            std::cout << "working ..." << std::endl;
            m_monitor->stop();
        }

        uint32_t events_count() const { return m_events_count; }

        void handle_state_changed(const std::string &iface_name, bool up_and_running)
        {
            m_events_count++;
        };

    private:
        std::shared_ptr<InterfaceStateMonitor> m_monitor;
        uint32_t m_events_count = 0;
    };

    MyClass instance(monitor);
    instance.work();

    ASSERT_EQ(1, instance.events_count());
}

TEST(InterfaceStateMonitorTest, InterfaceStateMonitor_ShouldSucceed_WithHandlerClassToFunction)
{

    auto monitor = std::make_shared<InterfaceStateMonitorMock>();

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Invoke([&]() -> bool {
            monitor->handle_state_changed("eth1", true);
            return true;
        }));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    }

    class StateChangeHandler {
    public:
        void operator()(const std::string &iface_name, bool up_and_running) { m_events_count++; }
        uint32_t events_count() const { return m_events_count; }

    private:
        uint32_t m_events_count = 0;
    };

    class MyClass {
    public:
        explicit MyClass(const std::shared_ptr<InterfaceStateMonitor> &monitor)
            : m_monitor(monitor), m_handler(std::make_shared<StateChangeHandler>())
        {

            /**
             * It's possible to create a wrapper std::function around the StateChangeHandler
             * class and install it
             */
            auto handler = [this](const std::string &iface_name, bool up_and_running) {
                (*m_handler)(iface_name, up_and_running);
            };

            m_monitor->set_handler(handler);
        }

        ~MyClass() { m_monitor->set_handler(nullptr); }

        void work()
        {
            m_monitor->start();
            std::cout << "working ..." << std::endl;
            m_monitor->stop();
        }

        uint32_t events_count() const { return m_handler->events_count(); }

    private:
        std::shared_ptr<InterfaceStateMonitor> m_monitor;
        std::shared_ptr<StateChangeHandler> m_handler;
    };

    MyClass instance(monitor);
    instance.work();

    ASSERT_EQ(1, instance.events_count());
}

} // namespace
// namespace
