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

#ifndef _CmduParser_H_
#define _CmduParser_H_

namespace ieee1905_1 {

class CmduMessageRx;

class CmduParser {

public:
    CmduParser() = delete;
    CmduParser(CmduMessageRx &cmdu) : cmdu_(cmdu) {}
    ~CmduParser() {};
    virtual bool parse() = 0;
    CmduParser &operator=(const CmduParser &) = delete;

protected:
    CmduMessageRx &cmdu_;
};

}; // close namespace: ieee1905_1

#endif //_CmduParser_H_
