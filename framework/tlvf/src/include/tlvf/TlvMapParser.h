/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */


#ifndef _TlvMapParser_H_
#define _TlvMapParser_H_

#include <tlvf/CmduParser.h>
#include <memory>

namespace ieee1905_1 {

class TlvMapParser : public CmduParser {
public:
    TlvMapParser();
    std::shared_ptr<BaseClass> Parse(CmduMessageRx &cmdu_rx);

};
};
#endif //_TlvMapParser_H_