/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_CLIENT_DUMMY_H__
#define __BWL_NL80211_CLIENT_DUMMY_H__

#include <bwl/nl80211_client.h>

namespace bwl {

/**
 * @brief NL80211 client dummy implementation.
 *
 * This class is used by the dummy flavor of the BWL library and it is intended for testing
 * purposes only.
 */
class nl80211_client_dummy : public nl80211_client {

public:
    /**
     * @brief Class constructor.
     */
    nl80211_client_dummy();

    /**
     * @brief Class destructor.
     */
    virtual ~nl80211_client_dummy();

    /**
     * @brief Gets station information.
     *
     * Fills station information with dummy data.
     *
     * @param[in] local_interface_name Virtual AP (VAP) interface name.
     * @param[in] sta_mac_address MAC address of a station connected to the local interface.
     * @param[out] sta_info Station information.
     *
     * @return Dummy implementation returns always true.
     */
    virtual bool get_sta_info(const std::string &local_interface_name,
                              const sMacAddr &sta_mac_address, sStaInfo &sta_info) override;
};

} // namespace bwl

#endif
