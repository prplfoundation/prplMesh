///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////
/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _TLVF_STRUCTS_H_
#define _TLVF_STRUCTS_H_

#pragma pack(push, 1)
struct Tlv {
    protected:
        uint8_t tlvType;
        uint16_t tlvLength;

    public:
        uint8_t type() { return tlvType; }
        Tlv *next() { return (Tlv *)((uint8_t *)this + size()); }
        uint8_t *data() { return (uint8_t *)this + sizeof(Tlv); }
        size_t size() { return sizeof(Tlv) + ntohs(tlvLength); }
    };
#pragma pack(pop)

// packed WSC Attribute struct to help in the parsing of the packet
#pragma pack(push, 1)
struct Wsc {
    protected:
        uint16_t wsc_type;
        uint16_t length;

    public:
        uint16_t type() { return ntohs(wsc_type); }
        Wsc *next() { return (Wsc *)((uint8_t *)this + size()); }
        uint8_t *data() { return (uint8_t *)this + sizeof(Wsc); }
        size_t size() { return sizeof(Wsc) + ntohs(length); }
    };
#pragma pack(pop)

#endif
