/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <algorithm>
#include <iomanip>
#include <mapf/common/logger.h>
#include <mapf/common/utils.h>
#include <sstream>

namespace mapf {
namespace utils {

std::string dump_buffer(uint8_t *buffer, size_t len)
{
    std::ostringstream hexdump;
    for (size_t i = 0; i < len; i += 16) {
        for (size_t j = i; j < len && j < i + 16; j++)
            hexdump << std::hex << std::setw(2) << std::setfill('0') << (unsigned)buffer[j] << " ";
        hexdump << std::endl;
    }
    return hexdump.str();
}

void copy_string(char *dst, const char *src, size_t dst_len)
{
    const char *src_end = std::find(src, src + dst_len, '\0');
    std::copy(src, src_end, dst);
    std::ptrdiff_t src_size = src_end - src;
    std::ptrdiff_t dst_size = dst_len;
    if (src_size < dst_size) {
        dst[src_size] = 0;
    } else {
        dst[dst_size - 1] = 0;
        MAPF_ERR("copy_string() overflow, src string:'" << src << "'"
                                                        << " dst_size=" << dst_size << std::endl);
    }
}

} // namespace utils
} // namespace mapf
