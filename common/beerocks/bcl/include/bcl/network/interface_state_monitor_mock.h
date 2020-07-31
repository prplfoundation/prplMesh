/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_INTERFACE_STATE_MONITOR_MOCK_H_
#define BCL_NETWORK_INTERFACE_STATE_MONITOR_MOCK_H_

#include "interface_state_monitor.h"

#include <gmock/gmock.h>

namespace beerocks {
namespace net {

class InterfaceStateMonitorMock : public InterfaceStateMonitor {
public:
    MOCK_METHOD(bool, start, (), (override));
    MOCK_METHOD(bool, stop, (), (override));

    /**
     * This method was inherited as protected but we're changing it to public via a using
     * declaration so we can invoke it from unit tests to emulate that a state-changed event has
     * occurred.
     */
    using InterfaceStateMonitor::notify_state_changed;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_INTERFACE_STATE_MONITOR_MOCK_H_ */
