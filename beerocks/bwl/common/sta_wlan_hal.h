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

#ifndef _BWL_STA_WLAN_HAL_H_
#define _BWL_STA_WLAN_HAL_H_
 
#include "base_wlan_hal.h"
#include "sta_wlan_hal_types.h"

namespace bwl {
    
/*!
 * Hardware abstraction layer for WLAN Station/Client.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class sta_wlan_hal : public virtual base_wlan_hal {

// Public definitions
public:

    // STA Evnets
    enum class Event {
        Invalid = 0,
        
        Connected,
        Disconnected,
        Terminating,
        ScanResults,
        ChannelSwitch,
        
        STA_Unassoc_RSSI
    };

// Public methods:
public:

    virtual ~sta_wlan_hal() = default;

    virtual bool initiate_scan() = 0;
    virtual int  get_scan_results(const std::string& ssid, SScanResult* list, int size) = 0;

    virtual bool connect(const std::string& ssid, const std::string& pass, 
        WiFiSec sec, const std::string& bssid, uint8_t channel, bool hidden_ssid) = 0;

    virtual bool disconnect() = 0;

    virtual bool roam(const std::string& bssid, uint8_t channel) = 0;

    virtual bool get_4addr_mode() = 0;
    virtual bool set_4addr_mode(bool enable) = 0;

    virtual bool unassoc_rssi_measurement(const std::string& mac, int chan, int bw, 
        int vht_center_frequency, int delay, int window_size) = 0;

    virtual bool is_connected() = 0;
    virtual int  get_rssi() = 0;
    virtual int  get_channel() = 0;
    
    virtual std::string get_ssid() = 0;
    virtual std::string get_bssid() = 0;
};

} // namespace bwl

// STA HAL factory types
extern "C" {
    
    // Types
    typedef bwl::sta_wlan_hal* (*sta_wlan_hal_create_t)(std::string, bwl::base_wlan_hal::hal_event_cb_t);
    typedef void               (*sta_wlan_hal_destroy_t)(bwl::sta_wlan_hal*);
    
    // Prototypes
    bwl::sta_wlan_hal* sta_wlan_hal_create(std::string iface_name, bwl::base_wlan_hal::hal_event_cb_t cb);
    void               sta_wlan_hal_destroy(bwl::sta_wlan_hal* obj);
    
}
    
#endif // _BWL_STA_WLAN_HAL_H_
