/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __LINK_METRICS_H__
#define __LINK_METRICS_H__

#include <string>

#include <tlvf/common/sMacAddr.h>

namespace beerocks {

/**
 * @brief Transmitter metrics information associated to the link between a local interface and a
 * neighbor's interface.
 *
 * Information in this structure is used to fill Transmitter Link Metric TLV.
 */
struct sTransmitterLinkMetrics {
    /**
     * Estimated number of lost packets on the transmit side of the link during the measurement
     * period.
     */
    uint32_t packet_errors = 0;

    /**
     * Estimated number of packets transmitted by the Transmitter of the link on the same
     * measurement period used to estimate tx_packet_errors.
     */
    uint32_t transmitted_packets = 0;

    /**
     * The maximum MAC throughput of the link estimated at the transmitter and expressed in Mb/s.
     */
    uint32_t mac_throughput_capacity_mbps = 0;

    /**
     * The estimated average percentage of time that the link is available for data transmissions.
     */
    uint16_t link_availability = 0;

    /**
     * If the media type of the link is IEEE 802.3, IEEE 1901, MoCA 1.1 or Generic Phy, then this
     * value is the PHY rate estimated at the transmitter of the link expressed in Mb/s;
     * otherwise, it is set to 0xFFFF.
     */
    uint32_t phy_rate_mbps = 0;
};

/**
 * @brief Receiver metrics information associated to the link between a local interface and a
 * neighbor's interface.
 *
 * Information in this structure is used to fill Receiver Link Metric TLV.
 */
struct sReceiverLinkMetrics {
    /**
     * Estimated number of lost packets during the measurement period.
     */
    uint32_t packet_errors = 0;

    /**
     * Number of packets received at the interface during the same measurement period used to count
     * packet_errors
     */
    uint32_t packets_received = 0;

    /**
     * If the media type of the link is IEEE 802.11, then this value is the estimated RSSI in dB at
     * the receive side of the link expressed in dB; otherwise, it is set to 0xFF.
     */
    uint8_t rssi = 0;
};

/**
 * @brief Metrics information associated to the link between a local interface and a
 * neighbor's interface.
 *
 * Information in this structure is used to build the Link Metric response message.
 */
struct sLinkMetrics {
    struct sTransmitterLinkMetrics transmitter; /**< Transmitter link metrics. */
    struct sReceiverLinkMetrics receiver;       /**< Receiver link metrics. */
};

/**
 * @brief Link metrics collector interface.
 *
 * This is a C++ interface: an abstract class that is designed to be specifically used as a base
 * class and which derived classes (implementations) will override each pure virtual function.
 *
 * Known implementations: ieee802_3_link_metrics_collector and ieee802_11_link_metrics_collector.
 */
class link_metrics_collector {

public:
    virtual ~link_metrics_collector() = default;

    /**
     * @brief Gets link metrics information.
     *
     * Gets link metrics associated to the link between given local interface and a neighbor's
     * interface whose MAC address is 'neighbor_interface_address'.
     *
     * @param[in] local_interface_name Name of the local interface.
     * @param[in] neighbor_interface_address MAC address at the other end of the link (this MAC
     * address belongs to a neighbor's interface.
     * @param[out] link_metrics Link metrics information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_link_metrics(const std::string &local_interface_name,
                                  const sMacAddr &neighbor_interface_address,
                                  sLinkMetrics &link_metrics) = 0;
};

} // namespace beerocks

#endif
