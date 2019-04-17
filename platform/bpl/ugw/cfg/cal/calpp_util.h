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

#ifndef __COMMON_CALPP_H_UTIL__
#define __COMMON_CALPP_H_UTIL__

#include <string>
#include "calpp_message.h"

namespace beerocks {
namespace bpl {

std::string owner_to_string(uint32_t owner);
std::string subop_to_string(uint32_t subop);
std::string mainop_to_string(uint32_t op);
void debug_log(cal_message& msg, const std::string& context = std::string("cal_message:"));

} // namespace bpl
} // namespace beerocks

#endif  // __COMMON_CALPP_UTIL_H__
