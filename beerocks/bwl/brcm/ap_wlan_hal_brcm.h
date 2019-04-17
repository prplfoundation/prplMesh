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

#ifndef _BWL_AP_WLAN_HAL_BRCM_H_
#define _BWL_AP_WLAN_HAL_BRCM_H_

#include "base_wlan_hal_brcm.h"
#include "../common/ap_wlan_hal.h"

namespace bwl {
namespace brcm {

/*!
 * Hardware abstraction layer for WLAN Access Point.
 */
class ap_wlan_hal_brcm : public base_wlan_hal_brcm, public ap_wlan_hal {

// Public methods
public:
    
    /*!
     * Constructor.
     *
     * @param [in] iface_name AP interface name.
     * @param [in] callback Callback for handling internal events.
     */
    ap_wlan_hal_brcm(std::string iface_name, bool acs_enabled, hal_event_cb_t callback);
    
    virtual ~ap_wlan_hal_brcm();

    virtual HALState attach(bool block = false) override;
    virtual bool detach() override;
    virtual bool process_ext_events() override;
    virtual bool sta_allow(const std::string& mac) override;
    virtual bool sta_deny(const std::string& mac, int reject_sta) override;
    virtual bool sta_disassoc(int8_t vap_id, const std::string& mac, uint32_t reason = 0) override;
    virtual bool sta_bss_steer(const std::string& mac, const std::string& bssid, int chan, int disassoc_timer, int valid_int) override;
    virtual bool sta_unassoc_rssi_measurement(const std::string& mac, int chan, int bw, int vht_center_frequency, int delay, int window_size) override;
    virtual bool switch_channel(int chan, int bw, int vht_center_frequency) override;
    virtual bool set_antenna_mode(AntMode mode) override;
    virtual bool wds_set_mode(WDSMode mode) override;
    virtual bool wds_add_sta(const std::string& mac) override;
    virtual bool wds_del_sta(const std::string& mac) override;
    virtual bool wds_clear_list() override;
    virtual bool failsafe_channel_set(int chan, int bw, int vht_center_frequency) override;
    virtual bool failsafe_channel_get(int& chan, int& bw) override;
    virtual bool restricted_channels_set(char* channel_list) override;
    virtual bool restricted_channels_get(char* channel_list) override;
    virtual bool read_acs_report() override;
    virtual std::string get_radio_driver_version() override;
    virtual bool set_vap_enable(const std::string& iface_name, const bool enable) override;
    virtual bool get_vap_enable(const std::string& iface_name, bool& enable) override;
    
// Protected methods:
protected:

    // Overload for AP events
    bool event_queue_push(ap_wlan_hal::Event event, std::shared_ptr<void> data = {})
    { return base_wlan_hal::event_queue_push(int(event), data); }
    
private:

    std::unordered_map<std::string, std::chrono::steady_clock::time_point> m_assoc_list;
};

} // namespace brcm
} // namespace bwl

#endif // _BWL_AP_WLAN_HAL_H_
 