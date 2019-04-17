/*
#############################################################################
# INTEL CONFIDENTIAL
# Copyright 2018 Intel Corporation All Rights Reserved.
#
# The source code contained or described herein and all documents related to
# the source code ("Material") are owned by Intel Corporation or its
# suppliers or licensors.  Title to the Material remains with Intel
# Corporation or its suppliers and licensors.  The Material contains trade
# secrets and proprietary and confidential information of Intel or its
# suppliers and licensors.  The Material is protected by worldwide copyright
# and trade secret laws and treaty provisions. No part of the Material may
# be used, copied, reproduced, modified, published, uploaded, posted,
# transmitted, distributed, or disclosed in any way without Intel's prior
# express written permission.
#
# No license under any patent, copyright, trade secret or other intellectual
# property right is granted to or conferred upon you by disclosure or
# delivery of the Materials,  either expressly, by implication, inducement,
# estoppel or otherwise.  Any license under such intellectual property
# rights must be express and approved by Intel in writing.
#############################################################################
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
    CmduMessageRx(CmduMessageRx& original);
    ~CmduMessageRx();

public:

    std::shared_ptr<cCmduHeader> parse(uint8_t* buff, size_t buff_len, bool swap_needed = true);
    CmduMessageRx& operator=(const CmduMessageRx&) = delete;
};

}; // close namespace: ieee1905_1

#endif //_CmduMessageRX_H_
