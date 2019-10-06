/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _NET_STRUCT_
#define _NET_STRUCT_

#include <cstring>
#include <stdint.h>
#include <string>

#include <tlvf/common/sMacAddr.h>

inline bool operator==(sMacAddr const &lhs, sMacAddr const &rhs)
{
    return (0 == std::memcmp(lhs.oct, rhs.oct, sizeof(sMacAddr)));
}

inline bool operator!=(sMacAddr const &lhs, sMacAddr const &rhs) { return !(rhs == lhs); }

template <> struct std::hash<sMacAddr> {
    size_t operator()(const sMacAddr &m) const
    {
        uint64_t value_to_hash = 0;
        for (size_t byte = 0; byte < sizeof(m.oct); byte++) {
            value_to_hash <<= 8;
            value_to_hash += m.oct[byte];
        }
        return std::hash<std::uint64_t>()(value_to_hash);
    }
};

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

} // namespace net
} // namespace beerocks

#endif
