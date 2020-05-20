/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bwl/nl80211_client.h"

#include <gtest/gtest.h>

namespace {

/**
 * Time on value used in test cases
 */
constexpr uint64_t test_time_on_ms = 100;

/**
 * 100% channel utilization value
 */
constexpr uint8_t channel_utilization_100_percent = UINT8_MAX;

/**
 * 50% channel utilization value
 */
constexpr uint8_t channel_utilization_50_percent = UINT8_MAX / 2;

/**
 * 0% channel utilization value
 */
constexpr uint8_t channel_utilization_0_percent = 0;

TEST(nl80211_client_test, get_channel_utilization_should_succeed_with_100_percent)
{
    bwl::nl80211_client::sChannelSurveyInfo channel_survey_info;
    channel_survey_info.in_use       = true;
    channel_survey_info.time_on_ms   = test_time_on_ms;
    channel_survey_info.time_busy_ms = test_time_on_ms;

    bwl::nl80211_client::SurveyInfo survey_info{{channel_survey_info}};

    uint8_t channel_utilization;
    ASSERT_TRUE(survey_info.get_channel_utilization(channel_utilization));
    ASSERT_EQ(channel_utilization_100_percent, channel_utilization);
}

TEST(nl80211_client_test, get_channel_utilization_should_succeed_with_50_percent)
{
    bwl::nl80211_client::sChannelSurveyInfo channel_survey_info;
    channel_survey_info.in_use       = true;
    channel_survey_info.time_on_ms   = test_time_on_ms;
    channel_survey_info.time_busy_ms = test_time_on_ms / 2;

    bwl::nl80211_client::SurveyInfo survey_info{{channel_survey_info}};

    uint8_t channel_utilization;
    ASSERT_TRUE(survey_info.get_channel_utilization(channel_utilization));
    ASSERT_EQ(channel_utilization_50_percent, channel_utilization);
}

TEST(nl80211_client_test, get_channel_utilization_should_succeed_with_0_percent)
{
    bwl::nl80211_client::sChannelSurveyInfo channel_survey_info;
    channel_survey_info.in_use       = true;
    channel_survey_info.time_on_ms   = test_time_on_ms;
    channel_survey_info.time_busy_ms = 0;

    bwl::nl80211_client::SurveyInfo survey_info{{channel_survey_info}};

    uint8_t channel_utilization;
    ASSERT_TRUE(survey_info.get_channel_utilization(channel_utilization));
    ASSERT_EQ(channel_utilization_0_percent, channel_utilization);
}

TEST(nl80211_client_test, get_channel_utilization_should_fail_if_no_channel_in_use)
{
    bwl::nl80211_client::sChannelSurveyInfo channel_survey_info;
    channel_survey_info.in_use       = false;
    channel_survey_info.time_on_ms   = test_time_on_ms;
    channel_survey_info.time_busy_ms = 0;

    bwl::nl80211_client::SurveyInfo survey_info{{channel_survey_info}};

    uint8_t channel_utilization;
    ASSERT_FALSE(survey_info.get_channel_utilization(channel_utilization));
}

TEST(nl80211_client_test, get_channel_utilization_should_fail_if_time_on_is_zero)
{
    bwl::nl80211_client::sChannelSurveyInfo channel_survey_info;
    channel_survey_info.in_use       = true;
    channel_survey_info.time_on_ms   = 0;
    channel_survey_info.time_busy_ms = 0;

    bwl::nl80211_client::SurveyInfo survey_info{{channel_survey_info}};

    uint8_t channel_utilization;
    ASSERT_FALSE(survey_info.get_channel_utilization(channel_utilization));
}

TEST(nl80211_client_test, get_channel_utilization_should_fail_if_time_busy_is_greater_than_time_on)
{
    bwl::nl80211_client::sChannelSurveyInfo channel_survey_info;
    channel_survey_info.in_use       = true;
    channel_survey_info.time_on_ms   = test_time_on_ms;
    channel_survey_info.time_busy_ms = test_time_on_ms + 1;

    bwl::nl80211_client::SurveyInfo survey_info{{channel_survey_info}};

    uint8_t channel_utilization;
    ASSERT_FALSE(survey_info.get_channel_utilization(channel_utilization));
}
} // namespace
