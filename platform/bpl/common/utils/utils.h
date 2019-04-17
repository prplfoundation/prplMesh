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

#ifndef _BPL_UTILS_H
#define _BPL_UTILS_H

#include <stdint.h>
#include <string>

namespace beerocks {
namespace bpl {
namespace utils {

void copy_string(char *dst, const char *src, size_t dst_len);
int64_t stoi(std::string str);
std::string int_to_hex_string(const unsigned int integer, const uint8_t number_of_digits);

} // namespace utils
} // namespace bpl
} // namespace beerocks

#endif // _BPL_UTILS_NET_H

