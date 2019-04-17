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

#ifndef __COMMON_CALPP_PARAM_LIST_H__
#define __COMMON_CALPP_PARAM_LIST_H__

#include <string>
#include <memory>
#include <iterator>

extern "C" {
#include "help_structs.h"
}  // extern "C"

namespace beerocks {
namespace bpl {

class cal_param_list
{
  public:
    class iterator : public std::iterator<std::bidirectional_iterator_tag, cal_param_list>
    {
      public:
        iterator(ObjList* obj, ParamList* param);
        iterator(const cal_param_list& param_list);

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
        std::unique_ptr<cal_param_list> m_it;
    };

    cal_param_list(ObjList* obj, ParamList* param = nullptr);
    ~cal_param_list() = default;  // non-owning reference so nothing to destroy
    // copy/assignment/move are all default actions - only define if tests fail

    iterator begin();
    iterator end();

    iterator find_param(const std::string& name);
    std::string operator[](const std::string& name);

    bool is_name(const std::string& name) const;
    std::string get_name();
    std::string get_value();

  private:
    ObjList* m_obj;  // non-owning reference to object holding this param_list
                     // required to get begin()/end() iterators.
    ParamList* m_param;
};

} // namespace bpl
} // namespace beerocks

#endif  // __COMMON_CALPP_PARAM_LIST_H__