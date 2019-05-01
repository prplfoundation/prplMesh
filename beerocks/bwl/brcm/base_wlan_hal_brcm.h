/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_BRCM_H_
#define _BWL_BASE_WLAN_HAL_BRCM_H_
 
#include "../common/base_wlan_hal.h"

#include <memory>
#include <chrono>

namespace bwl {
namespace brcm {

/*!
 * Base class for the BRCM abstraction layer.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class base_wlan_hal_brcm : public virtual base_wlan_hal {

// Public methods:
public:
 
    virtual ~base_wlan_hal_brcm();

    virtual bool refresh_radio_info() override;
    virtual bool refresh_vaps_info(int id) override;
    virtual std::string get_radio_mac() override;

// methods
protected:

    base_wlan_hal_brcm(HALType type, std::string iface_name, bool acs_enabled, IfaceType iface_type, hal_event_cb_t callback);

// Protected  data-members:
    char wl_ioctl_buf[8192];

};
 
} // namespace bwl
} // namespace brcm

#endif // _BWL_BASE_WLAN_HAL_BRCM_H_
