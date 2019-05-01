/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __COMMON_CALPP_OBJECT_LIST_H__
#define __COMMON_CALPP_OBJECT_LIST_H__

#include "calpp_param_list.h"

extern "C" {
#undef DEBUG
#include "cal.h"
}  // extern "C"

namespace beerocks {
namespace bpl {

// optionally-owning CAL object reference
class cal_object_list
{
  public:
    class iterator : public std::iterator<std::bidirectional_iterator_tag, cal_object_list>
    {
      public:
        iterator(ObjList* head, ObjList* obj, uint32_t sub_op);
        iterator(const cal_object_list& param_list);

        ~iterator();
        iterator(const iterator&);
        iterator& operator=(const iterator&);

        bool operator==(const iterator& rhs) const;
        bool operator!=(const iterator& rhs) const;

        iterator& operator++();
        iterator operator++(int);
        iterator& operator--();
        iterator operator--(int);
        reference operator*() const;
        pointer operator->();

      protected:
        std::unique_ptr<cal_object_list> m_it;
    };

    // head exists if the ObjList is owned externally or nullptr to create an owned ObjList
    // instance. Adds the appropriately configured object to the owned/referenced head.
    cal_object_list(ObjList* head, const std::string& name, uint32_t obj_op = NO_ARG_VALUE,
        uint16_t sid = NO_ARG_VALUE, uint32_t sub_op = SOPT_OBJVALUE);

    // add a new object to the current list that containce the referenced list_obj. This will
    // never be an owned instance since it will use the head of the provided list_ref.
    cal_object_list(cal_object_list& list_obj, const std::string& name,
        uint32_t obj_op = NO_ARG_VALUE, uint16_t sid = NO_ARG_VALUE);

    // creates a new cal_object_list instance to wrap access to a ObjList that is created/owned
    // externally. This will enable iterators and accessors.
    cal_object_list(ObjList* head, ObjList* item = nullptr, uint32_t sub_op = SOPT_OBJVALUE);

    // clean up the object if it owns the head pointer, otherwise just exit.
    ~cal_object_list();

    // cal_object_list is mostly just managed references to externally owned instances so
    // copy and assignment operations are implemented to create/duplicate non-owning copies
    // of the pointers in the src instance. Deep copies for owned instances should be
    // implemented in separate calls.
    cal_object_list(const cal_object_list& src);
    cal_object_list& operator=(const cal_object_list& src);

    // need to write a no_throw move -- what does move mean? it's just a transfer of
    // ownership/access of the instance contents, not a change to any of the data since all
    // meaningful members are pointers.
    // swap(cal_object_list& src);
    cal_object_list(cal_object_list&& src) noexcept;
    cal_object_list& operator=(cal_object_list&& src) noexcept;

    // List Construction
    cal_object_list add_get_object(const std::string& name, uint32_t sub_op = SOPT_OBJVALUE);
    cal_object_list add_set_object(const std::string& name, uint32_t set_op = OBJOPT_ADD,
        uint16_t sid = NO_ARG_VALUE, uint32_t sub_op = SOPT_OBJVALUE);

    // ParamList Construction
    cal_object_list& add_get_param(const std::string& name);
    cal_object_list& add_set_param(const std::string& name, const std::string& value);

    // List searching / lookup
    iterator begin();
    iterator end();

    cal_object_list operator[](const std::string& name);
    iterator find_object(const std::string& name);

    // Accessors
    // access params of the current object
    cal_param_list get_param_list();
    std::string get_param(
        const std::string& name, const std::string& default_value = std::string());
    cal_param_list::iterator find_param(const std::string& name);

    bool is_name(const std::string& name) const;
    std::string get_name();
    uint32_t get_sub_op();
    uint32_t get_obj_op();
    uint16_t get_sid();

    // Explicit and Implicit Conversion
    ObjList* get();
    operator ObjList*();

  private:
    ObjList* m_head;
    bool m_own_head;
    ObjList* m_obj;
    uint32_t m_sub_op;
};

} // namespace bpl
} // namespace beerocks

#endif  // __COMMON_CALPP_OBJECT_LIST_H__
