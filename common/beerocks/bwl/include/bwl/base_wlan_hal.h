/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_BASE_WLAN_HAL_H_
#define _BWL_BASE_WLAN_HAL_H_

#include "base_802_11_defs.h"
#include "base_wlan_hal_types.h"

#include <beerocks/bcl/beerocks_thread_safe_queue.h>

#include <functional>
#include <memory>
#include <string>

namespace bwl {

/*!
 * Base class for the WLAN hardware abstraction layer.
 */
class base_wlan_hal {

    // Public definitions
public:
    // Pair of event ID and payload pointer
    typedef std::pair<int, std::shared_ptr<void>> hal_event_t;
    typedef std::shared_ptr<hal_event_t> hal_event_ptr_t;
    typedef std::function<bool(hal_event_ptr_t)> hal_event_cb_t;

    // Public methods
public:
    virtual ~base_wlan_hal();

    /*!
     * Attach to the WLAN hardware/middleware.
     * 
     * Unless the "block" argument is set to "true", the implementation should
     * be non-blocking. If the internal implementation may block, the state
     * should be set to "Initializing" and the upper layer will call this method
     * again until the state will change to "Operational" or "Failed".
     *
     * @param [in] block Execute in non-blocking (default) or blocking mode.
     *
     * @return The state of the attach process.
     */
    virtual HALState attach(bool block = false) = 0;

    /*!
     * Detach from the WLAN hardware/middleware.
     *
     * @return true on success or false on error.
     */
    virtual bool detach() = 0;

    /*!
     * Refresh the internal radio (and VAPs) information structure 
     * with the latest values from the hardware.
     *
     * @return true on success or false on error.
     */
    virtual bool refresh_radio_info() = 0;

    /*!
     * Try to ping the control interface
     *
     * @return true on success or false on error.
     */
    virtual bool ping() = 0;

    /*!
     * Refresh the VAPs information structure on the 
     * internal radio information structure.
     *
     * @param [in] id vap to refresh, or all vaps if radio id.
     * 
     * @return true on success or false on error.
     */
    virtual bool refresh_vaps_info(int id = beerocks::IFACE_RADIO_ID) = 0;

    /*!
     * Process incoming events from the underlying hardware/middleware.
     * This method should be called if the file descriptor returned by
     * get_ext_events_fd() generated an event.
     *
     * @return true on success or false on error.
     */
    virtual bool process_ext_events() = 0;

    /*!
     * Process internal events (queued by the underlying hardware/middleware).
     * This method should be called if the file descriptor returned by
     * get_int_events_fd() generated an event.
     *
     * @return true on success or false on error.
     */
    virtual bool process_int_events();

    // Public getter methods:
public:
    /*!
     * Returns the type of the HAL instance.
     */
    HALType get_type() const { return (m_type); }

    /*!
     * Returns the current state.
     */
    HALState get_state() const { return (m_hal_state); }

    /*!
     * Returns a file descriptor to the external events queue, 0 is events
     * should processed synchronously (by directly calling the process method),
     * or -1 on error.
     * 
     * The returned file descriptor supports select(), poll() and epoll().
     */
    int get_ext_events_fd() const { return (m_fd_ext_events); }

    /*!
     * Returns a file descriptor to the internal events queue, or -1 on error.
     * The returned file descriptor supports select(), poll() and epoll().
     */
    int get_int_events_fd() const { return (m_fd_int_events); }

    /*!
     * Returns the interface name.
     */
    const std::string &get_iface_name() const { return (m_iface_name); }

    /*!
     * Returns the interface type
     */
    IfaceType get_iface_type() const { return (m_iface_type); }

    /*!
     * Return the radio information.
     */
    const RadioInfo &get_radio_info() const { return (m_radio_info); }

    /*!
     * Return HAL configuration.
     */
    const hal_conf_t &get_hal_conf() const { return (m_hal_conf); }

    /*!
     * Returns the Radio's main MAC address.
     */
    virtual std::string get_radio_mac() = 0;

    // Protected methods
protected:
    /*!
     * Constructor.
     *
     * @param [in] type The type of the HAL.
     * @param [in] iface_name Interface name.
     * @param [in] iface_type Interface type.
     * @param [in] callback Callback for handling internal events.
     * @param [in] hal_conf HAL configuration.
     */
    base_wlan_hal(HALType type, std::string iface_name, IfaceType iface_type,
                  hal_event_cb_t callback, hal_conf_t hal_conf = {});

    /*!
     * Push a new (internal) event into the queue.
     * 
     * @param [in] event Event opcode.
     * @param [in] data Pointer to the event payload.
     * 
     * @return true on success of false on failure.
     */
    bool event_queue_push(int event, std::shared_ptr<void> data = {});

    /*!
     * set a parameter in the interface
     *
     * @return true on success or false on error.
     */
    virtual bool set(const std::string &param, const std::string &value, int vap_id) = 0;

    // Protected methods:
protected:
    /*!
     * Default constructor (for virtual inheritance)
     */
    base_wlan_hal() = default;

    // Protected data-members:
protected:
    RadioInfo m_radio_info;

    HALState m_hal_state = HALState::Uninitialized;

    int m_fd_ext_events = -1;

    hal_conf_t m_hal_conf;

    // Private data-members:
private:
    HALType m_type = HALType::Invalid;

    std::string m_iface_name;

    IfaceType m_iface_type = IfaceType::Unsupported;

    bool m_acs_enabled = false;

    int m_fd_int_events = -1;

    hal_event_cb_t m_int_event_cb = nullptr;

    beerocks::thread_safe_queue<hal_event_ptr_t> m_queue_events;
};

} // namespace bwl

#endif // _BWL_AP_MANAGER_HAL_H_