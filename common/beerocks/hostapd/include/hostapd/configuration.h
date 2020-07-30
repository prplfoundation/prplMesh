/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace prplmesh {
namespace hostapd {

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
     * @param vap_indications - set the indications that a vap
     * configuration section begins. e.g. "bss=" and/or "interface="
     * note that the equal sign is expected to be part of vap_indication
     * std::string vap_indication("interface=");
     * vap indication can be any of the parameters supplied
     * @return *this (as bool, see above)
     */
    template<class... StringIndications>
    bool load(const StringIndications... vap_indications)
    {
        return load(std::set<std::string>{vap_indications...});
    }

    /**
     * @brief stores the configuration 
     * @details stores the internal representation of the configuration
     * into the file it was loaded from, effectively changing the configuration
     * @return *this (as bool, see above)
     */
    bool store();

    /**
     * @brief set key/value in the head section
     * @details set the key/vale in the head section,
     * either replace or create.
     * @note comments in the head section are not supported
     * for example, if the key/value line was commented before
     * the call to this function, it would be uncommented afterwards
     * @param 
     * - the key to set its value (string)
     * - the value (string)
     */
    bool set_create_head_value(const std::string &key, const std::string &value);

    /**
     * @brief set key/value in the head section
     * @details set the key/vale in the head section,
     * either replace or create.
     * @note comments in the head section are not supported
     * for example, if the key/value line was commented before
     * the call to this function, it would be uncommented afterwards
     * @param 
     * - the key to set its value (string)
     * - the value (int)
     */
    bool set_create_head_value(const std::string &key, const int value);

    /**
     * @brief get the value of the given key from the head
     * @param 
     * - the key to get its value (string)
     * @return a string with the value or empty if not found
     */
    std::string get_head_value(const std::string &key);

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
     * @details apply func to all vaps that ap_predicate returns
     * true for them. this enables leaving STAs vaps untouched for example.
     * @param func has the following interface:         void f(const std::string &vap);
     * @param ap_predicate has the following interface: bool f(const std::string &vap);
     */
    template <class func, class ap_predicate> void for_all_ap_vaps(func, ap_predicate);

    /**
     * @brief apply func to all ap vaps, increment the given iterator 
     * with each call to func: f(current_vap, ++user_itertor)
     * @details apply func to all vaps that ap_predicate returns true for
     * while incrementing the user iterator. 
     * this functionality enables iterating over two containers
     * in paralel: the ap-vaps and the user container.
     * @param func has the following interface:         template<class iter> f(const std::string& vap, iter user_iterator)
     * @param iter - iterator to the begining of the user sequence
     * @param ap_predicate has the following interface: bool f(const std::string &vap);
     */
    template <class func, class iter, class ap_predicate>
    void for_all_ap_vaps(func, iter current, const iter end, ap_predicate);

    /**
     * @brief for debug: return the last internal message
     * @details each action on this class changes its internal
     * message (similar to errno) - for debug usage
     * @return string describing the last message
     */
    const std::string &get_last_message() const;

private:
    /**
     * @brief helper: load configuration based on array of strings
     * indicating vap separation.
     * @param array of separations
     * @return *this as bool
     */
    bool load(const std::set<std::string> &vap_indicators);

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

    // m_ok hoslds the internal state of the configuration
    // the user may query the success/fail state of the last command
    // simply by reading the value of this variable.
    // the access to it is via operator bool()
    // m_ok itslef may be changed because of a call to
    // const function, therefore it is mutable
    mutable bool m_ok = false;

    // each string is a line in the original configuration file
    // that belongs to the "head" part. read the explenation at
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

    // see m_ok's comment
    mutable std::string m_last_message = "initial state, yet nothing was done";

    // for logs
    friend std::ostream &operator<<(std::ostream &, const Configuration &);
};

template <class func, class ap_predicate>
void Configuration::for_all_ap_vaps(func f, ap_predicate pred)
{
    auto f_with_iter = [&f](const std::string &vap, int iter) { f(vap); };

    int dummy(0);
    for_all_ap_vaps(f_with_iter, dummy, 10, pred);
}

template <class func, class iter, class ap_predicate>
void Configuration::for_all_ap_vaps(func f, iter current_iter, const iter end, ap_predicate pred)
{
    for_each(m_hostapd_config_vaps.begin(), m_hostapd_config_vaps.end(),
             [this, &f, &current_iter, &end,
              &pred](const std::pair<std::string, std::vector<std::string>> &vap) {
                 if (pred(vap.first)) {
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

} // namespace hostapd
} // namespace prplmesh
