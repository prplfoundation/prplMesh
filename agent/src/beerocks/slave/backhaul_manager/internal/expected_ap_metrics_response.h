/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _EXPECTED_AP_METRICS_RESPONSE_H
#define _EXPECTED_AP_METRICS_RESPONSE_H

#include <bcl/beerocks_defines.h>
#include <tlvf/CmduMessageTx.h>
#include <tlvf/common/sMacAddr.h>
#include <tlvf/tlvftypes.h>
#include <unordered_set>

class ExpectedApMetricsResponse {
public:
    /**
     * @brief resets this object with a new mid
     * @details resets all members of this object to its empty initial state except for the mid 
     * that is set as was given bt the parameter (mid default value is zero (0))
     * @param mid - the new mid value
     * @return radio mac of the found slave if found, otherwise empty string
     */
    void reset_to_new_mid(uint16_t mid = 0);
    uint16_t get_mid() const;
    ieee1905_1::CmduMessageTx &create_tx_message();

    // bssid related operations
public:
    void add_expected_bssid(const sMacAddr&);
    void remove_expected_bssid(const sMacAddr&);
    bool find_expected_bssid(const sMacAddr&) const;
    bool is_expected_bssid_empty() const;

private:
    uint16_t _mid = 0;
    std::unordered_set<sMacAddr> _expected_bssid_list;

private:
    uint8_t _response_buffer[beerocks::message::MESSAGE_BUFFER_LENGTH] = {0};
    ieee1905_1::CmduMessageTx _response = {_response_buffer, sizeof(_response_buffer)};
};

#endif
