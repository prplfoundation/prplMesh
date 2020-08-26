/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/network/interface_state_manager_impl.h>
#include <bcl/network/interface_state_monitor_mock.h>
#include <bcl/network/interface_state_reader_mock.h>

#include <bcl/beerocks_backport.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Invoke;
using ::testing::Return;

TEST(interface_state_manager_impl, start_stop_should_succeed)
{
    auto monitor = std::make_unique<beerocks::net::InterfaceStateMonitorMock>();
    auto reader  = std::make_unique<beerocks::net::InterfaceStateReaderMock>();

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Return(true));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    };

    beerocks::net::InterfaceStateManagerImpl interface_state_manager(std::move(monitor),
                                                                     std::move(reader));

    ASSERT_TRUE(interface_state_manager.start());
    ASSERT_TRUE(interface_state_manager.stop());
}

/**
 * In this test, no state-changed event has occurred yet so interface state is obtained with an
 * explicit read through the interface state reader.
 */
TEST(interface_state_manager_impl, read_state_should_succeed_before_event)
{
    auto monitor = std::make_unique<beerocks::net::InterfaceStateMonitorMock>();
    auto reader  = std::make_unique<beerocks::net::InterfaceStateReaderMock>();

    const char *iface_name          = "eth0";
    const bool expected_iface_state = true;
    bool actual_iface_state         = false;

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Return(true));
        EXPECT_CALL(*reader, read_state(iface_name, _))
            .WillOnce(Invoke([&](const std::string &iface_name, bool &iface_state) -> bool {
                iface_state = expected_iface_state;
                return true;
            }));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    };

    beerocks::net::InterfaceStateManagerImpl interface_state_manager(std::move(monitor),
                                                                     std::move(reader));

    ASSERT_TRUE(interface_state_manager.start());
    ASSERT_TRUE(interface_state_manager.read_state(iface_name, actual_iface_state));
    ASSERT_EQ(actual_iface_state, expected_iface_state);
    ASSERT_TRUE(interface_state_manager.stop());
}

/**
 * In this test, the interface state is obtained after a state-changed event (no explicit read
 * operation is required nor performed)
 */
TEST(interface_state_manager_impl, read_state_should_succeed_after_event)
{
    auto monitor = std::make_unique<beerocks::net::InterfaceStateMonitorMock>();
    auto reader  = std::make_unique<beerocks::net::InterfaceStateReaderMock>();

    const char *iface_name          = "eth0";
    const bool expected_iface_state = true;
    bool actual_iface_state         = false;

    // The monitor mock is used in the expectation to emulate that a state-changed event has
    // occurred.
    // Since the unique_ptr to the monitor mock is moved into the interface state manager, it
    // is not available inside the expectation. To overcome this problem, we use the raw pointer
    // instead.
    auto monitor_raw_ptr = monitor.get();

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Invoke([&]() -> bool {
            monitor_raw_ptr->notify_state_changed(iface_name, expected_iface_state);
            return true;
        }));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    };

    beerocks::net::InterfaceStateManagerImpl interface_state_manager(std::move(monitor),
                                                                     std::move(reader));

    ASSERT_TRUE(interface_state_manager.start());
    ASSERT_TRUE(interface_state_manager.read_state(iface_name, actual_iface_state));
    ASSERT_EQ(actual_iface_state, expected_iface_state);
    ASSERT_TRUE(interface_state_manager.stop());
}

TEST(interface_state_manager_impl, notify_state_changed_should_succeed)
{
    auto monitor = std::make_unique<beerocks::net::InterfaceStateMonitorMock>();
    auto reader  = std::make_unique<beerocks::net::InterfaceStateReaderMock>();

    const char *iface_name          = "eth0";
    const bool expected_iface_state = true;
    bool actual_iface_state         = false;

    // The monitor mock is used in the expectation to emulate that a state-changed event has
    // occurred.
    // Since the unique_ptr to the monitor mock is moved into the interface state manager, it
    // is not available inside the expectation. To overcome this problem, we use the raw pointer
    // instead.
    auto monitor_raw_ptr = monitor.get();

    {
        InSequence sequence;

        EXPECT_CALL(*monitor, start()).WillOnce(Invoke([&]() -> bool {
            monitor_raw_ptr->notify_state_changed(iface_name, expected_iface_state);
            return true;
        }));
        EXPECT_CALL(*monitor, stop()).WillOnce(Return(true));
    };

    beerocks::net::InterfaceStateManagerImpl interface_state_manager(std::move(monitor),
                                                                     std::move(reader));

    interface_state_manager.set_handler(
        [&](const std::string &iface_name, bool iface_state) { actual_iface_state = iface_state; });

    ASSERT_TRUE(interface_state_manager.start());
    ASSERT_EQ(actual_iface_state, expected_iface_state);
    ASSERT_TRUE(interface_state_manager.stop());
}
