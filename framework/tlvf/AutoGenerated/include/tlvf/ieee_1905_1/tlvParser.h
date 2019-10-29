///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_IEEE_1905_1_TLVPARSER_H_
#define _TLVF_IEEE_1905_1_TLVPARSER_H_

#include <cstddef>
#include <stdint.h>
#include <tlvf/swap.h>
#include <tlvf/CmduMessageRx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddressType.h>
#include <tlvf/ieee_1905_1/tlvMacAddress.h>
#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/ieee_1905_1/tlvDeviceBridgingCapability.h>
#include <tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h>
#include <tlvf/ieee_1905_1/tlv1905NeighborDevice.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/ieee_1905_1/tlvTransmitterLinkMetric.h>
#include <tlvf/ieee_1905_1/tlvReceiverLinkMetric.h>
#include <tlvf/ieee_1905_1/tlvVendorSpecific.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricResultCode.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvPushButtonEventNotification.h>
#include <tlvf/ieee_1905_1/tlvPushButtonJoinNotification.h>
#include <tlvf/wfa_map/tlvSupportedService.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvRadioOperationRestriction.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>
#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/wfa_map/tlvOperatingChannelReport.h>
#include <tlvf/wfa_map/tlvClientAssociationEvent.h>
#include <tlvf/wfa_map/tlvApMetricQuery.h>
#include <tlvf/wfa_map/tlvSteeringRequest.h>
#include <tlvf/wfa_map/tlvSteeringBTMReport.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>
#include <tlvf/wfa_map/tlvHigherLayerData.h>
#include <tlvf/wfa_map/tlvApCapability.h>

class tlvParser 
{
    public:
        static std::shared_ptr<BaseClass> parseTlv(ieee1905_1::CmduMessageRx &cmdu_rx);
};

#endif //_TLVF/IEEE_1905_1_TLVPARSER_H_
