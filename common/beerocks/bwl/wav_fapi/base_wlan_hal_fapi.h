/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_FAPI_H_
#define _BWL_BASE_WLAN_HAL_FAPI_H_

#include "../common/base_wlan_hal.h"
#include "fapi_attach_fsm.h"

#include <chrono>
#include <memory>

namespace bwl {
namespace wav_fapi {

// Forward declarations
class fapi_attach_fsm;

/*!
 * Base class for the UGW FAPI abstraction layer.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class base_wlan_hal_fapi : public virtual base_wlan_hal {

    // Public methods:
public:
    virtual ~base_wlan_hal_fapi();

    virtual HALState attach(bool block = false) override;
    virtual bool detach() override;
    virtual bool refresh_radio_info() override;
    virtual bool refresh_vaps_info(int id) override;
    virtual bool process_ext_events() override;
    virtual std::string get_radio_mac() override;

    // Protected methods
protected:
    base_wlan_hal_fapi(HALType type, std::string iface_name, IfaceType iface_type, bool acs_enabled,
                       hal_event_cb_t callback);

    // Parse FAPI event
    virtual bool parse_fapi_event(const std::string &opcode, std::shared_ptr<void> obj) = 0;

    // Private data-members:
private:
    std::shared_ptr<fapi_attach_fsm> m_fapi_attach_fsm;

    fapi_fsm_state m_last_attach_state = fapi_fsm_state::Detach;
};

} // namespace wav_fapi
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_FAPI_H_
