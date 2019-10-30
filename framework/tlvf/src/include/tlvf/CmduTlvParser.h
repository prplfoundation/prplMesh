/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduTlvParser_H_
#define _CmduTlvParser_H_

#include <tlvf/CmduParser.h>
#include <tlvf/BaseClass.h>

#include <memory>

namespace ieee1905_1 {

class CmduTlvParser : public CmduParser {

public:
    CmduTlvParser() = delete;
    CmduTlvParser(CmduMessageRx &cmdu);
    ~CmduTlvParser();
    virtual bool parse();
    CmduTlvParser &operator=(const CmduTlvParser &) = delete;

private:
    std::shared_ptr<BaseClass> parseWscTlv();
    std::shared_ptr<BaseClass> parseNextTlv();
};

}; // close namespace: ieee1905_1

#endif //_CmduTlvParser_H_
