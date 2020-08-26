/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_FLAGS_READER_IMPL_H_
#define BCL_NETWORK_INTERFACE_FLAGS_READER_IMPL_H_

#include "interface_flags_reader.h"

namespace beerocks {
namespace net {

class InterfaceFlagsReaderImpl : public InterfaceFlagsReader {
public:
    /**
     * @brief Reads interface flags.
     *
     * @see InterfaceFlagsReader::read_flags
     *
     * This implementation uses ioctl() with SIOCGIFFLAGS to read device flags.
     */
    bool read_flags(const std::string &iface_name, uint16_t &iface_flags) override;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_FLAGS_READER_IMPL_H_ */
