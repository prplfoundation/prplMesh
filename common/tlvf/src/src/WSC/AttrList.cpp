/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
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
        case ATTR_SSID:
            if (!addAttr<cWscAttrSsid>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrSsid";
                return false;
            }
            break;
        case ATTR_AUTH_TYPE:
            if (!addAttr<cWscAttrAuthenticationType>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrAuthenticationType";
                return false;
            }
            break;
        case ATTR_ENCR_TYPE:
            if (!addAttr<cWscAttrEncryptionType>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrEncryptionType";
                return false;
            }
            break;
        case ATTR_NETWORK_KEY:
            if (!addAttr<cWscAttrNetworkKey>()) {
                TLVF_LOG(ERROR) << "Failed to add cWscAttrNetworkKey";
                return false;
            }
            break;
        // Other attributes are not expected, if so ignore them silently
        default:
            TLVF_LOG(DEBUG) << "Unknown attribute " << getNextAttrType()
                            << " assuming end of AttrList";
            return true;
        }
    }
    return true;
}
