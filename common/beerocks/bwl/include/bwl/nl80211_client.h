/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_CLIENT_H__
#define __BWL_NL80211_CLIENT_H__

#include <tlvf/common/sMacAddr.h>

#include <string>

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
     * @brief Station information
     *
     * Information obtained with NL80211_CMD_GET_STATION command through a NL80211 socket.
     * See NL80211_STA_INFO_* in <linux/nl80211.h> for a description of each field.
     */
    struct sStaInfo {
        uint32_t inactive_time_ms   = 0;
        uint32_t rx_bytes           = 0;
        uint32_t rx_packets         = 0;
        uint32_t tx_bytes           = 0;
        uint32_t tx_packets         = 0;
        uint32_t tx_retries         = 0;
        uint32_t tx_failed          = 0;
        uint8_t signal_dBm          = 0;
        uint8_t signal_avg_dBm      = 0;
        uint16_t tx_bitrate_100kbps = 0;
        uint16_t rx_bitrate_100kbps = 0;
    };

    /**
     * @brief Class destructor.
     */
    virtual ~nl80211_client() = default;

    /**
     * @brief Gets station information.
     *
     * Station information contains basically metrics associated to the link between given local
     * interface and a the interface of a station whose MAC address is 'sta_mac_address'.
     *
     * @param[in] local_interface_name Virtual AP (VAP) interface name.
     * @param[in] sta_mac_address MAC address of a station connected to the local interface.
     * @param[out] sta_info Station information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_sta_info(const std::string &local_interface_name,
                              const sMacAddr &sta_mac_address, sStaInfo &sta_info) = 0;
    /**
     * @brief Set the tx power limit
     *
     * Set tx power limit for a radio
     *
     * @param[in] local_interface_name radio interface name.
     * @param[in] limit tx power limit in dBM to set 
     * @return true success and false otherwise
     */
    virtual bool set_tx_power_limit(const std::string &local_interface_name, uint32_t limit) = 0;

    // TODO: add remaining methods
};

} // namespace bwl

#endif
