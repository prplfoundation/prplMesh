/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_NL80211_H_
#define _BWL_BASE_WLAN_HAL_NL80211_H_

#include <bwl/base_wlan_hal.h>
#include <bwl/nl80211_client.h>

#include <bcl/beerocks_state_machine.h>

#include <chrono>
#include <list>
#include <memory>
#include <unordered_map>

// Forward declaration
struct wpa_ctrl;
struct nl_sock;
struct nl_msg;

namespace bwl {
namespace nl80211 {

enum class nl80211_fsm_state { Delay, Init, GetRadioInfo, Attach, Operational, Detach };
enum class nl80211_fsm_event { Attach, Detach };

/*!
 * Base class for the wav abstraction layer.
 * Read more about virtual inheritance: https://en.wikipedia.org/wiki/Virtual_inheritance
 */
class base_wlan_hal_nl80211
    : public virtual base_wlan_hal,
      protected beerocks::beerocks_fsm<nl80211_fsm_state, nl80211_fsm_event> {

    // Public types:
public:
    typedef std::unordered_map<std::string, std::string> parsed_obj_map_t;
    typedef std::list<parsed_obj_map_t> parsed_obj_listed_map_t;

    // Public methods
public:
    virtual ~base_wlan_hal_nl80211();

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
    base_wlan_hal_nl80211(HALType type, std::string iface_name, hal_event_cb_t callback,
                          int wpa_ctrl_buffer_size, hal_conf_t hal_conf = {});

    // Process hostapd/wpa_supplicant event
    virtual bool process_nl80211_event(parsed_obj_map_t &event) = 0;

    bool set(const std::string &param, const std::string &value,
             int vap_id = beerocks::IFACE_RADIO_ID);

    // Send a message via the WPA Control Interface
    bool wpa_ctrl_send_msg(const std::string &cmd, parsed_obj_map_t &reply);
    bool wpa_ctrl_send_msg(const std::string &cmd, parsed_obj_listed_map_t &reply);
    bool wpa_ctrl_send_msg(const std::string &cmd, char **reply); // for external process
    bool wpa_ctrl_send_msg(const std::string &cmd);

    virtual void send_ctrl_iface_cmd(std::string cmd); // HACK for development, to be removed

    // Send NL80211 message
    bool send_nl80211_msg(uint8_t command, int flags,
                          std::function<bool(struct nl_msg *msg)> msg_create,
                          std::function<bool(struct nl_msg *msg)> msg_handle);

    std::unique_ptr<nl80211_client> m_nl80211_client;

    // Private data-members:
private:
    bool fsm_setup();

    // FSM State and Timeout
    nl80211_fsm_state m_last_attach_state = nl80211_fsm_state::Detach;
    std::chrono::steady_clock::time_point m_state_timeout;

    // WPA Control Interface Objects
    struct wpa_ctrl *m_wpa_ctrl_cmd   = nullptr;
    struct wpa_ctrl *m_wpa_ctrl_event = nullptr;

    // NL80211 Socket
    std::shared_ptr<struct nl_sock> m_nl80211_sock;
    int m_nl80211_id  = 0;
    int m_iface_index = 0;

    // WPA Control Interface Communication Buffer
    std::shared_ptr<char> m_wpa_ctrl_buffer;
    size_t m_wpa_ctrl_buffer_size = 0;

    // Path for the WPA Control Interface Socket
    std::string m_wpa_ctrl_path;
};

} // namespace nl80211
} // namespace bwl

#endif // _BWL_BASE_WLAN_HAL_NL80211_H_
