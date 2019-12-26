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
    CmduMessageRx() = delete;
    CmduMessageRx(uint8_t *buff, size_t buff_len);
    ~CmduMessageRx();
    bool parse();
    CmduMessageRx &operator=(const CmduMessageRx &) = delete;

private:
    std::shared_ptr<CmduParser> parser;
};

}; // namespace ieee1905_1

#endif //_CmduMessageRX_H_
