/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bcl/beerocks_state_machine.h>

#include <bwl/base_wlan_hal_types.h>

#include <chrono>

// Forward declarations
// namespace bwl {
// struct RadioInfo;
// }

struct wpa_ctrl;

namespace bwl {
namespace wav_fapi {

enum class fapi_fsm_state { Delay, Init, GetRadioInfo, Attach, Operational, Detach };

enum class fapi_fsm_event { Attach, Detach };

class fapi_attach_fsm : protected beerocks::beerocks_fsm<fapi_fsm_state, fapi_fsm_event> {

    // Public definitions
public:
    // FAPI event callback
    typedef std::function<bool(const std::string &opcode, std::shared_ptr<void> obj)>
        fapi_event_cb_t;

    // Public methods:
public:
    fapi_attach_fsm(HALType type, RadioInfo &radio_info, fapi_event_cb_t cb);
    ~fapi_attach_fsm() = default;

    bool setup();
    fapi_fsm_state attach();
    bool detach();

    // Read the radio information from the hardware
    bool refresh_radio_info();

    // Refresh the VAPs information from the hardware
    bool refresh_vaps_info();

    //Read the interface information ,indication for interface and hostapd up.
    bool get_interface_info(std::string iface_name);

#ifdef USE_FAPI_DAEMON

    int get_wpa_ctrl_events_fd() const { return (m_fapi_event_fd); }

#else

    struct wpa_ctrl *get_wpa_ctrl_socket() const { return (m_wpa_ctrl); }

#endif

    // Inject FAPI event (from internal FAPI callback)
    void inject_fapi_event(const std::string opcode, void *obj);

    std::string state_enum_to_string(fapi_fsm_state state);

private:
    std::chrono::steady_clock::time_point m_state_timeout;

    HALType m_hal_type;
    RadioInfo &m_radio_info;

#ifdef USE_FAPI_DAEMON

    int m_fapi_event_fd = -1;

#else

    struct wpa_ctrl *m_wpa_ctrl = nullptr;

#endif

    std::shared_ptr<char> m_temp_fapi_value;

    // FAPI events callback
    fapi_event_cb_t m_fapi_event_cb;
};

} // namespace wav_fapi
} // namespace bwl
