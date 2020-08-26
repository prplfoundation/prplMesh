/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_MANAGER_H_
#define BCL_NETWORK_INTERFACE_STATE_MANAGER_H_

#include "interface_state_monitor.h"
#include "interface_state_reader.h"

namespace beerocks {
namespace net {

/**
 * The InterfaceStateManager is a facade interface for both the InterfaceStateMonitor and
 * InterfaceStateReader interfaces together.
 */
class InterfaceStateManager : public InterfaceStateMonitor, public InterfaceStateReader {
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_MANAGER_H_ */
