/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "base_wlan_hal_dwpal.h"

#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>
#include <bwl/nl80211_client_factory.h>

#include <easylogging++.h>

extern "C" {
#include <dwpal.h>
}

#define UNHANDLED_EVENTS_LOGS 20
#define MAX_FAILED_NL_MSG_GET 3

namespace bwl {
namespace dwpal {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Functions ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &out, const dwpal_fsm_state &value)
{
    switch (value) {
    case dwpal_fsm_state::Delay:
        out << "Delay";
        break;
    case dwpal_fsm_state::Init:
        out << "Init";
        break;
    case dwpal_fsm_state::GetRadioInfo:
        out << "GetRadioInfo";
        break;
    case dwpal_fsm_state::AttachVaps:
        out << "AttachVaps";
        break;
    case dwpal_fsm_state::Attach:
        out << "Attach";
        break;
    case dwpal_fsm_state::Operational:
        out << "Operational";
        break;
    case dwpal_fsm_state::Detach:
        out << "Detach";
        break;
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const dwpal_fsm_event &value)
{
    switch (value) {
    case dwpal_fsm_event::Attach:
        out << "Attach";
        break;
    case dwpal_fsm_event::Detach:
        out << "Detach";
        break;
    }
    return out;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

base_wlan_hal_dwpal::base_wlan_hal_dwpal(HALType type, std::string iface_name,
                                         hal_event_cb_t callback, hal_conf_t hal_conf)
    : base_wlan_hal(type, iface_name, IfaceType::Intel, callback, hal_conf),
      beerocks::beerocks_fsm<dwpal_fsm_state, dwpal_fsm_event>(dwpal_fsm_state::Delay),
      m_nl80211_client(nl80211_client_factory::create_instance())
{
    LOG_IF(!m_nl80211_client, FATAL) << "Failed to create nl80211_client instance";
    // Initialize the FSM
    fsm_setup();
}

base_wlan_hal_dwpal::~base_wlan_hal_dwpal() { base_wlan_hal_dwpal::detach(); }

bool base_wlan_hal_dwpal::fsm_setup()
{
    config()

        // Setup states:

        //////////////////////////////////////////////////////////////////////////
        ////////////////////////////// State: Delay //////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::Delay)

        // On Entry
        .entry([&](const void *args) -> bool {
            m_state_timeout = std::chrono::steady_clock::now() + std::chrono::seconds(5);
            return true;
        })

        // EVENT -> Attach
        .on(dwpal_fsm_event::Attach, dwpal_fsm_state::Init,
            [&](TTransition &transition, const void *args) -> bool {
                return (std::chrono::steady_clock::now() >= m_state_timeout);
            })

        //////////////////////////////////////////////////////////////////////////
        ////////////////////////////// State: Init ///////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::Init)

        .entry([&](const void *args) -> bool {
            m_state_timeout = std::chrono::steady_clock::now() + std::chrono::seconds(200);
            return true;
        })

        // Handle "Detach" event
        .on(dwpal_fsm_event::Detach, dwpal_fsm_state::Detach)

        // Handle "Attach" event
        .on(dwpal_fsm_event::Attach,
            {dwpal_fsm_state::Attach, dwpal_fsm_state::Detach, dwpal_fsm_state::GetRadioInfo},
            [&](TTransition &transition, const void *args) -> bool {
                // Open and attach a control interface to wpa_supplicant/hostapd.
                // Check if not exist before
                bool attached = false;
                if (m_dwpal_ctx[0] == nullptr) {
                    attached = attach_ctrl_interface(beerocks::IFACE_RADIO_ID);
                } else {
                    //Already attached
                    attached = true;
                }

                //Attach NL interface
                if (!m_dwpal_nl_ctx) {
                    //Attach nl interface
                    if (dwpal_driver_nl_attach(&m_dwpal_nl_ctx) == DWPAL_FAILURE) {
                        LOG(ERROR)
                            << "dwpal_driver_nl_attach returned ERROR for: "
                            << m_radio_info.iface_name << "disbling netlink for this platform";
                        m_dwpal_nl_ctx = nullptr;
                    } else {
                        LOG(DEBUG)
                            << "dwpal_driver_nl_attach() success for: " << m_radio_info.iface_name
                            << ", nl_context = " << m_dwpal_nl_ctx;
                    }
                }

                if (attached) {
                    if (get_type() != HALType::Station) {
                        transition.change_destination(dwpal_fsm_state::GetRadioInfo);
                    }
                    return true;
                }

                // False if timeout not reached yet, and True otherwise (switch state)
                if (std::chrono::steady_clock::now() >= m_state_timeout) {
                    LOG(ERROR) << "Failed attaching to the hostapd control interface of "
                               << m_radio_info.iface_name;
                    return (transition.change_destination(dwpal_fsm_state::Detach));
                }

                // Stay in the current state
                return false;
            })

        //////////////////////////////////////////////////////////////////////////
        ////////////////////////// State: GetRadioInfo ///////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::GetRadioInfo)

        .entry([&](const void *args) -> bool {
            m_state_timeout =
                std::chrono::steady_clock::now() + std::chrono::seconds(AP_ENABLED_TIMEOUT_SEC);
            return true;
        })

        // Handle "Detach" event
        .on(dwpal_fsm_event::Detach, dwpal_fsm_state::Detach)

        .on(dwpal_fsm_event::Attach, {dwpal_fsm_state::AttachVaps, dwpal_fsm_state::Detach},
            [&](TTransition &transition, const void *args) -> bool {
                // Attempt to read radio info
                if (!refresh_radio_info()) {
                    return (transition.change_destination(dwpal_fsm_state::Detach));
                }

                // If Non-AP HAL, continue to the next state
                if (get_type() != HALType::AccessPoint) {
                    return true;
                }

                auto fixed_channel =
                    (!get_hal_conf().ap_acs_enabled && !m_radio_info.is_dfs_channel);
                auto fixed_dfs_channel =
                    (!get_hal_conf().ap_acs_enabled && m_radio_info.is_dfs_channel);
                LOG(DEBUG) << "hal_conf.ap_acs_enabled=" << int(get_hal_conf().ap_acs_enabled)
                           << ", m_radio_info.is_dfs_channel=" << int(m_radio_info.is_dfs_channel)
                           << ", fixed_dfs_channel=" << int(fixed_dfs_channel)
                           << ", fixed_channel=" << int(fixed_channel);

                // AP is Enabled
                LOG(DEBUG) << "wifi_ctrl_enabled  = " << int(m_radio_info.wifi_ctrl_enabled);

                bool error = false;
                if (fixed_dfs_channel) {
                    if (m_radio_info.tx_enabled) {
                        LOG(DEBUG) << "ap_enabled tx = " << int(m_radio_info.tx_enabled);
                        return true;
                    } else {
                        LOG(ERROR) << "ap_enabled with tx OFF!";
                        error = true;
                    }
                } else if (fixed_channel) {
                    if (m_radio_info.wifi_ctrl_enabled == 1) {
                        if (m_radio_info.tx_enabled) {
                            LOG(DEBUG) << "ap_enabled tx = " << int(m_radio_info.tx_enabled);
                            return true;
                        } else {
                            LOG(ERROR) << "ap_enabled with tx OFF!";
                            error = true;
                        }
                    }
                } else { // Auto Channel
                    if (m_radio_info.wifi_ctrl_enabled == 1) {
                        if (m_radio_info.tx_enabled) {
                            LOG(DEBUG) << "ap_enabled tx = " << int(m_radio_info.tx_enabled);
                            return true;
                        } else {
                            LOG(ERROR) << "ap_enabled with tx OFF!";
                            error = true;
                        }
                    } else if (m_radio_info.wifi_ctrl_enabled != 0) {
                        LOG(ERROR)
                            << "Auto channel, invalid wifi_ctrl_enabled value. wifi_ctrl_enabled="
                            << m_radio_info.wifi_ctrl_enabled;
                        error = true;
                    }
                }

                if (error || (std::chrono::steady_clock::now() >= m_state_timeout)) {
                    return (transition.change_destination(dwpal_fsm_state::Detach));
                }

                // Remain in the current state
                return false;
            })

        //////////////////////////////////////////////////////////////////////////
        ////////////////////////// State: AttachVaps ///////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::AttachVaps)

        .entry([&](const void *args) -> bool { return true; })

        // Handle "Detach" event
        .on(dwpal_fsm_event::Detach, dwpal_fsm_state::Detach)

        .on(dwpal_fsm_event::Attach, {dwpal_fsm_state::Attach, dwpal_fsm_state::Detach},
            [&](TTransition &transition, const void *args) -> bool {
                uint attached = 0;
                for (auto &vap : m_radio_info.available_vaps) {
                    if (attach_ctrl_interface(vap.first)) {
                        attached++;
                    }
                }

                if (attached != m_radio_info.available_vaps.size()) {
                    return (transition.change_destination(dwpal_fsm_state::Detach));
                }

                // move to next state
                return true;
            })

        //////////////////////////////////////////////////////////////////////////
        ///////////////////////////// State: Attach //////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::Attach)

        // Handle "Detach" event
        .on(dwpal_fsm_event::Detach, dwpal_fsm_state::Detach)

        .on(dwpal_fsm_event::Attach,
            {dwpal_fsm_state::Operational, dwpal_fsm_state::Delay, dwpal_fsm_state::Detach},
            [&](TTransition &transition, const void *args) -> bool {
                // Get the wpa_supplicant/hostapd event interface file descriptor
                if (m_dwpal_ctx[0] != nullptr) {
                    if (dwpal_hostap_event_fd_get(m_dwpal_ctx[0], &m_fd_ext_events)) {
                        LOG(ERROR) << "dwpal_hostap_event_fd_get() failed for: "
                                   << m_radio_info.iface_name;
                        return (transition.change_destination(dwpal_fsm_state::Detach));
                    }
                } else {
                    LOG(ERROR) << "Can't get event fd since dwpal ctx is NULL!";
                    return (transition.change_destination(dwpal_fsm_state::Detach));
                }

                // Success
                LOG(DEBUG)
                    << "Open and attach an event interface to wpa_supplicant/hostapd - SUCCESS!";

                // Get the nl event interface file descriptor
                if (m_dwpal_nl_ctx) {
                    if (dwpal_driver_nl_fd_get(m_dwpal_nl_ctx, &m_fd_nl_events, &m_fd_nl_cmd_get)) {
                        LOG(ERROR) << "getting nl fd failed for: " << m_radio_info.iface_name
                                   << "disbling netlink for this platform";
                        m_dwpal_nl_ctx = nullptr;
                    } else {
                        LOG(DEBUG) << "Attach event interface to nl - SUCCESS! for: "
                                   << m_radio_info.iface_name << " nl_fd =  " << m_fd_nl_events
                                   << " fdcmdget_nl = " << m_fd_nl_cmd_get;
                    }
                } else {
                    LOG(ERROR) << "NL ctx is NULL! netlink is disabled for this platform";
                }

                return true;
            })

        //////////////////////////////////////////////////////////////////////////
        /////////////////////////// State: Operational ///////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::Operational)

        // Handle "Detach" event
        .on(dwpal_fsm_event::Detach, dwpal_fsm_state::Detach)

        //////////////////////////////////////////////////////////////////////////
        ////////////////////////////// State: Detach /////////////////////////////
        //////////////////////////////////////////////////////////////////////////

        .state(dwpal_fsm_state::Detach)

        .entry([&](const void *args) -> bool {
            bool success = true;

            LOG(DEBUG) << "dwpal_fsm_state::Detach";

            for (int i = 0; i < beerocks::IFACE_TOTAL_VAPS; i++) {
                if (m_dwpal_ctx[i]) {
                    LOG(INFO) << "Call DWPAL socket close";
                    DWPAL_Ret ret = dwpal_hostap_socket_close(&m_dwpal_ctx[i]);

                    if (ret == DWPAL_SUCCESS) {
                        m_dwpal_ctx[i] = nullptr;
                    } else {
                        LOG(ERROR) << "DWPAL socket close failed, ret=" << ret;
                        success = false;
                    }
                }
            }

            m_fd_ext_events = -1;

            // detach nl socket from main vap
            if (m_dwpal_nl_ctx) {
                LOG(DEBUG) << "detaching nl interface";
                if (dwpal_driver_nl_detach(&m_dwpal_nl_ctx) == DWPAL_FAILURE) {
                    LOG(ERROR) << "dwpal_driver_nl_detach() failed for radio ="
                               << m_radio_info.iface_name;
                }
                m_fd_nl_events  = -1;
                m_fd_nl_cmd_get = -1;
            }

            return success;
        })

        // Handle "Attach" event
        .on(dwpal_fsm_event::Attach, dwpal_fsm_state::Delay);

    // Start the FSM
    return (start());
}

HALState base_wlan_hal_dwpal::attach(bool block)
{
    while (true) {

        fire(dwpal_fsm_event::Attach);
        auto attach_state = state();
        if (m_last_attach_state != attach_state) {
            LOG(DEBUG) << "DWPAL FSM " << m_radio_info.iface_name
                       << " Attach State: " << attach_state;
            m_last_attach_state = attach_state;
        }

        switch (attach_state) {
        // Initializing
        case dwpal_fsm_state::Delay:
        case dwpal_fsm_state::Init:
        case dwpal_fsm_state::GetRadioInfo:
        case dwpal_fsm_state::AttachVaps:
        case dwpal_fsm_state::Attach: {
            if (block) {
                // TODO: Delay?
                continue;
            } else {
                return (m_hal_state = HALState::Initializing);
            }
        }

        // Initialization completed
        case dwpal_fsm_state::Operational: {
            return (m_hal_state = HALState::Operational);
        }

        // Initialization failed
        case dwpal_fsm_state::Detach: {
            return (m_hal_state = HALState::Failed);
        }

        // Invalid state
        default: {
            LOG(ERROR) << "Invalid DWPAL Attach State: " << int(attach_state);
        }
        }
    };
}

bool base_wlan_hal_dwpal::detach()
{
    fire(dwpal_fsm_event::Detach);
    return (state() == dwpal_fsm_state::Detach);
}

bool base_wlan_hal_dwpal::set(const std::string &param, const std::string &value, int vap_id)
{
    const std::string cmd = "SET " + param + " " + value;
    if (!dwpal_send_cmd(cmd, vap_id)) {
        LOG(ERROR) << "FAiled setting param " << param;
        return false;
    }

    return true;
}

bool base_wlan_hal_dwpal::ping()
{
    if (!dwpal_send_cmd("PING")) {
        return false;
    }

    return true;
}

bool base_wlan_hal_dwpal::dwpal_send_cmd(const std::string &cmd, int vap_id)
{
    int result;
    int try_cnt = 0;

    auto buffer         = m_wpa_ctrl_buffer;
    auto buff_size_copy = m_wpa_ctrl_buffer_size;
    int ctx_index       = vap_id + 1;

    if (ctx_index < 0 || ctx_index >= beerocks::IFACE_TOTAL_VAPS) {
        LOG(ERROR) << "ctx_index " << ctx_index << " is out of bounds";
        return false;
    }

    if (!m_dwpal_ctx[ctx_index]) {
        LOG(ERROR) << "m_dwpal_ctx[" << ctx_index << "]=nullptr";
        return false;
    }

    do {
        //LOG(DEBUG) << "Send dwpal cmd: " << cmd.c_str();
        result = dwpal_hostap_cmd_send(m_dwpal_ctx[ctx_index], cmd.c_str(), NULL, buffer,
                                       &buff_size_copy);
        if (result != 0) {
            LOG(DEBUG) << "Failed to send cmd to DWPAL: " << cmd << " --> Retry";
        }
    } while (result != 0 && ++try_cnt < 3);

    if (result < 0) {
        LOG(ERROR) << "can't send wpa_ctrl_request";
        LOG(ERROR) << "failed cmd: " << cmd;
        return false;
    }

    if (buff_size_copy >= m_wpa_ctrl_buffer_size) {
        LOG(ERROR) << "wpa_ctrl_request returned reply of size " << buff_size_copy;
        return false;
    }

    if ((!strncmp(buffer, "FAIL", 4)) || (!strncmp(buffer, "UNKNOWN", 7))) {
        LOG(DEBUG) << std::endl << "cmd failed: " << cmd;
        LOG(DEBUG) << std::endl << "reply: " << buffer;
        return false;
    }

    return true;
}

bool base_wlan_hal_dwpal::dwpal_send_cmd(const std::string &cmd, char **reply, int vap_id)
{
    if (!reply) {
        LOG(ERROR) << "Invalid reply pointer!";
        return false;
    }

    if (!dwpal_send_cmd(cmd, vap_id)) {
        return false;
    }

    *reply = m_wpa_ctrl_buffer;

    return true;
}

bool base_wlan_hal_dwpal::attach_ctrl_interface(int vap_id)
{
    int ctx_index = vap_id + 1;
    if (ctx_index < 0 || ctx_index >= beerocks::IFACE_TOTAL_VAPS) {
        LOG(ERROR) << "ctx_index " << ctx_index << " is out of bounds";
        return false;
    }

    if (m_dwpal_ctx[ctx_index]) {
        LOG(ERROR) << "m_dwpal_ctx[" << ctx_index << "] already attached";
        return false;
    }

    const std::string ifname =
        beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    int result = dwpal_hostap_interface_attach(&m_dwpal_ctx[ctx_index], ifname.c_str(), nullptr);

    if ((result == 0) && m_dwpal_ctx[ctx_index]) {
        LOG(DEBUG) << "dwpal_hostap_interface_attach() success for: " << ifname;
    } else if (result != 0) {
        LOG(ERROR) << "dwpal_hostap_interface_attach() failed for: " << ifname;
        return false;
    } else {
        LOG(ERROR) << "dwpal_hostap_interface_attach() returns 0 for: " << ifname
                   << " but context is NULL!";
        return false;
    }

    return true;
}

bool base_wlan_hal_dwpal::process_nl_events()
{
    if (!m_dwpal_nl_ctx) {
        LOG(ERROR) << "Invalid Netlink socket used for nl events (m_dwpal_nl_ctx == nullptr)";
        return false;
    }

    // check if there is nothing to proccess
    if (m_fd_nl_events <= 0) {
        LOG(ERROR) << __func__ << "nothing to proccess fd= " << m_fd_nl_events;
        return false;
    }

    // Passing a lambda with capture is not supported for standard C function
    // pointers. As a workaround, we create a static (but thread local) wrapper
    // function that calls the capturing lambda function.
    static __thread std::function<DWPAL_Ret(struct nl_msg * msg)> nl_handler_cb_wrapper;
    nl_handler_cb_wrapper = [&](struct nl_msg *msg) -> DWPAL_Ret {
        if (!process_dwpal_nl_event(msg)) {
            LOG(ERROR) << "User's netlink handler function failed!";
            return DWPAL_FAILURE;
        }
        return DWPAL_SUCCESS;
    };
    auto nl_handler_cb = [](struct nl_msg *msg) -> DWPAL_Ret { return nl_handler_cb_wrapper(msg); };

    // parsing is done in callback function
    // Added failure on attempt counter for dwpal_driver_nl_msg_get operation to prevent unnecessary
    // monitor thread restarts in case of a one-time buffer glitch in dwpal-nl-parser.
    // The above-mentioned scenario can happen when 2 sockets are using dwpal_driver_nl_msg_get
    // simultaneously and receiving an event at the same time.
    // If max-retries is reached, the process_nl_events() fails and the
    // monitor_thread_stop is performed.
    if (dwpal_driver_nl_msg_get(m_dwpal_nl_ctx, DWPAL_NL_UNSOLICITED_EVENT, NULL, nl_handler_cb) ==
        DWPAL_FAILURE) {
        m_nl_get_failed_attempts++;

        LOG(ERROR) << " dwpal_driver_nl_msg_get failed,"
                   << " ctx=" << m_dwpal_nl_ctx
                   << ", nl-get-failed-attempts=" << m_nl_get_failed_attempts;

        if (m_nl_get_failed_attempts >= MAX_FAILED_NL_MSG_GET) {
            LOG(ERROR) << " max allowed consecutive-nl-msg-get-failures reached,"
                       << " ctx=" << m_dwpal_nl_ctx << ", returning error!";
            return false;
        }
    }

    return true;
}

bool base_wlan_hal_dwpal::dwpal_nl_cmd_set(const std::string &ifname, unsigned int nl_cmd,
                                           const void *vendor_data, size_t vendor_data_size)
{
    if (vendor_data == nullptr) {
        LOG(ERROR) << __func__ << "vendor_data is NULL ==> Abort!";
        return false;
    }

    if (dwpal_driver_nl_cmd_send(
            m_dwpal_nl_ctx, DWPAL_NL_UNSOLICITED_EVENT, (char *)ifname.c_str(), NL80211_CMD_VENDOR,
            DWPAL_NETDEV_ID, ltq_nl80211_vendor_subcmds(nl_cmd),
            const_cast<unsigned char *>(reinterpret_cast<const unsigned char *>(vendor_data)),
            vendor_data_size) != DWPAL_SUCCESS) {
        LOG(ERROR) << __func__ << "ERROR for cmd = " << nl_cmd;
        return false;
    }

    return true;
}

size_t base_wlan_hal_dwpal::dwpal_nl_cmd_get(const std::string &ifname, unsigned int nl_cmd,
                                             unsigned char *out_buffer,
                                             const size_t max_buffer_size)
{
    size_t data_size = 0;

    if (out_buffer == nullptr) {
        LOG(ERROR) << __func__ << "out_buffer is invalid ==> Abort!";
        return data_size;
    }

    /* Handle a command which invokes an event with the output data */
    if (dwpal_driver_nl_cmd_send(
            m_dwpal_nl_ctx, DWPAL_NL_SOLICITED_EVENT, (char *)ifname.c_str(), NL80211_CMD_VENDOR,
            DWPAL_NETDEV_ID, (enum ltq_nl80211_vendor_subcmds)nl_cmd, NULL, 0) == DWPAL_FAILURE) {
        LOG(ERROR) << __func__ << "ERROR for cmd = " << nl_cmd;
        return data_size;
    }

    // Passing a lambda with capture is not supported for standard C function
    // pointers. As a workaround, we create a static (but thread local) wrapper
    // function that calls the capturing lambda function.
    static __thread std::function<DWPAL_Ret(char *ifname, int event, int subevent, size_t len,
                                            unsigned char *data)>
        nl_handler_cb_wrapper;
    nl_handler_cb_wrapper = [&](char *ifname, int event, int subevent, size_t len,
                                unsigned char *data) -> DWPAL_Ret {
        if (!len || !data) {
            LOG(ERROR) << "len=0 and/or data is NULL ==> Abort!";
            return DWPAL_FAILURE;
        }
        if (event == NL80211_CMD_VENDOR) {
            if (len >= max_buffer_size) {
                LOG(ERROR) << "NL size exceeds out_buffer size ==> Abort!";
                return DWPAL_FAILURE;
            }

            // copy result from nl data buffer to local buffer
            std::copy_n(data, len, out_buffer);

            // update data size
            data_size = len;
        } else {
            LOG(ERROR) << "not handling non vendor event = " << event;
            return DWPAL_FAILURE;
        }
        return DWPAL_SUCCESS;
    };
    auto nl_handler_cb = [](char *ifname, int event, int subevent, size_t len,
                            unsigned char *data) -> DWPAL_Ret {
        return nl_handler_cb_wrapper(ifname, event, subevent, len, data);
    };

    // Since we're expecting a Solicited (asynchronous) event from the driver,
    // and it's impossible to know the type of the received message without
    // processing it (using a callback function), we continue processing events
    // until data_size != 0.
    // 1 sec TO is added so we dont get stuck here forever.
    // In practice, the response usually arrives much faster.
    auto timeout = std::chrono::steady_clock::now() + std::chrono::seconds(1);
    while ((data_size == 0) && std::chrono::steady_clock::now() < timeout) {
        //parsing will be done in callback func
        if (dwpal_driver_nl_msg_get(m_dwpal_nl_ctx, DWPAL_NL_SOLICITED_EVENT, nl_handler_cb,
                                    NULL) == DWPAL_FAILURE) {
            LOG(ERROR) << " dwpal_driver_nl_msg_get failed,"
                       << " ctx=" << m_dwpal_nl_ctx;
            return data_size;
        }
    }

    return data_size;
}

bool base_wlan_hal_dwpal::dwpal_nl_cmd_scan_dump()
{
    // Passing a lambda with capture is not supported for standard C function
    // pointers. As a workaround, we create a static (but thread local) wrapper
    // function that calls the capturing lambda function.
    static __thread std::function<DWPAL_Ret(struct nl_msg * msg)> nl_handler_cb_wrapper;
    nl_handler_cb_wrapper = [&](struct nl_msg *msg) -> DWPAL_Ret {
        if (!process_dwpal_nl_event(msg)) {
            LOG(ERROR) << "User's netlink handler function failed!";
            return DWPAL_FAILURE;
        }
        return DWPAL_SUCCESS;
    };
    auto nl_handler_cb = [](struct nl_msg *msg) -> DWPAL_Ret { return nl_handler_cb_wrapper(msg); };

    if (dwpal_driver_nl_scan_dump(m_dwpal_nl_ctx, (char *)m_radio_info.iface_name.c_str(),
                                  nl_handler_cb) != DWPAL_SUCCESS) {
        LOG(ERROR) << "dwpal_driver_nl_scan_dump Failed to request the nl scan dump";
        return false;
    }

    return true;
}

bool base_wlan_hal_dwpal::refresh_radio_info()
{
    char *reply = nullptr;

    /**
     * Obtain frequency band and maximum supported bandwidth using NL80211.
     * As this information does not change, this is required the first time this method is called
     * only.
     */
    if (beerocks::eFreqType::FREQ_UNKNOWN == m_radio_info.frequency_band) {
        nl80211_client::radio_info radio_info;
        if (m_nl80211_client->get_radio_info(get_iface_name(), radio_info)) {
            if (!radio_info.bands.empty()) {
                nl80211_client::band_info band_info = radio_info.bands.at(0);

                m_radio_info.frequency_band = band_info.get_frequency_band();
                m_radio_info.max_bandwidth  = band_info.get_max_bandwidth();
            }
        }
    }

    // TODO: Add radio_info get for station

    // Station HAL
    if (get_type() == HALType::Station) {
        // update radio info struct
        // TODO test the the output
        // m_radio_info.wifi_ctrl_enabled = beerocks::string_utils::stoi(reply["HostapdEnabled"]);
        // m_radio_info.tx_enabled        = beerocks::string_utils::stoi(reply["TxPower"]);
        // m_radio_info.channel           = beerocks::string_utils::stoi(reply["Channel"]);
        // if (m_radio_info.channel <= 0) {
        // 	//LOG(ERROR) << "X_LANTIQ_COM_Vendor_Channel not valid: " << radio_info.channel;
        // 	return false;
        // } else if (son::wireless_utils::which_freq(m_radio_info.channel) == beerocks::eFreqType::FREQ_5G) {
        // 	m_radio_info.is_5ghz = true;
        // }
    } else {
        if (!dwpal_send_cmd("GET_RADIO_INFO", &reply)) {
            LOG(ERROR) << __func__ << " failed";
            return false;
        }
        // update radio info struct
        size_t replyLen               = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);
        size_t numOfValidArgs[7]      = {0};
        FieldsToParse fieldsToParse[] = {
            {(void *)&m_radio_info.ant_num, &numOfValidArgs[0], DWPAL_INT_PARAM, "TxAntennas=", 0},
            {(void *)&m_radio_info.tx_power, &numOfValidArgs[1], DWPAL_INT_PARAM, "TxPower=", 0},
            {(void *)&m_radio_info.bandwidth, &numOfValidArgs[2], DWPAL_INT_PARAM,
             "OperatingChannelBandwidt=", 0},
            {(void *)&m_radio_info.vht_center_freq, &numOfValidArgs[3], DWPAL_INT_PARAM, "Cf1=", 0},
            {(void *)&m_radio_info.wifi_ctrl_enabled, &numOfValidArgs[4], DWPAL_INT_PARAM,
             "HostapdEnabled=", 0},
            {(void *)&m_radio_info.tx_enabled, &numOfValidArgs[5], DWPAL_BOOL_PARAM,
             "TxEnabled=", 0},
            {(void *)&m_radio_info.channel, &numOfValidArgs[6], DWPAL_INT_PARAM, "Channel=", 0},
            /* Must be at the end */
            {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};

        if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(m_radio_info)) ==
            DWPAL_FAILURE) {
            LOG(ERROR) << "DWPAL parse error ==> Abort";
            return false;
        }

        /* TEMP: Traces... */
        // LOG(DEBUG) << "GET_RADIO_INFO reply=\n" << reply;
        // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " ant_num= " << m_radio_info.ant_num;
        // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " tx_power= " << m_radio_info.tx_power;
        // LOG(DEBUG) << "numOfValidArgs[2]= " << numOfValidArgs[2] << " bandwidth= " << m_radio_info.bandwidth;
        // LOG(DEBUG) << "numOfValidArgs[3]= " << numOfValidArgs[3] << " vht_center_freq= " << m_radio_info.vht_center_freq;
        // LOG(DEBUG) << "numOfValidArgs[4]= " << numOfValidArgs[4] << " wifi_ctrl_enabled= " << m_radio_info.wifi_ctrl_enabled;
        // LOG(DEBUG) << "numOfValidArgs[5]= " << numOfValidArgs[5] << " tx_enabled= " << m_radio_info.tx_enabled;
        // LOG(DEBUG) << "numOfValidArgs[6]= " << numOfValidArgs[6] << " channel= " << m_radio_info.channel;
        /* End of TEMP: Traces... */

        for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
            if (numOfValidArgs[i] == 0) {
                LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
                return false;
            }
        }

        m_radio_info.is_5ghz =
            (son::wireless_utils::which_freq(m_radio_info.channel) == beerocks::eFreqType::FREQ_5G);

        // If the VAPs map is empty, refresh it as well
        // TODO: update on every refresh?
        if (!m_radio_info.available_vaps.size()) {
            if (!refresh_vaps_info(beerocks::IFACE_RADIO_ID)) {
                return false;
            }
        }
    }

    return true;
}

bool base_wlan_hal_dwpal::refresh_vap_info(int vap_id)
{
    const std::string ifname =
        beerocks::utils::get_iface_string_from_iface_vap_ids(m_radio_info.iface_name, vap_id);
    if (ifname.empty()) {
        LOG(ERROR) << "Failed to get vap_iface from radio " << m_radio_info.iface_name
                   << " and vap id " << vap_id;
        return false;
    }

    char *reply              = nullptr;
    size_t numOfValidArgs[2] = {0}, replyLen = 0;
    char ssid[SSID_MAX_SIZE] = {0}, bssid[MAC_ADDR_SIZE] = {0};
    FieldsToParse fieldsToParse[] = {
        {(void *)bssid, &numOfValidArgs[0], DWPAL_STR_PARAM, "BSSID=", sizeof(bssid)},
        {(void *)ssid, &numOfValidArgs[1], DWPAL_STR_PARAM, "SSID=", sizeof(ssid)},
        /* Must be at the end */
        {NULL, NULL, DWPAL_NUM_OF_PARSING_TYPES, NULL, 0}};
    std::string cmd = "GET_VAP_MEASUREMENTS " + ifname;

    // Read the VAP information
    if (!dwpal_send_cmd(cmd, &reply) || reply[0] == '\0') {
        // If the command failed, assume that it doesn't exist, and return
        if (m_radio_info.available_vaps.find(vap_id) != m_radio_info.available_vaps.end()) {
            m_radio_info.available_vaps.erase(vap_id);
        }
        return false;
    }

    replyLen = strnlen(reply, HOSTAPD_TO_DWPAL_MSG_LENGTH);

    if (dwpal_string_to_struct_parse(reply, replyLen, fieldsToParse, sizeof(ssid)) ==
        DWPAL_FAILURE) {
        LOG(ERROR) << "DWPAL parse error on " << ifname;
        return false;
    }

    /* TEMP: Traces... */
    // LOG(DEBUG) << "GET_VAP_MEASUREMENTS reply=\n" << reply;
    // LOG(DEBUG) << "numOfValidArgs[0]= " << numOfValidArgs[0] << " BSSID= " << bssid;
    // LOG(DEBUG) << "numOfValidArgs[1]= " << numOfValidArgs[1] << " SSID= " << ssid;
    /* End of TEMP: Traces... */

    for (uint8_t i = 0; i < (sizeof(numOfValidArgs) / sizeof(size_t)); i++) {
        if (numOfValidArgs[i] == 0) {
            LOG(ERROR) << "Failed reading parsed parameter " << (int)i << " ==> Abort";
            return false;
        }
    }

    // New VAP Element
    VAPElement vapElement;

    vapElement.mac  = std::string(bssid);
    vapElement.ssid = std::string(ssid);

    // Store the VAP element
    LOG(DEBUG) << "Detected VAP ID (" << vap_id << ") - MAC: " << vapElement.mac
               << ", SSID: " << vapElement.ssid;

    m_radio_info.available_vaps[vap_id] = vapElement;

    return true;
}

bool base_wlan_hal_dwpal::refresh_vaps_info(int id)
{
    if (id > beerocks::IFACE_RADIO_ID)
        return refresh_vap_info(id);

    for (int vap_id = beerocks::IFACE_VAP_ID_MIN; vap_id <= beerocks::IFACE_VAP_ID_MAX; vap_id++) {
        refresh_vap_info(vap_id);
    }

    return true;
}

bool base_wlan_hal_dwpal::process_ext_events()
{
    char opCode[DWPAL_OPCODE_STRING_LENGTH] = {0};

    if (!m_dwpal_ctx[0]) {
        LOG(ERROR) << "Invalid WPA Control socket (m_dwpal_ctx == nullptr)";
        return false;
    }

    auto buffer         = m_wpa_ctrl_buffer;
    auto buff_size_copy = m_wpa_ctrl_buffer_size;

    // Check if there are pending event and get it
    int status = dwpal_hostap_event_get(m_dwpal_ctx[0], buffer, &buff_size_copy, opCode);

    if (status == DWPAL_FAILURE) {
        LOG(ERROR) << "Failed reading event from DWPAL socket --> detaching!";
        detach();
        return false;
    } else if (status == DWPAL_NO_PENDING_MESSAGES) {
        // No pending messages
        LOG(WARNING) << "base_wlan_hal_dwpal::process_ext_events() called but there are no pending "
                        "messages...";
        return false;
    }

    /* Silencing unhandled multiple events */
    if (!strncmp(opCode, "WPS-ENROLLEE-SEEN", sizeof(opCode))) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG)
            << "DWPAL unhandled event opcode recieved: " << opCode;
        return true;
    } else if (!strncmp(opCode, "AP-PROBE-REQ-RECEIVED", sizeof(opCode))) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG)
            << "DWPAL unhandled event opcode recieved: " << opCode;
        return true;
    } else if (!strncmp(opCode, "BEACON-REQ-TX-STATUS", sizeof(opCode))) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG)
            << "DWPAL unhandled event opcode recieved: " << opCode;
        return true;
    } else if (!strncmp(opCode, "CTRL-EVENT-BSS-ADDED", sizeof(opCode))) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG)
            << "DWPAL unhandled event opcode recieved: " << opCode;
        return true;
    } else if (!strncmp(opCode, "CTRL-EVENT-BSS-REMOVED", sizeof(opCode))) {
        LOG_EVERY_N(UNHANDLED_EVENTS_LOGS, DEBUG)
            << "DWPAL unhandled event opcode recieved: " << opCode;
        return true;
    }

    // Process the event with the DWPAL parser
    if (!process_dwpal_event(buffer, buff_size_copy, std::string(opCode))) {
        LOG(ERROR) << "Failed processing DWPAL event with DWPAL parser";
        return false;
    }

    return true;
}

std::string base_wlan_hal_dwpal::get_radio_mac()
{
    std::string mac;
    if (!beerocks::net::network_utils::linux_iface_get_mac(m_radio_info.iface_name, mac)) {
        LOG(ERROR) << "Failed to get radio mac from ifname " << m_radio_info.iface_name;
    }
    return mac;
}

} // namespace dwpal
} // namespace bwl
