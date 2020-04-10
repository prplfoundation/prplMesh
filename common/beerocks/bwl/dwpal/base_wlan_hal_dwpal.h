/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_DWPAL_H_
#define _BWL_BASE_WLAN_HAL_DWPAL_H_

#include <bwl/base_wlan_hal.h>
#include <bwl/nl80211_client.h>

#include <bcl/beerocks_state_machine.h>

extern "C" {
#include <dwpal.h>
}

#include <chrono>
#include <memory>

namespace bwl {
namespace dwpal {

enum class dwpal_fsm_state { Delay, Init, GetRadioInfo, AttachVaps, Attach, Operational, Detach };

enum class dwpal_fsm_event { Attach, Detach };

/*!
 * Base class for the dwpal abstraction layer.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class base_wlan_hal_dwpal : public virtual base_wlan_hal,
                            protected beerocks::beerocks_fsm<dwpal_fsm_state, dwpal_fsm_event> {

    // Public methods:
public:
    virtual ~base_wlan_hal_dwpal();

    virtual HALState attach(bool block = false) override;
    virtual bool detach() override;
    virtual bool ping() override;
    virtual bool refresh_radio_info() override;
    virtual bool refresh_vaps_info(int id) override;
    virtual bool process_ext_events() override;
    virtual bool process_nl_events() override;
    virtual std::string get_radio_mac() override;

    // Protected methods
protected:
    base_wlan_hal_dwpal(HALType type, std::string iface_name, hal_event_cb_t callback,
                        hal_conf_t hal_conf = {});

    // Process dwpal event
    virtual bool process_dwpal_event(char *buffer, int bufLen, const std::string &opcode) = 0;
    virtual bool process_dwpal_nl_event(struct nl_msg *msg)                               = 0;

    bool set(const std::string &param, const std::string &value,
             int vap_id = beerocks::IFACE_RADIO_ID);

    bool dwpal_send_cmd(const std::string &cmd, char **reply,
                        int vap_id = beerocks::IFACE_RADIO_ID); // for external process
    bool dwpal_send_cmd(const std::string &cmd, int vap_id = beerocks::IFACE_RADIO_ID);
    bool attach_ctrl_interface(int vap_id);

    /**
     * @brief handle get data cmd from netlink
     * @param ifname radio interface name
     * @param nl_cmd netlink get command number
     * @param out_buffer pointer to data buffer for the result
     * @param max_buffer_size buffer size
     * @return size of returned data (success if > 0)
     */
    size_t dwpal_nl_cmd_get(const std::string &ifname, unsigned int nl_cmd,
                            unsigned char *out_buffer, const size_t max_buffer_size);
    /**
     * @brief handle set vendor data cmd to netlink
     * @param ifname radio interface name
     * @param nl_cmd netlink set command number
     * @param vendor_data pointer to vendor data buffer
     * @param vendor_data_size size of data
     * @return true on success
     * @return false on failure
     */
    bool dwpal_nl_cmd_set(const std::string &ifname, unsigned int nl_cmd, const void *vendor_data,
                          size_t vendor_data_size);
    bool dwpal_nl_cmd_scan_dump();
    void *get_dwpal_nl_ctx() const { return (m_dwpal_nl_ctx); }

    std::unique_ptr<nl80211_client> m_nl80211_client;

    // Private data-members:
private:
    const uint32_t AP_ENABLED_TIMEOUT_SEC           = 15;
    const uint32_t AP_ENABLED_FIXED_DFS_TIMEOUT_SEC = 660;

    bool fsm_setup();
    bool refresh_vap_info(int vap_id);

    dwpal_fsm_state m_last_attach_state = dwpal_fsm_state::Detach;

    std::chrono::steady_clock::time_point m_state_timeout;

    void *m_dwpal_ctx[beerocks::IFACE_TOTAL_VAPS] = {nullptr};
    void *m_dwpal_nl_ctx                          = nullptr;

    int m_fd_nl_cmd_get = -1;

    char m_wpa_ctrl_buffer[HOSTAPD_TO_DWPAL_MSG_LENGTH];
    size_t m_wpa_ctrl_buffer_size = HOSTAPD_TO_DWPAL_MSG_LENGTH;

    int m_nl_get_failed_attempts = 0;
};

} // namespace dwpal
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_DWPAL_H_
