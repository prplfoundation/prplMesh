/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_utils.h>
#include <bcl/son/son_wireless_utils.h>

#include <gtest/gtest.h>

namespace {

/** @brief Helper function for testing get_operating_class_by_channel
 *
 * get_operating_class_by_channel takes sWifiChannel as argument. This function builds that struct
 * based on channel and bandwith arguments (the only ones relevant for
 * get_operating_class_by_channel) and calls get_operating_class_by_channel.
 *
 * @return The operating class, as an unsigned (to make it easier to log)
 */
static unsigned get_operating_class_by_channel_helper(uint8_t channel, uint8_t bandwidth)
{
    beerocks::message::sWifiChannel ch;
    ch.channel           = channel;
    ch.channel_bandwidth = beerocks::utils::convert_bandwidth_to_enum(bandwidth);
    return son::wireless_utils::get_operating_class_by_channel(ch);
}

// See https://docs.google.com/spreadsheets/d/1J2IYuGjFX_OQQpgb4OgsyDx73klAUk6qSmeB4acJ5us
// for the table we need to conform to.
TEST(wireless_utils_test, get_operating_class_by_channel_should_return_valid)
{
    EXPECT_EQ(get_operating_class_by_channel_helper(1, 20), 81);
    EXPECT_EQ(get_operating_class_by_channel_helper(13, 20), 81);

    EXPECT_EQ(get_operating_class_by_channel_helper(14, 20), 82);

    EXPECT_EQ(get_operating_class_by_channel_helper(1, 40), 83);
    EXPECT_EQ(get_operating_class_by_channel_helper(9, 40), 83);

    /* Note: we have no way to distinguish 40- and 40+ */
    EXPECT_EQ(get_operating_class_by_channel_helper(10, 40), 84);
    EXPECT_EQ(get_operating_class_by_channel_helper(13, 40), 84);

    EXPECT_EQ(get_operating_class_by_channel_helper(36, 20), 115);
    EXPECT_EQ(get_operating_class_by_channel_helper(48, 20), 115);

    EXPECT_EQ(get_operating_class_by_channel_helper(36, 40), 116);
    EXPECT_EQ(get_operating_class_by_channel_helper(44, 40), 116);

    EXPECT_EQ(get_operating_class_by_channel_helper(40, 40), 117);
    EXPECT_EQ(get_operating_class_by_channel_helper(48, 40), 117);

    EXPECT_EQ(get_operating_class_by_channel_helper(52, 20), 118);
    EXPECT_EQ(get_operating_class_by_channel_helper(64, 20), 118);

    EXPECT_EQ(get_operating_class_by_channel_helper(52, 40), 119);
    EXPECT_EQ(get_operating_class_by_channel_helper(60, 40), 119);

    EXPECT_EQ(get_operating_class_by_channel_helper(56, 40), 120);
    EXPECT_EQ(get_operating_class_by_channel_helper(64, 40), 120);

    EXPECT_EQ(get_operating_class_by_channel_helper(100, 20), 121);
    EXPECT_EQ(get_operating_class_by_channel_helper(144, 20), 121);

    EXPECT_EQ(get_operating_class_by_channel_helper(100, 40), 122);
    EXPECT_EQ(get_operating_class_by_channel_helper(140, 40), 122);

    EXPECT_EQ(get_operating_class_by_channel_helper(104, 40), 123);
    EXPECT_EQ(get_operating_class_by_channel_helper(144, 40), 123);

    EXPECT_EQ(get_operating_class_by_channel_helper(149, 20), 124);
    EXPECT_EQ(get_operating_class_by_channel_helper(161, 20), 124);

    EXPECT_EQ(get_operating_class_by_channel_helper(165, 20), 125);
    EXPECT_EQ(get_operating_class_by_channel_helper(169, 20), 125);

    EXPECT_EQ(get_operating_class_by_channel_helper(149, 40), 126);
    EXPECT_EQ(get_operating_class_by_channel_helper(157, 40), 126);

    EXPECT_EQ(get_operating_class_by_channel_helper(153, 40), 127);
    EXPECT_EQ(get_operating_class_by_channel_helper(161, 40), 127);

    /* For 80MHz+, we use starting freq but the table uses center freq */
    EXPECT_EQ(get_operating_class_by_channel_helper(36, 80), 128);
    EXPECT_EQ(get_operating_class_by_channel_helper(52, 80), 128);
    EXPECT_EQ(get_operating_class_by_channel_helper(100, 80), 128);
    EXPECT_EQ(get_operating_class_by_channel_helper(116, 80), 128);
    EXPECT_EQ(get_operating_class_by_channel_helper(132, 80), 128);
    EXPECT_EQ(get_operating_class_by_channel_helper(149, 80), 128);

    EXPECT_EQ(get_operating_class_by_channel_helper(36, 160), 129);
    EXPECT_EQ(get_operating_class_by_channel_helper(100, 160), 129);

    /* The helper function doesn't support 80+80 */
    beerocks::message::sWifiChannel channel;
    channel.channel_bandwidth = static_cast<uint8_t>(beerocks::BANDWIDTH_80_80);
    channel.channel           = 36;
    EXPECT_EQ(+son::wireless_utils::get_operating_class_by_channel(channel), 130);
    channel.channel = 149;
    EXPECT_EQ(+son::wireless_utils::get_operating_class_by_channel(channel), 130);
}

TEST(wireless_utils_test, get_operating_class_by_channel_should_return_invalid)
{
    EXPECT_EQ(get_operating_class_by_channel_helper(0, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(1, 19), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(1, 1), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(15, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(14, 40), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(32, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(34, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(42, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(68, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(96, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(148, 20), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(165, 40), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(40, 80), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(42, 80), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(44, 80), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(68, 80), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(106, 80), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(40, 160), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(42, 160), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(52, 160), 0);
    EXPECT_EQ(get_operating_class_by_channel_helper(149, 160), 0);
}

} // namespace
