/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_DWPAL_H_
#define _BWL_BASE_WLAN_HAL_DWPAL_H_

#include "../common/base_wlan_hal.h"

#include <beerocks/bcl/beerocks_state_machine.h>

#include <chrono>
#include <memory>

namespace bwl {
namespace dwpal {

enum class dwpal_fsm_state { Delay, Init, GetRadioInfo, Attach, Operational, Detach };

enum class dwpal_fsm_event { Attach, Detach };

/*!
 * Base class for the dwpal abstraction layer.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class base_wlan_hal_dwpal : public virtual base_wlan_hal,
                            protected beerocks::beerocks_fsm<dwpal_fsm_state, dwpal_fsm_event> {

    // Public methods:
public:
    virtual ~base_wlan_hal_dwpal();

    virtual HALState attach(bool block = false) override;
    virtual bool detach() override;
    virtual bool ping() override;
    virtual bool refresh_radio_info() override;
    virtual bool refresh_vaps_info(int id) override;
    virtual bool process_ext_events() override;
    virtual std::string get_radio_mac() override;

    // Protected methods
protected:
    base_wlan_hal_dwpal(HALType type, std::string iface_name, bool acs_enabled,
                        hal_event_cb_t callback, int wpa_ctrl_buffer_size);

    // Process dwpal event
    virtual bool process_dwpal_event(char *buffer, int bufLen, const std::string &opcode) = 0;

    bool dwpal_send_cmd(const std::string &cmd, char **reply); // for external process
    bool dwpal_send_cmd(const std::string &cmd);

    // Private data-members:
private:
    const uint32_t AP_ENABELED_TIMEOUT_SEC           = 15;
    const uint32_t AP_ENABELED_FIXED_DFS_TIMEOUT_SEC = 660;

    bool fsm_setup();
    bool refresh_vap_info(int vap_id);

    dwpal_fsm_state m_last_attach_state = dwpal_fsm_state::Detach;

    std::chrono::steady_clock::time_point m_state_timeout;

    void *m_dwpal_ctx = nullptr;

    std::shared_ptr<char> m_wpa_ctrl_buffer;
    size_t m_wpa_ctrl_buffer_size = 0;
};

} // namespace dwpal
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_DWPAL_H_
