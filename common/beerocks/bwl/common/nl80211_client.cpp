/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bwl/nl80211_client.h"

namespace bwl {

bool nl80211_client::SurveyInfo::get_channel_utilization(uint8_t &channel_utilization) const
{
    channel_utilization = 0;

    /**
     * TODO: Calculate channel utilization for the primary channel #1312
     *
     * According to 802.11 spec section 9.4.2.28: "When more than one channel is in use for the
     * BSS, the Channel Utilization field value is calculated only for the primary channel".
     *
     * For simplicity, if there is more than one channel in use, currently we take the first
     * channel found as the primary channel.
     *
     * A more elaborate solution shall correctly identify which is the primary channel.
     */
    auto it =
        std::find_if(data.begin(), data.end(), [](const sChannelSurveyInfo &channel_survey_info) {
            return channel_survey_info.in_use;
        });

    /**
     * No channel in use, no data available
     */
    if (it == data.end()) {
        return false;
    }

    auto survey_info = *it;

    /**
     * No data available
     */
    if (0 == survey_info.time_on_ms) {
        return false;
    }

    /**
     * This should never happen but if it does, then return error
     */
    if (survey_info.time_busy_ms > survey_info.time_on_ms) {
        return false;
    }

    /**
     * The channel utilization is defined as the percentage of time, linearly scaled with 255
     * representing 100%, that the AP sensed the medium was busy.
     */
    channel_utilization = (survey_info.time_busy_ms * UINT8_MAX) / survey_info.time_on_ms;

    return true;
}

} // namespace bwl
