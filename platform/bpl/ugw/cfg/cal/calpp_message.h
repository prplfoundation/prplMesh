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

#ifndef __COMMON_CALPP_MESSAGE_H__
#define __COMMON_CALPP_MESSAGE_H__

#include "calpp_object_list.h"

namespace beerocks {
namespace bpl {

class cal_message
{
  public:
    typedef cal_object_list::iterator iterator;

    cal_message(uint32_t main_op, uint32_t sub_op = SOPT_OBJVALUE, uint32_t owner = OWN_SERVD);
    ~cal_message();
    // cal_message(const cal_message& src);
    // cal_message& operator=(const cal_message& src);

    operator MsgHeader*();

    // Object List Construction
    cal_object_list add_get_object(const std::string& name, uint32_t sub_op = SOPT_OBJVALUE);
    cal_object_list add_set_object(const std::string& name, uint32_t set_op = OBJOPT_ADD,
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

#endif  // __COMMON_CALPP_MESSAGE_H__
