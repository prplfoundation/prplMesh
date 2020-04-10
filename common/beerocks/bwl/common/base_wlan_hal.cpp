/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bwl/base_wlan_hal.h>

#include <easylogging++.h>

#include <errno.h>
#include <sys/eventfd.h>
#include <unistd.h>

namespace bwl {

base_wlan_hal::base_wlan_hal(HALType type, std::string iface_name, IfaceType iface_type,
                             hal_event_cb_t callback, hal_conf_t hal_conf)
    : m_hal_conf(hal_conf), m_type(type), m_iface_name(iface_name), m_iface_type(iface_type),
      m_int_event_cb(callback)
{
    // Initialize radio info structure
    m_radio_info.iface_name = iface_name;
    m_radio_info.iface_type = iface_type;

    // Create an eventfd for internal events
    if ((m_fd_int_events = eventfd(0, EFD_SEMAPHORE)) < 0) {
        LOG(FATAL) << "Failed creating eventfd: " << strerror(errno);
    }

    // Initialize complex containers of the radio_info structure
    m_radio_info.supported_channels.resize(128 /* TODO: Get real value */);
}

base_wlan_hal::~base_wlan_hal()
{
    // Close the eventfd used for internal events
    if (m_fd_int_events != -1) {
        close(m_fd_int_events);
        m_fd_int_events = -1;
    }
}

bool base_wlan_hal::event_queue_push(int event, std::shared_ptr<void> data)
{
    // Create a new shared pointer of the event and the payload
    auto event_ptr = std::make_shared<hal_event_t>(hal_event_t(event, data));

    // Push the event into the queue
    // push with block (default true) can't return false
    m_queue_events.push(event_ptr);

    // Increment the eventfd counter by 1
    uint64_t counter = 1;
    if (write(m_fd_int_events, &counter, sizeof(counter)) < 0) {
        LOG(ERROR) << "Failed updating eventfd counter: " << strerror(errno);
        return false;
    }

    return true;
}

bool base_wlan_hal::process_int_events()
{
    // Read the counter value of the eventfd
    uint64_t counter = 0;
    if (read(m_fd_int_events, &counter, sizeof(counter)) < 0) {
        LOG(ERROR) << "Failed reading eventfd counter: " << strerror(errno);
        return false;
    }

    // Pop an event from the queue
    auto event = m_queue_events.pop(false);

    if (!event || !counter) {
        LOG(WARNING) << "process_int_events() called by the event queue pointer is " << event
                     << " and/or eventfd counter = " << counter;

        return false;
    }

    // Call the callback for handling the event
    if (!m_int_event_cb) {
        LOG(ERROR) << "Event callback not registered!";
        return false;
    }

    return m_int_event_cb(event);
}

} // namespace bwl
