/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2019 Tomer Eliyahu (Intel)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/WSC/AttrList.h>
#include <tlvf/WSC/m1.h>
#include <tlvf/WSC/m2.h>
#include <tlvf/tlvfdefines.h>
#include <tlvf/tlvflogging.h>

using namespace WSC;

std::shared_ptr<AttrList> AttrList::create(ieee1905_1::tlvWsc &tlv, const config &cfg)
{
    std::shared_ptr<AttrList> attributes = nullptr;
    if (cfg.msg_type == eWscMessageType::WSC_MSG_TYPE_M1) {
        attributes = std::make_shared<m1>(tlv.payload(), tlv.payload_length(), false);
    } else if (cfg.msg_type == eWscMessageType::WSC_MSG_TYPE_M2) {
        attributes = std::make_shared<m2>(tlv.payload(), tlv.payload_length(), false);
    } else {
        TLVF_LOG(ERROR) << "Invalid WSC message type";
        return nullptr;
    }

    if (!attributes || !attributes->init(cfg)) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }

    if (!attributes->finalize()) {
        TLVF_LOG(ERROR) << "Failed to finalize attributes";
        return nullptr;
    }
    tlv.addInnerClassList(attributes);
    return attributes;
}

static eWscMessageType wsc_msg_type_from_buffer(uint8_t *buff, size_t buff_len)
{
    auto attr                = reinterpret_cast<sWscAttrHeader *>(buff);
    size_t attributes_length = 0;
    while (ntohs(attr->type) != ATTR_MSG_TYPE) {
        attributes_length += ntohs(attr->length) + sizeof(sWscAttrHeader);
        if (attributes_length > buff_len)
            return eWscMessageType::WSC_MSG_TYPE_INVALID;
        attr = reinterpret_cast<sWscAttrHeader *>(buff + attributes_length);
    }
    return static_cast<eWscMessageType>(*attr->data());
}

std::shared_ptr<AttrList> AttrList::parse(ieee1905_1::tlvWsc &tlv)
{
    if (!tlv.payload_length()) {
        TLVF_LOG(ERROR) << "No room to add attribute list (payload length = 0)";
        return nullptr;
    }
    std::shared_ptr<AttrList> attributes = nullptr;
    //Check if its M1 or M2
    auto msg_type = wsc_msg_type_from_buffer(tlv.payload(), tlv.payload_length());
    if (msg_type == eWscMessageType::WSC_MSG_TYPE_M1) {
        attributes = std::make_shared<m1>(tlv.payload(), tlv.payload_length(), true);
    } else if (msg_type == eWscMessageType::WSC_MSG_TYPE_M2) {
        attributes = std::make_shared<m2>(tlv.payload(), tlv.payload_length(), true);
    }
    if (!attributes || !attributes->init()) {
        TLVF_LOG(ERROR) << "Failed to initialize attributes";
        return nullptr;
    }
    return attributes;
}

bool AttrList::valid() const
{
    if (!getAttr<cWscAttrVersion>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrVersion> failed";
        return false;
    }
    if (!getAttr<cWscAttrMessageType>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrMessageType> failed";
        return false;
    }
    if (!getAttr<cWscAttrEnrolleeNonce>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrEnrolleeNonce> failed";
        return false;
    }
    if (!getAttr<cWscAttrPublicKey>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrPublicKey> failed";
        return false;
    }
    if (!getAttr<cWscAttrAuthenticationTypeFlags>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrAuthenticationTypeFlags> failed";
        return false;
    }
    if (!getAttr<cWscAttrEncryptionTypeFlags>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrEncryptionTypeFlags> failed";
        return false;
    }
    if (!getAttr<cWscAttrConnectionTypeFlags>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrConnectionTypeFlags> failed";
        return false;
    }
    if (!getAttr<cWscAttrConfigurationMethods>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrConfigurationMethods> failed";
        return false;
    }
    if (!getAttr<cWscAttrManufacturer>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrManufacturer> failed";
        return false;
    }
    if (!getAttr<cWscAttrModelName>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrModelName> failed";
        return false;
    }
    if (!getAttr<cWscAttrModelNumber>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrModelNumber> failed";
        return false;
    }
    if (!getAttr<cWscAttrSerialNumber>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrSerialNumber> failed";
        return false;
    }
    if (!getAttr<cWscAttrPrimaryDeviceType>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrPrimaryDeviceType> failed";
        return false;
    }
    if (!getAttr<cWscAttrDeviceName>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrDeviceName> failed";
        return false;
    }
    if (!getAttr<cWscAttrRfBands>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrRfBands> failed";
        return false;
    }
    if (!getAttr<cWscAttrAssociationState>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrAssociationState> failed";
        return false;
    }
    if (!getAttr<cWscAttrConfigurationError>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrConfigurationError> failed";
        return false;
    }
    if (!getAttr<cWscAttrDevicePasswordID>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrDevicePasswordID> failed";
        return false;
    }
    if (!getAttr<cWscAttrOsVersion>()) {
        TLVF_LOG(ERROR) << "getAttr<cWscAttrOsVersion> failed";
        return false;
    }
    return valid_custom();
}

bool AttrList::init()
{
    if (!m_parse) {
        TLVF_LOG(ERROR) << "init() called but m_parse is not set!";
        return false; // Used for parse only
    }
    while (getRemainingBytes() > 0) {
        switch (getNextAttrType()) {
        case ATTR_ASSOC_STATE:
            if (!addAttr<cWscAttrAssociationState>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrAssociationState";
                return false;
            }
            break;
        case ATTR_AUTH_TYPE_FLAGS:
            if (!addAttr<cWscAttrAuthenticationTypeFlags>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrAuthenticationTypeFlags";
                return false;
            }
            break;
        case ATTR_CONFIG_METHODS:
            if (!addAttr<cWscAttrConfigurationMethods>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrConfigurationMethods";
                return false;
            }
            break;
        case ATTR_CONFIG_ERROR:
            if (!addAttr<cWscAttrConfigurationError>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrConfigurationError";
                return false;
            }
            break;
        case ATTR_CONN_TYPE_FLAGS:
            if (!addAttr<cWscAttrConnectionTypeFlags>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrConnectionTypeFlags";
                return false;
            }
            break;
        case ATTR_ENCR_TYPE_FLAGS:
            if (!addAttr<cWscAttrEncryptionTypeFlags>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrEncryptionTypeFlags";
                return false;
            }
            break;
        case ATTR_DEV_NAME:
            if (!addAttr<cWscAttrDeviceName>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrDeviceName";
                return false;
            }
            break;
        case ATTR_DEV_PASSWORD_ID:
            if (!addAttr<cWscAttrDevicePasswordID>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrDevicePasswordID";
                return false;
            }
            break;
        case ATTR_ENROLLEE_NONCE:
            if (!addAttr<cWscAttrEnrolleeNonce>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrEnrolleeNonce";
                return false;
            }
            break;
        case ATTR_MAC_ADDR:
            if (!addAttr<cWscAttrMac>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrMac";
                return false;
            }
            break;
        case ATTR_MANUFACTURER:
            if (!addAttr<cWscAttrManufacturer>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrManufacturer";
                return false;
            }
            break;
        case ATTR_MSG_TYPE:
            if (!addAttr<cWscAttrMessageType>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrMessageType";
                return false;
            }
            break;
        case ATTR_MODEL_NAME:
            if (!addAttr<cWscAttrModelName>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrModelName";
                return false;
            }
            break;
        case ATTR_MODEL_NUMBER:
            if (!addAttr<cWscAttrModelNumber>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrModelNumber";
                return false;
            }
            break;
        case ATTR_OS_VERSION:
            if (!addAttr<cWscAttrOsVersion>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrOsVersion";
                return false;
            }
            break;
        case ATTR_PUBLIC_KEY:
            if (!addAttr<cWscAttrPublicKey>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrPublicKey";
                return false;
            }
            break;
        case ATTR_RF_BANDS:
            if (!addAttr<cWscAttrRfBands>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrRfBands";
                return false;
            }
            break;
        case ATTR_SERIAL_NUMBER:
            if (!addAttr<cWscAttrSerialNumber>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrSerialNumber";
                return false;
            }
            break;
        case ATTR_WSC_STATE:
            if (!addAttr<cWscAttrWscState>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrWscState";
                return false;
            }
            break;
        case ATTR_UUID_E:
            if (!addAttr<cWscAttrUuidE>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrUuidE";
                return false;
            }
            break;
        case ATTR_VENDOR_EXTENSION:
            if (!addAttr<cWscVendorExtWfa>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscVendorExtWfa";
                return false;
            }
            break;
        case ATTR_VERSION:
            if (!addAttr<cWscAttrVersion>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrVersion";
                return false;
            }
            break;
        case ATTR_PRIMARY_DEV_TYPE:
            if (!addAttr<cWscAttrPrimaryDeviceType>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrPrimaryDeviceType";
                return false;
            }
            break;
        case ATTR_REGISTRAR_NONCE:
            if (!addAttr<cWscAttrRegistrarNonce>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrRegistrarNonce";
                return false;
            }
            break;
        case ATTR_UUID_R:
            if (!addAttr<cWscAttrUuidR>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrUuidR";
                return false;
            }
            break;
        case ATTR_ENCR_SETTINGS:
            if (!addAttr<cWscAttrEncryptedSettings>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrEncryptedSettings";
                return false;
            }
            break;
        case ATTR_AUTHENTICATOR:
            if (!addAttr<cWscAttrAuthenticator>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrAuthenticator";
                return false;
            }
            break;
        // Other attributes are not expected, if so ignore them
        default:
            TLVF_LOG(ERROR) << "Unknown attribute " << getNextAttrType();
            break;
        }
    }
    return true;
}
