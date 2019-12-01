/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessage_H_
#define _CmduMessage_H_

#include "ieee_1905_1/cCmduHeader.h"
#include "ieee_1905_1/eTlvType.h"
#include "TlvList.h"
#include <memory>
#include <vector>

namespace ieee1905_1 {

class CmduMessage {

public:
    CmduMessage(uint8_t *buff, size_t buff_len);
    ~CmduMessage();

public:
    std::shared_ptr<cCmduHeader> getCmduHeader() const;

    ieee1905_1::TlvList tlvs;

    template <class T> std::shared_ptr<T> addClass() { return tlvs.addClass<T>(); }
    template <class T> std::shared_ptr<T> dynamicCast(std::shared_ptr<BaseClass> ptr) const
    {
        return tlvs.dynamicCast<T>(ptr);
    }

    static uint16_t getCmduHeaderLength() { return kCmduHeaderLength; }

    /**
     * @brief Get the Class object at index idx in the all classes array
     * 
     * @param idx index in the all classes array
     * @return std::shared_ptr<BaseClass> to the class object at index idx, nullptr if not found
     */
    std::shared_ptr<BaseClass> getClass(size_t idx) const { return tlvs.getClass(idx);}

    /**
     * @brief Get the (first) Class object
     * 
     * @tparam T class template
     * @return std::shared_ptr<T> to the first object found of type T, nullptr if not found
     */
    template <class T> std::shared_ptr<T> getClass() const { return tlvs.getClass<T>(); }

    /**
     * @brief Get a class object of type T in index `idx` in the logical array containing
     *        all classes of type T.
     * 
     * @tparam T class template
     * @param idx index in the class T array
     * @return std::shared_ptr<T> to the T class at index `idx` in the class T array
     */
    template <class T> std::shared_ptr<T> getClass(size_t idx) const { return tlvs.getClass<T>(idx); }

    /**
     * @brief Get the number of classes of type T
     * 
     * @tparam T class template
     * @return size_t number of classes of type T
     */
    template <class T> size_t getClassCount() const { return tlvs.getClassCount<T>(); }
    size_t getClassCount() const { return tlvs.getClassCount(); }
    const std::vector<std::shared_ptr<BaseClass>> &getClassVector() const
    {
        return tlvs.getClassVector();
    }
    size_t getMessageLength() const;
    size_t getMessageBuffLength() const;
    uint8_t *getMessageBuff() const;
    bool getNextTlvType(eTlvType &tlvType) const;
    int getNextTlvType() const;
    uint16_t getNextTlvLength() const;
    uint8_t *getNextTlvData() const;
    void swap();
    bool swap_needed() {return tlvs.swap_needed(); }
    bool is_finalized() const { return tlvs.is_finalized(); };
    bool is_swapped() const { return tlvs.is_swapped(); };
    eMessageType getMessageType();
    uint16_t getMessageId();

protected:
    void reset();

    uint8_t *m_buff              = nullptr;
    size_t m_buff_len          = 0;
    std::shared_ptr<cCmduHeader> m_cmdu_header;
    static const uint16_t kCmduHeaderLength = 8;
    static const uint16_t kTlvHeaderLength  = 3;
};

}; // close namespace: ieee1905_1

#endif //_CmduMessage_H_
