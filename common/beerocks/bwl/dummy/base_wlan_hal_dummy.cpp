/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "base_wlan_hal_dummy.h"

#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/beerocks_utils.h>
#include <beerocks/bcl/network/network_utils.h>
#include <beerocks/bcl/son/son_wireless_utils.h>

#include <easylogging++.h>
#include <limits.h>
#include <sys/inotify.h>

#define UNHANDLED_EVENTS_LOGS 20

namespace bwl {
namespace dummy {

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Functions ////////////////////////////
//////////////////////////////////////////////////////////////////////////////

std::ostream &operator<<(std::ostream &out, const dummy_fsm_state &value)
{
    switch (value) {
    case dummy_fsm_state::Delay:
        out << "Delay";
        break;
    case dummy_fsm_state::Init:
        out << "Init";
        break;
    case dummy_fsm_state::GetRadioInfo:
        out << "GetRadioInfo";
        break;
    case dummy_fsm_state::Attach:
        out << "Attach";
        break;
    case dummy_fsm_state::Operational:
        out << "Operational";
        break;
    case dummy_fsm_state::Detach:
        out << "Detach";
        break;
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const dummy_fsm_event &value)
{
    switch (value) {
    case dummy_fsm_event::Attach:
        out << "Attach";
        break;
    case dummy_fsm_event::Detach:
        out << "Detach";
        break;
    }
    return out;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

base_wlan_hal_dummy::base_wlan_hal_dummy(HALType type, std::string iface_name, bool acs_enabled,
                                         hal_event_cb_t callback, int wpa_ctrl_buffer_size)
    : base_wlan_hal(type, iface_name, IfaceType::Intel, acs_enabled, callback),
      beerocks::beerocks_fsm<dummy_fsm_state, dummy_fsm_event>(dummy_fsm_state::Delay),
      m_wpa_ctrl_buffer_size(wpa_ctrl_buffer_size)
{

    // Allocate wpa_ctrl buffer
    if (m_wpa_ctrl_buffer_size) {
        m_wpa_ctrl_buffer = std::shared_ptr<char>(new char[m_wpa_ctrl_buffer_size], [](char *obj) {
            if (obj)
                delete[] obj;
        });
    }

    // Set up dummy external events fd
    // dummy implementation is based on monitoring text file changes which
    // are used to simulate clients activity - STA connected, disconnected, etc.
    // For that we are creating the INOTIFY instance
    // inotify_init1 not available with older kernels, consequently inotify reads block.
    // inotify_init1 allows directory events to complete immediately, avoiding buffering delays. In practice,
    // this significantly improves monotiring of newly created subdirectories.
    // Note that IN_NONBLOCK is not defined in old kernels so we get to the #else and call inotify_init()
    // in this case.
#ifdef IN_NONBLOCK
    m_fd_ext_events = inotify_init1(IN_NONBLOCK);
#else
    m_fd_ext_events = inotify_init();
#endif

    if (m_fd_ext_events < 0) {
        LOG(FATAL) << "Failed creating m_fd_ext_events: " << strerror(errno);
    }

    std::string path = std::string(BEEROCKS_TMP_PATH) + "/" + get_iface_name();
    mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    inotify_add_watch(m_fd_ext_events, path.c_str(), (IN_CREATE | IN_DELETE | IN_MODIFY));

    // Initialize the FSM
    fsm_setup();
}

base_wlan_hal_dummy::~base_wlan_hal_dummy() { detach(); }

bool base_wlan_hal_dummy::fsm_setup() { return true; }

HALState base_wlan_hal_dummy::attach(bool block)
{
    refresh_radio_info();
    return (m_hal_state = HALState::Operational);
}

bool base_wlan_hal_dummy::detach() { return true; }

bool base_wlan_hal_dummy::ping() { return true; }

bool base_wlan_hal_dummy::dummy_send_cmd(const std::string &cmd) { return false; }

bool base_wlan_hal_dummy::dummy_send_cmd(const std::string &cmd, char **reply) { return false; }

bool base_wlan_hal_dummy::refresh_radio_info()
{
    if (get_iface_name() == "wlan2") {
        m_radio_info.is_5ghz = true;
    }
    return true;
}

bool base_wlan_hal_dummy::refresh_vap_info(int vap_id) { return true; }

bool base_wlan_hal_dummy::refresh_vaps_info(int id) { return true; }

/**
 * @brief process simulated events
 *        events are expected to be simulated by writing the event
 *        string to the first line in the EVENT file.
 *        For example, simulating client connected event:
 *        echo "STA_CONNECTED,11:22:33:44:55:66"
 *
 * @return true on success
 * @return false on failure
 */
bool base_wlan_hal_dummy::process_ext_events()
{
    // From http://man7.org/linux/man-pages/man7/inotify.7.html:
    // The behavior when the buffer given to read(2) is too small to return
    // information about the next event depends on the kernel version: in
    // kernels before 2.6.21, read(2) returns 0; since kernel 2.6.21,
    // read(2) fails with the error EINVAL.  Specifying a buffer of size
    // sizeof(struct inotify_event) + NAME_MAX + 1
    // will be sufficient to read at least one event.
    const size_t event_size = sizeof(struct inotify_event) + NAME_MAX + 1;
    // Use a static buffer big enough to hold up to 128 events
    // We don't actually read the events.
    // The purpose is to always be able to clear all events with a single read.
    static char buffer[128 * event_size];

    LOG(DEBUG) << "in process external events";

    // dummy read to clear event
    int length = read(m_fd_ext_events, buffer, sizeof(buffer));
    if (length < 0) {
        LOG(ERROR) << "Failed to read from m_fd_ext_events";
        return false;
    }

    std::string event_file = std::string(BEEROCKS_TMP_PATH) + "/" + get_iface_name() + "/EVENT";
    std::ifstream stream;
    stream.open(event_file);
    if (!stream.is_open()) {
        LOG(DEBUG) << "Invalid event, missing " << event_file << " file";
        return true;
    }
    std::string event;
    std::getline(stream, event);
    LOG(DEBUG) << "Received event " << event;

    // TODO Handle event
    stream.close();
    return true;
}

std::string base_wlan_hal_dummy::get_radio_mac()
{
    std::string mac;
    if (!beerocks::net::network_utils::linux_iface_get_mac(m_radio_info.iface_name, mac)) {
        LOG(ERROR) << "Failed to get radio mac from ifname " << m_radio_info.iface_name;
    }
    return mac;
}

} // namespace dummy
} // namespace bwl
