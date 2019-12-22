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
#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <string>

class ClassList;

class BaseClass {
protected:
    BaseClass(uint8_t *buff_, const size_t buff_len_, const bool parse_ = false);
    ~BaseClass();

public:
    uint8_t *getBuffPtr();
    uint8_t *getStartBuffPtr();
    size_t getBuffRemainingBytes(void *start = nullptr);
    bool buffPtrIncrementSafe(size_t length);

    size_t getLen();
    bool isInitialized();
    virtual bool isPostInitSucceeded() { return true; };
    virtual void class_swap() = 0;
    bool finalize();
    bool is_finalized() { return m_finalized; };
    void addInnerClassList(std::shared_ptr<ClassList> list) { m_inner__ = list; };
    std::shared_ptr<ClassList> getInnerClassList() { return m_inner__; };

protected:
    uint8_t *m_buff__;
    uint8_t *m_buff_ptr__;
    const size_t m_buff_len__;
    const bool m_parse__;
    bool m_finalized;
    bool m_init_succeeded = false;
    std::shared_ptr<ClassList> m_inner__;
};

#endif //_BaseClass_H_
