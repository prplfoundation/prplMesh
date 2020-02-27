/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef __BWL_NL80211_CLIENT_FACTORY_H__
#define __BWL_NL80211_CLIENT_FACTORY_H__

#include "nl80211_client.h"

#include <memory>

namespace bwl {

/**
 * @brief NL80211 client factory.
 *
 * Factory to create NL80211 client instances. Each BWL implementation should define one.
 */
class nl80211_client_factory {

public:
    /**
     * @brief Creates a new NL80211 client instance.
     *
     * @return NL80211 client instance.
     */
    static std::unique_ptr<nl80211_client> create_instance();
};

} // namespace bwl

#endif
