#ifndef InterfaceStateMonitor_H
#define InterfaceStateMonitor_H

#include <functional>

namespace beerocks {
namespace sockets {

using StateChangeHandler = std::function<void(const std::string &iface_name, bool up_and_running)>;

class InterfaceStateMonitor {
public:
    virtual ~InterfaceStateMonitor() = default;

    virtual bool start() = 0;
    virtual bool stop()  = 0;
    void set_handler(const StateChangeHandler &handler) { m_handler = handler; }

protected:
    void handle_state_changed(const std::string &iface_name, bool up_and_running)
    {
        if (m_handler) {
            m_handler(iface_name, up_and_running);
        }
    }

private:
    StateChangeHandler m_handler;
};

} // namespace sockets
} // namespace beerocks

#endif
