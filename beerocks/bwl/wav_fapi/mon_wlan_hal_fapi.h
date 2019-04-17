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

#ifndef _BWL_MON_WLAN_HAL_FAPI_H_
#define _BWL_MON_WLAN_HAL_FAPI_H_

#include "base_wlan_hal_fapi.h"
#include "../common/mon_wlan_hal.h"

namespace bwl {
namespace wav_fapi {

/*!
 * Hardware abstraction layer for WLAN Monitor.
 */
class mon_wlan_hal_fapi : public base_wlan_hal_fapi, public mon_wlan_hal {

// Public methods
public:
    
    /*!
     * Constructor.
     *
     * @param [in] iface_name Monitor interface name.
     * @param [in] callback Callback for handling internal events.
     */
    mon_wlan_hal_fapi(std::string iface_name, hal_event_cb_t callback);
    virtual ~mon_wlan_hal_fapi();

    virtual bool update_radio_stats(SRadioStats& radio_stats) override;
    virtual bool update_vap_stats(const std::string vap_iface_name, SVapStats& vap_stats) override;
    virtual bool update_stations_stats(const std::string vap_iface_name, const std::string sta_mac, SStaStats& sta_stats) override;
    
    virtual bool sta_channel_load_11k_request(const SStaChannelLoadRequest11k& req) override;
    virtual bool sta_beacon_11k_request(const SBeaconRequest11k& req, int& dialog_token) override;
    virtual bool sta_statistics_11k_request(const SStatisticsRequest11k& req) override;
    virtual bool sta_link_measurements_11k_request(const std::string& sta_mac) override;

// Protected methods:
protected:
    
    virtual bool parse_fapi_event(const std::string& opcode, std::shared_ptr<void> obj) override;

    // Overload for Monitor events
    bool event_queue_push(mon_wlan_hal::Event event, std::shared_ptr<void> data = {})
    { return base_wlan_hal::event_queue_push(int(event), data); }

// Private data-members:
private:
    
    std::shared_ptr<char> m_temp_fapi_value;        
};

} // namespace wav_fapi
} // namespace bwl

#endif // _BWL_MON_WLAN_HAL_FAPI_H_