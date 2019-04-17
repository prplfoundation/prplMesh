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

#ifndef _BWL_MON_WLAN_HAL_H_
#define _BWL_MON_WLAN_HAL_H_
 
#include "base_wlan_hal.h"
#include "mon_wlan_hal_types.h"
 
namespace bwl {
 
 /*!
  * Hardware abstraction layer for WLAN Monitor.
  * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
  */
class mon_wlan_hal : public virtual base_wlan_hal {

// Public definitions
public:

    enum class Event {
        Invalid = 0,
        
        AP_Enabled,
        AP_Disabled,
        // RRM (802.11k) Events
        RRM_Channel_Load_Response,
        RRM_Beacon_Response,
        RRM_STA_Statistics_Response,
        RRM_Link_Measurement_Response
    };       

// Public methods:
public:

    virtual ~mon_wlan_hal() = default;

    virtual bool update_radio_stats(SRadioStats& radio_stats) = 0;
    virtual bool update_vap_stats(const std::string vap_iface_name, SVapStats& vap_stats) = 0; 
    virtual bool update_stations_stats(const std::string vap_iface_name, const std::string sta_mac, SStaStats& sta_stats) = 0;

    virtual bool sta_channel_load_11k_request(const SStaChannelLoadRequest11k& req) = 0;
    virtual bool sta_beacon_11k_request(const SBeaconRequest11k& req, int& dialog_token) = 0;
    virtual bool sta_statistics_11k_request(const SStatisticsRequest11k& req) = 0;
    virtual bool sta_link_measurements_11k_request(const std::string& sta_mac) = 0;
};

} // namespace bwl

// Monitor HAL factory types
extern "C" {

// Types
typedef bwl::mon_wlan_hal* (*mon_wlan_hal_create_t)(std::string, bwl::base_wlan_hal::hal_event_cb_t);
typedef void               (*mon_wlan_hal_destroy_t)(bwl::mon_wlan_hal*);

// Prototypes
bwl::mon_wlan_hal* mon_wlan_hal_create(std::string iface_name, bwl::base_wlan_hal::hal_event_cb_t cb);
void               mon_wlan_hal_destroy(bwl::mon_wlan_hal* obj);

}

#endif // _BWL_MON_WLAN_HAL_H_
