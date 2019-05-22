/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "calpp_param_list.h"

#include <algorithm>
#include <stdexcept>

#include <string.h>

namespace beerocks {
namespace bpl {

cal_param_list::iterator::iterator(ObjList *obj, ParamList *param)
    : m_it(new cal_param_list(obj, param)){};

cal_param_list::iterator::iterator(const cal_param_list &param_list)
    : m_it(new cal_param_list(param_list)){};

cal_param_list::iterator::iterator(const cal_param_list::iterator &src)
    : m_it(new cal_param_list(*(src.m_it.get()))){};

cal_param_list::iterator &cal_param_list::iterator::operator=(const cal_param_list::iterator &src)
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

bool cal_param_list::iterator::operator==(const cal_param_list::iterator &rhs) const
{
    return (m_it->m_param == rhs.m_it->m_param);
}

bool cal_param_list::iterator::operator!=(const cal_param_list::iterator &rhs) const
{
    return (m_it->m_param != rhs.m_it->m_param);
}

cal_param_list::iterator &cal_param_list::iterator::operator++()
{
    if ((nullptr == m_it->m_param) || (nullptr == m_it->m_param->xPlist.next)) {
        throw std::runtime_error("ParamList->next should never null");
    }
    m_it->m_param = reinterpret_cast<ParamList *>(
        reinterpret_cast<char *>(m_it->m_param->xPlist.next) - offsetof(ParamList, xPlist));
    return *this;
}

cal_param_list::iterator cal_param_list::iterator::operator++(int)
{
    iterator retv(*this);
    ++(*this);
    return retv;
}

cal_param_list::iterator &cal_param_list::iterator::operator--()
{
    if ((nullptr == m_it->m_param) || (nullptr == m_it->m_param->xPlist.prev)) {
        throw std::runtime_error("ParamList->prev should never null");
    }
    m_it->m_param = reinterpret_cast<ParamList *>(
        reinterpret_cast<char *>(m_it->m_param->xPlist.prev) - offsetof(ParamList, xPlist));
    return *this;
}

cal_param_list::iterator cal_param_list::iterator::operator--(int)
{
    iterator retv(*this);
    --(*this);
    return retv;
}

cal_param_list::iterator::reference cal_param_list::iterator::operator*() const { return (*m_it); }

cal_param_list::iterator::pointer cal_param_list::iterator::operator->() { return m_it.get(); };

cal_param_list::cal_param_list(ObjList *obj, ParamList *param) : m_obj(obj), m_param(param)
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

cal_param_list::iterator cal_param_list::find_param(const std::string &name)
{
    return find_if(begin(), end(),
                   [&name](const cal_param_list &param) { return param.is_name(name); });
}

std::string cal_param_list::operator[](const std::string &name)
{
    auto it = find_param(name);
    if (it != end()) {
        return it->get_value();
    }
    throw std::range_error("invalid parameter: " + name);
}

bool cal_param_list::is_name(const std::string &name) const
{
    // TODO: safe strcmp with buffer size restriction
    return (0 == strcmp(m_param->sParamName, name.c_str()));
}

std::string cal_param_list::get_name() { return std::string(m_param->sParamName); }

std::string cal_param_list::get_value() { return std::string(m_param->sParamValue); }

} // namespace bpl
} // namespace beerocks