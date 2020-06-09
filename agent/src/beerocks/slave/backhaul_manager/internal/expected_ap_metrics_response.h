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

namespace beerocks {

class ExpectedApMetricsResponse {
public:
    /**
     * @brief resets this object with a new mid
     * @details resets all members of this object to its empty initial state except for the mid 
     * that is set as was given bt the parameter (mid default value is zero (0))
     * @param mid the new mid value
     * @return radio mac of the found slave if found, otherwise empty string
     */
    void reset_to_new_mid(uint16_t mid = 0);

    /**
     * @brief returns the mid of this object
     * @return mid
     */
    uint16_t get_mid() const;

    /**
     * @brief creates and returns a AP-RESPONSE-METRICS Tx message 
     * @return a fresh new CmduMessage with the correct type and mid
     */
    ieee1905_1::CmduMessageTx &create_tx_message();

    // bssid related operations

    /**
     * @brief stores expected bssids
     * @details adds the bssids in the range [first,last) to the already existing internal list
     * @param first iterator to the source begin
     * @param last iterator to the source last
     * @return - nothing
     */
    template <class FirstIt, class LastIt> void add_expected_bssid(FirstIt first, LastIt last);

    /**
     * @brief stores expected bssid
     * @details adds the input bssid to the already existing internal list
     * @param bssid the bssid to store
     * @return nothing
     */
    void add_expected_bssid(const sMacAddr &bssid);

    /**
     * @brief removes the given bssid from the internal list
     * @param bssid the bssid to remove
     * @return - nothing
     */
    void remove_expected_bssid(const sMacAddr &);

    /**
     * @brief search for the given bssid in the internal list
     * @param bssid the bssid to search
     * @return bool exists / does not exists
     */
    bool find_expected_bssid(const sMacAddr &) const;

    /**
     * @brief checks if the internal list is empty
     * @return bool empty / not empty
     */
    bool is_expected_bssid_empty() const;

private:
    uint16_t m_mid = 0;
    std::unordered_set<sMacAddr> m_expected_bssid_list;
    uint8_t m_response_buffer[beerocks::message::MESSAGE_BUFFER_LENGTH] = {0};
    ieee1905_1::CmduMessageTx m_response = {m_response_buffer, sizeof(m_response_buffer)};
};

template <class FirstIt, class LastIt>
void ExpectedApMetricsResponse::add_expected_bssid(FirstIt first, LastIt last)
{
    m_expected_bssid_list.insert(first, last);
}

} // namespace beerocks

#endif
