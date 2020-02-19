/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "base_wlan_hal_dummy.h"

#include <bcl/beerocks_string_utils.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <bcl/son/son_wireless_utils.h>

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

static void map_obj_parser(std::stringstream &ss_in, std::list<char> delimiter_list,
                           parsed_obj_map_t &map_obj)
{
    if (delimiter_list.empty())
        return;

    std::string str_storage, key;
    bool kv = true; // '1'=key, '0'=val;
    while (std::getline(ss_in, str_storage, delimiter_list.front())) {
        if (delimiter_list.size() == 1) {
            if (kv) {
                key = str_storage; // save key
            } else {
                map_obj[key] = str_storage; // save val
            }
            kv = !kv;

        } else {
            auto delimiter_list_out(delimiter_list);
            delimiter_list_out.erase(delimiter_list_out.begin()); // delete first delimiter
            std::stringstream ss_out(str_storage);
            map_obj_parser(ss_out, delimiter_list_out, map_obj);
        }
    }
}

static std::string::size_type find_first_of_delimiter_pair(std::string &str,
                                                           std::string::size_type pos,
                                                           char delim_near, char delim_far)
{
    // finds first occurrence on string 'str' of <'delim_near'><any_characters except delimiters><'delim_far'>
    auto idx = str.find_first_of(delim_far, pos);
    if (idx == std::string::npos) {
        return idx;
    }
    idx = str.rfind(delim_near, idx);
    return idx;
}

static void map_event_obj_parser(std::string event_str, parsed_obj_map_t &map_obj)
{
    // eliminate event log level from the begining of the event string : "<3>"
    size_t idx_start = 0;

    // find params without key end index
    auto idx = find_first_of_delimiter_pair(event_str, idx_start, ' ', '=');

    // put null terminator at the end of our key=val, for ss construction
    if (idx != std::string::npos) {
        event_str[idx] = '\0';
    }

    // insert to map known prams without key
    std::stringstream ss(event_str.c_str() + idx_start);
    std::string str_storage;
    bool opcode = true;
    bool type   = true;
    while (std::getline(ss, str_storage, ' ')) {
        if (type) {
            // assume that the first param is type - DATA or EVENT
            map_obj[DUMMY_EVENT_KEYLESS_PARAM_TYPE] = str_storage;
            type                                    = false;
        } else if (opcode) {
            // assume that the second param is data or event name
            map_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE] = str_storage;
            opcode                                    = false;
        } else if (beerocks::net::network_utils::is_valid_mac(str_storage)) {
            map_obj[DUMMY_EVENT_KEYLESS_PARAM_MAC] = str_storage;
        } else if (!strncmp(str_storage.c_str(), "wlan", 4)) {
            map_obj[DUMMY_EVENT_KEYLESS_PARAM_IFACE] = str_storage;
        }
    }

    // fill the map with the rest of event data
    while (idx != std::string::npos) {

        idx_start = ++idx;

        // find first '=' to skip on it
        idx = event_str.find_first_of('=', idx_start);

        // find the next pair of delimiters index
        idx = find_first_of_delimiter_pair(event_str, ++idx, ' ', '=');

        if (idx != std::string::npos) {
            // put null terminator at the end of our key=val, for ss_in construction
            event_str[idx] = '\0';
        }

        // parse key=val
        std::stringstream ss_in(event_str.c_str() + idx_start);
        map_obj_parser(ss_in, {'='}, map_obj);
    }
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bool base_wlan_hal_dummy::dummy_obj_read_int(const std::string &key, parsed_obj_map_t &obj,
                                             int64_t &value, bool ignore_unknown)
{
    auto val_iter = obj.find(key);
    if (val_iter == obj.end()) {
        LOG(ERROR) << "param :" << key << " is not exist";
        return false;
    }

    static const std::string unknown_string = "UNKNOWN";

    if (ignore_unknown && !unknown_string.compare(val_iter->second)) {
        value = 0;
        return true;
    }

    value = beerocks::string_utils::stoi(val_iter->second);

    return (true);
}

bool base_wlan_hal_dummy::dummy_obj_read_str(const std::string &key, parsed_obj_map_t &obj,
                                             char **value)
{
    auto val_iter = obj.find(key);
    if (val_iter == obj.end()) {
        LOG(ERROR) << "param :" << key << " does not exist";
        return false;
    }

    *value = (char *)((val_iter->second).c_str());
    return (true);
}

void base_wlan_hal_dummy::parsed_obj_debug(parsed_obj_map_t &obj)
{
    LOG(TRACE) << "parsed_obj_debug:";
    std::stringstream ss_obj;
    ss_obj << std::endl << "parsed_obj_debug: " << std::endl;
    for (auto element : obj) {
        LOG(TRACE) << "key: " << element.first << ", value: " << element.second;
        ss_obj << "key: " << element.first << ", value: " << element.second << std::endl;
    }

    LOG(DEBUG) << ss_obj.str();
}

void base_wlan_hal_dummy::parsed_obj_debug(parsed_obj_listed_map_t &obj)
{
    LOG(TRACE) << "parsed_obj_debug:";
    std::stringstream ss_obj;
    ss_obj << std::endl << "parsed_obj_debug: " << std::endl;
    int element_num = 0;
    for (auto list_element : obj) {
        LOG(TRACE) << "vector element: " << element_num;
        ss_obj << "vector element: " << element_num << std::endl;
        for (auto map_element : list_element) {
            LOG(TRACE) << "key: " << map_element.first << ", value: " << map_element.second;
            ss_obj << "key: " << map_element.first << ", value: " << map_element.second
                   << std::endl;
        }
        element_num++;
    }
    LOG(DEBUG) << ss_obj.str();
}

base_wlan_hal_dummy::base_wlan_hal_dummy(HALType type, std::string iface_name,
                                         hal_event_cb_t callback, hal_conf_t hal_conf)
    : base_wlan_hal(type, iface_name, IfaceType::Intel, callback, hal_conf),
      beerocks::beerocks_fsm<dummy_fsm_state, dummy_fsm_event>(dummy_fsm_state::Delay)
{
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

bool base_wlan_hal_dummy::set(const std::string &param, const std::string &value, int vap_id)
{
    return true;
}

bool base_wlan_hal_dummy::ping() { return true; }

bool base_wlan_hal_dummy::dummy_send_cmd(const std::string &cmd) { return false; }

bool base_wlan_hal_dummy::dummy_send_cmd(const std::string &cmd, char **reply) { return false; }

bool base_wlan_hal_dummy::process_nl_events()
{
    LOG(ERROR) << __func__ << "not implemented";
    return false;
}

bool base_wlan_hal_dummy::refresh_radio_info()
{
    //if (get_iface_name() == "wlan2") {
    if (get_iface_name() == "rai0") {
        m_radio_info.is_5ghz = true;
    }
    std::string radio_mac;
    beerocks::net::network_utils::linux_iface_get_mac(m_radio_info.iface_name, radio_mac);
    for (int vap_id = 0; vap_id < 4; vap_id++) {
        auto mac = beerocks::net::network_utils::mac_from_string(radio_mac);
        mac.oct[5] += vap_id;
        m_radio_info.available_vaps[vap_id].mac = beerocks::net::network_utils::mac_to_string(mac);
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
    if (event.empty()) {
        LOG(DEBUG) << "Received empty event, ignoring";
        return true;
    }
    LOG(DEBUG) << "Received event " << event;

    parsed_obj_map_t event_obj;
    map_event_obj_parser(event, event_obj);
    //base_wlan_hal_dummy::parsed_obj_debug(event_obj);

    // Process the event
    if (event_obj[DUMMY_EVENT_KEYLESS_PARAM_TYPE] == "EVENT") {
        if (!process_dummy_event(event_obj)) {
            LOG(ERROR) << "Failed processing DUMMY event: "
                       << event_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE];
            return false;
        }
    }
    // Process data
    else if (event_obj[DUMMY_EVENT_KEYLESS_PARAM_TYPE] == "DATA") {
        if (!process_dummy_data(event_obj)) {
            LOG(ERROR) << "Failed processing DUMMY data: "
                       << event_obj[DUMMY_EVENT_KEYLESS_PARAM_OPCODE];
            return false;
        }
    } else {
        LOG(ERROR) << "Unsupported type " << event_obj[DUMMY_EVENT_KEYLESS_PARAM_TYPE];
        return false;
    }

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
