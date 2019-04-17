/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#include "utils.h"

#include <cstring>
#include <stdio.h>
#include <unistd.h>

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <algorithm>

#include <mapf/common/logger.h>

namespace beerocks {
namespace bpl {
namespace utils {

void copy_string(char *dst, const char *src, size_t dst_len)
{
    const char* src_end = std::find((char*)src, ((char*)src) + dst_len, '\0');
    std::copy(src, src_end, dst);
    std::ptrdiff_t src_size = src_end - src;
    std::ptrdiff_t dst_size = dst_len;
    if (src_size < dst_size ) {
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

