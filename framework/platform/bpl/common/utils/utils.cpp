/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "utils.h"

#include <cstring>
#include <stdio.h>
#include <unistd.h>

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <mapf/common/logger.h>

namespace beerocks {
namespace bpl {
namespace utils {

static const std::string WHITESPACE_CHARS(" \t\n\r\f\v");

void ltrim(std::string &str, std::string additional_chars)
{
    str.erase(0, str.find_first_not_of(WHITESPACE_CHARS + additional_chars));
}

void rtrim(std::string &str, std::string additional_chars)
{
    str.erase(str.find_last_not_of(WHITESPACE_CHARS + additional_chars) + 1);
}

void trim(std::string &str, std::string additional_chars)
{
    ltrim(str, additional_chars);
    rtrim(str, additional_chars);
}

void copy_string(char *dst, const char *src, size_t dst_len)
{
    const char *src_end = std::find((char *)src, ((char *)src) + dst_len, '\0');
    std::copy(src, src_end, dst);
    std::ptrdiff_t src_size = src_end - src;
    std::ptrdiff_t dst_size = dst_len;
    if (src_size < dst_size) {
        dst[src_size] = 0;
    } else {
        dst[dst_size - 1] = 0;
    }
}

int64_t stoi(std::string str)
{
    std::stringstream val_s(str);
    int64_t val;
    val_s >> val;
    if (val_s.fail()) {
        MAPF_WARN("stoi(), cannot convert \"" << str << "\" to int64_t");
        return 0;
    }
    return val;
}

std::string int_to_hex_string(const unsigned int integer, const uint8_t number_of_digits)
{
    // 'number_of_digits' represent how much digits the number should have, so the function will
    // pad the number with zeroes from left, if necessary.
    // for example: int_to_hex_string(255, 4) -> "00ff"
    //              int_to_hex_string(255, 1) -> "ff"

    std::string return_string;
    std::stringstream ss_hex_string;

    // convert to hex
    ss_hex_string << std::setw(number_of_digits) << std::setfill('0') << std::hex << integer;

    return ss_hex_string.str();
}

} // namespace utils
} // namespace bpl
} // namespace beerocks
