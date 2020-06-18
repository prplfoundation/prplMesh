/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_KEY_VALUE_PARSER_H_
#define _BWL_KEY_VALUE_PARSER_H_

#include <list>
#include <string>
#include <unordered_map>

namespace bwl {

/**
 * @brief On KeyValueParser there are two types of object that can be parsed:
 * parsed_line_t - Shall be used on key-value data with unique keys for each element, seperated by
 * a space or a new line (Default seperator, can be modified on use). For example:
 * "key1=value1 key2=value2 key3=value3 ..."
 * --Or--
 * "key1=value1
 *  key2=value2
 *  key3=value3
 *  ..."
 * 
 * parsed_multiline_t - Shall be used in case there are 2 or more "parsed_line_t" which each "line"
 * could have the same keys as the previous one. For example: 
 * "key1=Line1value1 key2=Line1value2 key3=Line1value3 ...
 *  key1=Line2value1 key2=Line2value2 key3=Line2value3 ...
 *  key1=Line3value1 key2=Line3value2 key3=Line3value3 ...
 *  ...""
 */

typedef std::unordered_map<std::string, std::string> parsed_line_t;
typedef std::list<parsed_line_t> parsed_multiline_t;

class KeyValueParser {
protected:
    static void parse_line(std::istringstream &iss, std::list<char> delimiter_list,
                           parsed_line_t &parsed_line);

    static void parse_multiline(std::istringstream &iss, std::list<char> delimiter_list,
                                parsed_multiline_t &parsed_multiline);

    static bool read_param(const std::string &key, parsed_line_t &obj, int64_t &value,
                           bool ignore_unknown = false);

    static bool read_param(const std::string &key, parsed_line_t &obj, const char **value);

    static void parsed_obj_debug(const parsed_line_t &obj);
    static void parsed_obj_debug(const parsed_multiline_t &obj);

    virtual void parse_event(std::string event_str, parsed_line_t &parsed_line);

    virtual size_t event_buffer_start_process_idx(const std::string &event_str);

    virtual void parse_event_keyless_params(const std::string &event_str, size_t idx_start,
                                            parsed_line_t &parsed_line);
};

} // namespace bwl

#endif // _BWL_KEY_VALUE_PARSER_H_
