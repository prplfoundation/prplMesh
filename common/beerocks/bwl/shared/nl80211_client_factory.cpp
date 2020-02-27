/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 MaxLinear
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <bwl/nl80211_client_factory.h>

#include "nl80211_client_impl.h"

#include <bcl/beerocks_backport.h>

#include <easylogging++.h>

namespace bwl {

std::unique_ptr<nl80211_client> nl80211_client_factory::create_instance()
{
    // Create NL80211 socket to communicate with WiFi driver
    auto socket = std::make_unique<bwl::nl80211_socket>();

    // Connect NL80211 socket (it will be automatically closed when it is not needed any more)
    if (!socket->connect()) {
        LOG(ERROR) << "Failed to connect nl80211 socket!";
        return nullptr;
    }

    // Create NL80211 client to send standard NL80211 commands to WiFi driver using NL80211 socket
    return std::make_unique<nl80211_client_impl>(std::move(socket));
}

} // namespace bwl
