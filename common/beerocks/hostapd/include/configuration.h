/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <iostream>
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
    Configuration(const std::string &file_name);

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
     */
    bool load();

    /**
     * @brief stores the configuration 
     * @details stores the internal representation of the configuration
     * into the file it was loaded from, effectively changing the configuration
     * @return *this (as bool, see above)
     */
    bool store();

    /**
     * @brief set key/value for the given vap
     * @details set the key/vale for the given vap, either replace or create.
     * @param 
     * - the vap to set the value for (string)
     * - the key to set its value (string)
     * - the value (string)
     * @return 
     * true - the vap exists, values were set.
     * false - the given vap was not found
     */
    bool set_create_vap_value(const std::string &vap, const std::string &key,
                              const std::string &value);

    /**
     * @brief set key/value for the given vap
     * @details set the key/vale for the given vap, either replace or create.
     * @param 
     * - the vap to set the value for (string)
     * - the key to set its value (string)
     * - the value (int)
     * @return 
     * true - the vap exists, values were set.
     * false - the given vap was not found
     */
    bool set_create_vap_value(const std::string &vap, const std::string &key, const int value);

    /**
     * @brief get the value of the given key for the given vap
     * @param 
     * - the vap to get the value for (string)
     * - the key to get its value (string)
     * @return a string with the value or empty if not found
     */
    std::string get_vap_value(const std::string &vap, const std::string &key);

    /**
     * @brief disables vap by adding a comment to it
     * e.g: 
     * before:
     * bss=wlan0_0 
     * ssid=test2 
     * bss=wlan0_1
     * after:
     * #bss=wlan0_0 
     * #ssid=test2 
     * bss=wlan0_1
     *
     */
    void comment_vap(const std::string &vap);

    /**
     * @brief enables vap by removing comments from it
     * e.g: 
     * before:
     * #bss=wlan0_0 
     * ##ssid=test2 
     * bss=wlan0_1
     * after:
     * bss=wlan0_0 
     * ssid=test2 
     * bss=wlan0_1
     *
     */
    void uncomment_vap(const std::string &vap);

    /**
     * @brief apply func to all ap vaps
     * @details apply func to all vaps that thier mode is "ap"
     * (leaving STAs vaps untouched for example).
     */
    template <class func> void for_all_ap_vaps(func);

    /**
     * @brief apply func to all ap vaps, increment the given iterator 
     * with each call to func: f(current_vap, ++user_itertor)
     * func interface: template<class iter> f(const std::string& vap, iter user_iterator)
     * @details apply func to all vaps that thier mode is "ap" while incrementing
     * the user iterator. this functionality enables iterating over two containers
     * in paralel: the ap-vaps and the user container.
     */
    template <class func, class iter> void for_all_ap_vaps(func, iter current, const iter end);

    /**
     * @brief for debug: return the last internal message
     * @details each action on this class changes its internal
     * message (similar to errno) - for debug usage
     * @return string describing the last message
     */
    const std::string &get_last_message() const;

private:
    /**
     * @brief helper: get exiting vap
     * @details any function that works on a specific vap does
     * the same: search the vap, set a variable when found
     * and set an error state when not found, this function
     * does it instead of copy/paste the same code
     * @param the calling function, string (to report error)
     * @param the requested vap, string
     * @return tuple<bool,std::vector<std::string>*> (found/not found, a 
     * pointer to the vap's array)
     */
    std::tuple<bool, std::vector<std::string> *> get_vap(const std::string &calling_function,
                                                         const std::string &vap);

    /**
     * @brief helper: check if the line contains the key
     * @param line in the format key=value or commented ###key=value
     * @param the requested key, string
     * @return true/false if the line contains the key
     */
    bool is_key_in_line(const std::string &line, const std::string &key) const;

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

template <class func> void Configuration::for_all_ap_vaps(func f)
{
    auto f_with_iter = [&f](const std::string &vap, int iter) { f(vap); };

    int dummy(0);
    for_all_ap_vaps(f_with_iter, dummy, 10);
}

template <class func, class iter>
void Configuration::for_all_ap_vaps(func f, iter current_iter, const iter end)
{
    for_each(m_hostapd_config_vaps.begin(), m_hostapd_config_vaps.end(),
             [this, &f, &current_iter,
              &end](const std::pair<std::string, std::vector<std::string>> &vap) {
                 if (get_vap_value(vap.first, "mode") == "ap") {
                     if (end == current_iter) {
                         f(vap.first, end);
                     } else {
                         f(vap.first, current_iter++);
                     }
                 }
             });
}

// for logs
std::ostream &operator<<(std::ostream &, const Configuration &);

} // namespace config
} // namespace hostapd
} // namespace prplmesh
