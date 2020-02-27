/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_MON_WLAN_HAL_NL80211_H_
#define _BWL_MON_WLAN_HAL_NL80211_H_

#include "base_wlan_hal_nl80211.h"
#include <bwl/mon_wlan_hal.h>

namespace bwl {
namespace nl80211 {

/*!
 * Hardware abstraction layer for WLAN Monitor.
 */
class mon_wlan_hal_nl80211 : public base_wlan_hal_nl80211, public mon_wlan_hal {

    // Public methods
public:
    /*!
     * Constructor.
     *
     * @param [in] iface_name Monitor interface name.
     * @param [in] callback Callback for handling internal events.
     */
    mon_wlan_hal_nl80211(std::string iface_name, hal_event_cb_t callback);
    virtual ~mon_wlan_hal_nl80211();

    virtual bool update_radio_stats(SRadioStats &radio_stats) override;
    virtual bool update_vap_stats(const std::string vap_iface_name, SVapStats &vap_stats) override;
    virtual bool update_stations_stats(const std::string vap_iface_name, const std::string sta_mac,
                                       SStaStats &sta_stats) override;

    virtual bool sta_channel_load_11k_request(const SStaChannelLoadRequest11k &req) override;
    virtual bool sta_beacon_11k_request(const SBeaconRequest11k &req, int &dialog_token) override;
    virtual bool sta_statistics_11k_request(const SStatisticsRequest11k &req) override;
    virtual bool sta_link_measurements_11k_request(const std::string &sta_mac) override;
    virtual bool channel_scan_trigger(int dwell_time_msec,
                                      const std::vector<unsigned int> &channel_pool) override;
    // Protected methods:
protected:
    virtual bool process_nl80211_event(parsed_obj_map_t &parsed_obj) override;

    // Overload for Monitor events
    bool event_queue_push(mon_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

    // Private data-members:
private:
    std::shared_ptr<char> m_temp_wav_value;
};

} // namespace nl80211
} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_NL80211_H_
