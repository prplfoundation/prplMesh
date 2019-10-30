/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

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

using namespace ieee1905_1;

CmduMessageRx::CmduMessageRx() : CmduMessage() {}

CmduMessageRx::CmduMessageRx(CmduMessageRx &original) : CmduMessage()
{
    m_dynamically_allocated = true;
    size_t buff_len         = original.getMessageBuffLength();
    m_buff                  = new uint8_t[buff_len];
    std::copy(original.getMessageBuff(), original.getMessageBuff() + buff_len, m_buff);
    parse(m_buff, buff_len, original.m_swap);
    if (m_swap) {
        m_cmdu_header->class_swap();
    }
}

CmduMessageRx::~CmduMessageRx()
{
    if (m_dynamically_allocated) {
        delete m_buff;
    }
}

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

std::shared_ptr<BaseClass> CmduMessageRx::parseWscTlv()
{
    Tlv *tlv = (Tlv *)getClassVector().back()->getBuffPtr();

    if (eTlvType(tlv->type()) != eTlvType::TLV_WSC)
        return nullptr;

    Wsc *wsc = (Wsc *)(tlv->data());
    uint16_t type = wsc->type();
    while (type != WSC::ATTR_MSG_TYPE) {
        wsc = wsc->next();
        type = wsc->type();
    }
    if (*wsc->data() == WSC::WSC_MSG_TYPE_M2)
        return addClass<tlvWscM2>();
    else
        return addClass<tlvWscM1>();

    return nullptr;
}

std::shared_ptr<BaseClass> CmduMessageRx::parseNextTlv()
{
    switch(getNextTlvType())
    {
        case (0):{
        return addClass<tlvEndOfMessage>();
        }
        case (1):{
        return addClass<tlvAlMacAddressType>();
        }
        case (2):{
        return addClass<tlvMacAddress>();
        }
        case (3):{
        return addClass<tlvDeviceInformation>();
        }
        case (4):{
        return addClass<tlvDeviceBridgingCapability>();
        }
        case (6):{
        return addClass<tlvNon1905neighborDeviceList>();
        }
        case (7):{
        return addClass<tlv1905NeighborDevice>();
        }
        case (8):{
        return addClass<tlvLinkMetricQuery>();
        }
        case (9):{
        return addClass<tlvTransmitterLinkMetric>();
        }
        case (10):{
        return addClass<tlvReceiverLinkMetric>();
        }
        case (11):{
        return addClass<tlvVendorSpecific>();
        }
        case (12):{
        return addClass<tlvLinkMetricResultCode>();
        }
        case (13):{
        return addClass<tlvSearchedRole>();
        }
        case (14):{
        return addClass<tlvAutoconfigFreqBand>();
        }
        case (15):{
        return addClass<tlvSupportedRole>();
        }
        case (16):{
        return addClass<tlvSupportedFreqBand>();
        }
        #if 1
        case (17): {
            return parseWscTlv();
        }
        #endif
        case (18):{
        return addClass<tlvPushButtonEventNotification>();
        }
        case (19):{
        return addClass<tlvPushButtonJoinNotification>();
        }
        case (128):{
        return addClass<wfa_map::tlvSupportedService>();
        }
        case (129):{
        return addClass<wfa_map::tlvSearchedService>();
        }
        case (130):{
        return addClass<wfa_map::tlvApRadioIdentifier>();
        }
        case (133):{
        return addClass<wfa_map::tlvApRadioBasicCapabilities>();
        }
        case (139):{
        return addClass<wfa_map::tlvChannelPreference>();
        }
        case (140):{
        return addClass<wfa_map::tlvRadioOperationRestriction>();
        }
        case (141):{
        return addClass<wfa_map::tlvTransmitPowerLimit>();
        }
        case (142):{
        return addClass<wfa_map::tlvChannelSelectionResponse>();
        }
        case (143):{
        return addClass<wfa_map::tlvOperatingChannelReport>();
        }
        case (146):{
        return addClass<wfa_map::tlvClientAssociationEvent>();
        }
        case (147):{
        return addClass<wfa_map::tlvApMetricQuery>();
        }
        case (155):{
        return addClass<wfa_map::tlvSteeringRequest>();
        }
        case (156):{
        return addClass<wfa_map::tlvSteeringBTMReport>();
        }
        case (157):{
        return addClass<wfa_map::tlvClientAssociationControlRequest>();
        }
        case (160):{
        return addClass<wfa_map::tlvHigherLayerData>();
        }
        case (161):{
        return addClass<wfa_map::tlvApCapability>();
        }
        default:{
        return addClass<tlvUnknown>();
        }
    }
}

bool CmduMessageRx::parse(uint8_t *buff, size_t buff_len, bool swap_needed, bool parse_tlvs)
{
    reset();
    m_parse       = true;
    m_swap        = swap_needed;
    m_buff        = buff;
    m_buff_len    = buff_len;
    m_cmdu_header = std::make_shared<cCmduHeader>(buff, buff_len, true, false);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        m_cmdu_header = nullptr;
        return false;
    }
    
    if (!parse_tlvs)
        return true;

    while (auto tlv = parseNextTlv()) {
        if (std::dynamic_pointer_cast<tlvEndOfMessage>(tlv)) {
            return true;
        }
    }

    return false;
}
