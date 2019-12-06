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

#include <tlvf/TlvList.h>

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

    if (!m_swap)
        return;

    // call all tlv finalize functions
    for (auto const &c : m_class_vector) {
        c->class_swap();
    }

    m_swapped = !m_swapped;
}

void TlvList::reset()
{
    m_finalized = false;
    m_swapped   = false;
    for (auto &c : m_class_vector) {
        c.reset();
    }
    m_class_vector.clear();
}
