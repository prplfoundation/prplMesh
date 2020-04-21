/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee802_11_link_metrics_collector.h"

#include <bwl/nl80211_client_factory.h>

namespace beerocks {

bool ieee802_11_link_metrics_collector::get_link_metrics(const std::string &local_interface_name,
                                                         const sMacAddr &neighbor_interface_address,
                                                         sLinkMetrics &link_metrics)
{
    std::unique_ptr<bwl::nl80211_client> client = bwl::nl80211_client_factory::create_instance();
    bwl::nl80211_client::sta_info sta_info;

    if (!client->get_sta_info(local_interface_name, neighbor_interface_address, sta_info)) {
        return false;
    }

    link_metrics.transmitter.packet_errors                = sta_info.tx_failed;
    link_metrics.transmitter.transmitted_packets          = sta_info.tx_packets;
    link_metrics.transmitter.mac_throughput_capacity_mbps = sta_info.tx_bitrate_100kbps / 10;
    // Note: For simplicity, link availability is set to "100% of the time"
    link_metrics.transmitter.link_availability = 100;
    link_metrics.transmitter.phy_rate_mbps     = UINT16_MAX;
    // Note: rx errors cannot be obtained with NL80211_CMD_GET_STATION command.
    link_metrics.receiver.packet_errors    = 0;
    link_metrics.receiver.packets_received = sta_info.rx_packets;
    // Note: according to <linux/nl80211.h>, the value of NL80211_STA_INFO_SIGNAL
    // attribute (i.e.: signal strength of last received PPDU) is expressed in dBm.
    // However IEEE 1905.1 standard requires RSSI to be expressed in dB.
    link_metrics.receiver.rssi = sta_info.signal_dbm;

    return true;
}

} // namespace beerocks
