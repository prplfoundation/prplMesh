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
#define LOGGING_ID calpp_object_list
#endif

#ifndef PACKAGE_ID
#define PACKAGE_ID "0"
#endif

#pragma GCC diagnostic ignored "-Wattributes"
#include "calpp_object_list.h"

#include <algorithm>
#include <stdexcept>

#include <string.h>

#include <mapf/common/logger.h>

extern "C" {
	uint16_t uncalpp_object_listLogLevel = 7;
	uint16_t uncalpp_object_listLogType = 1;
}

namespace beerocks {
namespace bpl {

cal_object_list::iterator::iterator(ObjList* head, ObjList* obj, uint32_t sub_op)
    : m_it(new cal_object_list(head, obj, sub_op)){};

cal_object_list::iterator::iterator(const cal_object_list& obj)
    : m_it(new cal_object_list(obj.m_head, obj.m_obj, obj.m_sub_op)){};

cal_object_list::iterator::iterator(const cal_object_list::iterator& src)
    : m_it(new cal_object_list(*(src.m_it.get())))
{
}

cal_object_list::iterator& cal_object_list::iterator::operator=(
    const cal_object_list::iterator& src)
{
    if (this != &src) {
        (*m_it) = (*src.m_it);
    }
    return *this;
}

cal_object_list::iterator::~iterator()
{
    // Nothing to do. The only destruction required is handled by unique_ptr, but use
    // of unique_ptr breaks "= default".
}

bool cal_object_list::iterator::operator==(const cal_object_list::iterator& rhs) const
{
    return (m_it->m_obj == rhs.m_it->m_obj);
}

bool cal_object_list::iterator::operator!=(const cal_object_list::iterator& rhs) const
{
    return (m_it->m_obj != rhs.m_it->m_obj);
}

cal_object_list::iterator& cal_object_list::iterator::operator++()
{
    if ((nullptr == m_it->m_obj) || (nullptr == m_it->m_obj->xOlist.next)) {
        throw std::runtime_error("ObjList->next should never null");
    }
    m_it->m_obj = reinterpret_cast<ObjList*>(
        reinterpret_cast<char*>(m_it->m_obj->xOlist.next) - offsetof(ObjList, xOlist));
    return *this;
}

cal_object_list::iterator cal_object_list::iterator::operator++(int)
{
    iterator retv(*this);
    ++(*this);
    return retv;
}

cal_object_list::iterator& cal_object_list::iterator::operator--()
{
    if ((nullptr == m_it->m_obj) || (nullptr == m_it->m_obj->xOlist.prev)) {
        throw std::runtime_error("ObjList->next should never null");
    }
    m_it->m_obj = reinterpret_cast<ObjList*>(
        reinterpret_cast<char*>(m_it->m_obj->xOlist.prev) - offsetof(ObjList, xOlist));
    return *this;
}

cal_object_list::iterator cal_object_list::iterator::operator--(int)
{
    iterator retv(*this);
    --(*this);
    return retv;
}

cal_object_list::iterator::reference cal_object_list::iterator::operator*() const
{
    return (*m_it);
}

cal_object_list::iterator::pointer cal_object_list::iterator::operator->()
{
    return m_it.get();
}

cal_object_list::cal_object_list(
    ObjList* head, const std::string& name, uint32_t obj_op, uint16_t sid, uint32_t sub_op)
    : m_head(head), m_own_head(nullptr == head), m_obj(nullptr), m_sub_op(sub_op)
{
    if (m_own_head) {
        m_head = static_cast<ObjList*>(HELP_CREATE_OBJ(m_sub_op));
        if (nullptr == m_head) {
            throw std::runtime_error("unable to allocate cal_object_list head");
        }
    }
    m_obj = help_addObjList(m_head, name.c_str(), sid, NO_ARG_VALUE, obj_op, NO_ARG_VALUE);
}

cal_object_list::cal_object_list(
    cal_object_list& list_obj, const std::string& name, uint32_t obj_op, uint16_t sid)
    : m_head(list_obj.m_head), m_own_head(false), m_obj(nullptr), m_sub_op(list_obj.m_sub_op)
{
    m_obj = help_addObjList(m_head, name.c_str(), sid, NO_ARG_VALUE, obj_op, NO_ARG_VALUE);
}

cal_object_list::cal_object_list(ObjList* head, ObjList* item, uint32_t sub_op)
    : m_head(head), m_own_head(false), m_obj(item), m_sub_op(sub_op)
{
    if (nullptr == head) {
        throw std::invalid_argument(
            "cal_object_list in place initialization requires a valid head");
    }
    if (nullptr == m_obj) {
        cal_object_list::iterator first(m_head, m_head, m_sub_op);
        m_obj = (++first)->m_obj;
    }
}

cal_object_list::~cal_object_list()
{
    if (m_own_head) {
        HELP_DELETE_OBJ(m_head, m_sub_op, 1);
    }
}

cal_object_list::cal_object_list(const cal_object_list& src)
    : m_head(src.m_head), m_own_head(false), m_obj(src.m_obj), m_sub_op(src.m_sub_op)
{
}

/*
Deep copy of a single object. This should be supported in a clone() or dup() call
{
    m_head = static_cast<ObjList*>(HELP_CREATE_OBJ(m_sub_op));
    if (nullptr == m_head) {
        throw std::runtime_error("unable to allocate cal_object_list head");
    }
    help_copyObjList(m_head, m_sub_op, src.m_obj);

    // retrieve m_obj pointer for the copied object since the copy call doesn't return in.
    m_obj = cal_obj_list_next_entry(m_head);
}
*/

cal_object_list& cal_object_list::operator=(const cal_object_list& src)
{
    if (this != &src) {
        // ensure we don't leak if we are assigning into an owning list element.
        if (m_own_head) {
            HELP_DELETE_OBJ(m_head, m_sub_op, 1);
        }

        m_head = src.m_head;
        m_own_head = false;
        m_obj = src.m_obj;
        m_sub_op = src.m_sub_op;
    }

    return *this;
}

/*
Deep copy of a single object. This should be supported in a clone() or dup() call
{
    if (this != &src) {
        // move the contents of the current instance to a local temp until after the assignment
        cal_object_list temp(std::move(*this));

        m_own_head = true;
        m_sub_op = src.m_sub_op;
        m_head = static_cast<ObjList*>(HELP_CREATE_OBJ(m_sub_op));
        if (nullptr == m_head) {
            throw std::runtime_error("unable to allocate cal_object_list head");
        }

        help_copyObjList(m_head, m_sub_op, src.m_obj);

        // retrieve m_obj pointer for the copied object since the copy call doesn't return in.
        m_obj = cal_obj_list_next_entry(m_head);

        // temp destructor will be called going out of scope which will delete owned objects
    }
    return *this;
}
*/

cal_object_list::cal_object_list(cal_object_list&& src) noexcept : m_head(src.m_head),
                                                                   m_own_head(src.m_own_head),
                                                                   m_obj(src.m_obj),
                                                                   m_sub_op(src.m_sub_op)
{
    src.m_own_head = false;
    src.m_head = nullptr;
    src.m_obj = nullptr;
}

cal_object_list& cal_object_list::operator=(cal_object_list&& src) noexcept
{
    if (this != &src) {
        m_head = src.m_head;
        m_own_head = src.m_own_head;
        m_obj = src.m_obj;
        m_sub_op = src.m_sub_op;

        src.m_own_head = false;
        src.m_head = nullptr;
        src.m_obj = nullptr;
    }
    return *this;
}

cal_object_list cal_object_list::add_get_object(const std::string& name, uint32_t sub_op)
{
    return cal_object_list(m_head, name, NO_ARG_VALUE, NO_ARG_VALUE, sub_op);
}

cal_object_list cal_object_list::add_set_object(
    const std::string& name, uint32_t set_op, uint16_t sid, uint32_t sub_op)
{
    return cal_object_list(m_head, name, set_op, sid, sub_op);
}

cal_object_list& cal_object_list::add_get_param(const std::string& name)
{
    HELP_PARAM_GET(m_obj, name.c_str(), m_sub_op);
    return *this;
}

cal_object_list& cal_object_list::add_set_param(const std::string& name, const std::string& value)
{
    HELP_PARAM_SET(m_obj, name.c_str(), value.c_str(), m_sub_op);
    return *this;
}

cal_object_list::iterator cal_object_list::begin()
{
    cal_object_list::iterator first(m_head, m_head, m_sub_op);
    return (++first);
}

cal_object_list::iterator cal_object_list::end()
{
    return cal_object_list::iterator(m_head, m_head, m_sub_op);
}

cal_object_list cal_object_list::operator[](const std::string& name)
{
    auto it = find_object(name);
    if (it != end()) {
        return (*it);
    }
    throw std::range_error("invalid object: " + name);
}

cal_object_list::iterator cal_object_list::find_object(const std::string& name)
{
    return find_if(
        begin(), end(), [&name](const cal_object_list& obj) { return obj.is_name(name); });
}

cal_param_list::iterator cal_object_list::find_param(const std::string& name)
{
    return get_param_list().find_param(name);
}

std::string cal_object_list::get_param(const std::string& name, const std::string& default_value)
{
    auto it = find_param(name);
    if (it != get_param_list().end()) {
        return it->get_value();
    }
    return default_value;
}

cal_param_list cal_object_list::get_param_list()
{
    return cal_param_list(m_obj, &(m_obj->xParamList));
}

bool cal_object_list::is_name(const std::string& name) const
{
    // TODO: safe strcmp with buffer size restriction
    return (0 == strcmp(m_obj->sObjName, name.c_str()));
}

std::string cal_object_list::get_name()
{
    return std::string(m_obj->sObjName);
}

uint32_t cal_object_list::get_sub_op()
{
    return m_sub_op;
}

uint32_t cal_object_list::get_obj_op()
{
    return m_obj->unObjOper;
}

uint16_t cal_object_list::get_sid()
{
    return m_obj->unSid;
}

ObjList* cal_object_list::get()
{
    return m_obj;
}

cal_object_list::operator ObjList*()
{
    return m_obj;
}

} // namespace bpl
} // namespace beerocks