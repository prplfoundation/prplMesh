/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessageRx.h>
#include <tlvf/tlvfstructs.h>
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
#include <tlvf/ieee_1905_1/tlvUnknown.h>
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

#include <tlvf/WSC/eWscAttributes.h>
#include <tlvf/ieee_1905_1/tlvWscM2.h>
#include <tlvf/ieee_1905_1/tlvWscM1.h>



#include <tlvf/TlvMapParser.h>
#include <tlvf/BaseClass.h>
#include <tlvf/CmduMessageRx.h>

using namespace ieee1905_1;

TlvMapParser::TlvMapParser(){}

std::shared_ptr<BaseClass> TlvMapParser::Parse(CmduMessageRx &cmdu_rx)
{
    switch(cmdu_rx.getNextTlvType())
    {
        case (0):{
        return cmdu_rx.addClass<tlvEndOfMessage>();
        }
        case (1):{
        return cmdu_rx.addClass<tlvAlMacAddressType>();
        }
        case (2):{
        return cmdu_rx.addClass<tlvMacAddress>();
        }
        case (3):{
        return cmdu_rx.addClass<tlvDeviceInformation>();
        }
        case (4):{
        return cmdu_rx.addClass<tlvDeviceBridgingCapability>();
        }
        case (6):{
        return cmdu_rx.addClass<tlvNon1905neighborDeviceList>();
        }
        case (7):{
        return cmdu_rx.addClass<tlv1905NeighborDevice>();
        }
        case (8):{
        return cmdu_rx.addClass<tlvLinkMetricQuery>();
        }
        case (9):{
        return cmdu_rx.addClass<tlvTransmitterLinkMetric>();
        }
        case (10):{
        return cmdu_rx.addClass<tlvReceiverLinkMetric>();
        }
        case (11):{
        return cmdu_rx.addClass<tlvVendorSpecific>();
        }
        case (12):{
        return cmdu_rx.addClass<tlvLinkMetricResultCode>();
        }
        case (13):{
        return cmdu_rx.addClass<tlvSearchedRole>();
        }
        case (14):{
        return cmdu_rx.addClass<tlvAutoconfigFreqBand>();
        }
        case (15):{
        return cmdu_rx.addClass<tlvSupportedRole>();
        }
        case (16):{
        return cmdu_rx.addClass<tlvSupportedFreqBand>();
        }
        case (18):{
        return cmdu_rx.addClass<tlvPushButtonEventNotification>();
        }
        case (19):{
        return cmdu_rx.addClass<tlvPushButtonJoinNotification>();
        }
        case (128):{
        return cmdu_rx.addClass<wfa_map::tlvSupportedService>();
        }
        case (129):{
        return cmdu_rx.addClass<wfa_map::tlvSearchedService>();
        }
        case (130):{
        return cmdu_rx.addClass<wfa_map::tlvApRadioIdentifier>();
        }
        case (133):{
        return cmdu_rx.addClass<wfa_map::tlvApRadioBasicCapabilities>();
        }
        case (139):{
        return cmdu_rx.addClass<wfa_map::tlvChannelPreference>();
        }
        case (140):{
        return cmdu_rx.addClass<wfa_map::tlvRadioOperationRestriction>();
        }
        case (141):{
        return cmdu_rx.addClass<wfa_map::tlvTransmitPowerLimit>();
        }
        case (142):{
        return cmdu_rx.addClass<wfa_map::tlvChannelSelectionResponse>();
        }
        case (143):{
        return cmdu_rx.addClass<wfa_map::tlvOperatingChannelReport>();
        }
        case (146):{
        return cmdu_rx.addClass<wfa_map::tlvClientAssociationEvent>();
        }
        case (147):{
        return cmdu_rx.addClass<wfa_map::tlvApMetricQuery>();
        }
        case (155):{
        return cmdu_rx.addClass<wfa_map::tlvSteeringRequest>();
        }
        case (156):{
        return cmdu_rx.addClass<wfa_map::tlvSteeringBTMReport>();
        }
        case (157):{
        return cmdu_rx.addClass<wfa_map::tlvClientAssociationControlRequest>();
        }
        case (160):{
        return cmdu_rx.addClass<wfa_map::tlvHigherLayerData>();
        }
        case (161):{
        return cmdu_rx.addClass<wfa_map::tlvApCapability>();
        }
        default:{
        return cmdu_rx.addClass<tlvUnknown>();
        }
    }
}