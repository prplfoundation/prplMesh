/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef _BWL_STA_WLAN_HAL_FAPI_H_
#define _BWL_STA_WLAN_HAL_FAPI_H_

#include "base_wlan_hal_fapi.h"
#include <bwl/sta_wlan_hal.h>

namespace bwl {
namespace wav_fapi {

/*!
 * Hardware abstraction layer for WLAN Station/Client.
 */
class sta_wlan_hal_fapi : public base_wlan_hal_fapi, public sta_wlan_hal {

    // Public methods
public:
    /*!
     * Constructor.
     *
     * @param [in] iface_name STA/Client interface name.
     * @param [in] callback Callback for handling internal events.
     */
    sta_wlan_hal_fapi(std::string iface_name, hal_event_cb_t callback);
    virtual ~sta_wlan_hal_fapi();

    virtual bool detach() override;

    virtual bool initiate_scan() override;
    virtual int get_scan_results(const std::string &ssid, net::sScanResult *list,
                                 int size) override;

    virtual bool connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                         const std::string &bssid, uint8_t channel, bool hidden_ssid) override;

    virtual bool disconnect() override;

    virtual bool roam(const std::string &bssid, uint8_t channel) override;

    virtual bool get_4addr_mode() override;
    virtual bool set_4addr_mode(bool enable) override;

    virtual bool unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                          int vht_center_frequency, int delay,
                                          int window_size) override;

    virtual bool is_connected() override;
    virtual int get_rssi() override;
    virtual int get_channel() override;

    std::string get_ssid() override;
    std::string get_bssid() override;

    // Protected methods:
protected:
    virtual bool parse_fapi_event(const std::string &opcode, std::shared_ptr<void> obj) override;

    // Overload for Monitor events
    bool event_queue_push(sta_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

    // Private data-members:
private:
    std::shared_ptr<char> m_temp_fapi_value;

    // Active network parameters
    std::string m_active_ssid;
    std::string m_active_bssid;
    std::string m_active_pass;
    WiFiSec m_active_security = WiFiSec::Invalid;
    uint8_t m_active_channel  = 0;
    int m_active_network_id   = -1;

    // Unassociated measurement state variables
    std::chrono::steady_clock::time_point m_unassoc_measure_start;
    int m_unassoc_measure_window_size = 0;
    int m_unassoc_measure_delay       = 0;
};

} // namespace wav_fapi
} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_FAPI_H_
