/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Arnout Vandecappelle (Essensium/Mind)
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/TlvList.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <tlvf/tlvflogging.h>
#include <algorithm>

using namespace ieee1905_1;

TlvList::TlvList(uint8_t *buff, size_t buff_len, bool parse, bool swap)
    : m_buff(buff), m_buff_len(buff_len), m_parse(parse), m_swap(swap) {}

TlvList::~TlvList() {}

std::shared_ptr<BaseClass> TlvList::getClass(size_t idx) const
{
    if (m_class_vector.size() > idx) {
        return std::shared_ptr<BaseClass>(m_class_vector.at(idx));
    } else {
        return nullptr;
    }
}

size_t TlvList::getClassCount() const { return m_class_vector.size(); }

const std::vector<std::shared_ptr<BaseClass>> &TlvList::getClassVector() const
{
    return (const std::vector<std::shared_ptr<BaseClass>> &)m_class_vector;
}

size_t TlvList::getMessageLength() const
{
    if (m_buff == nullptr)
        return 0;
    size_t msg_len = 0;
    for (auto const &c : m_class_vector) {
        msg_len += c->getLen();
    }
    return msg_len;
}

size_t TlvList::getMessageBuffLength() const { return m_buff_len; }

uint8_t *TlvList::getMessageBuff() const
{
    return m_buff;
}

void TlvList::swap()
{
    if (!m_buff)
        return;

    // call all tlv finalize functions
    for (auto const &c : m_class_vector) {
        c->class_swap();
    }

    m_swapped = !m_swapped;
}

void TlvList::reset(bool parse, bool swap)
{
    if (!parse)
        memset(m_buff, 0, m_buff_len);
    m_parse = parse;
    m_swap = swap;
    m_finalized = false;
    m_swapped   = false;
    for (auto &c : m_class_vector) {
        c.reset();
    }
    m_class_vector.clear();
    m_inner_tlv_lists.clear();
}

bool TlvList::resizeTlvs(bool swap_needed)
{
    for (auto list : m_inner_tlv_lists) {
        size_t tailroom = list.second->getMessageBuffLength() - list.second->getMessageLength();
        if (!tailroom)
            continue;
        auto tlv = getTlv(list.first);
        if (!tlv)
            return false;
        TlvHeader *tlvhdr = reinterpret_cast<TlvHeader *>(tlv->getStartBuffPtr());
        if (swap_needed)
            tlvf_swap(16, reinterpret_cast<uint8_t*>(&tlvhdr->length));
        tlvhdr->length -= tailroom;
        // move the rest of the buffer
        uint8_t *dst = reinterpret_cast<uint8_t *>(tlvhdr) + tlvhdr->length + sizeof(*tlvhdr);
        uint8_t *src = dst + tailroom;
        uint8_t *end = m_class_vector.back()->getBuffPtr();
        size_t len = end - src;
        if (swap_needed)
            tlvf_swap(16, reinterpret_cast<uint8_t*>(&tlvhdr->length));
        std::copy_n(src, len, dst);
        // update parent TLV class size by trimming the end of the buffer (m_buff_ptr__)
        trim(*tlv, tailroom);
    }

    return true;
}

bool TlvList::finalize(bool swap_needed)
{
    if (m_finalized)
        return true;

    for (auto &it: m_class_vector){
        if(!(it->isPostInitSucceeded())){
            TLVF_LOG(ERROR) << "TLV post init failed";
            return false;
        }
    }

    for (auto list : m_inner_tlv_lists)
        list.second->finalize(swap_needed);

    if (!addClass<tlvEndOfMessage>())
        return false;

    if (swap_needed)
        swap();

    // shrink back inner tlv lists
    if (!resizeTlvs(swap_needed))
        return false;

    m_finalized = true;
    return true;
}
