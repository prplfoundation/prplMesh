/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _NET_STRUCT_
#define _NET_STRUCT_

#include <cstring>
#include <stdint.h>
#include <string>

#include <tlvf/tlvftypes.h>

namespace beerocks {
namespace net {

enum eNetworkStructsConsts {
    MAC_ADDR_LEN = 6,
    IP_ADDR_LEN  = 4,
};

typedef struct sIpv4Addr {
    uint8_t oct[IP_ADDR_LEN];
    void struct_swap() {}
    void struct_init() { std::memset(oct, 0, IP_ADDR_LEN); }
    bool operator==(sIpv4Addr const &rhs) const
    {
        return (0 == std::memcmp(this->oct, rhs.oct, IP_ADDR_LEN));
    }
    bool operator!=(sIpv4Addr const &rhs) const { return !(rhs == *this); }
} __attribute__((packed)) sIpv4Addr;

typedef struct sScanResult {
    sMacAddr mac;
    uint8_t channel;
    int8_t rssi;
    void struct_swap() {}
    void struct_init()
    {
        mac.struct_init();
        channel = 0;
        rssi    = 0;
    }
    bool operator==(sScanResult const &rhs) const
    {
        return (channel == rhs.channel && rssi == rhs.rssi && mac == rhs.mac);
    }
    bool operator!=(sScanResult const &rhs) const { return !(rhs == *this); }
} __attribute__((packed)) sScanResult;

/**
 * @brief Interface statistics.
 *
 * Information in this structure is obtained from IFLA_STATS attribute of a rtnetlink message
 * through a Rtnetlink socket.
 */
struct sInterfaceStats {
    /**
     * Total bytes transmitted.
     */
    uint32_t tx_bytes = 0;

    /**
     * Packet transmit problems.
     */
    uint32_t tx_errors = 0;

    /**
     * Total packets transmitted.
     */
    uint32_t tx_packets = 0;

    /**
     * Total bytes received.
     */
    uint32_t rx_bytes = 0;

    /**
     * Bad packets received.
     */
    uint32_t rx_errors = 0;

    /**
     * Total packets received.
     */
    uint32_t rx_packets = 0;
};

} // namespace net
} // namespace beerocks

#endif
