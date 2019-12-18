/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _CmduMessage_H_
#define _CmduMessage_H_

#include <memory>
#include <tlvf/TlvList.h>
#include <tlvf/ieee_1905_1/cCmduHeader.h>
#include <tlvf/ieee_1905_1/eTlvType.h>
#include <tlvf/ieee_1905_1/tlvEndOfMessage.h>
#include <vector>

namespace ieee1905_1 {

class CmduMessage {

public:
    CmduMessage() = delete;
    CmduMessage(uint8_t *buff, size_t buff_len);
    ~CmduMessage();

    std::shared_ptr<cCmduHeader> getCmduHeader() const;

    TlvList tlvs;

    template <class T> std::shared_ptr<T> addClass() { return tlvs.addClass<T>(); }

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
    template <class T> std::shared_ptr<T> getClass(size_t idx) const
    {
        return tlvs.getClass<T>(idx);
    }

    /**
     * @brief Get the number of classes of type T
     * 
     * @tparam T class template
     * @return size_t number of classes of type T
     */
    template <class T> size_t getClassCount() const { return tlvs.getClassCount<T>(); }

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
    bool is_finalized() const { return tlvs.is_finalized(); };
    bool is_swapped() const { return tlvs.is_swapped(); };
    eMessageType getMessageType();
    uint16_t getMessageId();
    void set_max_tlv_length(size_t length) { m_max_tlv_length = length; };
    size_t max_tlv_length() { return m_max_tlv_length; };

    static const uint16_t kCmduHeaderLength;
    static const uint16_t kTlvHeaderLength;
    static const size_t kMaxCmduLength;

private:
    // By default, we assume that the CMDU will be sent over the ieee1905_1 transport.
    // Since the ieee1905_1 transport only supports CMDU fragmantation on TLVs boundaries,
    // a TLV max size can't be larger than the MTU - CMDU header length - EOM TLV length.
    // However, when the CMDU is sent internally via UDS, this restriction does not apply.
    // So we allo changing it via set_max_tlv_length() API.
    size_t m_max_tlv_length =
        kMaxCmduLength - kCmduHeaderLength - ieee1905_1::tlvEndOfMessage::get_initial_size();
};

}; // namespace ieee1905_1

#endif //_CmduMessage_H_
