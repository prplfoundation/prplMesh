/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __TLVF_UTILS_H__
#define __TLVF_UTILS_H__

#include <tlvf/CmduMessageTx.h>

#include <beerocks/tlvf/beerocks_message.h>

namespace beerocks {

class tlvf_utils {
public:
    /**
     * @brief Adds a new AP Radio Basic Capabilities TLV to given message.
     *
     * @param[in,out] cmdu_tx CDMU message.
     * @param[in] ruid Radio unique identifier of the radio for which capabilities are reported.
     * @param[in] supported_channels List of channels supported by AP.
     *
     * @return True on success and false otherwise.
     */
    static bool add_ap_radio_basic_capabilities(
        ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &ruid,
        const std::array<beerocks::message::sWifiChannel,
                         beerocks::message::SUPPORTED_CHANNELS_LENGTH> &supported_channels);
};

} // namespace beerocks

#endif // __TLVF_UTILS_H__
