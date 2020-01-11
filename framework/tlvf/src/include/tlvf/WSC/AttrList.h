/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel Corporation)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_WSC_ATTR_LIST_H_
#define _TLVF_WSC_ATTR_LIST_H_

#include <memory>
#include <tlvf/ClassList.h>
#include <tlvf/WSC/WSC_Attributes.h>
#include <tlvf/ieee_1905_1/tlvWsc.h>
#include <tlvf/tlvflogging.h>

namespace WSC {

typedef struct sWscAttrHeader {
    uint16_t type;
    uint16_t length;
    uint8_t *data() { return ((uint8_t *)this + sizeof(*this)); }
} __attribute__((packed)) sWscAttrHeader;

/**
 * @brief wsc configuration
 * 
 */
struct config {
    eWscMessageType msg_type;
    uint8_t uuid_e[WSC_UUID_LENGTH];
    uint8_t uuid_r[WSC_UUID_LENGTH];
    sMacAddr mac;
    uint8_t enrollee_nonce[WSC_NONCE_LENGTH];
    uint8_t registrar_nonce[WSC_NONCE_LENGTH];
    uint8_t pub_key[WSC_PUBLIC_KEY_LENGTH];
    uint16_t encr_type_flags;
    uint16_t auth_type_flags;
    std::string manufacturer;
    std::string model_name;
    std::string model_number;
    std::string serial_number;
    uint16_t primary_dev_type_id;
    std::string device_name;
    eWscRfBands bands;
    std::vector<uint8_t> encrypted_settings;
    uint8_t iv[WSC_ENCRYPTED_SETTINGS_IV_LENGTH];
};

class AttrList : public ClassList {
protected:
    AttrList(uint8_t *buff, size_t buff_len, bool parse) : ClassList(buff, buff_len, parse) {}

public:
    virtual ~AttrList()                  = default;
    virtual bool init(const config &cfg) = 0; // used for create
    bool init();                              // used for parse

    static std::shared_ptr<AttrList> create(ieee1905_1::tlvWsc &tlv, const config &cfg);
    static std::shared_ptr<AttrList> parse(ieee1905_1::tlvWsc &tlv);

    template <class T> std::shared_ptr<T> getAttr() const { return this->getClass<T>(); };
    template <class T> std::shared_ptr<T> addAttr() { return this->addClass<T>(); };
    bool finalize() { return ClassList::finalize(); };
    size_t len() const { return this->getMessageLength(); };
    uint8_t *buffer() { return this->getMessageBuff(); };
    bool valid() const;

protected:
    sWscAttrHeader *getNextAttrHdr()
    {
        return reinterpret_cast<sWscAttrHeader *>(
            m_class_vector.empty() ? getMessageBuff() : m_class_vector.back()->getBuffPtr());
    }
    uint16_t getNextAttrType() { return ntohs(getNextAttrHdr()->type); };
    size_t getRemainingBytes()
    {
        return m_class_vector.empty() ? getMessageBuffLength()
                                      : m_class_vector.back()->getBuffRemainingBytes();
    };
    virtual bool valid_custom() const = 0;
};

} // namespace WSC

#endif // _TLVF_WSC_ATTR_LIST_H_
