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

#include <tlvf/wfa_map/tlvSteeringBTMReport.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvSteeringBTMReport::tlvSteeringBTMReport(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
tlvSteeringBTMReport::tlvSteeringBTMReport(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
tlvSteeringBTMReport::~tlvSteeringBTMReport() {
}
const eTlvTypeMap& tlvSteeringBTMReport::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvSteeringBTMReport::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvSteeringBTMReport::bssid() {
    return (sMacAddr&)(*m_bssid);
}

sMacAddr& tlvSteeringBTMReport::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

uint8_t& tlvSteeringBTMReport::btm_status_code() {
    return (uint8_t&)(*m_btm_status_code);
}

sMacAddr& tlvSteeringBTMReport::target_bssid() {
    return (sMacAddr&)(*m_target_bssid);
}

void tlvSteeringBTMReport::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_bssid->struct_swap();
    m_sta_mac->struct_swap();
    m_target_bssid->struct_swap();
}

size_t tlvSteeringBTMReport::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(sMacAddr); // sta_mac
    class_size += sizeof(uint8_t); // btm_status_code
    class_size += sizeof(sMacAddr); // target_bssid
    return class_size;
}

bool tlvSteeringBTMReport::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_STEERING_BTM_REPORT;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) { return false; }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) { return false; }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    m_btm_status_code = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_target_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_target_bssid->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_STEERING_BTM_REPORT) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_STEERING_BTM_REPORT) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}


