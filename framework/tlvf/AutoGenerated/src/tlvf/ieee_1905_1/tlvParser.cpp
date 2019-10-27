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

void ParseTlv(ieee1905_1::CmduMessageRx cmdu_rx)
{
    switch(cmdu_rx.getNextTlvType())
    {
        case (0):{
        cmdu_rx.addClass<ieee1905_1::tlvEndOfMessage>();
        return;
        }
        case (1):{
        cmdu_rx.addClass<ieee1905_1::tlvAlMacAddressType>();
        return;
        }
        case (2):{
        cmdu_rx.addClass<ieee1905_1::tlvMacAddress>();
        return;
        }
        case (3):{
        cmdu_rx.addClass<ieee1905_1::tlvDeviceInformation>();
        return;
        }
        case (4):{
        cmdu_rx.addClass<ieee1905_1::tlvDeviceBridgingCapability>();
        return;
        }
        case (6):{
        cmdu_rx.addClass<ieee1905_1::tlvNon1905neighborDeviceList>();
        return;
        }
        case (7):{
        cmdu_rx.addClass<ieee1905_1::tlv1905NeighborDevice>();
        return;
        }
        case (8):{
        cmdu_rx.addClass<ieee1905_1::tlvLinkMetricQuery>();
        return;
        }
        case (9):{
        cmdu_rx.addClass<ieee1905_1::tlvTransmitterLinkMetric>();
        return;
        }
        case (10):{
        cmdu_rx.addClass<ieee1905_1::tlvReceiverLinkMetric>();
        return;
        }
        case (11):{
        cmdu_rx.addClass<ieee1905_1::tlvVendorSpecific>();
        return;
        }
        case (12):{
        cmdu_rx.addClass<ieee1905_1::tlvLinkMetricResultCode>();
        return;
        }
        case (13):{
        cmdu_rx.addClass<ieee1905_1::tlvSearchedRole>();
        return;
        }
        case (14):{
        cmdu_rx.addClass<ieee1905_1::tlvAutoconfigFreqBand>();
        return;
        }
        case (15):{
        cmdu_rx.addClass<ieee1905_1::tlvSupportedRole>();
        return;
        }
        case (16):{
        cmdu_rx.addClass<ieee1905_1::tlvSupportedFreqBand>();
        return;
        }
        case (18):{
        cmdu_rx.addClass<ieee1905_1::tlvPushButtonEventNotification>();
        return;
        }
        case (19):{
        cmdu_rx.addClass<ieee1905_1::tlvPushButtonJoinNotification>();
        return;
        }
        case (128):{
        cmdu_rx.addClass<wfa_map::tlvSupportedService>();
        return;
        }
        case (129):{
        cmdu_rx.addClass<wfa_map::tlvSearchedService>();
        return;
        }
        case (130):{
        cmdu_rx.addClass<wfa_map::tlvApRadioIdentifier>();
        return;
        }
        case (133):{
        cmdu_rx.addClass<wfa_map::tlvApRadioBasicCapabilities>();
        return;
        }
        case (139):{
        cmdu_rx.addClass<wfa_map::tlvChannelPreference>();
        return;
        }
        case (140):{
        cmdu_rx.addClass<wfa_map::tlvRadioOperationRestriction>();
        return;
        }
        case (141):{
        cmdu_rx.addClass<wfa_map::tlvTransmitPowerLimit>();
        return;
        }
        case (142):{
        cmdu_rx.addClass<wfa_map::tlvChannelSelectionResponse>();
        return;
        }
        case (143):{
        cmdu_rx.addClass<wfa_map::tlvOperatingChannelReport>();
        return;
        }
        case (146):{
        cmdu_rx.addClass<wfa_map::tlvClientAssociationEvent>();
        return;
        }
        case (147):{
        cmdu_rx.addClass<wfa_map::tlvApMetricQuery>();
        return;
        }
        case (155):{
        cmdu_rx.addClass<wfa_map::tlvSteeringRequest>();
        return;
        }
        case (156):{
        cmdu_rx.addClass<wfa_map::tlvSteeringBTMReport>();
        return;
        }
        case (157):{
        cmdu_rx.addClass<wfa_map::tlvClientAssociationControlRequest>();
        return;
        }
        case (160):{
        cmdu_rx.addClass<wfa_map::tlvHigherLayerData>();
        return;
        }
        case (161):{
        cmdu_rx.addClass<wfa_map::tlvApCapability>();
        return;
        }
    }
}
