/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "configuration.h"
#include <algorithm>
#include <easylogging++.h>
#include <fstream>

namespace prplmesh {
namespace hostapd {

Configuration::Configuration(const std::string &file_name) : m_configuration_file(file_name) {}

Configuration::operator bool() const { return m_ok; }

bool Configuration::load(const std::string &vap_indication)
{
    // please take a look at the end of this file for
    // the expected format of hostapd configuration file
    // loading the file relays on the expected format
    // otherwise the load fails

    // for cases when load is called more than once, we
    // first clear internal data
    m_hostapd_config_head.clear();
    m_hostapd_config_vaps.clear();

    // for cases when load is called more than once, we
    // first clear internal data
    m_hostapd_config_head.clear();
    m_hostapd_config_vaps.clear();

    // strat reading
    std::ifstream ifs(m_configuration_file);
    std::string line;

    bool parsing_vaps = false;
    std::string cur_vap;

    // go over line by line in the file
    while (getline(ifs, line)) {

        // skip empty lines
        if (std::all_of(line.begin(), line.end(), isspace)) {
            continue;
        }
        // check if the string belongs to a vap config part and capture which one.
        auto end_comment = line.find_first_not_of('#');
        if (line.compare(end_comment, vap_indication.length(), vap_indication) == 0) {

            // from now on we are in the vaps area, all
            // key/value pairs belongs to vaps
            parsing_vaps = true;

            // copy the vap value
            cur_vap.assign(line, end_comment + vap_indication.length(), std::string::npos);
        }

        // if not a vap line store it in the header part of the config,
        // otherwise add to the currently being parsed vap storage.
        if (!parsing_vaps) {
            m_hostapd_config_head.push_back(line);
        } else {
            m_hostapd_config_vaps[cur_vap].push_back(line);
        }
    }

    std::stringstream load_message;
    load_message << "load() final message: os - " << strerror(errno) << "; existing vaps - "
                 << std::boolalpha << parsing_vaps;
    m_last_message = load_message.str();

    // if we've got to parsing vaps and no read errors, assume all is good
    m_ok = parsing_vaps && !ifs.bad();

    // return this as bool
    return *this;
}

bool Configuration::store()
{
    std::ofstream out_file(m_configuration_file, std::ofstream::out | std::ofstream::trunc);

    // store the head
    for (const auto &line : m_hostapd_config_head) {
        out_file << line << "\n";
    }

    // store the vaps
    for (auto &vap : m_hostapd_config_vaps) {

        // add empty line for readability
        out_file << "\n";

        for (auto &line : vap.second) {
            out_file << line << "\n";
        }
    }

    // close the file
    out_file.close();
    if (out_file.fail()) {
        m_last_message = strerror(errno);
        m_ok           = false;
    }

    m_ok = true;
    return *this;
}

bool Configuration::set_create_head_value(const std::string &key, const std::string &value)
{
    // search for the key
    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        m_hostapd_config_head.begin(), m_hostapd_config_head.end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    // we first delete the key, and if the requested value is non empty
    // we push it to the end of the array

    // delete the key-value if found
    if (line_iter != m_hostapd_config_head.end()) {
        line_iter = m_hostapd_config_head.erase(line_iter);
    } else {
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for head was not found";
    }

    // when the new value is provided add the key back with that new value
    if (value.length() != 0) {
        m_hostapd_config_head.push_back(key_eq + value);
        m_last_message = std::string(__FUNCTION__) + " the key '" + key + "' was (re)added to head";
    } else {
        m_last_message = std::string(__FUNCTION__) + " the key '" + key + "' was deleted from head";
    }

    m_ok = true;
    return *this;
}

bool Configuration::set_create_head_value(const std::string &key, const int value)
{
    return set_create_head_value(key, std::to_string(value));
}

std::string Configuration::get_head_value(const std::string &key)
{
    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        m_hostapd_config_head.begin(), m_hostapd_config_head.end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    if (line_iter == m_hostapd_config_head.end()) {
        m_last_message = std::string(__FUNCTION__) + " couldn't find requested key in head: " + key;
        return "";
    }

    // return from just after the '=' sign to the end of the string
    return line_iter->substr(line_iter->find('=') + 1);
}

bool Configuration::set_create_vap_value(const std::string &vap, const std::string &key,
                                         const std::string &value)
{
    // search for the requested vap
    auto find_vap = get_vap(std::string(__FUNCTION__) + " key/value: " + key + '/' + value, vap);
    if (!std::get<0>(find_vap)) {
        return false;
    }
    auto existing_vap           = std::get<1>(find_vap);
    bool existing_vap_commented = existing_vap->front()[0] == '#';

    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        existing_vap->begin(), existing_vap->end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    // we first delete the key, and if the requested value is non empty
    // we push it to the end of the array

    // delete the key-value if found
    if (line_iter != existing_vap->end()) {
        line_iter = existing_vap->erase(line_iter);
    } else {
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for vap " + vap + " was not found";
    }

    // when the new value is provided add the key back with that new value
    if (value.length() != 0) {
        if (existing_vap_commented) {
            existing_vap->push_back('#' + key_eq + value);
        } else {
            existing_vap->push_back(key_eq + value);
        }
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for vap " + vap + " was (re)added";
    } else {
        m_last_message =
            std::string(__FUNCTION__) + " the key '" + key + "' for vap " + vap + " was deleted";
    }

    m_ok = true;
    return *this;
}

bool Configuration::set_create_vap_value(const std::string &vap, const std::string &key,
                                         const int value)
{
    return set_create_vap_value(vap, key, std::to_string(value));
}

std::string Configuration::get_vap_value(const std::string &vap, const std::string &key)
{
    // search for the requested vap
    auto find_vap = get_vap(std::string(__FUNCTION__), vap);
    if (!std::get<0>(find_vap)) {
        return "";
    }
    const auto &existing_vap = std::get<1>(find_vap);

    // from now on this function is ok with all situations
    // (e.g. not finding the requested key)
    m_ok = true;

    std::string key_eq(key + "=");
    auto line_iter = std::find_if(
        existing_vap->begin(), existing_vap->end(),
        [&key_eq, this](const std::string &line) -> bool { return is_key_in_line(line, key_eq); });

    if (line_iter == existing_vap->end()) {
        m_last_message = std::string(__FUNCTION__) +
                         " couldn't find requested key for vap: " + vap + "; requested key: " + key;
        return "";
    }

    // return from the just after the '=' sign to the end of the string
    return line_iter->substr(line_iter->find('=') + 1);
}

void Configuration::comment_vap(const std::string &vap)
{
    // search for the requested vap
    auto find_vap = get_vap(std::string(__FUNCTION__), vap);
    if (!std::get<0>(find_vap)) {
        return;
    }
    const auto &existing_vap = std::get<1>(find_vap);

    std::for_each(existing_vap->begin(), existing_vap->end(),
                  [](std::string &line) { line.insert(0, 1, '#'); });
}

void Configuration::uncomment_vap(const std::string &vap)
{
    // search for the requested vap
    auto find_vap = get_vap(std::string(__FUNCTION__), vap);
    if (!std::get<0>(find_vap)) {
        return;
    }
    const auto &existing_vap = std::get<1>(find_vap);

    std::for_each(existing_vap->begin(), existing_vap->end(), [](std::string &line) {
        auto end_comment = line.find_first_not_of('#');
        if (std::string::npos != end_comment) {
            line.erase(0, end_comment);
        };
    });
}

const std::string &Configuration::get_last_message() const { return m_last_message; }

std::tuple<bool, std::vector<std::string> *>
Configuration::get_vap(const std::string &calling_function, const std::string &vap)
{
    // search for the requested vap - ignore comments
    // by searching from the back of the saved vap (rfind)
    auto existing_vap =
        std::find_if(m_hostapd_config_vaps.begin(), m_hostapd_config_vaps.end(),
                     [&vap](const std::pair<std::string, std::vector<std::string>> &current_vap) {
                         return current_vap.first.rfind(vap) != std::string::npos;
                     });

    if (existing_vap == m_hostapd_config_vaps.end()) {
        m_last_message = calling_function + " couldn't find requested vap: " + vap;
        m_ok           = false;
        return std::make_tuple(false, nullptr);
    }

    m_ok = true;
    return std::make_tuple(true, &existing_vap->second);
}

bool Configuration::is_key_in_line(const std::string &line, const std::string &key) const
{
    // we need to make sure when searching for example
    // for "ssid", to ignore cases like:
    // multi_ap_backhaul_ssid="Multi-AP-24G-2"
    //                   ^^^^^
    // bssid=02:9A:96:FB:59:11
    //  ^^^^^
    // and we need to take into consideration
    // that the key might be or might not be commented.
    // so the search algorithm is:
    // - find the requested key and
    // - make sure it is either on the first position
    // or it has a comment sign just before it
    auto found_pos = line.rfind(key);
    bool ret = found_pos != std::string::npos && (found_pos == 0 || line.at(found_pos - 1) == '#');

    return ret;
}

std::ostream &operator<<(std::ostream &o, const Configuration &conf)
{
    o << "== configuration details ==\n"
      << "= ok:           " << std::boolalpha << conf.m_ok << '\n'
      << "= last message: " << conf.m_last_message << '\n'
      << "= file:         " << conf.m_configuration_file << '\n'
      << "= head:         " << '\n';

    for (const auto &line : conf.m_hostapd_config_head) {
        o << line << '\n';
    }

    o << "== vaps (total of: " << conf.m_hostapd_config_vaps.size() << " vaps) ==\n";

    for (const auto &vap : conf.m_hostapd_config_vaps) {
        o << "   vap: " << vap.first << "\n";
        for (const auto &line : vap.second) {
            o << line << '\n';
        }
    }

    return o;
}

} // namespace hostapd
} // namespace prplmesh

/*
//////////////// hostapd configuration format ////////////////

hostapd has a special format that does NOT have an ini like format.
between /// BEGIN hostapd.conf /// and /// END hostapd.conf /// is the 
format of the file.
note: the string "bss=" may be replaced by the
user in the call to load() with another vap-indicator (e.g. "interface=")

/// BEGIN hostapd.conf ///  

## "head" part ##

# everything until the first `bss=` in the file
# is considered "head"
# after the first `bss=` "vaps" are presented.
# so we expect no parametrs that does not belong to vaps
# after the first `bss=`
# take a look below for more details

# note that we don't expect a space between the key and the equal sign
# the code in this class seeks for `key=` when a key is needed.
# therefore `key =` (with space) will fail
# also, everything immidiatly after the equal sign (=) is 
# part of the value. the space before 11 in this is part of the value:
# bassid= 11:22:33:44:55:66
key=value 

## "vaps part - we expect at least one vap to be configured ##

# vap (bss and vap are interchangable) 
bss=wlan0_0

# this key (ssid) belongs to the previous vap (bss value which is wlan0_0)
ssid=test2

# another vap 
bss=wlan0_1

# this key (bssid) belongs to the previous vap wlan0_1
bssid=00:13:10:95:fe:0b

///// END hostapd.conf ///
*/
