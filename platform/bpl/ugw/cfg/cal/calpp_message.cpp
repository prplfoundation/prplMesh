/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
 */

#ifndef LOGGING_ID
#define LOGGING_ID calpp_message
#endif

#ifndef PACKAGE_ID
#define PACKAGE_ID "0"
#endif


#include "calpp_message.h"

#include <algorithm>
#include <stdexcept>
#include <string.h>

extern "C" {
	uint16_t uncalpp_messageLogLevel = 7;
	uint16_t uncalpp_messageLogType = 1;
}

namespace beerocks {
namespace bpl {

cal_message::cal_message(uint32_t main_op, uint32_t sub_op, uint32_t owner)
{
    memset(&m_msg_header, 0, sizeof(m_msg_header));
    HELP_CREATE_MSG(&m_msg_header, main_op, sub_op, owner, 1);
}

cal_message::~cal_message()
{
    HELP_DELETE_MSG(&m_msg_header);
}

cal_message::operator MsgHeader*()
{
    return &m_msg_header;
}

uint32_t cal_message::get_main_op() const
{
    return m_msg_header.unMainOper;
}

uint32_t cal_message::get_sub_op() const
{
    return m_msg_header.unSubOper;
}

uint32_t cal_message::get_owner() const
{
    return m_msg_header.unOwner;
}

cal_object_list cal_message::add_get_object(const std::string& name, uint32_t sub_op)
{
    return cal_object_list(
        static_cast<ObjList*>(m_msg_header.pObjType), name, NO_ARG_VALUE, NO_ARG_VALUE, sub_op);
}

cal_object_list cal_message::add_set_object(
    const std::string& name, uint32_t set_op, uint16_t sid, uint32_t sub_op)
{
    return cal_object_list(static_cast<ObjList*>(m_msg_header.pObjType), name, set_op, sid, sub_op);
}

cal_object_list::iterator cal_message::begin()
{
    ObjList* head = static_cast<ObjList*>(m_msg_header.pObjType);
    cal_object_list::iterator first(head, head, m_msg_header.unSubOper);
    return (++first);
}

cal_object_list::iterator cal_message::end()
{
    ObjList* head = static_cast<ObjList*>(m_msg_header.pObjType);
    return cal_object_list::iterator(head, head, m_msg_header.unSubOper);
}

cal_object_list cal_message::get_object_list()
{
    return (*begin());
}

} // namespace bpl
} // namespace beerocks