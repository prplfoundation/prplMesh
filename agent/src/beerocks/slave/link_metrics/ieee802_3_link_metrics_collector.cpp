/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "ieee802_3_link_metrics_collector.h"

#include <bcl/network/network_utils.h>

#include <easylogging++.h>

// SPEED values
#include <linux/ethtool.h>

namespace beerocks {

bool ieee802_3_link_metrics_collector::get_link_metrics(
    const std::string &local_interface_name,
    [[gnu::unused]] const sMacAddr &neighbor_interface_address, sLinkMetrics &link_metrics)
{
    net::sInterfaceStats iface_stats;
    if (!net::network_utils::get_iface_stats(local_interface_name, iface_stats)) {
        LOG(ERROR) << "Failed getting interface statistics for interface " << local_interface_name;
        return false;
    }

    /**
     * Get interface speed into PHY rate.
     */
    uint32_t phy_rate_mbps = UINT32_MAX;
    net::network_utils::linux_iface_get_speed(local_interface_name, phy_rate_mbps);

    /**
     * Note: The MAC throughput capacity is a function of the physical data rate and
     * of the MAC overhead. We could somehow compute such overhead or, for simplicity,
     * set the MAC throughput as a percentage of the physical data rate.
     * For Ethernet, we can estimate the overhead: 7 bytes preamble, 1 byte SFD, 14
     * bytes header, 4 bytes CRC and 12 bytes of interpacket gap on a 1500 byte
     * payload. So 1500/1538.
     * (see https://en.wikipedia.org/wiki/Ethernet_frame)
     */
    const float layer2_payload_size = 1500;
    const float layer1_total_size   = 1538;
    link_metrics.transmitter.mac_throughput_capacity_mbps =
        phy_rate_mbps * (layer2_payload_size / layer1_total_size);
    // Note: For simplicity, link availability is set to "100% of the time"
    link_metrics.transmitter.link_availability = 100;
    link_metrics.transmitter.phy_rate_mbps     = phy_rate_mbps;

    link_metrics.transmitter.packet_errors       = iface_stats.tx_errors;
    link_metrics.transmitter.transmitted_packets = iface_stats.tx_packets;

    link_metrics.receiver.packet_errors    = iface_stats.rx_errors;
    link_metrics.receiver.packets_received = iface_stats.rx_packets;
    link_metrics.receiver.rssi             = UINT8_MAX;

    return true;
}

} // namespace beerocks
