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

CmduMessageRx::CmduMessageRx(uint8_t *buff, size_t buff_len) : CmduMessage(buff, buff_len)
{
    parsers_.emplace_back(std::make_shared<CmduTlvParser>(*this)); // default parser
}


CmduMessageRx::~CmduMessageRx()
{
}

bool CmduMessageRx::parse(bool swap_needed, bool parse_tlvs)
{
    reset();
    // m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, kCmduHeaderLength);
    m_cmdu_header = std::make_shared<cCmduHeader>(m_buff, kCmduHeaderLength, swap_needed, parse_tlvs);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        m_cmdu_header = nullptr;
        return false;
    }

    tlvs.set_swap_value(swap_needed);
    
    if (!parse_tlvs)
        return true;

    for (auto parser : parsers_) {
        if (parser->parse())
            return true;
    }

    
    return false;
}
