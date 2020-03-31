/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_CLIENT_H__
#define __BWL_NL80211_CLIENT_H__

#include <bcl/beerocks_defines.h>
#include <bcl/beerocks_message_structs.h>
#include <bcl/son/son_wireless_utils.h>
#include <tlvf/common/sMacAddr.h>

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Length of HT MCS set.
 *
 * According to <linux/nl80211.h>, NL80211_BAND_ATTR_HT_MCS_SET is a 16-byte attribute containing
 * the MCS set as defined in 802.11n
 */
static constexpr size_t ht_mcs_set_size = 16;

/**
 * @brief Length of VHT MCS set.
 *
 * According to <linux/nl80211.h>, NL80211_BAND_ATTR_VHT_MCS_SET is a 32-byte attribute containing
 * the MCS set as defined in 802.11ac
 */
static constexpr size_t vht_mcs_set_size = 32;

namespace bwl {

/**
 * @brief NL80211 client interface.
 *
 * This interface lists the methods to read and write wireless hardware status and configuration
 * through a NL80211 socket.
 *
 * Programming to an interface allows dependent classes to remain unaware of the different
 * implementations of the interface as well as facilitates the creation of mock implementations for
 * unit testing.
 *
 * This is a C++ interface: an abstract class that is designed to be specifically used as a base
 * class and which derived classes (implementations) will override each pure virtual function.
 *
 * Known implementations: nl80211_client_impl (uses NL80211 socket), nl80211_client_dummy (fake
 * implementation that returns dummy data).
 */
class nl80211_client {

public:
    /**
     * @brief Interface information
     *
     * Information obtained with NL80211_CMD_GET_INTERFACE command through a NL80211 socket.
     * See <linux/nl80211.h> for a description of each field.
     */
    struct interface_info {

        /**
         * Network interface name.
         * Obtained from NL80211_ATTR_IFNAME
         */
        std::string name;

        /**
         * Network interface index of the device to operate on.
         * Obtained from NL80211_ATTR_IFINDEX
         */
        uint32_t index = 0;

        /**
         * MAC address of the interface.
         * Obtained from NL80211_ATTR_MAC
         */
        sMacAddr addr;

        /**
         * SSID of the interface.
         * Obtained from NL80211_ATTR_SSID
         */
        std::string ssid;

        /**
         * Type of virtual interface.
         * Obtained from NL80211_ATTR_IFTYPE
         * Possible values are defined in enum nl80211_iftype in <linux/nl80211.h>
         */
        uint32_t type = 0;

        /**
         * Index of wiphy to operate on.
         * Obtained from NL80211_ATTR_WIPHY
         */
        uint32_t wiphy = 0;
    };

    /**
     * @brief Channel information
     *
     * Used in band_info structure as each band contains a map of supported channels.
     *
     * Information obtained with NL80211_CMD_GET_WIPHY command through a NL80211 socket.
     * See NL80211_FREQUENCY_ATTR_* in <linux/nl80211.h> for a description of each field.
     */
    struct channel_info {

        /**
         * Channel number.
         * Obtained from NL80211_FREQUENCY_ATTR_FREQ (see 802.11-2007 17.3.8.3.2 and Annex J)
         */
        uint8_t number = 0;

        /**
         * Supported channel bandwidths.
         */
        std::vector<beerocks::eWiFiBandwidth> supported_bandwidths;

        /**
         * Maximum transmission power in dBm.
         * Obtained as 0.01 * NL80211_FREQUENCY_ATTR_MAX_TX_POWER
         */
        uint8_t tx_power = 0;

        /**
         * Radar detection is mandatory on this channel in current regulatory domain.
         * Set to true if attribute NL80211_FREQUENCY_ATTR_RADAR is present.
         */
        bool is_dfs = false;

        /**
         * True if current state for DFS is NL80211_DFS_UNAVAILABLE.
         * Obtained from NL80211_FREQUENCY_ATTR_DFS_STATE
         */
        bool radar_affected = false;

        /**
         * @brief Gets the maximum supported bandwidth by the channel.
         *
         * @return Maximum supported bandwidth
         */
        beerocks::eWiFiBandwidth get_max_bandwidth() const
        {
            if (supported_bandwidths.empty()) {
                return beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN;
            }

            return *std::max_element(supported_bandwidths.begin(), supported_bandwidths.end());
        }
    };

    /**
     * @brief Band information
     *
     * Used in radio_info structure as each radio contains a list of bands.
     *
     * Information obtained with NL80211_CMD_GET_WIPHY command through a NL80211 socket.
     * See NL80211_BAND_ATTR_* in <linux/nl80211.h> for a description of each field.
     */
    struct band_info {

        /**
         * Value of NL80211_BAND_ATTR_HT_CAPA, see iw/util.c print_ht_capability() as a bit
         * interpretation example.
         */
        uint16_t ht_capability = 0;

        /**
         * Value of NL80211_BAND_ATTR_HT_MCS_SET, see iw/util.c print_ht_mcs() as a byte
         * interpretation example.
         */
        uint8_t ht_mcs_set[ht_mcs_set_size];

        /**
         * Value of NL80211_BAND_ATTR_VHT_CAPA, see iw/util.c print_vht_info() as a bit
         * interpretation example.
         */
        uint32_t vht_capability = 0;

        /**
         * Value of NL80211_BAND_ATTR_VHT_MCS_SET, see iw/util.c print_vht_info() as a byte
         * interpretation example.
         */
        uint8_t vht_mcs_set[vht_mcs_set_size];

        /**
         * Channels supported in this band (obtained from NL80211_BAND_ATTR_FREQS).
         * Map key is the channel number and map value is the channel information.
         */
        std::unordered_map<uint8_t, channel_info> supported_channels;

        /**
         * @brief Gets the frequency band of this band.
         *
         * Return value can be obtained from whatever of the supported channels.
         *
         * @return Frequency band
         */
        beerocks::eFreqType get_frequency_band() const
        {
            if (supported_channels.empty()) {
                return beerocks::eFreqType::FREQ_UNKNOWN;
            }

            return son::wireless_utils::which_freq(supported_channels.begin()->first);
        }

        /**
         * @brief Checks if this band is the 5GHz band.
         *
         * @return True if this band is the 5GHz band and false otherwise.
         */
        bool is_5ghz_band() const { return beerocks::eFreqType::FREQ_5G == get_frequency_band(); }

        /**
         * @brief Gets the maximum supported bandwidth by all the channels in the band.
         *
         * @return Maximum supported bandwidth
         */
        beerocks::eWiFiBandwidth get_max_bandwidth() const
        {
            beerocks::eWiFiBandwidth max_bandwith = beerocks::eWiFiBandwidth::BANDWIDTH_UNKNOWN;

            for (const auto &it : supported_channels) {
                channel_info channel = it.second;
                max_bandwith         = std::max(max_bandwith, channel.get_max_bandwidth());
            }

            return max_bandwith;
        }
    };

    /**
     * @brief Radio information
     *
     * Information obtained with NL80211_CMD_GET_WIPHY command through a NL80211 socket.
     * See <linux/nl80211.h> for a description of each field.
     */
    struct radio_info {

        /**
         * Bands included in this radio (obtained from NL80211_ATTR_WIPHY_BANDS)
         */
        std::vector<band_info> bands;
    };

    /**
     * @brief Station information
     *
     * Information obtained with NL80211_CMD_GET_STATION command through a NL80211 socket.
     * See NL80211_STA_INFO_* in <linux/nl80211.h> for a description of each field.
     */
    struct sta_info {
        uint32_t inactive_time_ms   = 0;
        uint32_t rx_bytes           = 0;
        uint32_t rx_packets         = 0;
        uint32_t tx_bytes           = 0;
        uint32_t tx_packets         = 0;
        uint32_t tx_retries         = 0;
        uint32_t tx_failed          = 0;
        uint8_t signal_dbm          = 0;
        uint8_t signal_avg_dbm      = 0;
        uint16_t tx_bitrate_100kbps = 0;
        uint16_t rx_bitrate_100kbps = 0;
    };

    /**
     * @brief Class destructor.
     */
    virtual ~nl80211_client() = default;

    /**
     * @brief Gets interface information.
     *
     * Interface information contains, among others, the MAC address and SSID of the given network
     * interface.
     *
     * @param[in] interface_name Interface name, either radio or Virtual AP (VAP).
     * @param[out] interface_info Interface information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_interface_info(const std::string &interface_name,
                                    interface_info &interface_info) = 0;

    /**
     * @brief Gets radio information.
     *
     * Radio information contains HT/VHT capabilities and the list of supported channels.
     *
     * @param[in] interface_name Interface name, either radio or Virtual AP (VAP).
     * @param[out] radio_info Radio information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_radio_info(const std::string &interface_name, radio_info &radio_info) = 0;

    /**
     * @brief Gets station information.
     *
     * Station information contains basically metrics associated to the link between given local
     * interface and a the interface of a station whose MAC address is 'sta_mac_address'.
     *
     * @param[in] interface_name Virtual AP (VAP) interface name.
     * @param[in] sta_mac_address MAC address of a station connected to the local interface.
     * @param[out] sta_info Station information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_sta_info(const std::string &interface_name, const sMacAddr &sta_mac_address,
                              sta_info &sta_info) = 0;

    /**
     * @brief Set the tx power limit
     *
     * Set tx power limit for a radio
     *
     * @param[in] interface_name radio interface name.
     * @param[in] limit tx power limit in dBm to set
     * @return true success and false otherwise
     */
    virtual bool set_tx_power_limit(const std::string &interface_name, uint32_t limit) = 0;
};

} // namespace bwl

#endif
