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

#include <tlvf/wfa_map/tlvBeaconMetricsQuery.h>
#include <tlvf/tlvflogging.h>

using namespace wfa_map;

tlvBeaconMetricsQuery::tlvBeaconMetricsQuery(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
tlvBeaconMetricsQuery::tlvBeaconMetricsQuery(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
tlvBeaconMetricsQuery::~tlvBeaconMetricsQuery() {
}
const eTlvTypeMap& tlvBeaconMetricsQuery::type() {
    return (const eTlvTypeMap&)(*m_type);
}

const uint16_t& tlvBeaconMetricsQuery::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& tlvBeaconMetricsQuery::associated_sta_mac() {
    return (sMacAddr&)(*m_associated_sta_mac);
}

uint8_t& tlvBeaconMetricsQuery::operating_class() {
    return (uint8_t&)(*m_operating_class);
}

uint8_t& tlvBeaconMetricsQuery::channel_number() {
    return (uint8_t&)(*m_channel_number);
}

sMacAddr& tlvBeaconMetricsQuery::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint8_t& tlvBeaconMetricsQuery::reporting_detail_value() {
    return (uint8_t&)(*m_reporting_detail_value);
}

uint8_t& tlvBeaconMetricsQuery::ssid_length() {
    return (uint8_t&)(*m_ssid_length);
}

std::string tlvBeaconMetricsQuery::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* tlvBeaconMetricsQuery::ssid(size_t length) {
    if( (m_ssid_idx__ == 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool tlvBeaconMetricsQuery::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool tlvBeaconMetricsQuery::set_ssid(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_ssid received a null pointer.";
        return false;
    }
    if (!alloc_ssid(size)) { return false; }
    std::copy(str, str + size, m_ssid);
    return true;
}
bool tlvBeaconMetricsQuery::alloc_ssid(size_t count) {
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
    m_ap_channel_reports_list_length = (uint8_t *)((uint8_t *)(m_ap_channel_reports_list_length) + len);
    m_ap_channel_reports_list = (cApChannelReports *)((uint8_t *)(m_ap_channel_reports_list) + len);
    m_elemnt_id_list_length = (uint8_t *)((uint8_t *)(m_elemnt_id_list_length) + len);
    m_elemnt_id_list = (uint8_t *)((uint8_t *)(m_elemnt_id_list) + len);
    m_ssid_idx__ += count;
    *m_ssid_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

uint8_t& tlvBeaconMetricsQuery::ap_channel_reports_list_length() {
    return (uint8_t&)(*m_ap_channel_reports_list_length);
}

std::tuple<bool, cApChannelReports&> tlvBeaconMetricsQuery::ap_channel_reports_list(size_t idx) {
    bool ret_success = ( (m_ap_channel_reports_list_idx__ > 0) && (m_ap_channel_reports_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_ap_channel_reports_list_vector[ret_idx]));
}

std::shared_ptr<cApChannelReports> tlvBeaconMetricsQuery::create_ap_channel_reports_list() {
    if (m_lock_order_counter__ > 1) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ap_channel_reports_list, abort!";
        return nullptr;
    }
    size_t len = cApChannelReports::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 1;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_ap_channel_reports_list;
    if (m_ap_channel_reports_list_idx__ > 0) {
        src = (uint8_t *)m_ap_channel_reports_list_vector[m_ap_channel_reports_list_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_elemnt_id_list_length = (uint8_t *)((uint8_t *)(m_elemnt_id_list_length) + len);
    m_elemnt_id_list = (uint8_t *)((uint8_t *)(m_elemnt_id_list) + len);
    return std::make_shared<cApChannelReports>(src, getBuffRemainingBytes(src), m_parse__);
}

bool tlvBeaconMetricsQuery::add_ap_channel_reports_list(std::shared_ptr<cApChannelReports> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_ap_channel_reports_list was called before add_ap_channel_reports_list";
        return false;
    }
    uint8_t *src = (uint8_t *)m_ap_channel_reports_list;
    if (m_ap_channel_reports_list_idx__ > 0) {
        src = (uint8_t *)m_ap_channel_reports_list_vector[m_ap_channel_reports_list_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_ap_channel_reports_list_idx__++;
    if (!m_parse__) { (*m_ap_channel_reports_list_length)++; }
    size_t len = ptr->getLen();
    m_elemnt_id_list_length = (uint8_t *)((uint8_t *)(m_elemnt_id_list_length) + len - ptr->get_initial_size());
    m_elemnt_id_list = (uint8_t *)((uint8_t *)(m_elemnt_id_list) + len - ptr->get_initial_size());
    m_ap_channel_reports_list_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(!m_parse__ && m_length){ (*m_length) += len; }
    m_lock_allocation__ = false;
    return true;
}

uint8_t& tlvBeaconMetricsQuery::elemnt_id_list_length() {
    return (uint8_t&)(*m_elemnt_id_list_length);
}

uint8_t* tlvBeaconMetricsQuery::elemnt_id_list(size_t idx) {
    if ( (m_elemnt_id_list_idx__ == 0) || (m_elemnt_id_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_elemnt_id_list[idx]);
}

bool tlvBeaconMetricsQuery::set_elemnt_id_list(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_elemnt_id_list received a null pointer.";
        return false;
    }
    if (!alloc_elemnt_id_list(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_elemnt_id_list);
    return true;
}
bool tlvBeaconMetricsQuery::alloc_elemnt_id_list(size_t count) {
    if (m_lock_order_counter__ > 2) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list elemnt_id_list, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 2;
    uint8_t *src = (uint8_t *)&m_elemnt_id_list[*m_elemnt_id_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_elemnt_id_list_idx__ += count;
    *m_elemnt_id_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    return true;
}

void tlvBeaconMetricsQuery::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_associated_sta_mac->struct_swap();
    m_bssid->struct_swap();
    for (size_t i = 0; i < m_ap_channel_reports_list_idx__; i++){
        std::get<1>(ap_channel_reports_list(i)).class_swap();
    }
}

bool tlvBeaconMetricsQuery::finalize()
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

size_t tlvBeaconMetricsQuery::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eTlvTypeMap); // type
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // associated_sta_mac
    class_size += sizeof(uint8_t); // operating_class
    class_size += sizeof(uint8_t); // channel_number
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint8_t); // reporting_detail_value
    class_size += sizeof(uint8_t); // ssid_length
    class_size += sizeof(uint8_t); // ap_channel_reports_list_length
    class_size += sizeof(uint8_t); // elemnt_id_list_length
    return class_size;
}

bool tlvBeaconMetricsQuery::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_type = (eTlvTypeMap*)m_buff_ptr__;
    if (!m_parse__) *m_type = eTlvTypeMap::TLV_BEACON_METRICS_QUERY;
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
    m_associated_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_associated_sta_mac->struct_init(); }
    m_operating_class = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_channel_number = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_reporting_detail_value = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_ssid_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_ssid_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_ssid = (char*)m_buff_ptr__;
    uint8_t ssid_length = *m_ssid_length;
    m_ssid_idx__ = ssid_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (ssid_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (ssid_length) << ") Failed!";
        return false;
    }
    m_ap_channel_reports_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_ap_channel_reports_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_ap_channel_reports_list = (cApChannelReports*)m_buff_ptr__;
    uint8_t ap_channel_reports_list_length = *m_ap_channel_reports_list_length;
    m_ap_channel_reports_list_idx__ = 0;
    for (size_t i = 0; i < ap_channel_reports_list_length; i++) {
        auto ap_channel_reports_list = create_ap_channel_reports_list();
        if (!ap_channel_reports_list) {
            TLVF_LOG(ERROR) << "create_ap_channel_reports_list() failed";
            return false;
        }
        if (!add_ap_channel_reports_list(ap_channel_reports_list)) {
            TLVF_LOG(ERROR) << "add_ap_channel_reports_list() failed";
            return false;
        }
        // swap back since ap_channel_reports_list will be swapped as part of the whole class swap
        ap_channel_reports_list->class_swap();
    }
    m_elemnt_id_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_elemnt_id_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_elemnt_id_list = (uint8_t*)m_buff_ptr__;
    uint8_t elemnt_id_list_length = *m_elemnt_id_list_length;
    m_elemnt_id_list_idx__ = elemnt_id_list_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (elemnt_id_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (elemnt_id_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    if (m_parse__) {
        if (*m_type != eTlvTypeMap::TLV_BEACON_METRICS_QUERY) {
            TLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(eTlvTypeMap::TLV_BEACON_METRICS_QUERY) << ", received value: " << int(*m_type);
            return false;
        }
    }
    return true;
}

cApChannelReports::cApChannelReports(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cApChannelReports::cApChannelReports(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cApChannelReports::~cApChannelReports() {
}
uint8_t& cApChannelReports::ap_channel_report_list_length() {
    return (uint8_t&)(*m_ap_channel_report_list_length);
}

uint8_t* cApChannelReports::ap_channel_report_list(size_t idx) {
    if ( (m_ap_channel_report_list_idx__ == 0) || (m_ap_channel_report_list_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_ap_channel_report_list[idx]);
}

bool cApChannelReports::set_ap_channel_report_list(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_ap_channel_report_list received a null pointer.";
        return false;
    }
    if (!alloc_ap_channel_report_list(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_ap_channel_report_list);
    return true;
}
bool cApChannelReports::alloc_ap_channel_report_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list ap_channel_report_list, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_ap_channel_report_list[*m_ap_channel_report_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_ap_channel_report_list_idx__ += count;
    *m_ap_channel_report_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cApChannelReports::class_swap()
{
}

bool cApChannelReports::finalize()
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

size_t cApChannelReports::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // ap_channel_report_list_length
    return class_size;
}

bool cApChannelReports::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_channel_report_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_ap_channel_report_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_ap_channel_report_list = (uint8_t*)m_buff_ptr__;
    uint8_t ap_channel_report_list_length = *m_ap_channel_report_list_length;
    m_ap_channel_report_list_idx__ = ap_channel_report_list_length;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (ap_channel_report_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (ap_channel_report_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


