///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvChannelScanRequest.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvChannelScanRequest::tlvChannelScanRequest(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvChannelScanRequest::tlvChannelScanRequest(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvChannelScanRequest::~tlvChannelScanRequest() {
}
const eTlvTypeMap& tlvChannelScanRequest::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvChannelScanRequest::length() {
    return (const uint16_t&)(*m_length);
}

tlvChannelScanRequest::ePerformFreshScan& tlvChannelScanRequest::perform_fresh_scan() {
    return (ePerformFreshScan&)(*m_perform_fresh_scan);
}

uint8_t& tlvChannelScanRequest::radio_list_length() {
    return (uint8_t&)(*m_radio_list_length);
}

std::tuple<bool, cRadiosToScan&> tlvChannelScanRequest::radio_list(size_t idx) {
    bool ret_success = ( (m_radio_list_idx__ > 0) && (m_radio_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_radio_list_vector[ret_idx]));
}

std::shared_ptr<cRadiosToScan> tlvChannelScanRequest::create_radio_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list radio_list, abort!";
        return nullptr;
    }
    size_t len = cRadiosToScan::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_radio_list;
    if (m_radio_list_idx__ > 0) {
        src = (uint8_t *)m_radio_list_vector[m_radio_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cRadiosToScan>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvChannelScanRequest::add_radio_list(std::shared_ptr<cRadiosToScan> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_radio_list was called before add_radio_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_radio_list;
    if (m_radio_list_idx__ > 0) {
        src = (uint8_t *)m_radio_list_vector[m_radio_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_radio_list_idx__++;
    if (!m_parse__) { (*m_radio_list_length)++; }
    size_t len = ptr->getLen();
    m_radio_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvChannelScanRequest::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    tlvf_swap(8*sizeof(ePerformFreshScan), reinterpret_cast<uint8_t*>(m_perform_fresh_scan));
    for (size_t i = 0; i < m_radio_list_idx__; i++){
        std::get<1>(radio_list(i)).class_swap();
    }
}

bool tlvChannelScanRequest::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
        *m_length -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t tlvChannelScanRequest::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(ePerformFreshScan); // perform_fresh_scan
    class_size += sizeof(uint8_t); // radio_list_length
    return class_size;
}

bool tlvChannelScanRequest::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = reinterpret_cast<eTlvTypeMap*>(m_buff_ptr__);
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_CHANNEL_SCAN_REQUEST;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_perform_fresh_scan = reinterpret_cast<ePerformFreshScan*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(ePerformFreshScan))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(ePerformFreshScan) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(ePerformFreshScan); }
    m_radio_list_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_radio_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_radio_list = (cRadiosToScan*)m_buff_ptr__;
    uint8_t radio_list_length = *m_radio_list_length;
    m_radio_list_idx__ = 0;
    for (size_t i = 0; i < radio_list_length; i++) {
        auto radio_list = create_radio_list();
        if (!radio_list) {
            TLVF_LOG(ERROR) << "create_radio_list() failed";
            return false;
        }
        if (!add_radio_list(radio_list)) {
            TLVF_LOG(ERROR) << "add_radio_list() failed";
            return false;
        }
        // swap back since radio_list will be swapped as part of the whole class swap
        radio_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_CHANNEL_SCAN_REQUEST) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_CHANNEL_SCAN_REQUEST) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cRadiosToScan::cRadiosToScan(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cRadiosToScan::cRadiosToScan(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cRadiosToScan::~cRadiosToScan() {
}
sMacAddr& cRadiosToScan::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& cRadiosToScan::operating_classes_list_length() {
    return (uint8_t&)(*m_operating_classes_list_length);
}

std::tuple<bool, cRadiosToScan::sOperatingClasses&> cRadiosToScan::operating_classes_list(size_t idx) {
    bool ret_success = ( (m_operating_classes_list_idx__ > 0) && (m_operating_classes_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_operating_classes_list[ret_idx]);
}

bool cRadiosToScan::alloc_operating_classes_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list operating_classes_list, abort!";
        return false;
    }
    size_t len = sizeof(sOperatingClasses) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_operating_classes_list[*m_operating_classes_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_operating_classes_list_idx__ += count;
    *m_operating_classes_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_operating_classes_list_idx__ - count; i < m_operating_classes_list_idx__; i++) { m_operating_classes_list[i].struct_init(); }
    }
    return true;
}

void cRadiosToScan::class_swap()
{
    m_radio_uid->struct_swap();
    for (size_t i = 0; i < m_operating_classes_list_idx__; i++){
        m_operating_classes_list[i].struct_swap();
    }
}

bool cRadiosToScan::finalize()
{
    if (m_parse__) {
        TLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";
        return true;
    }
    if (m_finalized__) {
        TLVF_LOG(DEBUG) << "finalize() called for already finalized class";
        return true;
    }
    if (!isPostInitSucceeded()) {
        TLVF_LOG(ERROR) << "post init check failed";
        return false;
    }
    if (m_inner__) {
        if (!m_inner__->finalize()) {
            TLVF_LOG(ERROR) << "m_inner__->finalize() failed";
            return false;
        }
        auto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();
        m_buff_ptr__ -= tailroom;
    }
    class_swap();
    m_finalized__ = true;
    return true;
}

size_t cRadiosToScan::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // operating_classes_list_length
    return class_size;
}

bool cRadiosToScan::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_uid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_operating_classes_list_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_operating_classes_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_operating_classes_list = (sOperatingClasses*)m_buff_ptr__;
    uint8_t operating_classes_list_length = *m_operating_classes_list_length;
    m_operating_classes_list_idx__ = operating_classes_list_length;
    if (!buffPtrIncrementSafe(sizeof(sOperatingClasses) * (operating_classes_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sOperatingClasses) * (operating_classes_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


