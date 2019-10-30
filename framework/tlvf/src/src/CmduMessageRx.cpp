/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/CmduMessageRx.h>
#include <tlvf/CmduParser.h>
#include <tlvf/CmduTlvParser.h>

using namespace ieee1905_1;

CmduMessageRx::CmduMessageRx() : CmduMessage()
{
    parsers_.emplace_back(std::make_shared<CmduTlvParser>(*this)); // default parser
}

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

    for (auto parser : parsers_) {
        if (parser->parse())
            return true;
    }

    
    return false;
}
