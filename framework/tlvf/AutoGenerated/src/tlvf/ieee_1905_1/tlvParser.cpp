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

#include "tlvParser.h"

std::shared_ptr<BaseClass> ParseTlv(ieee1905_1::CmduMessageRx cmdu_rx)
{
    switch(cmdu_rx.getNextTlvType())
    {
        case (0):{
        return cmdu_rx.addClass<ieee1905_1::tlvEndOfMessage>();
        }
        case (1):{
        return cmdu_rx.addClass<ieee1905_1::tlvAlMacAddressType>();
        }
        case (2):{
        return cmdu_rx.addClass<ieee1905_1::tlvMacAddress>();
        }
        case (3):{
        return cmdu_rx.addClass<ieee1905_1::tlvDeviceInformation>();
        }
        case (4):{
        return cmdu_rx.addClass<ieee1905_1::tlvDeviceBridgingCapability>();
        }
        case (6):{
        return cmdu_rx.addClass<ieee1905_1::tlvNon1905neighborDeviceList>();
        }
        case (7):{
        return cmdu_rx.addClass<ieee1905_1::tlv1905NeighborDevice>();
        }
        case (8):{
        return cmdu_rx.addClass<ieee1905_1::tlvLinkMetricQuery>();
        }
        case (9):{
        return cmdu_rx.addClass<ieee1905_1::tlvTransmitterLinkMetric>();
        }
        case (10):{
        return cmdu_rx.addClass<ieee1905_1::tlvReceiverLinkMetric>();
        }
        case (11):{
        return cmdu_rx.addClass<ieee1905_1::tlvVendorSpecific>();
        }
        case (12):{
        return cmdu_rx.addClass<ieee1905_1::tlvLinkMetricResultCode>();
        }
        case (13):{
        return cmdu_rx.addClass<ieee1905_1::tlvSearchedRole>();
        }
        case (14):{
        return cmdu_rx.addClass<ieee1905_1::tlvAutoconfigFreqBand>();
        }
        case (15):{
        return cmdu_rx.addClass<ieee1905_1::tlvSupportedRole>();
        }
        case (16):{
        return cmdu_rx.addClass<ieee1905_1::tlvSupportedFreqBand>();
        }
        case (18):{
        return cmdu_rx.addClass<ieee1905_1::tlvPushButtonEventNotification>();
        }
        case (19):{
        return cmdu_rx.addClass<ieee1905_1::tlvPushButtonJoinNotification>();
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
    }
}
