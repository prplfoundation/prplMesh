/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessageRX_H_
#define _CmduMessageRX_H_

#include "ieee_1905_1/cCmduHeader.h"
#include "ieee_1905_1/tlvVendorSpecific.h"

#include "CmduMessage.h"

namespace ieee1905_1 {

class CmduMessageRx : public CmduMessage {

public:
    CmduMessageRx();
    CmduMessageRx(CmduMessageRx &original);
    ~CmduMessageRx();

public:
    std::shared_ptr<cCmduHeader> parse(uint8_t *buff, size_t buff_len, bool swap_needed = true);
    CmduMessageRx &operator=(const CmduMessageRx &) = delete;
};

}; // close namespace: ieee1905_1

#endif //_CmduMessageRX_H_
