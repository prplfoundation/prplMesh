/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessageRX_H_
#define _CmduMessageRX_H_

#include <tlvf/CmduMessage.h>
#include <tlvf/ieee_1905_1/cCmduHeader.h>

#include <list>
#include <memory>

namespace ieee1905_1 {

class CmduParser;

class CmduMessageRx : public CmduMessage {

public:
    CmduMessageRx();
    CmduMessageRx(CmduMessageRx &original);
    ~CmduMessageRx();
    bool parse(uint8_t *buff, size_t buff_len, bool swap_needed = true, bool parse_tlvs = false);
    CmduMessageRx &operator=(const CmduMessageRx &) = delete;

private:
    std::list<std::shared_ptr<CmduParser>> parsers_;
};

}; // namespace ieee1905_1

#endif //_CmduMessageRX_H_
