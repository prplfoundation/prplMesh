/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
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
    virtual ~BaseClass() = default;

public:
    uint8_t *getBuffPtr();
    uint8_t *getStartBuffPtr();
    size_t getBuffRemainingBytes(void *start = nullptr);
    bool buffPtrIncrementSafe(size_t length);

    size_t getLen();
    bool isInitialized();
    virtual bool isPostInitSucceeded() { return true; };
    virtual void class_swap() = 0;
    virtual bool finalize()   = 0;
    bool is_finalized() { return m_finalized__; };
    void addInnerClassList(std::shared_ptr<ClassList> list) { m_inner__ = list; };
    std::shared_ptr<ClassList> getInnerClassList() { return m_inner__; };
    /**
     * @brief cast class
     *
     * Cast self to std::shared_ptr<T> which will first swap the whole class back
     * to the original byte order, then create a shared_ptr<T> on the buffer.
     * Used only in parsing.
     *
     * @tparam T template type to cast to
     * @return std::shared_ptr<T> pointer to the casted class
     */
    template <class T> std::shared_ptr<T> class_cast()
    {
        if (m_parse__) {
            class_swap();
            return std::make_shared<T>(m_buff__, m_buff_len__, true);
        }
        return nullptr;
    }

protected:
    uint8_t *m_buff__;
    uint8_t *m_buff_ptr__;
    const size_t m_buff_len__;
    const bool m_parse__;
    bool m_finalized__    = false;
    bool m_init_succeeded = false;
    std::shared_ptr<ClassList> m_inner__;
};

#endif //_BaseClass_H_
