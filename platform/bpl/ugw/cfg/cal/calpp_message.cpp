/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
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