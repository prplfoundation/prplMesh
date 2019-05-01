/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_AP_WLAN_HAL_WAV_H_
#define _BWL_AP_WLAN_HAL_WAV_H_

#include "base_wlan_hal_wav.h"
#include "../common/ap_wlan_hal.h"

namespace bwl {
namespace wav {

/*!
 * Hardware abstraction layer for WLAN Access Point.
 */
class ap_wlan_hal_wav : public base_wlan_hal_wav, public ap_wlan_hal {

// Public methods
public:
    
    /*!
     * Constructor.
     *
     * @param [in] iface_name AP interface name.
     * @param [in] callback Callback for handling internal events.
     */
    ap_wlan_hal_wav(std::string iface_name, bool acs_enabled, hal_event_cb_t callback);
    
    virtual ~ap_wlan_hal_wav();

    virtual HALState attach(bool block = false) override;
    virtual bool sta_allow(const std::string& mac) override;
    virtual bool sta_deny(const std::string& mac, int reject_sta) override;
    virtual bool sta_disassoc(int8_t vap_id, const std::string& mac, uint32_t reason = 0) override;
    virtual bool sta_deauth(int8_t vap_id, const std::string& mac, uint32_t reason = 0) override;
    virtual bool sta_bss_steer(const std::string& mac, const std::string& bssid, int chan, int disassoc_timer, int valid_int) override;
    virtual bool sta_unassoc_rssi_measurement(const std::string& mac, int chan, int bw, int vht_center_frequency, int delay, int window_size) override;
    virtual bool sta_softblock_add(const std::string& vap_name, const std::string& client_mac, uint8_t reject_error_code, 
                                        uint8_t probe_rssi_threshold_hi, uint8_t probe_rssi_threshold_lo, 
                                        uint8_t authetication_rssi_threshold_hi, uint8_t authetication_rssi_threshold_lo) override { return false;}

    // TODO: To be removed?  since hostapd doesn't support removeing STA from softblock list.
    virtual bool sta_soft_block_remove(const std::string& iface_name) override {return false;}
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

    virtual bool process_wav_event(parsed_obj_map_t& parsed_obj) override;

    // Overload for AP events
    bool event_queue_push(ap_wlan_hal::Event event, std::shared_ptr<void> data = {})
    { return base_wlan_hal::event_queue_push(int(event), data); }
    
private:

    // Unassociated measurement state variables
    std::chrono::steady_clock::time_point   m_unassoc_measure_start;
    int                                     m_unassoc_measure_window_size   = 0;
    int                                     m_unassoc_measure_delay         = 0;

    bool                                    m_drop_csa = false;
    std::chrono::steady_clock::time_point   m_csa_event_filtering_timestamp;
};

} // namespace wav
} // namespace bwl

#endif // _BWL_AP_WLAN_HAL_H_
 