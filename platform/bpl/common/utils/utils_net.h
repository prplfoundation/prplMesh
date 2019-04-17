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

#ifndef _BPL_UTILS_NET_H
#define _BPL_UTILS_NET_H

#include <stdint.h>
#include <string>

namespace beerocks {
namespace bpl {
namespace utils {

std::string mac_to_string(const uint8_t* mac);
void mac_from_string(uint8_t* buf, const std::string& mac);

std::string ipv4_to_string(uint32_t ip);
std::string ipv4_to_string(const uint8_t *ip);
void ipv4_from_string(uint8_t* buf, const std::string& ip_str);
uint32_t uint_ipv4_from_string(const std::string& ip_str);

bool linux_iface_get_mac(const std::string& iface, std::string& mac);
bool linux_iface_get_ip(const std::string& iface, std::string& ip);
uint32_t linux_iface_to_index(const std::string& iface);

} // namespace utils
} // namespace bpl
} // namespace beerocks

#endif // _BPL_UTILS_NET_H