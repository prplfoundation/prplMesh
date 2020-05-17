/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <climits>
#include <linux/if_ether.h>
#include <tlvf/tlvftypes.h>

namespace tlvf {

static constexpr const char mac_bytes_separator = ':';

std::string int_to_hex_string(const unsigned int integer, const uint8_t number_of_digits)
{
    std::stringstream ss_hex_string;

    // convert to hex
    ss_hex_string << std::setw(number_of_digits) << std::setfill('0') << std::hex << integer;

    return ss_hex_string.str();
};

std::string mac_to_string(const uint8_t *mac_address)
{
    std::string mac_addr_string;

    mac_addr_string = int_to_hex_string((uint32_t)mac_address[0], 2) + mac_bytes_separator +
                      int_to_hex_string((uint32_t)mac_address[1], 2) + mac_bytes_separator +
                      int_to_hex_string((uint32_t)mac_address[2], 2) + mac_bytes_separator +
                      int_to_hex_string((uint32_t)mac_address[3], 2) + mac_bytes_separator +
                      int_to_hex_string((uint32_t)mac_address[4], 2) + mac_bytes_separator +
                      int_to_hex_string((uint32_t)mac_address[5], 2);

    return mac_addr_string;
}

std::string mac_to_string(const sMacAddr &mac) { return mac_to_string((const uint8_t *)mac.oct); }

// Converts uint64_t mac address to string format
std::string mac_to_string(const uint64_t mac)
{
    uint8_t mac_address[ETH_ALEN];
    int8_t i;
    uint8_t *p = mac_address;
    for (i = 5; i >= 0; i--) {
        *p++ = mac >> (CHAR_BIT * i);
    }
    return mac_to_string(mac_address);
}

void mac_from_string(uint8_t *buf, const std::string &mac)
{
    if (buf) {
        if (mac.empty()) {
            memset(buf, 0, ETH_ALEN);
        } else {
            std::stringstream mac_ss(mac);
            std::string token;

            for (int i = 0; i < ETH_ALEN; i++) {
                std::getline(mac_ss, token, mac_bytes_separator);
                buf[i] = std::stoul(token, nullptr, 16);
            }
        }
    }
}

sMacAddr mac_from_string(const std::string &mac)
{
    sMacAddr ret;

    mac_from_string(ret.oct, mac);

    return ret;
}
} // namespace tlvf