/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#ifndef _BWL_BASE_WLAN_HAL_FAPI_H_
#define _BWL_BASE_WLAN_HAL_FAPI_H_
 
#include "../common/base_wlan_hal.h"
#include "fapi_attach_fsm.h"

#include <memory>
#include <chrono>

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

    base_wlan_hal_fapi(HALType type, std::string iface_name, IfaceType iface_type, bool acs_enabled ,hal_event_cb_t callback);

    // Parse FAPI event
    virtual bool parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj) = 0;

// Private data-members:
private:

    std::shared_ptr<fapi_attach_fsm> m_fapi_attach_fsm;

    fapi_fsm_state m_last_attach_state = fapi_fsm_state::Detach;
};
 
} // namespace bwl
} // namespace wav_fapi 

#endif // _BWL_BASE_WLAN_HAL_FAPI_H_
