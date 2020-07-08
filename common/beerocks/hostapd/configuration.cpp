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
namespace config {

Configuration::Configuration(const std::string& file_name) : m_configuration_file(file_name) {}

Configuration::operator bool() const { return m_ok; }

bool Configuration::load()
{
    // please take a look at the end of this file for
    // the expected format of hostapd configuration file
    // loading the file relays on the expected format
    // otherwise the load fails

    std::ifstream ifs(m_configuration_file);
    std::string line;

    bool parsing_vaps = false;
    std::string cur_vap;

    while (getline(ifs, line)) {

        // skip empty lines
        if (std::all_of(line.begin(), line.end(), isspace)) {
            continue;
        }
        // check if the string belongs to a vap config part and capture which one.
        const std::string bss_eq("bss=");
        if (line.compare(0, bss_eq.length(), bss_eq) == 0) {

            // from now on we are in the vaps area, all
            // key/value pairs belongs to vaps
            parsing_vaps = true;

            // copy the bss (vap) value
            cur_vap.assign(line, bss_eq.length(), std::string::npos);
        }

        // if not a vap line store it in the header part of the config,
        // otherwise add to the currently being parsed vap storage.
        if (!parsing_vaps) {
            m_hostapd_config_head.push_back(line);
        } else {
            m_hostapd_config_vaps[cur_vap].push_back(line);
        }
    }

    m_error_description = strerror(errno);

    // if we've got to parsing vaps and no read errors, assume all is good
    m_ok = parsing_vaps && !ifs.bad();
    return m_ok;
}

std::ostream &operator<<(std::ostream &o, const Configuration &conf)
{
    o << "== configuration details ==\n"
      << "= ok:           " << std::boolalpha << conf.m_ok << '\n'
      << "= erorr text:   " << conf.m_error_description << '\n' 
      << "= file:         " << conf.m_configuration_file << '\n'
      << "= head:         " << '\n';

    for (const auto &line : conf.m_hostapd_config_head) {
        o << line << '\n';
    }

    o << "== vaps (" << conf.m_hostapd_config_vaps.size() << ") ==\n";

    for (const auto &vap : conf.m_hostapd_config_vaps) {
        o << "   = vap (" << vap.first << " ) =\n";
        for (const auto &line : vap.second) {
            o << line << '\n';
        }
    }

    return o;
}

} // namespace config
} // namespace hostapd
} // namespace prplmesh

/*
//////////////// hostapd configuration format ////////////////

hostapd has a special format that does NOT have an ini like format.
We expect the following format of the file:
(between /// BEGIN hostapd.conf /// and /// END hostapd.conf ///)

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
