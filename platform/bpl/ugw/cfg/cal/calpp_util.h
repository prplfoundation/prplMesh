/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __COMMON_CALPP_H_UTIL__
#define __COMMON_CALPP_H_UTIL__

#include "calpp_message.h"
#include <string>

namespace beerocks {
namespace bpl {

std::string owner_to_string(uint32_t owner);
std::string subop_to_string(uint32_t subop);
std::string mainop_to_string(uint32_t op);
void debug_log(cal_message &msg, const std::string &context = std::string("cal_message:"));

} // namespace bpl
} // namespace beerocks

#endif // __COMMON_CALPP_UTIL_H__
