/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../include/beerocks/bcl/beerocks_string_utils.h"

#include <easylogging++.h>

#include <iomanip>

using namespace beerocks;

static const std::string WHITESPACE_CHARS(" \t\n\r\f\v");

void string_utils::ltrim(std::string &str, std::string additional_chars)
{
    str.erase(0, str.find_first_not_of(WHITESPACE_CHARS + additional_chars));
}

void string_utils::rtrim(std::string &str, std::string additional_chars)
{
    str.erase(str.find_last_not_of(WHITESPACE_CHARS + additional_chars) + 1);
}

void string_utils::trim(std::string &str, std::string additional_chars)
{
    ltrim(str, additional_chars);
    rtrim(str, additional_chars);
}

std::string string_utils::ltrimmed_substr(const std::string &str)
{
    auto start = str.find_first_not_of(WHITESPACE_CHARS);
    if (std::string::npos != start) {
        return str.substr(start);
    }
    return std::string();
}

std::string string_utils::rtrimmed_substr(const std::string &str)
{
    auto end = str.find_last_not_of(WHITESPACE_CHARS);
    if (std::string::npos != end) {
        return str.substr(0, end + 1);
    }
    return std::string();
}

std::string string_utils::trimmed_substr(const std::string &str)
{
    auto start = str.find_first_not_of(WHITESPACE_CHARS);
    auto end   = str.find_last_not_of(WHITESPACE_CHARS);
    if ((std::string::npos != start) && (std::string::npos != end)) {
        return str.substr(start, end - start + 1);
    }
    return std::string();
}

std::string string_utils::bool_str(bool val)
{
    return (val) ? std::string("true") : std::string("false");
}

void string_utils::copy_string(char *dst, const char *src, size_t dst_len)
{
    const char *src_end = std::find((char *)src, ((char *)src) + dst_len, '\0');
    std::copy(src, src_end, dst);
    std::ptrdiff_t src_size = src_end - src;
    std::ptrdiff_t dst_size = dst_len;
    if (src_size < dst_size) {
        dst[src_size] = 0;
    } else {
        dst[dst_size - 1] = 0;
        LOG(ERROR) << "copy_string() overflow, src string:'" << src << "'"
                   << " dst_size=" << dst_size << std::endl;
    }
}

std::vector<std::string> string_utils::str_split(const std::string &s, char delim)
{
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }

    return elems;
}

int64_t string_utils::stoi(const std::string &str, const char *calling_file, int calling_line)
{
    if (str.find_first_not_of("0123456789") != std::string::npos) {
        auto calling_file_str       = std::string(calling_file);
        const auto caller_file_name = calling_file_str.substr(calling_file_str.rfind('/') + 1);
        LOG(WARNING) << "string_utils::stoi(), string \"" << str
                     << "\" has illegal characters, caller: " << caller_file_name << "["
                     << calling_line << "]";
        return 0;
    }
    std::stringstream val_s(str);
    int64_t val;
    val_s >> val;
    if (val_s.fail()) {
        auto calling_file_str       = std::string(calling_file);
        const auto caller_file_name = calling_file_str.substr(calling_file_str.rfind('/') + 1);
        LOG(WARNING) << "string_utils::stoi(), cannot convert \"" << str
                     << "\" to int64_t, caller: " << caller_file_name << "[" << calling_line << "]";
        return 0;
    }
    return val;
}

std::string string_utils::int_to_hex_string(const unsigned int integer,
                                            const uint8_t number_of_digits)
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
};
