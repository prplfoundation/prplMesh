#ifndef InterfaceStateMonitorMock_H
#define InterfaceStateMonitorMock_H

#include "gmock/gmock.h"
#include "interface_state_monitor.h"

namespace beerocks {
namespace sockets {
class InterfaceStateMonitorMock : public InterfaceStateMonitor {
public:
    MOCK_METHOD0(start, bool());
    MOCK_METHOD0(stop, bool());

    /**
         * This method was inherited as protected but we're changing it to public via a using
         * declaration so we can invoke it from unit tests
         */
    using InterfaceStateMonitor::handle_state_changed;
};
} // namespace sockets
} // namespace beerocks

#endif
