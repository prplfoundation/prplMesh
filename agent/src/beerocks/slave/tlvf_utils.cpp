/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2020 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "tlvf_utils.h"

#include <bcl/son/son_wireless_utils.h>
#include <easylogging++.h>

#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>

using namespace beerocks;

bool tlvf_utils::add_ap_radio_basic_capabilities(
    ieee1905_1::CmduMessageTx &cmdu_tx, const sMacAddr &ruid,
    const std::array<beerocks::message::sWifiChannel, beerocks::message::SUPPORTED_CHANNELS_LENGTH>
        &supported_channels)
{
    std::vector<uint8_t> operating_classes;

    auto radio_basic_caps = cmdu_tx.addClass<wfa_map::tlvApRadioBasicCapabilities>();
    if (!radio_basic_caps) {
        LOG(ERROR) << "Error creating TLV_AP_RADIO_BASIC_CAPABILITIES";
        return false;
    }
    radio_basic_caps->radio_uid() = ruid;
    //TODO get maximum supported VAPs from AP Capabilities from BWL
    radio_basic_caps->maximum_number_of_bsss_supported() = 2;

    operating_classes =
        son::wireless_utils::get_supported_operating_classes(supported_channels.data());

    for (auto op_class : operating_classes) {
        auto operationClassesInfo = radio_basic_caps->create_operating_classes_info_list();
        if (!operationClassesInfo) {
            LOG(ERROR) << "Failed creating operating classes info list";
            return false;
        }

        operationClassesInfo->operating_class() = op_class;
        operationClassesInfo->maximum_transmit_power_dbm() =
            son::wireless_utils::get_operating_class_max_tx_power(supported_channels.data(),
                                                                  op_class);

        std::vector<uint8_t> non_oper_channels;
        non_oper_channels = son::wireless_utils::get_operating_class_non_oper_channels(
            supported_channels.data(), op_class);
        // Create list of statically non-oper channels
        operationClassesInfo->alloc_statically_non_operable_channels_list(non_oper_channels.size());
        uint8_t idx = 0;
        for (auto non_oper : non_oper_channels) {
            *operationClassesInfo->statically_non_operable_channels_list(idx) = non_oper;
            idx++;
        }

        if (!radio_basic_caps->add_operating_classes_info_list(operationClassesInfo)) {
            LOG(ERROR) << "add_operating_classes_info_list failed";
            return false;
        }
    }

    return true;
}
