/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __COMMON_CALPP_PARAM_LIST_H__
#define __COMMON_CALPP_PARAM_LIST_H__

#include <iterator>
#include <memory>
#include <string>

extern "C" {
#include "help_structs.h"
} // extern "C"

namespace beerocks {
namespace bpl {

class cal_param_list {
public:
    class iterator : public std::iterator<std::bidirectional_iterator_tag, cal_param_list> {
    public:
        iterator(ObjList *obj, ParamList *param);
        iterator(const cal_param_list &param_list);

        ~iterator();
        iterator(const iterator &);
        iterator &operator=(const iterator &);

        bool operator==(const iterator &rhs) const;
        bool operator!=(const iterator &rhs) const;

        iterator &operator++();
        iterator operator++(int);
        iterator &operator--();
        iterator operator--(int);
        reference operator*() const;
        pointer operator->();

    protected:
        std::unique_ptr<cal_param_list> m_it;
    };

    cal_param_list(ObjList *obj, ParamList *param = nullptr);
    ~cal_param_list() = default; // non-owning reference so nothing to destroy
    // copy/assignment/move are all default actions - only define if tests fail

    iterator begin();
    iterator end();

    iterator find_param(const std::string &name);
    std::string operator[](const std::string &name);

    bool is_name(const std::string &name) const;
    std::string get_name();
    std::string get_value();

private:
    ObjList *m_obj; // non-owning reference to object holding this param_list
                    // required to get begin()/end() iterators.
    ParamList *m_param;
};

} // namespace bpl
} // namespace beerocks

#endif // __COMMON_CALPP_PARAM_LIST_H__