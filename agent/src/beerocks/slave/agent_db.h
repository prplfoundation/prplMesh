/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#include <bcl/beerocks_defines.h>
#include <bcl/network/network_utils.h>
#include <bwl/sta_wlan_hal.h>

#include <iostream>
#include <memory>
#include <mutex>

namespace beerocks {

/**
 * @brief AgentDB is a class used by all main Agent threads on the Agent process to store the Agent
 * common data.
 * 
 * The class is implemented using singleton design pattern.
 * It is thread safe and being locked and released automatically.
 * 
 * How to use:
 * @code
 * auto db = AgentDB::get();            // Get DB and automatically lock it. The lock will be
 *                                      // Released when 'db' will be destructed.
 * 
 * db->foo = 42;                        // Change database member 'foo' to value 42.
 * db->bar = "prpl"                     // Set another database member.
 * auto &my_foo = db->foo;              // Get a refernce to a member.
 * 
 * AgentDB::get()->foo = 44;            // Set database member directly.
 * 
 * 
 * Unsafe operation which should never be done:
 * auto my_foo = AgentDB::get()->foo;   // Unsafe! Don't do it! The database might be unlocked,
 *                                      // if used on ranged loop. So better not use it.
 * 
 * auto &foo = AgentDB::get()->foo;     // Unsafe! Don't do it! The database will be unlocked,
 *                                      // but the reference will remain.
 * 
 * std::string &get_foo() {             // Unsafe! Don't do it! Returning refernce to the database
 *     auto db = AgentDB::get();        // on a wrapper function is unsafe because the database will
 *     return db->foo;                  // be unlocked when the function ends, and the caller will
 * }                                    // hold a refernce to it.
 * @endcode
 */
class AgentDB {
public:
    class SafeDB {
    public:
        explicit SafeDB(AgentDB &db) : m_db(db) { m_db.db_lock(); }
        ~SafeDB() { m_db.db_unlock(); }
        AgentDB *operator->() { return &m_db; }

    private:
        AgentDB &m_db;
    };
    static SafeDB get()
    {
        // Guaranteed to be destroyed.
        // Instantiated on first use.
        static AgentDB instance;
        return SafeDB(instance);
    }
    AgentDB(const AgentDB &) = delete;
    void operator=(const AgentDB &) = delete;

private:
    // Private constructor so that no objects can be created.
    AgentDB() = default;
    std::recursive_mutex m_db_mutex;
    void db_lock() { m_db_mutex.lock(); }
    void db_unlock() { m_db_mutex.unlock(); }

    /* Put down from here database members and functions used by the Agent modules */

public:
    /* Agent Configuration */
    struct sDeviceConf {
        struct sFrontRadio {

        } front_radio;

        struct sBackRadio {

        } back_radio;

        bool local_gw;
        bool local_controller;
    } device_conf;

    /** 
     * Agent Sub Entities Data
     */
    struct sBridge {
        sMacAddr mac;
        std::string iface_name;
    } bridge;

    struct sBackhaul {
        enum class eConnectionType { Invalid = 0, Wired, Wireless } connection_type;
        std::string selected_iface_name;
    } backhaul;

    struct sEthernet {
        sMacAddr mac;
        std::string iface_name;
    } ethernet;

    struct sRadio {
        sRadio(const std::string &front_iface_name, const std::string &back_iface_name)
            : front(front_iface_name), back(back_iface_name)
        {
        }

        struct sFront {
            explicit sFront(const std::string &iface_name_)
                : iface_name(iface_name_), iface_mac(net::network_utils::ZERO_MAC),
                  max_supported_bw(eWiFiBandwidth::BANDWIDTH_UNKNOWN),
                  freq_type(eFreqType::FREQ_UNKNOWN)
            {
            }
            std::string iface_name;
            sMacAddr iface_mac;
            eWiFiBandwidth max_supported_bw;
            eFreqType freq_type;

            struct sBssid {
                sMacAddr mac;
                std::string ssid;
                enum class eType { fAP, bAP } type;
            };
            std::array<sBssid, eBeeRocksIfaceIds::IFACE_TOTAL_VAPS> bssids;
        } front;

        struct sBack {
            explicit sBack(const std::string &iface_name_)
                : iface_name(iface_name_), iface_mac(net::network_utils::ZERO_MAC)
            {
            }
            std::string iface_name;
            sMacAddr iface_mac;
        } back;

        struct sClient {
            sClient(sMacAddr bssid_, size_t association_frame_length_, uint8_t *association_frame_)
                : bssid(bssid_), association_time(std::chrono::steady_clock::now()),
                  association_frame_length(association_frame_length_)
            {
                std::copy_n(association_frame_, association_frame_length_,
                            association_frame.begin());
            }
            sMacAddr bssid;
            std::chrono::steady_clock::time_point association_time;
            size_t association_frame_length;
            std::array<uint8_t, ASSOCIATION_FRAME_SIZE> association_frame;
        };
        // Associated clients grouped by Client MAC.
        std::unordered_map<sMacAddr, sClient> associated_clients;
    };

    /**
     * @brief Get pointer to the radio data struct of a specific interface. The function can
     * accepts either front or back interface name.
     * 
     * @param iface_name Interface name of a radio, front or back.
     * @return std::unique_ptr<sRadio> to the radio struct if exist, otherwise, nullptr.
     */
    sRadio *radio(const std::string &iface_name);

    /**
     * @brief Add radio node to the database. At least one of the input arguments must not be empty.
     * This function should be called only once with valid arguments. If called once and then called
     * again, the radio struct on the database will not be updated with new arguments, and the
     * function will return false.
     * 
     * @param front_iface_name Front interface name.
     * @param back_iface_name Back interface name.
     * @return true if a radio struct has been added to the database, otherwise return false.
     */
    bool add_radio(const std::string &front_iface_name, const std::string &back_iface_name);

    /**
     * @brief Get list of all radio objects on the database.
     * This function shall be used in order to iterate over all the radios. 
     * 
     * @return const std::vector<std::string &>& Intefaces names list.
     */
    const std::vector<sRadio *> &get_radios_list() { return m_radios_list; };

    /* Helper enum for get_radio_by_mac() function */
    enum class eMacType : uint8_t { ALL, RADIO, BSSID, CLIENT };

    /**
     * @brief Get a pointer to the parent radio struct of given MAC address. 
     * 
     * @param mac MAC address of radio interface or bssid.
     * @param mac_type_hint Hint for the MAC type, for faster lookup.
     * @return sRadio* A pointer to the radio struct containing the given MAC address.
     */
    sRadio *get_radio_by_mac(const sMacAddr &mac, eMacType mac_type_hint = eMacType::ALL);

    /**
     * @brief Erase client from associated_clients list.
     * If @a bssid is given, then remove client only from its radio, otherwise remove from all
     * radios.
     * 
     * @param client_mac The client MAC address.
     * @param bssid The bssid that the client will be removed from its radio.
     */
    void erase_client(const sMacAddr &client_mac, sMacAddr bssid = net::network_utils::ZERO_MAC);

    /**
     * @brief Get the MAC address (or bssid) of an AP based on the ruid and ssid.
     *
     * @param[in] ruid The Radio UID.
     * @param[in] ssid The ssid of the AP.
     * @param[out] value The mac address/bssid if found, else an invalid MAC (zero).
     * @return true if the mac/bssid was found, false otherwise.
     */
    bool get_mac_by_ssid(const sMacAddr &ruid, const std::string &ssid, sMacAddr &value);

    /**
     * @brief 1905.1 Neighbor device information
     * Information gathered from a neighbor device upon reception of a Topology Discovery message.
     */
    struct sNeighborDevice {
        // MAC address of the interface on which the Topology Discovery message is transmitted.
        sMacAddr transmitting_iface_mac;

        // Timestamp of the last Topology Discovery message received from this neighbor device.
        std::chrono::steady_clock::time_point timestamp;
    };

    /*
     * @brief List of known 1905 neighbor devices.
     * 
     * Upper key: Local interface MAC on which the Topology Discovery message was received from.
     * Upper value: Map containing 1905.1 device information ordered by neighbor device al_mac -
     *  Sub-key: 1905.1 AL MAC address of the Topology Discovery message transmitting device.
     *  Sub-value: 1905.1 device information.
     * 
     * Devices are being added to the list when receiving a 1905.1 Topology Discovery message from
     * an unknown 1905.1 device. Every 1905.1 device shall send this message every 60 seconds, and
     * we update the time stamp in which the message is received.
     */
    std::unordered_map<sMacAddr, std::unordered_map<sMacAddr, sNeighborDevice>> neighbor_devices;

private:
    std::list<sRadio> m_radios;
    std::vector<sRadio *> m_radios_list;
};

} // namespace beerocks
