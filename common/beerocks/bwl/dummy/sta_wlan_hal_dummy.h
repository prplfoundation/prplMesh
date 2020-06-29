/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_DUMMY_H_
#define _BWL_STA_WLAN_HAL_DUMMY_H_

#include "base_wlan_hal_dummy.h"
#include <bwl/sta_wlan_hal.h>

namespace bwl {
namespace dummy {

/*!
 * Hardware abstraction layer for WLAN Station/Client.
 */
class sta_wlan_hal_dummy : public base_wlan_hal_dummy, public sta_wlan_hal {

    // Public methods
public:
    /*!
     * Constructor.
     *
     * @param [in] iface_name STA/Client interface name.
     * @param [in] callback Callback for handling internal events.
     */
    sta_wlan_hal_dummy(const std::string &iface_name, hal_event_cb_t callback);
    virtual ~sta_wlan_hal_dummy();

    virtual bool detach() override;

    virtual bool initiate_scan() override;
    virtual int get_scan_results(const std::string &ssid, std::vector<SScanResult> &list,
                                 bool parse_vsie = false) override;

    virtual bool connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                         bool mem_only_psk, const std::string &bssid, uint8_t channel,
                         bool hidden_ssid) override;

    virtual bool disconnect() override;

    virtual bool roam(const sMacAddr &bssid, uint8_t channel) override;

    virtual bool get_4addr_mode() override;
    virtual bool set_4addr_mode(bool enable) override;

    virtual bool unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                          int vht_center_frequency, int delay,
                                          int window_size) override;

    virtual bool is_connected() override;
    virtual int get_channel() override;
    virtual bool update_status() override;

    std::string get_ssid() override;
    std::string get_bssid() override;

protected:
    virtual bool process_dummy_data(parsed_obj_map_t &parsed_obj) override;
    virtual bool process_dummy_event(parsed_obj_map_t &parsed_obj) override;

    // Overload for Monitor events
    bool event_queue_push(sta_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

private:
    // Active network parameters
    std::string m_active_ssid;
    std::string m_active_bssid;
    uint8_t m_active_channel = 0;
};

} // namespace dummy
} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_DUMMY_H_
