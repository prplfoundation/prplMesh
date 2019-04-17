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

#include "calpp_param_list.h"

#include <algorithm>
#include <stdexcept>

#include <string.h>

namespace beerocks {
namespace bpl {

cal_param_list::iterator::iterator(ObjList* obj, ParamList* param)
    : m_it(new cal_param_list(obj, param)){};

cal_param_list::iterator::iterator(const cal_param_list& param_list)
    : m_it(new cal_param_list(param_list)){};

cal_param_list::iterator::iterator(const cal_param_list::iterator& src)
    : m_it(new cal_param_list(*(src.m_it.get()))){};

cal_param_list::iterator& cal_param_list::iterator::operator=(const cal_param_list::iterator& src)
{
    if (this != &src) {
        (*m_it) = (*src.m_it);
    }
    return *this;
};

cal_param_list::iterator::~iterator(){
    // Nothing to do. The only destruction required is handled by unique_ptr, but use
    // of unique_ptr breaks "destructor = default".
};

bool cal_param_list::iterator::operator==(const cal_param_list::iterator& rhs) const
{
    return (m_it->m_param == rhs.m_it->m_param);
}

bool cal_param_list::iterator::operator!=(const cal_param_list::iterator& rhs) const
{
    return (m_it->m_param != rhs.m_it->m_param);
}

cal_param_list::iterator& cal_param_list::iterator::operator++()
{
    if ((nullptr == m_it->m_param) || (nullptr == m_it->m_param->xPlist.next)) {
        throw std::runtime_error("ParamList->next should never null");
    }
    m_it->m_param = reinterpret_cast<ParamList*>(
        reinterpret_cast<char*>(m_it->m_param->xPlist.next) - offsetof(ParamList, xPlist));
    return *this;
}

cal_param_list::iterator cal_param_list::iterator::operator++(int)
{
    iterator retv(*this);
    ++(*this);
    return retv;
}

cal_param_list::iterator& cal_param_list::iterator::operator--()
{
    if ((nullptr == m_it->m_param) || (nullptr == m_it->m_param->xPlist.prev)) {
        throw std::runtime_error("ParamList->prev should never null");
    }
    m_it->m_param = reinterpret_cast<ParamList*>(
        reinterpret_cast<char*>(m_it->m_param->xPlist.prev) - offsetof(ParamList, xPlist));
    return *this;
}

cal_param_list::iterator cal_param_list::iterator::operator--(int)
{
    iterator retv(*this);
    --(*this);
    return retv;
}

cal_param_list::iterator::reference cal_param_list::iterator::operator*() const
{
    return (*m_it);
}

cal_param_list::iterator::pointer cal_param_list::iterator::operator->()
{
    return m_it.get();
};

cal_param_list::cal_param_list(ObjList* obj, ParamList* param) : m_obj(obj), m_param(param)
{
    if (nullptr == m_obj) {
        throw std::invalid_argument(
            "cal_param_list initialization requires a valid ObjList* to containing object");
    }
    if (nullptr == m_param) {
        cal_param_list::iterator first(m_obj, &(m_obj->xParamList));
        m_param = (++first)->m_param;
    }
}

cal_param_list::iterator cal_param_list::begin()
{
    cal_param_list::iterator first(m_obj, &(m_obj->xParamList));
    return (++first);
}

cal_param_list::iterator cal_param_list::end()
{
    return cal_param_list::iterator(m_obj, &(m_obj->xParamList));
}

cal_param_list::iterator cal_param_list::find_param(const std::string& name)
{
    return find_if(
        begin(), end(), [&name](const cal_param_list& param) { return param.is_name(name); });
}

std::string cal_param_list::operator[](const std::string& name)
{
    auto it = find_param(name);
    if (it != end()) {
        return it->get_value();
    }
    throw std::range_error("invalid parameter: " + name);
}

bool cal_param_list::is_name(const std::string& name) const
{
    // TODO: safe strcmp with buffer size restriction
    return (0 == strcmp(m_param->sParamName, name.c_str()));
}

std::string cal_param_list::get_name()
{
    return std::string(m_param->sParamName);
}

std::string cal_param_list::get_value()
{
    return std::string(m_param->sParamValue);
}

} // namespace bpl
} // namespace beerocks