/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessageTx.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>

#include <tlvf/tlvflogging.h>

using namespace ieee1905_1;

CmduMessageTx::CmduMessageTx(uint8_t *buff, size_t buff_len) : CmduMessage()
{
    m_buff     = buff;
    m_buff_len = buff_len;
}

CmduMessageTx::~CmduMessageTx() {}

std::shared_ptr<cCmduHeader> CmduMessageTx::create(uint16_t id, eMessageType message_type)
{
    if (!m_buff || !m_buff_len)
        return nullptr;

    m_parse = false;
    memset(m_buff, 0, m_buff_len);
    reset();
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, m_buff_len);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    m_cmdu_header->message_type() = message_type;
    m_cmdu_header->message_id()   = id;
    return m_cmdu_header;
}

std::shared_ptr<cCmduHeader> CmduMessageTx::load()
{
    if (!m_buff || !m_buff_len)
        return nullptr;

    m_parse = true;
    reset();
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, m_buff_len, m_parse);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    return m_cmdu_header;
}

std::shared_ptr<tlvVendorSpecific> CmduMessageTx::add_vs_tlv(tlvVendorSpecific::eVendorOUI voui)
{
    if (!m_buff || !m_buff_len)
        return nullptr;

    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        return nullptr;
    }

    auto tlv = addClass<tlvVendorSpecific>();
    if (!tlv) {
        m_cmdu_header.reset();
        m_cmdu_header = nullptr;
        return nullptr;
    }
    //set vendor specific oui
    tlv->vendor_oui() = (uint32_t)voui;

    return tlv;
}

bool CmduMessageTx::finalize(bool swap_needed)
{
    if (m_finalized)
        return true;

    if (!m_cmdu_header)
        return false; 

    // Update vendor specific length which is seperated to 3 classes
    for (auto class_it = m_class_vector.begin(); class_it != m_class_vector.end(); class_it++) {
        if(!(*class_it)->isPostInitSucceeded()){
            TLVF_LOG(ERROR) << "TLV post init failed";
            return false;
        }
        // Type is the first byte in the tlv buffer
        uint8_t tlv_type = *(*class_it)->getStartBuffPtr();
        if (tlv_type == uint8_t(eTlvType::TLV_VENDOR_SPECIFIC)) {
            uint16_t* tlv_length = reinterpret_cast<uint16_t *>
                ((class_it->get())->getStartBuffPtr() + sizeof(uint8_t));
            // tlv vendor specific length already include OUI and beerocks header length
            // need to add only beerocks message length.
            // Classes order: +0 = tlvVendorSpecific, +1 = beerocks_header, +2 = beerocks_message
            if (std::next(class_it, 1) == m_class_vector.end() ||
                std::next(class_it, 2)  == m_class_vector.end()) {
                TLVF_LOG(ERROR) << "vendor specific TLV has not been added correctly"; 
                return false;
            }
            *tlv_length += (std::next(class_it, 2)->get())->getLen();
            class_it += 2;
        }
    }

    if (!addClass<tlvEndOfMessage>())
        return false;

    if (swap_needed)
        swap();

    m_finalized = true;
    return true;
}
