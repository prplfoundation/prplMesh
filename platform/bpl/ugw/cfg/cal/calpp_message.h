/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __COMMON_CALPP_MESSAGE_H__
#define __COMMON_CALPP_MESSAGE_H__

#include "calpp_object_list.h"

namespace beerocks {
namespace bpl {

class cal_message {
public:
    typedef cal_object_list::iterator iterator;

    cal_message(uint32_t main_op, uint32_t sub_op = SOPT_OBJVALUE, uint32_t owner = OWN_SERVD);
    ~cal_message();
    // cal_message(const cal_message& src);
    // cal_message& operator=(const cal_message& src);

    operator MsgHeader *();

    // Object List Construction
    cal_object_list add_get_object(const std::string &name, uint32_t sub_op = SOPT_OBJVALUE);
    cal_object_list add_set_object(const std::string &name, uint32_t set_op = OBJOPT_ADD,
                                   uint16_t sid = NO_ARG_VALUE, uint32_t sub_op = SOPT_OBJVALUE);

    // List searching / lookup
    // cal_object_list& operator [](const std::string& name);
    iterator begin();
    iterator end();

    cal_object_list get_object_list();
    // cal_object_list find_object(const std::string& name);
    // cal_object_list& operator [](const std::string& name);

    // Accessors
    uint32_t get_main_op() const;
    uint32_t get_sub_op() const;
    uint32_t get_owner() const;

protected:
    MsgHeader m_msg_header;
};

} // namespace bpl
} // namespace beerocks

#endif // __COMMON_CALPP_MESSAGE_H__
