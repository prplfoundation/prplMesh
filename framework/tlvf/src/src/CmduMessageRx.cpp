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

CmduMessageRx::CmduMessageRx(uint8_t *buff, size_t buff_len)
    : CmduMessage(buff, buff_len), parser(std::make_shared<CmduTlvParser>(*this))
{
}

CmduMessageRx::~CmduMessageRx() {}

bool CmduMessageRx::parse(bool parse_tlvs)
{
    tlvs.reset(true);
    auto cmduhdr = tlvs.addClass<cCmduHeader>();
    if (!cmduhdr)
        return false;
    cmduhdr->class_swap(); //swap back cmduheader

    if (!parse_tlvs)
        return true;

    if (parser->parse())
        return true;

    return false;
}
