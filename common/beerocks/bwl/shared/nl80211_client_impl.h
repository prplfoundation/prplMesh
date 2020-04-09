/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_CLIENT_IMPL_H__
#define __BWL_NL80211_CLIENT_IMPL_H__

#include "bwl/nl80211_client.h"
#include "nl80211_socket.h"

#include <memory>

namespace bwl {

/**
 * @brief NL80211 client implementation.
 *
 * This class implements the NL80211 client interface. Communication with the WiFi driver is
 * performed with a NL80211 socket injected in the class constructor.
 *
 * Both the DWPAL and the NL80211 flavors of the BWL library use this implementation to send
 * standard NL80211 commands to the WiFi driver.
 */
class nl80211_client_impl : public nl80211_client {

public:
    /**
     * @brief Class constructor.
     *
     * Note: provided socket must be connected before calling any of the methods in this class.
     *
     * @param[in] socket NL80211 socket to send messages and receive responses to/from the WiFi
     * driver.
     */
    explicit nl80211_client_impl(std::unique_ptr<nl80211_socket> socket);

    /**
     * @brief Class destructor.
     */
    virtual ~nl80211_client_impl() = default;

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
                                    interface_info &interface_info) override;
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
    virtual bool get_radio_info(const std::string &interface_name, radio_info &radio_info) override;

    /**
     * @brief Gets station information.
     *
     * Station information contains basic metrics associated to the link between given local
     * interface and the interface of a station with MAC address 'sta_mac_address'.
     *
     * @param[in] interface_name Virtual AP (VAP) interface name.
     * @param[in] sta_mac_address MAC address of a station connected to the local interface.
     * @param[out] sta_info Station information.
     *
     * @return True on success and false otherwise.
     */
    virtual bool get_sta_info(const std::string &interface_name, const sMacAddr &sta_mac_address,
                              sta_info &sta_info) override;

    /**
     * @brief Set the tx power limit
     *
     * Set tx power limit for a radio
     *
     * @param[in] interface_name radio interface name.
     * @param[in] limit tx power limit in dBm to set
     * @return true success and false otherwise
     */
    virtual bool set_tx_power_limit(const std::string &interface_name, uint32_t limit) override;

private:
    /**
     * NL80211 socket to send messages and receive responses to/from the WiFi driver.
     */
    std::unique_ptr<nl80211_socket> m_socket;
};

} // namespace bwl

#endif
