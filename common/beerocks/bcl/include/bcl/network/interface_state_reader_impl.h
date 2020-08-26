/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_READER_IMPL_H_
#define BCL_NETWORK_INTERFACE_STATE_READER_IMPL_H_

#include "interface_state_reader.h"

#include <memory>

namespace beerocks {
namespace net {

class InterfaceFlagsReader;

class InterfaceStateReaderImpl : public InterfaceStateReader {
public:
    /**
     * @brief Class constructor
     */
    explicit InterfaceStateReaderImpl(
        const std::shared_ptr<InterfaceFlagsReader> &interface_flags_reader);

    /**
     * @brief Reads interface up-and-running state.
     *
     * @see InterfaceStateReader::read_state
     *
     * This implementation uses an interface flags reader to read device flags and compares the
     * bitmask against bits IFF_UP & IFF_RUNNING.
     */
    bool read_state(const std::string &iface_name, bool &iface_state) override;

private:
    /**
     * Interface flags reader used to read device flags.
     */
    std::shared_ptr<InterfaceFlagsReader> m_interface_flags_reader;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_READER_IMPL_H_ */
