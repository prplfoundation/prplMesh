///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/wfa_map/tlvAssociatedClients.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvAssociatedClients::tlvAssociatedClients(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvAssociatedClients::tlvAssociatedClients(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvAssociatedClients::~tlvAssociatedClients() {
}
const eTlvTypeMap& tlvAssociatedClients::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvAssociatedClients::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvAssociatedClients::bss_list_length() {
    return (uint8_t&)(*m_bss_list_length);
}

std::tuple<bool, cBssInfo&> tlvAssociatedClients::bss_list(size_t idx) {
    bool ret_success = ( (m_bss_list_idx__ > 0) && (m_bss_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_bss_list_vector[ret_idx]));
}

std::shared_ptr<cBssInfo> tlvAssociatedClients::create_bss_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list bss_list, abort!";
        return nullptr;
    }
    size_t len = cBssInfo::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_bss_list;
    if (m_bss_list_idx__ > 0) {
        src = (uint8_t *)m_bss_list_vector[m_bss_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cBssInfo>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvAssociatedClients::add_bss_list(std::shared_ptr<cBssInfo> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_bss_list was called before add_bss_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_bss_list;
    if (m_bss_list_idx__ > 0) {
        src = (uint8_t *)m_bss_list_vector[m_bss_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_bss_list_idx__++;
    if (!m_parse__) { (*m_bss_list_length)++; }
    size_t len = ptr->getLen();
    m_bss_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

void tlvAssociatedClients::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    for (size_t i = 0; i < (size_t)*m_bss_list_length; i++){
        std::get<1>(bss_list(i)).class_swap();
    }
}

bool tlvAssociatedClients::finalize()
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

size_t tlvAssociatedClients::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // bss_list_length
    return class_size;
}

bool tlvAssociatedClients::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_ASSOCIATED_CLIENTS;
    if (!buffPtrIncrementSafe(sizeof(eTlvTypeMap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eTlvTypeMap) << ") Failed!";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_bss_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_bss_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bss_list = (cBssInfo*)m_buff_ptr__;
    uint8_t bss_list_length = *m_bss_list_length;
    m_bss_list_idx__ = 0;
    for (size_t i = 0; i < bss_list_length; i++) {
        auto bss_list = create_bss_list();
        if (!bss_list) {
            TLVF_LOG(ERROR) << "create_bss_list() failed";
            return false;
        }
        if (!add_bss_list(bss_list)) {
            TLVF_LOG(ERROR) << "add_bss_list() failed";
            return false;
        }
        // swap back since bss_list will be swapped as part of the whole class swap
        bss_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_ASSOCIATED_CLIENTS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_ASSOCIATED_CLIENTS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cBssInfo::cBssInfo(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cBssInfo::cBssInfo(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cBssInfo::~cBssInfo() {
}
sMacAddr& cBssInfo::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint16_t& cBssInfo::clients_associated_list_length() {
    return (uint16_t&)(*m_clients_associated_list_length);
}

std::tuple<bool, cClientInfo&> cBssInfo::clients_associated_list(size_t idx) {
    bool ret_success = ( (m_clients_associated_list_idx__ > 0) && (m_clients_associated_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_clients_associated_list_vector[ret_idx]));
}

std::shared_ptr<cClientInfo> cBssInfo::create_clients_associated_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list clients_associated_list, abort!";
        return nullptr;
    }
    size_t len = cClientInfo::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_clients_associated_list;
    if (m_clients_associated_list_idx__ > 0) {
        src = (uint8_t *)m_clients_associated_list_vector[m_clients_associated_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cClientInfo>(src, getBuffRemainingBytes(src), m_parse__);
}

bool cBssInfo::add_clients_associated_list(std::shared_ptr<cClientInfo> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_clients_associated_list was called before add_clients_associated_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_clients_associated_list;
    if (m_clients_associated_list_idx__ > 0) {
        src = (uint8_t *)m_clients_associated_list_vector[m_clients_associated_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_clients_associated_list_idx__++;
    if (!m_parse__) { (*m_clients_associated_list_length)++; }
    size_t len = ptr->getLen();
    m_clients_associated_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    m_lock_allocation__ = false;
    return true;
}

void cBssInfo::class_swap()
{
    m_bssid->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_clients_associated_list_length));
    for (size_t i = 0; i < (size_t)*m_clients_associated_list_length; i++){
        std::get<1>(clients_associated_list(i)).class_swap();
    }
}

bool cBssInfo::finalize()
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

size_t cBssInfo::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint16_t); // clients_associated_list_length
    return class_size;
}

bool cBssInfo::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_clients_associated_list_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_clients_associated_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_clients_associated_list = (cClientInfo*)m_buff_ptr__;
    uint16_t clients_associated_list_length = *m_clients_associated_list_length;
    if (m_parse__) {  tlvf_swap(16, reinterpret_cast<uint8_t*>(&clients_associated_list_length)); }
    m_clients_associated_list_idx__ = 0;
    for (size_t i = 0; i < clients_associated_list_length; i++) {
        auto clients_associated_list = create_clients_associated_list();
        if (!clients_associated_list) {
            TLVF_LOG(ERROR) << "create_clients_associated_list() failed";
            return false;
        }
        if (!add_clients_associated_list(clients_associated_list)) {
            TLVF_LOG(ERROR) << "add_clients_associated_list() failed";
            return false;
        }
        // swap back since clients_associated_list will be swapped as part of the whole class swap
        clients_associated_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cClientInfo::cClientInfo(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cClientInfo::cClientInfo(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cClientInfo::~cClientInfo() {
}
sMacAddr& cClientInfo::mac() {
    return (sMacAddr&)(*m_mac);
}

uint16_t& cClientInfo::time_since_last_association_sec() {
    return (uint16_t&)(*m_time_since_last_association_sec);
}

void cClientInfo::class_swap()
{
    m_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_time_since_last_association_sec));
}

bool cClientInfo::finalize()
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

size_t cClientInfo::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(uint16_t); // time_since_last_association_sec
    return class_size;
}

bool cClientInfo::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_time_since_last_association_sec = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


