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

class tlvParser {

    void ParseTlv(CmduMessageRx cmdu_rx)
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
            cmdu_rx.addClass<ieee1905_1::tlvNon1905NeighborDeviceList>();
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
            case (17):{
            cmdu_rx.addClass<ieee1905_1::tlvWsc>();
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
            case (131):{
            cmdu_rx.addClass<wfa_map::tlvApOperationalBss>();
            return;
            }
            case (132):{
            cmdu_rx.addClass<wfa_map::tlvAssociatedClients>();
            return;
            }
            case (133):{
            cmdu_rx.addClass<wfa_map::tlvApRadioBasicCapabilities>();
            return;
            }
            case (134):{
            cmdu_rx.addClass<wfa_map::tlvApHtCapabilities>();
            return;
            }
            case (135):{
            cmdu_rx.addClass<wfa_map::tlvApVhtCapabilities>();
            return;
            }
            case (136):{
            cmdu_rx.addClass<wfa_map::tlvApHeCapabilities>();
            return;
            }
            case (137):{
            cmdu_rx.addClass<wfa_map::tlvSteeringPolicy>();
            return;
            }
            case (138):{
            cmdu_rx.addClass<wfa_map::tlvMetricReportingPolicy>();
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
            case (144):{
            cmdu_rx.addClass<wfa_map::tlvClientInfo>();
            return;
            }
            case (145):{
            cmdu_rx.addClass<wfa_map::tlvClientCapabilityReport>();
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
            case (148):{
            cmdu_rx.addClass<wfa_map::tlvApMetric>();
            return;
            }
            case (149):{
            cmdu_rx.addClass<wfa_map::tlvStamacAddressType>();
            return;
            }
            case (150):{
            cmdu_rx.addClass<wfa_map::tlvAssociatedStaLinkMetrics>();
            return;
            }
            case (151):{
            cmdu_rx.addClass<wfa_map::tlvUnassociatedStaLinkMetricsQuery>();
            return;
            }
            case (152):{
            cmdu_rx.addClass<wfa_map::tlvUnassociatedStaLinkMetricsResponse>();
            return;
            }
            case (153):{
            cmdu_rx.addClass<wfa_map::tlvBeaconMetricsQuery>();
            return;
            }
            case (154):{
            cmdu_rx.addClass<wfa_map::tlvBeaconMetricsResponse>();
            return;
            }
            case (155):{
            cmdu_rx.addClass<wfa_map::tlvSteeringRequest>();
            return;
            }
            case (156):{
            cmdu_rx.addClass<wfa_map::tlvSteeringBtmReport>();
            return;
            }
            case (157):{
            cmdu_rx.addClass<wfa_map::tlvClientAssociationControlRequest>();
            return;
            }
            case (158):{
            cmdu_rx.addClass<wfa_map::tlvBackhaulSteeringRequest>();
            return;
            }
            case (159):{
            cmdu_rx.addClass<wfa_map::tlvBackhaulSteeringResponse>();
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
            case (162):{
            cmdu_rx.addClass<wfa_map::tlvAssociatedStaTrafficStats>();
            return;
            }
            case (163):{
            cmdu_rx.addClass<wfa_map::tlvErrorCode>();
            return;
            }
        }
    }
}
