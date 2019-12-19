/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Arnout Vandecappelle (Essensium/Mind)
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _ClassList_H_
#define _ClassList_H_

#include <memory>
#include <tlvf/BaseClass.h>
#include <tlvf/ieee_1905_1/eTlvType.h>
#include <tlvf/tlvflogging.h>
#include <vector>

class ClassList {

public:
    ClassList() = delete;
    ClassList(uint8_t *buff, size_t buff_len, bool parse = false);
    virtual ~ClassList() = default;

public:
    const std::vector<std::shared_ptr<BaseClass>> &getClassVector() const
    {
        return (const std::vector<std::shared_ptr<BaseClass>> &)m_class_vector;
    }

    template <class T> std::shared_ptr<T> addClass()
    {
        std::shared_ptr<T> ptr;
        if (m_buff) {
            if (m_class_vector.size() == 0) {
                ptr = std::make_shared<T>(m_buff, m_buff_len, m_parse);
            } else {
                // do not allow to use addClass method if the previous tlv is not fully initialized
                if (!m_class_vector.back()->isPostInitSucceeded()) {
                    TLVF_LOG(ERROR) << "TLV post init failed";
                    return nullptr;
                }
                ptr = std::make_shared<T>(m_class_vector.back(), m_parse);
            }
            if (!ptr || ptr->isInitialized() == false) {
                return std::shared_ptr<T>();
            } else {
                m_class_vector.push_back(std::shared_ptr<BaseClass>(ptr));
            }
        } else {
            return std::shared_ptr<T>();
        }
        return ptr;
    }

    /**
     * @brief Get the Class object at index idx in the all classes array
     *
     * @param idx index in the all classes array
     * @return std::shared_ptr<BaseClass> to the class object at index idx, nullptr if not found
     */
    std::shared_ptr<BaseClass> getClass(size_t idx) const;

    /**
     * @brief Get the (first) Class object
     *
     * @tparam T class template
     * @return std::shared_ptr<T> to the first object found of type T, nullptr if not found
     */
    template <class T> std::shared_ptr<T> getClass() const
    {
        for (size_t idx = 0; idx < getClassCount(); idx++) {
            if (auto c = std::dynamic_pointer_cast<T>(getClass(idx)))
                return c;
        }
        return nullptr;
    }

    /**
     * @brief Get a class object of type T in index `idx` in the logical array containing
     *        all classes of type T.
     *
     * @tparam T class template
     * @param idx index in the class T array
     * @return std::shared_ptr<T> to the T class at index `idx` in the class T array
     */
    template <class T> std::shared_ptr<T> getClass(size_t idx) const
    {
        size_t idx_ = 0;
        for (size_t i = 0; i < getClassCount(); i++) {
            if (auto c = std::dynamic_pointer_cast<T>(getClass(i))) {
                if (idx_++ == idx)
                    return c;
            }
        }
        return nullptr;
    }

    /**
     * @brief Get the number of classes of type T
     *
     * @tparam T class template
     * @return size_t number of classes of type T
     */
    template <class T> size_t getClassCount() const
    {
        size_t count = 0;
        for (size_t i = 0; i < getClassCount(); i++) {
            if (auto c = std::dynamic_pointer_cast<T>(getClass(i))) {
                count++;
            }
        }
        return count;
    }

    /**
     * @brief get previousely added class
     *
     * Get a pointer to the last added class.
     *
     * @return std::shared_ptr<BaseClass> 
     */
    std::shared_ptr<BaseClass> prevClass() const
    {
        return m_class_vector.empty() ? nullptr : m_class_vector.back();
    };

    size_t getClassCount() const { return m_class_vector.size(); };
    size_t getMessageLength() const;
    size_t getMessageBuffLength() const { return m_buff_len; };
    uint8_t *getMessageBuff() const { return m_buff; };
    void swap();
    bool is_finalized() const { return m_finalized; };
    bool is_swapped() const { return m_swapped; };
    bool finalize();
    void reset(bool parse);

protected:
    uint8_t *const m_buff;
    size_t m_buff_len;

    bool m_parse     = false;
    bool m_finalized = false;
    bool m_swapped   = false;
    std::vector<std::shared_ptr<BaseClass>> m_class_vector;
};

#endif //_TlvList_H_
