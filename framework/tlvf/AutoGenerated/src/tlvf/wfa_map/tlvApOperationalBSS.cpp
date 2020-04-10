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

#include <tlvf/wfa_map/tlvApOperationalBSS.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvApOperationalBSS::tlvApOperationalBSS(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvApOperationalBSS::tlvApOperationalBSS(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvApOperationalBSS::~tlvApOperationalBSS() {
}
const eTlvTypeMap& tlvApOperationalBSS::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvApOperationalBSS::length() {
    return (const uint16_t&)(*m_length);
}

uint8_t& tlvApOperationalBSS::radio_list_length() {
    return (uint8_t&)(*m_radio_list_length);
}

std::tuple<bool, cRadioInfo&> tlvApOperationalBSS::radio_list(size_t idx) {
    bool ret_success = ( (m_radio_list_idx__ > 0) && (m_radio_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_radio_list_vector[ret_idx]));
}

std::shared_ptr<cRadioInfo> tlvApOperationalBSS::create_radio_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list radio_list, abort!";
        return nullptr;
    }
    size_t len = cRadioInfo::get_initial_size();
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
    return std::make_shared<cRadioInfo>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvApOperationalBSS::add_radio_list(std::shared_ptr<cRadioInfo> ptr) {
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

void tlvApOperationalBSS::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    for (size_t i = 0; i < (size_t)*m_radio_list_length; i++){
        std::get<1>(radio_list(i)).class_swap();
    }
}

bool tlvApOperationalBSS::finalize()
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

size_t tlvApOperationalBSS::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(uint8_t); // radio_list_length
    return class_size;
}

bool tlvApOperationalBSS::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_AP_OPERATIONAL_BSS;
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
    m_radio_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_radio_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_radio_list = (cRadioInfo*)m_buff_ptr__;
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
        if (*m_type != eTlvTypeMap::TLV_AP_OPERATIONAL_BSS) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_AP_OPERATIONAL_BSS) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cRadioInfo::cRadioInfo(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cRadioInfo::cRadioInfo(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cRadioInfo::~cRadioInfo() {
}
sMacAddr& cRadioInfo::radio_uid() {
    return (sMacAddr&)(*m_radio_uid);
}

uint8_t& cRadioInfo::radio_bss_list_length() {
    return (uint8_t&)(*m_radio_bss_list_length);
}

std::tuple<bool, cRadioBssInfo&> cRadioInfo::radio_bss_list(size_t idx) {
    bool ret_success = ( (m_radio_bss_list_idx__ > 0) && (m_radio_bss_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_radio_bss_list_vector[ret_idx]));
}

std::shared_ptr<cRadioBssInfo> cRadioInfo::create_radio_bss_list() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list radio_bss_list, abort!";
        return nullptr;
    }
    size_t len = cRadioBssInfo::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_radio_bss_list;
    if (m_radio_bss_list_idx__ > 0) {
        src = (uint8_t *)m_radio_bss_list_vector[m_radio_bss_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<cRadioBssInfo>(src, getBuffRemainingBytes(src), m_parse__);
}

bool cRadioInfo::add_radio_bss_list(std::shared_ptr<cRadioBssInfo> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_radio_bss_list was called before add_radio_bss_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_radio_bss_list;
    if (m_radio_bss_list_idx__ > 0) {
        src = (uint8_t *)m_radio_bss_list_vector[m_radio_bss_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_radio_bss_list_idx__++;
    if (!m_parse__) { (*m_radio_bss_list_length)++; }
    size_t len = ptr->getLen();
    m_radio_bss_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    m_lock_allocation__ = false;
    return true;
}

void cRadioInfo::class_swap()
{
    m_radio_uid->struct_swap();
    for (size_t i = 0; i < (size_t)*m_radio_bss_list_length; i++){
        std::get<1>(radio_bss_list(i)).class_swap();
    }
}

bool cRadioInfo::finalize()
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

size_t cRadioInfo::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_uid
    class_size += sizeof(uint8_t); // radio_bss_list_length
    return class_size;
}

bool cRadioInfo::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_uid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_uid->struct_init(); }
    m_radio_bss_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_radio_bss_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_radio_bss_list = (cRadioBssInfo*)m_buff_ptr__;
    uint8_t radio_bss_list_length = *m_radio_bss_list_length;
    m_radio_bss_list_idx__ = 0;
    for (size_t i = 0; i < radio_bss_list_length; i++) {
        auto radio_bss_list = create_radio_bss_list();
        if (!radio_bss_list) {
            TLVF_LOG(ERROR) << "create_radio_bss_list() failed";
            return false;
        }
        if (!add_radio_bss_list(radio_bss_list)) {
            TLVF_LOG(ERROR) << "add_radio_bss_list() failed";
            return false;
        }
        // swap back since radio_bss_list will be swapped as part of the whole class swap
        radio_bss_list->class_swap();
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cRadioBssInfo::cRadioBssInfo(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cRadioBssInfo::cRadioBssInfo(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cRadioBssInfo::~cRadioBssInfo() {
}
sMacAddr& cRadioBssInfo::radio_bssid() {
    return (sMacAddr&)(*m_radio_bssid);
}

uint8_t& cRadioBssInfo::ssid_length() {
    return (uint8_t&)(*m_ssid_length);
}

std::string cRadioBssInfo::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* cRadioBssInfo::ssid(size_t length) {
    if( (m_ssid_idx__ == 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cRadioBssInfo::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool cRadioBssInfo::set_ssid(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_ssid received a null pointer.";
        return false;
    }
    if (!alloc_ssid(size)) { return false; }
    std::copy(str, str + size, m_ssid);
    return true;
}
bool cRadioBssInfo::alloc_ssid(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ssid, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_ssid[*m_ssid_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_ssid_idx__ += count;
    *m_ssid_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cRadioBssInfo::class_swap()
{
    m_radio_bssid->struct_swap();
}

bool cRadioBssInfo::finalize()
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

size_t cRadioBssInfo::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_bssid
    class_size += sizeof(uint8_t); // ssid_length
    return class_size;
}

bool cRadioBssInfo::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_bssid->struct_init(); }
    m_ssid_length = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_ssid = (char*)m_buff_ptr__;
    uint8_t ssid_length = *m_ssid_length;
    m_ssid_idx__ = ssid_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (ssid_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (ssid_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


