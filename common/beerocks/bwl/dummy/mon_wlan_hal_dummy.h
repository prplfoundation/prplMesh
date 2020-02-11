/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_MON_WLAN_HAL_DUMMY_H_
#define _BWL_MON_WLAN_HAL_DUMMY_H_

#include "base_wlan_hal_dummy.h"
#include <bwl/mon_wlan_hal.h>

namespace bwl {
namespace dummy {

/*!
 * Hardware abstraction layer for WLAN Monitor.
 */
class mon_wlan_hal_dummy : public base_wlan_hal_dummy, public mon_wlan_hal {

    // Public definitions
public:
    enum class Data { Invalid = 0, STA_Update_Stats, RRM_Update_Beacon_Measurements };

    // Public methods
public:
    /*!
     * Constructor.
     *
     * @param [in] iface_name Monitor interface name.
     * @param [in] callback Callback for handling internal events.
     */
    mon_wlan_hal_dummy(std::string iface_name, hal_event_cb_t callback);
    virtual ~mon_wlan_hal_dummy();

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
    virtual bool channel_scan_dump_results() override;
    // Protected methods:
protected:
    virtual bool process_dummy_event(parsed_obj_map_t &parsed_obj) override;
    virtual bool process_dummy_data(parsed_obj_map_t &parsed_obj) override;

    // Overload for Monitor events
    bool event_queue_push(mon_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

    virtual bool set(const std::string &param, const std::string &value, int vap_id) override;

    // Private data-members:
private:
    std::shared_ptr<char> m_temp_dummy_value;
    struct sDummyStaStats {
        SStaStats sta_stats;
        std::unordered_map<std::string, parsed_obj_map_t> beacon_measurment_events;
        // key=bssid, value=event object
    };
    std::unordered_map<std::string, struct sDummyStaStats> m_dummy_stas_map; // key=sta_mac
};

} // namespace dummy
} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_DUMMY_H_
