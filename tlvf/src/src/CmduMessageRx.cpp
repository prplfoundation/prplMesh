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

#include <tlvf/CmduMessageRx.h>


using namespace ieee1905_1;

CmduMessageRx::CmduMessageRx() : CmduMessage()
{
}

CmduMessageRx::CmduMessageRx(CmduMessageRx& original) : CmduMessage()
{
    m_dynamically_allocated = true;
    size_t buff_len = original.getMessageBuffLength();
    m_buff = new uint8_t[buff_len];
    std::copy(original.getMessageBuff(), original.getMessageBuff()+buff_len, m_buff);
    parse(m_buff, buff_len, original.m_swap);
    if (m_swap) {
        m_cmdu_header->class_swap();
    }

}

CmduMessageRx::~CmduMessageRx()
{
    if (m_dynamically_allocated)
    {
        delete m_buff;
    }
}

/*
 * TODO
 * change all pointer arguments to const type where applicable
 */

std::shared_ptr<cCmduHeader> CmduMessageRx::parse(uint8_t* buff, size_t buff_len, bool swap_needed)
{
    reset();
    m_parse = true;
    m_swap = swap_needed;
    m_buff = buff;
    m_buff_len = buff_len;
    m_cmdu_header = std::make_shared<cCmduHeader>(buff, buff_len, true, false);
    if (!m_cmdu_header || m_cmdu_header->isInitialized() == false) {
        m_cmdu_header = nullptr;
    }

    return m_cmdu_header;
}

