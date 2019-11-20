/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_DWPAL_H_
#define _BWL_STA_WLAN_HAL_DWPAL_H_

#include "base_wlan_hal_dwpal.h"
#include <sta_wlan_hal.h>

namespace bwl {
namespace dwpal {

/*!
     * Hardware abstraction layer for WLAN Station/Client.
     */
class sta_wlan_hal_dwpal : public base_wlan_hal_dwpal, public sta_wlan_hal {

    // Public methods
public:
    /*!
             * Constructor.
             *
             * @param [in] iface_name STA/Client interface name.
             * @param [in] callback Callback for handling internal events.
             */
    sta_wlan_hal_dwpal(std::string iface_name, hal_event_cb_t callback);
    virtual ~sta_wlan_hal_dwpal();

    virtual bool initiate_scan() override;
    virtual int get_scan_results(const std::string &ssid, std::vector<SScanResult> &list,
                                 bool parse_vsie) override;

    virtual bool connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                         bool mem_only_psk, const std::string &bssid, uint8_t channel,
                         bool hidden_ssid) override;

    virtual bool disconnect() override;

    virtual bool roam(const std::string &bssid, uint8_t channel) override;

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
    virtual bool process_dwpal_event(char *buffer, int bufLen, const std::string &opcode) override;

    // Overload for Monitor events
    bool event_queue_push(sta_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

private:
    // Client connection status
    struct ConnectionStatus {
        std::string bssid;
        int freq;
        std::string ssid;
        int id;
        std::string mode;
        std::string pairwise_cipher;
        std::string group_cipher;
        std::string key_mgmt;
        std::string wpa_state;
        std::string address;
        std::string uuid;
    };

    int add_network();
    bool set_network(int network_id, const std::string &param, const std::string &value);
    bool set_network_params(int network_id, const std::string &ssid, const std::string &bssid,
                            WiFiSec sec, bool mem_only_psk, const std::string &pass,
                            bool hidden_ssid, int freq = 0);
    bool read_status(ConnectionStatus &connection_status);
    void update_status(const ConnectionStatus &connection_status);
    bool enable_network(int network_id);
    bool is_connected(const std::string &wpa_state);

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

} // namespace dwpal
} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_DWPAL_H_
