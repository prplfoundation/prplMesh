/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "nl80211_client_dummy.h"

namespace bwl {

nl80211_client_dummy::nl80211_client_dummy() {}

nl80211_client_dummy::~nl80211_client_dummy() {}

bool nl80211_client_dummy::get_sta_info(const std::string &local_interface_name,
                                        const sMacAddr &sta_mac_address, sStaInfo &sta_info)
{
    // Suppress "unused parameter" warning
    (void)local_interface_name;
    (void)sta_mac_address;

    static int inactive_time_ms        = 0;
    static int rx_bytes                = 0;
    static int rx_packets              = 0;
    static uint32_t tx_bytes           = 0;
    static uint32_t tx_packets         = 0;
    static uint32_t tx_retries         = 0;
    static uint32_t tx_failed          = 0;
    static uint8_t signal_dBm          = 0;
    static uint8_t signal_avg_dBm      = 0;
    static uint16_t tx_bitrate_100kbps = 0;
    static uint16_t rx_bitrate_100kbps = 0;

    sta_info.inactive_time_ms   = inactive_time_ms++;
    sta_info.rx_bytes           = rx_bytes++;
    sta_info.rx_packets         = rx_packets++;
    sta_info.tx_bytes           = tx_bytes++;
    sta_info.tx_packets         = tx_packets++;
    sta_info.tx_retries         = tx_retries++;
    sta_info.tx_failed          = tx_failed++;
    sta_info.signal_dBm         = signal_dBm++;
    sta_info.signal_avg_dBm     = signal_avg_dBm++;
    sta_info.tx_bitrate_100kbps = tx_bitrate_100kbps++;
    sta_info.rx_bitrate_100kbps = rx_bitrate_100kbps++;

    return true;
}

bool nl80211_client_dummy::set_tx_power_limit(const std::string &local_interface_name,
                                              uint32_t limit)
{
    return true;
}

} // namespace bwl
