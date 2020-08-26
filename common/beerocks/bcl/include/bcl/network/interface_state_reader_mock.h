/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_READER_MOCK_H_
#define BCL_NETWORK_INTERFACE_STATE_READER_MOCK_H_

#include "interface_state_reader.h"

#include <gmock/gmock.h>

namespace beerocks {
namespace net {

class InterfaceStateReaderMock : public InterfaceStateReader {
public:
    MOCK_METHOD(bool, read_state, (const std::string &iface_name, bool &iface_state), (override));
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_READER_MOCK_H_ */
