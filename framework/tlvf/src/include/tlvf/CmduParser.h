/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef _CmduParser_H_
#define _CmduParser_H_

#include <memory>
#include <BaseClass.h>

namespace ieee1905_1 {
    class CmduParser
    {
    private:
    public:
CmduParser();
    virtual std::shared_ptr<BaseClass> Parse(CmduMessageRx &cmdu_rx)=0;
        // CmduParser(/* args */);
        // ~CmduParser();
    };
    
    // CmduParser::CmduParser(/* args */)
    // {
    // }
    
    // CmduParser::~CmduParser()
    // {
    // }
    
}

#endif //_CmduParser_H_
