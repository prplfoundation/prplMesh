/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <map>
#include <string>
#include <vector>

namespace prplmesh {
namespace hostapd {
namespace config {

class Configuration {

public:
    /**
     * @brief construct a Configurationn object
     * @param file_name the name of the configuration file.
     */
    Configuration(const std::string& file_name);

    ~Configuration()                     = default;
    Configuration(const Configuration &) = default;
    Configuration(Configuration &&)      = default;

    /**
     * @brief for simple use in if statements
     * @return true or false with the following meaning:
     * true - configuration is non empty and valid
     * false - otherwise
     */
    operator bool() const;

    /**
     * @brief load the configuration 
     * @details loads the file this object is constructed with
     * @return *this (as bool, see above)
     * e.g.
     * Configuratio conf("hostapd.conf");
     * if (!conf.load()) { 
     *  // handle error state
     * }
     *
     */
    bool load();

    /**
     * @brief set key/value for the given vap
     * @details set the key/vale for the given vap, either replace or create.
     * @return 
     * true - the vap exists, values were set.
     * false - the given vap was not found
     */
    bool set_create_vap_value(const std::string &vap, const std::string &key,
                              const std::string &value);

    /**
     * @brief for debug: return the last internal message
     * @details each action on this class changes its internal
     * message (similar to errno) - for debug usage
     * @return s string describing the last message
     */
    const std::string &get_last_message() const;

private:
    std::string m_configuration_file;
    bool m_ok = false;

    // each string is a line in the original configuration file
    // that belongs to the "head" part. read the explnation at
    // the end of the cpp file for more details
    std::vector<std::string> m_hostapd_config_head;

    // a map between a vap (the key) to its key/value pairs.
    // each string in the value part of the map is the line in the original
    // configuration file with the original key=value
    // e.g. the following lines in the configuration file:
    // bss=wlan0.1 <------------------------------  the key in the map: "wlan0.1"
    // ctrl_interface=/var/run/hostapd <----------  each line is an element in the array of the map's value
    // ap_isolate=1
    // ap_max_inactivity=60
    // bss_transition=1
    // interworking=1
    // disassoc_low_ack=1
    // bss=wlan0.2  <------------------------------ the key in the map: "wlan0.2"
    // ctrl_interface=/var/run/hostXXd
    // ap_isolate=1
    // ap_max_inactivity=60
    // bss_transition=0
    // interworking=3
    // creates two entries in the map:
    // { "wlan0.1" : ["ctrl_interface=/var/run/hostapd", "ap_isolate=1", "ap_max_inactivity=60", "bss_transition=1", "interworking=1", "disassoc_low_ack=1"] },
    // { "wlan0.2" : ["ctrl_interface=/var/run/hosXXpd", "ap_isolate=1", "ap_max_inactivity=60", "bss_transition=0", "interworking=3"] }
    std::map<std::string, std::vector<std::string>> m_hostapd_config_vaps;

    std::string m_last_message = "all good";

    // for logs
    friend std::ostream &operator<<(std::ostream &, const Configuration &);
};

// for logs
std::ostream &operator<<(std::ostream &, const Configuration &);

} // namespace config
} // namespace hostapd
} // namespace prplmesh
