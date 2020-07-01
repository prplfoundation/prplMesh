/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <cstdint>

#include <mapf/common/message.h>

namespace mapf {

// Static members definition
constexpr uint32_t Message::kMessageMagic;
constexpr uint32_t Message::kMessageHeaderVersion;
constexpr uint32_t Message::kMaxTopicSize;
constexpr uint32_t Message::kMaxFrameLength;

} // namespace mapf
