/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_UTILS_H_
#define _BEEROCKS_UTILS_H_

#include <mapf/common/utils.h>

#include "beerocks_defines.h"
#include "beerocks_string_utils.h"

#include <cstddef>
#include <fcntl.h>
#include <iostream>
#include <set>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace beerocks {
class utils {
public:
    ///
    /// @brief Function template to isolate unused variables in the source code
    /// Note : This is a tool to ONLY help building the source code locally and a way
    /// to resolve unused variables before passing onto Gerrit. <Helps isolate and search for
    /// such variables >
    ///
    template <class T> static void IGNORE_UNUSED_VARIABLE(const T &) {}

    ///
    /// @brief Template function to automatically fill in parameters to memset with type safety.
    ///
    /// @return void since the call is made through a reference, returning a pointer does not make
    ///     sense.
    ///
    template <typename _Tp> static void zero_memory(_Tp &v)
    {
        memset(static_cast<void *>(&v), 0, sizeof(v));
    }

    typedef struct {
        int8_t iface_id = beerocks::IFACE_ID_INVALID;
        int8_t vap_id   = beerocks::IFACE_ID_INVALID;
    } sIfaceVapIds;

    template <typename A>
    static typename std::enable_if<std::is_array<A>::value, size_t>::type array_length(const A &a)
    {
        return std::extent<A>::value;
    }

    static int write_to_file(std::string full_path, const std::string &val);

    static beerocks::eIfaceType get_iface_type_from_string(std::string iface_type_name);
    static std::string get_iface_type_string(beerocks::eIfaceType iface_type);
    static bool is_node_wireless(beerocks::eIfaceType iface_type);
    static sIfaceVapIds get_ids_from_iface_string(const std::string &iface);
    static std::string get_iface_string_from_iface_vap_ids(int8_t iface_id, int8_t vap_id);
    static std::string get_iface_string_from_iface_vap_ids(const std::string &iface, int8_t vap_id);

    static beerocks::eWiFiBandwidth convert_bandwidth_to_enum(int bandwidth_int);
    static int convert_bandwidth_to_int(beerocks::eWiFiBandwidth bw);
    static std::string convert_channel_ext_above_to_string(bool channel_ext_above_secondary,
                                                           beerocks::eWiFiBandwidth bandwidth);
    static void merge_list(std::vector<uint8_t> &ret, std::vector<uint8_t> &list);
    static void hex_dump(const std::string &description, uint8_t *addr, int len,
                         const char *calling_file = __builtin_FILE(),
                         int calling_line         = __builtin_LINE());
    static std::string dump_buffer(const uint8_t *buffer, size_t len);
};

} //namespace beerocks

#endif //_BEEROCKS_UTILS_H_
