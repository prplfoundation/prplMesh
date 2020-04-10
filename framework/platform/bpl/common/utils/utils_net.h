/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_UTILS_NET_H
#define _BPL_UTILS_NET_H

#include <stdint.h>
#include <string>

namespace beerocks {
namespace bpl {
namespace utils {

std::string mac_to_string(const uint8_t *mac);
void mac_from_string(uint8_t *buf, const std::string &mac);

std::string ipv4_to_string(uint32_t ip);
std::string ipv4_to_string(const uint8_t *ip);
void ipv4_from_string(uint8_t *buf, const std::string &ip_str);
uint32_t uint_ipv4_from_string(const std::string &ip_str);

bool linux_iface_get_mac(const std::string &iface, std::string &mac);
bool linux_iface_get_ip(const std::string &iface, std::string &ip);
uint32_t linux_iface_to_index(const std::string &iface);

} // namespace utils
} // namespace bpl
} // namespace beerocks

#endif // _BPL_UTILS_NET_H