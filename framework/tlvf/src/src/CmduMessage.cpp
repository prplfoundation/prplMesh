/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessage.h>

using namespace ieee1905_1;

CmduMessage::CmduMessage() {}

CmduMessage::~CmduMessage() {}

std::shared_ptr<cCmduHeader> CmduMessage::getCmduHeader() const { return m_cmdu_header; }

std::shared_ptr<BaseClass> CmduMessage::getClass(size_t idx) const
{
    if (m_class_vector.size() > idx) {
        return std::shared_ptr<BaseClass>(m_class_vector.at(idx));
    } else {
        return nullptr;
    }
}

bool CmduMessage::hasTlv(uint8_t tlvType) const
{
    uint8_t type = 0;
    Tlv *tlvhdr;

    // for already parsed TLVs, check the class vector (no need for endianess conversion)
    for (size_t idx = 0; idx < m_class_vector.size(); idx++) {
        type = *m_class_vector.at(idx).get()->getBuffPtr();
        if (type == tlvType)
            return true;
    }
    // for TLVs not yet parsed, go through the rest till EOM or the type which is searched
    if (m_class_vector.size() == 0) {
        tlvhdr = reinterpret_cast<Tlv *>(m_cmdu_header->getBuffPtr());
    } else {
        tlvhdr = reinterpret_cast<Tlv *>(m_class_vector.back()->getBuffPtr());
    }
    
    do {
        if (tlvhdr->type() == tlvType)
            return true;
        tlvhdr = tlvhdr->next();
    } while (tlvhdr->type() != 0);

    return false;
}

int CmduMessage::getNextTlvType() const
{
    uint8_t tlvValue = 0;
    if (!m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() < kTlvHeaderLength)) {
        return -1;
    }

    if (m_class_vector.size() == 0) {
        tlvValue = *m_cmdu_header->getBuffPtr();
    } else {
        tlvValue = *m_class_vector.back()->getBuffPtr();
    }
    
    return int(tlvValue);
}

bool CmduMessage::getNextTlvType(eTlvType &tlvType) const
{
    uint16_t tlvValue = 0;
    if (!m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() < kTlvHeaderLength)) {
        return false;
    }

    if (m_class_vector.size() == 0) {
        tlvValue = *m_cmdu_header->getBuffPtr();
    } else {
        tlvValue = *m_class_vector.back()->getBuffPtr();
    }
    if (eTlvTypeValidate::check(tlvValue)) {
        tlvType = (eTlvType)tlvValue;
        return true;
    } else {
        return false;
    }
}

uint16_t CmduMessage::getNextTlvLength() const
{
    uint16_t tlvLength;
    if (m_cmdu_header && (m_cmdu_header->getBuffRemainingBytes() > kTlvHeaderLength)) {
        if (m_class_vector.size() == 0) {
            tlvLength = *((uint16_t *)(m_cmdu_header->getBuffPtr() + sizeof(uint8_t)));
        } else {
            tlvLength = *((uint16_t *)(m_class_vector.back()->getBuffPtr() + sizeof(uint8_t)));
        }

        if (m_swap) {
            swap_16((uint16_t &)tlvLength);
        }
    } else {
        tlvLength = 0;
    }
    return tlvLength;
}

size_t CmduMessage::getClassCount() const { return m_class_vector.size(); }

const std::vector<std::shared_ptr<BaseClass>> &CmduMessage::getClassVector() const
{
    return (const std::vector<std::shared_ptr<BaseClass>> &)m_class_vector;
}

size_t CmduMessage::getMessageLength() const
{
    if (m_cmdu_header == nullptr)
        return 0;
    size_t msg_len = m_cmdu_header->getLen();
    for (auto const &c : m_class_vector) {
        msg_len += c->getLen();
    }
    return msg_len;
}

size_t CmduMessage::getMessageBuffLength() const { return m_buff_len; }

uint8_t *CmduMessage::getMessageBuff() const
{
    return (m_cmdu_header ? m_cmdu_header->getStartBuffPtr() : nullptr);
}

void CmduMessage::swap()
{
    if (!m_cmdu_header)
        return;

    m_cmdu_header
        ->class_swap(); // the header isn't part of the m_class_vector, so we should swap it separately

    // call all tlv finalize functions
    for (auto const &c : m_class_vector) {
        c->class_swap();
    }

    m_swapped = !m_swapped;
}

void CmduMessage::reset()
{
    m_finalized = false;
    m_swapped   = false;
    m_swap      = false;
    if (m_cmdu_header)
        m_cmdu_header.reset();
    for (auto &c : m_class_vector) {
        c.reset();
    }
    m_class_vector.clear();
}

eMessageType CmduMessage::getMessageType()
{
    uint16_t msgValue = 0;

    msgValue = (uint16_t)getCmduHeader()->message_type();
    if (m_swap && !m_swapped) {
        swap_16((uint16_t &)msgValue);
    }

    return (eMessageType)msgValue;
}

uint16_t CmduMessage::getMessageId()
{
    uint16_t mid = getCmduHeader()->message_id();

    if (m_swap && !m_swapped) {
        swap_16((uint16_t &)mid);
    }

    return mid;
}
