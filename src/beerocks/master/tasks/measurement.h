/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _MEASUREMENT_H_
#define _MEASUREMENT_H_

#include "son_types.h"

#include <chrono>

namespace son {
class measurement {
public:
    const std::string ap_mac;
    int8_t rssi;
    std::chrono::steady_clock::time_point timestamp;
    measurement(std::string ap_mac_, int8_t rssi_) : ap_mac(ap_mac_), rssi(rssi_)
    {
        timestamp = std::chrono::steady_clock::now();
    }
};

} // namespace son

#endif
