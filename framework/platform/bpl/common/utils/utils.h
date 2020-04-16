/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_UTILS_H
#define _BPL_UTILS_H

#include <stdint.h>
#include <string>

namespace beerocks {
namespace bpl {
namespace utils {

void ltrim(std::string &str, std::string additional_chars = std::string());
void rtrim(std::string &str, std::string additional_chars = std::string());
void trim(std::string &str, std::string additional_chars = std::string());

int64_t stoi(std::string str);
std::string int_to_hex_string(const unsigned int integer, const uint8_t number_of_digits);

} // namespace utils
} // namespace bpl
} // namespace beerocks

#endif // _BPL_UTILS_NET_H
