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

protected:
    uint8_t *m_buff__;
    uint8_t *m_buff_ptr__;
    const size_t m_buff_len__;
    const bool m_parse__;
    bool m_init_succeeded = false;

private:
    /**
     * @brief trim buffer by tailroom size
     *
     * This is intended to be used __only__ by friend class TlvList on finalize
     * When resizing buffers.
     *
     * @param tailroom size to trim
     * @return true on success
     * @return false on failure
     */
    friend void trim(BaseClass &obj, size_t tailroom) { obj.m_buff_ptr__ -= tailroom; };
};

#endif //_BaseClass_H_
