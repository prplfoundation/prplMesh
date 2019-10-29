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

#include <tlvf/ieee_1905_1/tlvParser.h>
#include <tlvf/ieee_1905_1/tlvWscM2.h>
#include <tlvf/ieee_1905_1/tlvWscM1.h>
#include <tlvf/WSC/eWscAttributes.h>

#pragma pack(push, 1)
struct Tlv {
    protected:
        uint8_t tlvType;
        uint16_t tlvLength;

    public:
        uint8_t type() { return tlvType; }

        Tlv *next() { return (Tlv *)((uint8_t *)this + size()); }

        uint8_t *data() { return (uint8_t *)this + sizeof(Tlv); }

        size_t size() { return sizeof(Tlv) + ntohs(tlvLength); }
    };
#pragma pack(pop)

// packed WSC Attribute struct to help in the parsing of the packet
#pragma pack(push, 1)
struct Wsc {
    protected:
        uint16_t wsc_type;
        uint16_t length;

    public:
        uint16_t type() { return ntohs(wsc_type); }

        Wsc *next() { return (Wsc *)((uint8_t *)this + size()); }

        uint8_t *data() { return (uint8_t *)this + sizeof(Wsc); }

        size_t size() { return sizeof(Wsc) + ntohs(length); }
    };
#pragma pack(pop)

std::shared_ptr<BaseClass> tlvParser::parseWsc(ieee1905_1::CmduMessageRx &cmdu_rx)
{
    Tlv *tlv = (Tlv *)cmdu_rx.getClassVector().back()->getBuffPtr();

    if (ieee1905_1::eTlvType(tlv->type()) != ieee1905_1::eTlvType::TLV_WSC)
        return nullptr;

    Wsc *wsc = (Wsc *)(tlv->data());
    uint16_t type = wsc->type();
    while (type != WSC::ATTR_MSG_TYPE) {
        wsc = wsc->next();
        type = wsc->type();
    }
    if (*wsc->data() == WSC::WSC_MSG_TYPE_M2)
        return cmdu_rx.addClass<ieee1905_1::tlvWscM2>();
    else
        return cmdu_rx.addClass<ieee1905_1::tlvWscM1>();

    return nullptr;
}

std::shared_ptr<BaseClass> tlvParser::parseTlv(ieee1905_1::CmduMessageRx &cmdu_rx)
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
        case (17): {
            return parseWsc(cmdu_rx);
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
    return nullptr; // unknown tlv
}
