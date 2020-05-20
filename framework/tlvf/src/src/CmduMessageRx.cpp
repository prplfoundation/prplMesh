/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessageRx.h>
#include <tlvf/WSC/AttrList.h>
#include <tlvf/ieee_1905_1/tlv1905NeighborDevice.h>
#include <tlvf/ieee_1905_1/tlvAlMacAddressType.h>
#include <tlvf/ieee_1905_1/tlvAutoconfigFreqBand.h>
#include <tlvf/ieee_1905_1/tlvDeviceBridgingCapability.h>
#include <tlvf/ieee_1905_1/tlvDeviceInformation.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricQuery.h>
#include <tlvf/ieee_1905_1/tlvLinkMetricResultCode.h>
#include <tlvf/ieee_1905_1/tlvMacAddress.h>
#include <tlvf/ieee_1905_1/tlvNon1905neighborDeviceList.h>
#include <tlvf/ieee_1905_1/tlvPushButtonEventNotification.h>
#include <tlvf/ieee_1905_1/tlvPushButtonJoinNotification.h>
#include <tlvf/ieee_1905_1/tlvReceiverLinkMetric.h>
#include <tlvf/ieee_1905_1/tlvSearchedRole.h>
#include <tlvf/ieee_1905_1/tlvSupportedFreqBand.h>
#include <tlvf/ieee_1905_1/tlvSupportedRole.h>
#include <tlvf/ieee_1905_1/tlvTransmitterLinkMetric.h>
#include <tlvf/ieee_1905_1/tlvUnknown.h>
#include <tlvf/ieee_1905_1/tlvVendorSpecific.h>
#include <tlvf/ieee_1905_1/tlvWsc.h>
#include <tlvf/wfa_map/tlvApCapability.h>
#include <tlvf/wfa_map/tlvApMetricQuery.h>
#include <tlvf/wfa_map/tlvApMetrics.h>
#include <tlvf/wfa_map/tlvApRadioBasicCapabilities.h>
#include <tlvf/wfa_map/tlvApRadioIdentifier.h>
#include <tlvf/wfa_map/tlvBeaconMetricsQuery.h>
#include <tlvf/wfa_map/tlvChannelPreference.h>
#include <tlvf/wfa_map/tlvChannelSelectionResponse.h>
#include <tlvf/wfa_map/tlvClientAssociationControlRequest.h>
#include <tlvf/wfa_map/tlvClientAssociationEvent.h>
#include <tlvf/wfa_map/tlvClientCapabilityReport.h>
#include <tlvf/wfa_map/tlvClientInfo.h>
#include <tlvf/wfa_map/tlvHigherLayerData.h>
#include <tlvf/wfa_map/tlvMetricReportingPolicy.h>
#include <tlvf/wfa_map/tlvOperatingChannelReport.h>
#include <tlvf/wfa_map/tlvRadioOperationRestriction.h>
#include <tlvf/wfa_map/tlvSearchedService.h>
#include <tlvf/wfa_map/tlvStaMacAddressType.h>
#include <tlvf/wfa_map/tlvSteeringBTMReport.h>
#include <tlvf/wfa_map/tlvSteeringPolicy.h>
#include <tlvf/wfa_map/tlvSteeringRequest.h>
#include <tlvf/wfa_map/tlvSupportedService.h>
#include <tlvf/wfa_map/tlvTransmitPowerLimit.h>

#include <iostream>

using namespace ieee1905_1;

int CmduMessageRx::getNextTlvType() const
{
    if (!getCmduHeader())
        return -1;
    sTlvHeader *tlv = reinterpret_cast<sTlvHeader *>(msg.prevClass()->getBuffPtr());
    return tlv->type;
}

uint16_t CmduMessageRx::getNextTlvLength() const
{
    if (!getCmduHeader()) {
        return UINT16_MAX;
    }
    sTlvHeader *tlv = reinterpret_cast<sTlvHeader *>(msg.prevClass()->getBuffPtr());

    uint16_t tlv_length = tlv->length;
    swap_16(tlv_length);

    return tlv_length;
}

std::shared_ptr<BaseClass> CmduMessageRx::parseNextTlv()
{
    auto tlv_type = getNextTlvType();
    switch (tlv_type) {
    case (0): {
        return msg.addClass<tlvEndOfMessage>();
    }
    case (1): {
        return msg.addClass<tlvAlMacAddressType>();
    }
    case (2): {
        return msg.addClass<tlvMacAddress>();
    }
    case (3): {
        return msg.addClass<tlvDeviceInformation>();
    }
    case (4): {
        return msg.addClass<tlvDeviceBridgingCapability>();
    }
    case (6): {
        return msg.addClass<tlvNon1905neighborDeviceList>();
    }
    case (7): {
        return msg.addClass<tlv1905NeighborDevice>();
    }
    case (8): {
        /**
         * The IEEE 1905.1 standard says about the Link Metric Query TLV and the neighbor type
         * octet that "If the value is 0, then the EUI48 field is not present; if the value is 1,
         * then the EUI-48 field shall be present."
         *
         * However, optional fields are not currently supported by TLVF.
         *
         * As a workaround, instead of defining a tlvLinkMetricQuery TLV with an optional field,
         * we have defined two different TLVs, one with the optional field and the other one
         * without it. Application must then check the length of received TLV to know if optional
         * field (MAC address of neighbor device) is present or not and then create an instance of
         * either tlvLinkMetricQuery or tlvLinkMetricQueryAllNeighbors respectively.
         */
        const uint16_t all_neighbors_tlv_length = 2;
        uint16_t tlv_length                     = getNextTlvLength();

        if (all_neighbors_tlv_length == tlv_length) {
            return msg.addClass<tlvLinkMetricQueryAllNeighbors>();
        } else {
            return msg.addClass<tlvLinkMetricQuery>();
        }
    }
    case (9): {
        return msg.addClass<tlvTransmitterLinkMetric>();
    }
    case (10): {
        return msg.addClass<tlvReceiverLinkMetric>();
    }
    case (11): {
        return msg.addClass<tlvVendorSpecific>();
    }
    case (12): {
        return msg.addClass<tlvLinkMetricResultCode>();
    }
    case (13): {
        return msg.addClass<tlvSearchedRole>();
    }
    case (14): {
        return msg.addClass<tlvAutoconfigFreqBand>();
    }
    case (15): {
        return msg.addClass<tlvSupportedRole>();
    }
    case (16): {
        return msg.addClass<tlvSupportedFreqBand>();
    }
    case (17): {
        return msg.addClass<ieee1905_1::tlvWsc>();
    }
    case (18): {
        return msg.addClass<tlvPushButtonEventNotification>();
    }
    case (19): {
        return msg.addClass<tlvPushButtonJoinNotification>();
    }
    case (128): {
        return msg.addClass<wfa_map::tlvSupportedService>();
    }
    case (129): {
        return msg.addClass<wfa_map::tlvSearchedService>();
    }
    case (130): {
        return msg.addClass<wfa_map::tlvApRadioIdentifier>();
    }
    case (133): {
        return msg.addClass<wfa_map::tlvApRadioBasicCapabilities>();
    }
    case (137): {
        return msg.addClass<wfa_map::tlvSteeringPolicy>();
    }
    case (138): {
        return msg.addClass<wfa_map::tlvMetricReportingPolicy>();
    }
    case (139): {
        return msg.addClass<wfa_map::tlvChannelPreference>();
    }
    case (140): {
        return msg.addClass<wfa_map::tlvRadioOperationRestriction>();
    }
    case (141): {
        return msg.addClass<wfa_map::tlvTransmitPowerLimit>();
    }
    case (142): {
        return msg.addClass<wfa_map::tlvChannelSelectionResponse>();
    }
    case (143): {
        return msg.addClass<wfa_map::tlvOperatingChannelReport>();
    }
    case (144): {
        return msg.addClass<wfa_map::tlvClientInfo>();
    }
    case (145): {
        return msg.addClass<wfa_map::tlvClientCapabilityReport>();
    }
    case (146): {
        return msg.addClass<wfa_map::tlvClientAssociationEvent>();
    }
    case (147): {
        return msg.addClass<wfa_map::tlvApMetricQuery>();
    }
    case (148): {
        return msg.addClass<wfa_map::tlvApMetrics>();
    }
    case (149): {
        return msg.addClass<wfa_map::tlvStaMacAddressType>();
    }
    case (153): {
        return msg.addClass<wfa_map::tlvBeaconMetricsQuery>();
    }
    case (155): {
        return msg.addClass<wfa_map::tlvSteeringRequest>();
    }
    case (156): {
        return msg.addClass<wfa_map::tlvSteeringBTMReport>();
    }
    case (157): {
        return msg.addClass<wfa_map::tlvClientAssociationControlRequest>();
    }
    case (160): {
        return msg.addClass<wfa_map::tlvHigherLayerData>();
    }
    case (161): {
        return msg.addClass<wfa_map::tlvApCapability>();
    }
    default: {
        LOG(DEBUG) << "Unknown TLV type: " << tlv_type;
        return msg.addClass<tlvUnknown>();
    }
    }
}

bool CmduMessageRx::parse()
{
    msg.reset(true);
    auto cmduhdr = msg.addClass<cCmduHeader>();
    if (!cmduhdr)
        return false;

    while (auto tlv = parseNextTlv()) {
        if (std::dynamic_pointer_cast<tlvEndOfMessage>(tlv)) {
            return true;
        }
    }

    return false;
}
