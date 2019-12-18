/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Arnout Vandecappelle (Essensium/Mind)
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TlvList_H_
#define _TlvList_H_

#include <tlvf/ieee_1905_1/eTlvType.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <tlvf/BaseClass.h>

namespace ieee1905_1 {

typedef struct TlvHeader {
    uint8_t type;
    uint16_t length;
} __attribute__((packed)) TlvHeader;

class TlvList {

public:
    TlvList() = delete;
    TlvList(uint8_t *buff, size_t buff_len, bool parse = false, bool swap = false);

    ~TlvList();

public:
    template <class T> std::shared_ptr<T> addClass()
    {
        std::shared_ptr<T> ptr;
        if (m_buff) {
            if (m_class_vector.size() == 0) {
                ptr = std::make_shared<T>(m_buff, m_buff_len, m_parse, m_swap);
            } else {
                ptr = std::make_shared<T>(m_class_vector.back(), m_parse, m_swap);
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

    template <class T> std::shared_ptr<T> dynamicCast(std::shared_ptr<BaseClass> ptr) const
    {
        return std::dynamic_pointer_cast<T>(ptr);
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

    size_t getClassCount() const;
    const std::vector<std::shared_ptr<BaseClass>> &getClassVector() const;
    size_t getMessageLength() const;
    size_t getMessageBuffLength() const;
    uint8_t *getMessageBuff() const;
    void swap();
    bool swap_needed() {return m_swap; }
    bool is_finalized() const { return m_finalized; };
    bool is_swapped() const { return m_swapped; };
    virtual bool finalize(bool swap_needed);
    void reset(bool parse, bool swap);
    void addInnerTlvList(eTlvType type, std::shared_ptr<TlvList> list)
    {
        m_inner_tlv_lists[type] = list;
    }
    template <class T> std::shared_ptr<T>
    getInnerTlvList(eTlvType type)
    {
        return std::dynamic_pointer_cast<T>(m_inner_tlv_lists[type]);
    }

protected:

    uint8_t * const m_buff;
    size_t m_buff_len;
    
    bool m_parse                 = false;
    bool m_swap                  = false;
    bool m_finalized             = false;
    bool m_swapped               = false;
    bool m_dynamically_allocated = false;
    std::vector<std::shared_ptr<BaseClass>> m_class_vector;
    std::unordered_map<eTlvType, std::shared_ptr<TlvList>> m_inner_tlv_lists;

private:
    /**
     * @brief Get the Tlv. If multiple exist, get the one at index idx
     *
     * This method returns the n'th TLV header by comparing the startBuffPtr
     * to the type parameter. It has no meaning when using on non-TLV classes.
     * TODO add is_tlv_class to the TLVF BaseClass and set it to true for TLV classes.
     *
     * @param type TLV type
     * @param idx TLV index out of all TLVs with the same type
     * @return pointer to a TlvHeader structure if found, nullptr otherwise
     */
    std::shared_ptr<BaseClass>
    getTlv(eTlvType type, size_t idx = 0)
    {
        size_t idx_ = 0;
        for (size_t i = 0; i < getClassCount(); i++) {
            auto c = getClass(i);
            TlvHeader *tlvhdr = (TlvHeader *)c->getStartBuffPtr();
            if ((static_cast<eTlvType>(tlvhdr->type) == type) && (idx_++ == idx))
                return c;
        }
        return nullptr;
    }
};

}; // close namespace: ieee1905_1

#endif //_TlvList_H_
