/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "nl80211_client_dummy.h"

#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>

namespace bwl {

bool nl80211_client_dummy::get_interface_info(const std::string &interface_name,
                                              interface_info &interface_info)
{
    interface_info.name  = interface_name;
    interface_info.index = 1;
    interface_info.addr  = beerocks::net::network_utils::mac_from_string("01:02:03:04:05:06");
    interface_info.ssid  = "prplmesh";
    interface_info.type  = 3; // NL80211_IFTYPE_AP
    interface_info.wiphy = 2;

    return true;
}

bool nl80211_client_dummy::get_radio_info(const std::string &interface_name, radio_info &radio_info)
{
    // Suppress "unused parameter" warning
    (void)interface_name;

    band_info band{};

    // 5GHz simulated report
    for (uint16_t ch = 36; ch <= 64; ch += 4) {
        for (uint16_t step = 0; step < 3; step++) {
            channel_info channel;

            channel.number = ch;
            channel.supported_bandwidths.push_back(
                beerocks::utils::convert_bandwidth_to_enum(20 + step * 20));
            channel.is_dfs = (ch > 48);

            band.supported_channels[channel.number] = channel;
        }
    }
    for (uint16_t ch = 100; ch <= 144; ch += 4) {
        for (uint16_t step = 0; step < 3; step++) {
            channel_info channel;

            channel.number = ch;
            channel.supported_bandwidths.push_back(
                beerocks::utils::convert_bandwidth_to_enum(20 + step * 20));
            channel.is_dfs = true;

            band.supported_channels[channel.number] = channel;
        }
    }
    for (uint16_t ch = 149; ch <= 165; ch += 4) {
        for (uint16_t step = 0; step < 3; step++) {
            channel_info channel;

            channel.number = ch;
            channel.supported_bandwidths.push_back(
                beerocks::utils::convert_bandwidth_to_enum(20 + step * 20));
            channel.is_dfs = false;

            band.supported_channels[channel.number] = channel;
        }
    }

    radio_info.bands.push_back(band);

    return true;
}

bool nl80211_client_dummy::get_sta_info(const std::string &interface_name,
                                        const sMacAddr &sta_mac_address, sta_info &sta_info)
{
    // Suppress "unused parameter" warning
    (void)interface_name;
    (void)sta_mac_address;

    static int inactive_time_ms        = 0;
    static int rx_bytes                = 0;
    static int rx_packets              = 0;
    static uint32_t tx_bytes           = 0;
    static uint32_t tx_packets         = 0;
    static uint32_t tx_retries         = 0;
    static uint32_t tx_failed          = 0;
    static uint8_t signal_dbm          = 0;
    static uint8_t signal_avg_dbm      = 0;
    static uint16_t tx_bitrate_100kbps = 0;
    static uint16_t rx_bitrate_100kbps = 0;

    sta_info.inactive_time_ms   = inactive_time_ms++;
    sta_info.rx_bytes           = rx_bytes++;
    sta_info.rx_packets         = rx_packets++;
    sta_info.tx_bytes           = tx_bytes++;
    sta_info.tx_packets         = tx_packets++;
    sta_info.tx_retries         = tx_retries++;
    sta_info.tx_failed          = tx_failed++;
    sta_info.signal_dbm         = signal_dbm++;
    sta_info.signal_avg_dbm     = signal_avg_dbm++;
    sta_info.tx_bitrate_100kbps = tx_bitrate_100kbps++;
    sta_info.rx_bitrate_100kbps = rx_bitrate_100kbps++;

    return true;
}

bool nl80211_client_dummy::set_tx_power_limit(const std::string &interface_name, uint32_t limit)
{
    return true;
}

} // namespace bwl
