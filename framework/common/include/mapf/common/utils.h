/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef MAPFUTILS_H_
#define MAPFUTILS_H_

#include <string>

namespace mapf {

/**
 * @brief mapf utility functions
 */
namespace utils {

/**
 * @brief dump buffer in hex mode
 * 
 * Debugging utility to get a string representation of a given
 * buffer in hexadecimal, split to bytes.
 * 
 * @param buffer input buffer
 * @param len input buffer length
 * @return std::string string containing the hexadecimal buffer contets
 */
std::string dump_buffer(uint8_t *buffer, size_t len);

} // namespace utils
} // namespace mapf
#endif // MAPFUTILS_H_
