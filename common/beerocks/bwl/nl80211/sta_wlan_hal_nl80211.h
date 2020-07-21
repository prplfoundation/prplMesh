/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_STA_WLAN_HAL_NL80211_H_
#define _BWL_STA_WLAN_HAL_NL80211_H_

#include "base_wlan_hal_nl80211.h"
#include <bwl/sta_wlan_hal.h>

#define BUFFER_SIZE 4096

namespace bwl {
namespace nl80211 {

/*!
 * Hardware abstraction layer for WLAN Station/Client.
 */
class sta_wlan_hal_nl80211 : public base_wlan_hal_nl80211, public sta_wlan_hal {

    // Public methods
public:
    /*!
     * Constructor.
     *
     * @param [in] iface_name STA/Client interface name.
     * @param [in] callback Callback for handling internal events.
     */
    sta_wlan_hal_nl80211(const std::string &iface_name, hal_event_cb_t callback,
                         const bwl::hal_conf_t &hal_conf);
    virtual ~sta_wlan_hal_nl80211();

    virtual bool start_wps_pbc() override;
    virtual bool detach() override;

    virtual bool initiate_scan() override;
    virtual int get_scan_results(const std::string &ssid, std::vector<SScanResult> &list,
                                 bool parse_vsie = false) override;

    virtual bool connect(const std::string &ssid, const std::string &pass, WiFiSec sec,
                         bool mem_only_psk, const std::string &bssid, uint8_t channel,
                         bool hidden_ssid) override;

    virtual bool disconnect() override;

    virtual bool roam(const sMacAddr &bssid, uint8_t channel) override;

    virtual bool get_4addr_mode() override;
    virtual bool set_4addr_mode(bool enable) override;

    virtual bool unassoc_rssi_measurement(const std::string &mac, int chan, int bw,
                                          int vht_center_frequency, int delay,
                                          int window_size) override;

    virtual bool is_connected() override;
    virtual int get_channel() override;
    virtual bool update_status() override;

    std::string get_ssid() override;
    std::string get_bssid() override;

protected:
    virtual bool process_nl80211_event(parsed_obj_map_t &parsed_obj) override;

    // Overload for Monitor events
    bool event_queue_push(sta_wlan_hal::Event event, std::shared_ptr<void> data = {})
    {
        return base_wlan_hal::event_queue_push(int(event), data);
    }

private:
    /**
     * @brief Connection status information.
     *
     * Current WPA/EAPOL/EAP status information obtained with the STATUS wpa_supplicant command.
     */
    struct ConnectionStatus {
        /**
         * Basic Service Set Identifier.
         */
        std::string bssid;

        /**
         * Channel frequency in MHz for IBSS.
         */
        int freq;

        /**
         * Service Set Identifier (network name).
         */
        std::string ssid;

        /**
         * Network ID
         */
        int id;

        /**
         * IEEE 802.11 operation mode
         */
        std::string mode;

        /**
         * Allowed pairwise ciphers (WPA_CIPHER_*)
         */
        std::string pairwise_cipher;

        /**
         * Allowed group ciphers (WPA_CIPHER_*)
         */
        std::string group_cipher;

        /**
         * Allowed key management protocols (WPA_KEY_MGMT_*)
         */
        std::string key_mgmt;

        /**
         * Current wpa_supplicant state
         */
        std::string wpa_state;

        /**
         * MAC address of the network interface
         */
        std::string address;

        /**
         * Universally Unique Identifier of the device
         */
        std::string uuid;
    };

    /**
     * @brief Adds a new network.
     *
     * Executes wpa_supplicant command ADD_NETWORK to create new network with empty configuration.
     * The new network is disabled and once it has been configured it can be enabled with
     * ENABLE_NETWORK command.
     *
     * @return Network ID of the new network or -1 on failure.
     */
    int add_network();

    /**
     * @brief Sets a network variable.
     *
     * Executes wpa_supplicant command SET_NETWORK to set a network variable value.
     *
     * This command uses the same variables and data formats as the wpa_supplicant configuration
     * file. Examples:
     * - ssid (network name, SSID)
     * - psk (WPA passphrase or pre-shared key)
     * - key_mgmt (key management protocol)
     * - identity (EAP identity)
     * - password (EAP password)
     * - ...
     *
     * @param network_id Network ID of the network whose variable to set.
     * @param param Name of the variable.
     * @param value Value to set.
     * @return True on success and false otherwise.
     */
    bool set_network(int network_id, const std::string &param, const std::string &value);

    /**
     * @brief Configures a network.
     *
     * Configures a newly added network before it can be enabled.
     *
     * @param network_id Network ID of the network to be configured.
     * @param ssid Service Set Identifier (network name).
     * @param bssid Basic Service Set Identifier.
     * @param sec  WLAN Security Type (converted to a key management protocol).
     * @param mem_only_psk Keep PSK/passphrase only in memory.
     * @param pass WPA ASCII passphrase. If this is set, PSK will be generated using the SSID and
     * passphrase configured for the network. ASCII passphrase must be between 8 and 63 characters
     * (inclusive).
     * @param hidden_ssid Scan this SSID with Probe Requests (scan_ssid can be used to scan for
     * APs using hidden SSIDs).
     * @param freq Frequency in MHz to allow for selecting the BSS. If 0, all frequencies are
     * considered when selecting a BSS.
     * @return True on success and false otherwise.
     */
    bool set_network_params(int network_id, const std::string &ssid, const std::string &bssid,
                            WiFiSec sec, bool mem_only_psk, const std::string &pass,
                            bool hidden_ssid, int freq = 0);

    /**
     * @brief Reads connection status information.
     *
     * Executes wpa_supplicant command STATUS to read current WPA/EAPOL/EAP status information and
     * fills in given structure.
     *
     * @param[in,out] connection_status Connection status structure to fill in.
     * @return True on success and false otherwise.
     */
    bool read_status(ConnectionStatus &connection_status);

    /**
     * @brief Updates connection status information.
     *
     * Sets class members containing active connection information with given connection status
     * information (i.e.: network ID, BSSID, channel and SSID).
     *
     * @param connection_status Connection status information.
     */
    void update_status(const ConnectionStatus &connection_status);

    /**
     * @brief Enables a network.
     *
     * Executes wpa_supplicant command ENABLE_NETWORK to enable a network.
     *
     * @param network_id Network ID of the network to be enabled.
     * @return True on success and false otherwise.
     */
    bool enable_network(int network_id);

    /**
     * @brief Checks if all authentication completed.
     *
     * Checks if wpa_supplicant has completed its processing for the association phase and that
     * data connection is fully configured.
     *
     * @param wpa_state wpa_supplicant state (obtained with STATUS wpa_supplicant command).
     * @return True if given wpa_supplicant state is "COMPLETED" and false otherwise.
     */
    bool is_connected(const std::string &wpa_state);

    // Active network parameters
    std::string m_active_ssid;
    std::string m_active_bssid;
    std::string m_active_pass;
    WiFiSec m_active_secutiry = WiFiSec::Invalid;
    uint8_t m_active_channel  = 0;
    int m_active_network_id   = -1;

    // Unassociated measurement state variables
    std::chrono::steady_clock::time_point m_unassoc_measure_start;
    int m_unassoc_measure_window_size = 0;
    int m_unassoc_measure_delay       = 0;
};

} // namespace nl80211
} // namespace bwl

#endif // _BWL_STA_WLAN_HAL_NL80211_H_
