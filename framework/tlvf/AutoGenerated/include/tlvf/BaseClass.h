///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////
/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BaseClass_H_
#define _BaseClass_H_

#include <cstddef>
#include <stddef.h>
#include <stdint.h>
#include <string>

class BaseClass {
protected:
    BaseClass(uint8_t *buff_, const size_t buff_len_, const bool parse_ = false,
              const bool swap_needed = false);
    ~BaseClass();

public:
    uint8_t *getBuffPtr();
    uint8_t *getStartBuffPtr();
    size_t getBuffRemainingBytes(void *start = nullptr);
    bool buffPtrIncrementSafe(size_t length);
    size_t getLen();
    bool isInitialized();
    virtual void class_swap() = 0;

protected:
    uint8_t *m_buff__;
    uint8_t *m_buff_ptr__;
    const size_t m_buff_len__;
    const bool m_parse__;
    const bool m_swap__;
    bool m_init_succeeded;
    static const size_t kMinimumLength = 3; // tlv header length
};

#endif //_BaseClass_H_
