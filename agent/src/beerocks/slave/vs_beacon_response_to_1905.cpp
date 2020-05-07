/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "vs_beacon_response_to_1905.h"
#include <tlvf/wfa_map/tlvBeaconMetricsResponse.h>

namespace gate {

/*
typedef struct sBeaconResponse11k {
    uint8_t channel;
    uint8_t op_class;
    uint8_t dialog_token;
    uint8_t measurement_token;
    uint8_t rep_mode;
    uint8_t phy_type;
    uint8_t frame_type;
    int8_t rcpi;
    uint8_t rsni;
    uint8_t ant_id;
    uint16_t duration;
    uint32_t parent_tsf;
    uint64_t start_time;
    sMacAddr sta_mac;
    sMacAddr bssid;
    uint32_t new_ch_width;
    uint32_t new_ch_center_freq_seg_0;
    uint32_t new_ch_center_freq_seg_1;
    uint8_t use_optional_wide_band_ch_switch;
    }
*/

bool load(ieee1905_1::CmduMessageTx &lhs,
          std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE> rhs)
{
    // escape I
    if (!rhs) {
        LOG(ERROR) << "nullptr vs pointer. exiting";
        return false;
    }

    // escape II
    auto tlv = lhs.getClass<wfa_map::tlvBeaconMetricsResponse>();
    if (!tlv) {
        LOG(ERROR)
            << "could not find wfa_map::tlvBeaconMetricsResponse within the input tx. exiting";
        return false;
    }

    const auto &beacon_response_11k = rhs->params();

    tlv->associated_sta_mac() = beacon_response_11k.sta_mac;

    return true;
}

} // namespace gate
