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

#include <beerocks/tlvf/beerocks_message_backhaul.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_BACKHAUL_REGISTER_REQUEST::cACTION_BACKHAUL_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_REQUEST::cACTION_BACKHAUL_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_REQUEST::~cACTION_BACKHAUL_REGISTER_REQUEST() {
}
std::string cACTION_BACKHAUL_REGISTER_REQUEST::sta_iface_str() {
    char *sta_iface_ = sta_iface();
    if (!sta_iface_) { return std::string(); }
    return std::string(sta_iface_, m_sta_iface_idx__);
}

char* cACTION_BACKHAUL_REGISTER_REQUEST::sta_iface(size_t length) {
    if( (m_sta_iface_idx__ == 0) || (m_sta_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "sta_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_sta_iface);
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(const std::string& str) { return set_sta_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_sta_iface received a null pointer.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_sta_iface);
    return true;
}
std::string cACTION_BACKHAUL_REGISTER_REQUEST::hostap_iface_str() {
    char *hostap_iface_ = hostap_iface();
    if (!hostap_iface_) { return std::string(); }
    return std::string(hostap_iface_, m_hostap_iface_idx__);
}

char* cACTION_BACKHAUL_REGISTER_REQUEST::hostap_iface(size_t length) {
    if( (m_hostap_iface_idx__ == 0) || (m_hostap_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "hostap_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_hostap_iface);
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(const std::string& str) { return set_hostap_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_hostap_iface received a null pointer.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_hostap_iface);
    return true;
}
uint8_t& cACTION_BACKHAUL_REGISTER_REQUEST::local_master() {
    return (uint8_t&)(*m_local_master);
}

uint8_t& cACTION_BACKHAUL_REGISTER_REQUEST::local_gw() {
    return (uint8_t&)(*m_local_gw);
}

uint8_t& cACTION_BACKHAUL_REGISTER_REQUEST::sta_iface_filter_low() {
    return (uint8_t&)(*m_sta_iface_filter_low);
}

uint8_t& cACTION_BACKHAUL_REGISTER_REQUEST::onboarding() {
    return (uint8_t&)(*m_onboarding);
}

uint8_t& cACTION_BACKHAUL_REGISTER_REQUEST::certification_mode() {
    return (uint8_t&)(*m_certification_mode);
}

void cACTION_BACKHAUL_REGISTER_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_REGISTER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // sta_iface
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // hostap_iface
    class_size += sizeof(uint8_t); // local_master
    class_size += sizeof(uint8_t); // local_gw
    class_size += sizeof(uint8_t); // sta_iface_filter_low
    class_size += sizeof(uint8_t); // onboarding
    class_size += sizeof(uint8_t); // certification_mode
    return class_size;
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sta_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_sta_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_hostap_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_hostap_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_local_master = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_local_gw = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_sta_iface_filter_low = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_onboarding = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_certification_mode = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_REGISTER_RESPONSE::cACTION_BACKHAUL_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_RESPONSE::cACTION_BACKHAUL_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_RESPONSE::~cACTION_BACKHAUL_REGISTER_RESPONSE() {
}
uint8_t& cACTION_BACKHAUL_REGISTER_RESPONSE::is_backhaul_manager() {
    return (uint8_t&)(*m_is_backhaul_manager);
}

void cACTION_BACKHAUL_REGISTER_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_REGISTER_RESPONSE::finalize()
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

size_t cACTION_BACKHAUL_REGISTER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // is_backhaul_manager
    return class_size;
}

bool cACTION_BACKHAUL_REGISTER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_is_backhaul_manager = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_BUSY_NOTIFICATION::cACTION_BACKHAUL_BUSY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_BUSY_NOTIFICATION::cACTION_BACKHAUL_BUSY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_BUSY_NOTIFICATION::~cACTION_BACKHAUL_BUSY_NOTIFICATION() {
}
void cACTION_BACKHAUL_BUSY_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_BUSY_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_BUSY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BACKHAUL_BUSY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ENABLE::cACTION_BACKHAUL_ENABLE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE::cACTION_BACKHAUL_ENABLE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE::~cACTION_BACKHAUL_ENABLE() {
}
sMacAddr& cACTION_BACKHAUL_ENABLE::iface_mac() {
    return (sMacAddr&)(*m_iface_mac);
}

std::string cACTION_BACKHAUL_ENABLE::wire_iface_str() {
    char *wire_iface_ = wire_iface();
    if (!wire_iface_) { return std::string(); }
    return std::string(wire_iface_, m_wire_iface_idx__);
}

char* cACTION_BACKHAUL_ENABLE::wire_iface(size_t length) {
    if( (m_wire_iface_idx__ == 0) || (m_wire_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "wire_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_wire_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_wire_iface(const std::string& str) { return set_wire_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_wire_iface(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_wire_iface received a null pointer.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_wire_iface);
    return true;
}
std::string cACTION_BACKHAUL_ENABLE::sta_iface_str() {
    char *sta_iface_ = sta_iface();
    if (!sta_iface_) { return std::string(); }
    return std::string(sta_iface_, m_sta_iface_idx__);
}

char* cACTION_BACKHAUL_ENABLE::sta_iface(size_t length) {
    if( (m_sta_iface_idx__ == 0) || (m_sta_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "sta_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_sta_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_sta_iface(const std::string& str) { return set_sta_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_sta_iface(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_sta_iface received a null pointer.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_sta_iface);
    return true;
}
std::string cACTION_BACKHAUL_ENABLE::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* cACTION_BACKHAUL_ENABLE::ssid(size_t length) {
    if( (m_ssid_idx__ == 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cACTION_BACKHAUL_ENABLE::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_ssid(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_ssid received a null pointer.";
        return false;
    }
    if (size > beerocks::message::WIFI_SSID_MAX_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_ssid);
    return true;
}
std::string cACTION_BACKHAUL_ENABLE::pass_str() {
    char *pass_ = pass();
    if (!pass_) { return std::string(); }
    return std::string(pass_, m_pass_idx__);
}

char* cACTION_BACKHAUL_ENABLE::pass(size_t length) {
    if( (m_pass_idx__ == 0) || (m_pass_idx__ < length) ) {
        TLVF_LOG(ERROR) << "pass length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_pass);
}

bool cACTION_BACKHAUL_ENABLE::set_pass(const std::string& str) { return set_pass(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_pass(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_pass received a null pointer.";
        return false;
    }
    if (size > beerocks::message::WIFI_PASS_MAX_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_pass);
    return true;
}
uint32_t& cACTION_BACKHAUL_ENABLE::security_type() {
    return (uint32_t&)(*m_security_type);
}

sMacAddr& cACTION_BACKHAUL_ENABLE::preferred_bssid() {
    return (sMacAddr&)(*m_preferred_bssid);
}

uint8_t& cACTION_BACKHAUL_ENABLE::wire_iface_type() {
    return (uint8_t&)(*m_wire_iface_type);
}

uint8_t& cACTION_BACKHAUL_ENABLE::wireless_iface_type() {
    return (uint8_t&)(*m_wireless_iface_type);
}

uint8_t& cACTION_BACKHAUL_ENABLE::mem_only_psk() {
    return (uint8_t&)(*m_mem_only_psk);
}

uint8_t& cACTION_BACKHAUL_ENABLE::backhaul_preferred_radio_band() {
    return (uint8_t&)(*m_backhaul_preferred_radio_band);
}

beerocks::eFreqType& cACTION_BACKHAUL_ENABLE::frequency_band() {
    return (beerocks::eFreqType&)(*m_frequency_band);
}

beerocks::eWiFiBandwidth& cACTION_BACKHAUL_ENABLE::max_bandwidth() {
    return (beerocks::eWiFiBandwidth&)(*m_max_bandwidth);
}

uint8_t& cACTION_BACKHAUL_ENABLE::ht_supported() {
    return (uint8_t&)(*m_ht_supported);
}

uint16_t& cACTION_BACKHAUL_ENABLE::ht_capability() {
    return (uint16_t&)(*m_ht_capability);
}

uint8_t* cACTION_BACKHAUL_ENABLE::ht_mcs_set(size_t idx) {
    if ( (m_ht_mcs_set_idx__ == 0) || (m_ht_mcs_set_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_ht_mcs_set[idx]);
}

bool cACTION_BACKHAUL_ENABLE::set_ht_mcs_set(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_ht_mcs_set received a null pointer.";
        return false;
    }
    if (size > beerocks::message::HT_MCS_SET_SIZE) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than buffer length";
        return false;
    }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_ht_mcs_set);
    return true;
}
uint8_t& cACTION_BACKHAUL_ENABLE::vht_supported() {
    return (uint8_t&)(*m_vht_supported);
}

uint32_t& cACTION_BACKHAUL_ENABLE::vht_capability() {
    return (uint32_t&)(*m_vht_capability);
}

uint8_t* cACTION_BACKHAUL_ENABLE::vht_mcs_set(size_t idx) {
    if ( (m_vht_mcs_set_idx__ == 0) || (m_vht_mcs_set_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_vht_mcs_set[idx]);
}

bool cACTION_BACKHAUL_ENABLE::set_vht_mcs_set(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_vht_mcs_set received a null pointer.";
        return false;
    }
    if (size > beerocks::message::VHT_MCS_SET_SIZE) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than buffer length";
        return false;
    }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_vht_mcs_set);
    return true;
}
uint8_t& cACTION_BACKHAUL_ENABLE::preferred_channels_size() {
    return (uint8_t&)(*m_preferred_channels_size);
}

std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_BACKHAUL_ENABLE::preferred_channels(size_t idx) {
    bool ret_success = ( (m_preferred_channels_idx__ > 0) && (m_preferred_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_preferred_channels[ret_idx]);
}

bool cACTION_BACKHAUL_ENABLE::alloc_preferred_channels(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list preferred_channels, abort!";
        return false;
    }
    size_t len = sizeof(beerocks::message::sWifiChannel) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_preferred_channels[*m_preferred_channels_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_preferred_channels_idx__ += count;
    *m_preferred_channels_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_preferred_channels_idx__ - count; i < m_preferred_channels_idx__; i++) { m_preferred_channels[i].struct_init(); }
    }
    return true;
}

void cACTION_BACKHAUL_ENABLE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_iface_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_security_type));
    m_preferred_bssid->struct_swap();
    tlvf_swap(8*sizeof(beerocks::eFreqType), reinterpret_cast<uint8_t*>(m_frequency_band));
    tlvf_swap(8*sizeof(beerocks::eWiFiBandwidth), reinterpret_cast<uint8_t*>(m_max_bandwidth));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_ht_capability));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_vht_capability));
    for (size_t i = 0; i < (size_t)*m_preferred_channels_size; i++){
        m_preferred_channels[i].struct_swap();
    }
}

bool cACTION_BACKHAUL_ENABLE::finalize()
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

size_t cACTION_BACKHAUL_ENABLE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // iface_mac
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // wire_iface
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // sta_iface
    class_size += beerocks::message::WIFI_SSID_MAX_LENGTH * sizeof(char); // ssid
    class_size += beerocks::message::WIFI_PASS_MAX_LENGTH * sizeof(char); // pass
    class_size += sizeof(uint32_t); // security_type
    class_size += sizeof(sMacAddr); // preferred_bssid
    class_size += sizeof(uint8_t); // wire_iface_type
    class_size += sizeof(uint8_t); // wireless_iface_type
    class_size += sizeof(uint8_t); // mem_only_psk
    class_size += sizeof(uint8_t); // backhaul_preferred_radio_band
    class_size += sizeof(beerocks::eFreqType); // frequency_band
    class_size += sizeof(beerocks::eWiFiBandwidth); // max_bandwidth
    class_size += sizeof(uint8_t); // ht_supported
    class_size += sizeof(uint16_t); // ht_capability
    class_size += beerocks::message::HT_MCS_SET_SIZE * sizeof(uint8_t); // ht_mcs_set
    class_size += sizeof(uint8_t); // vht_supported
    class_size += sizeof(uint32_t); // vht_capability
    class_size += beerocks::message::VHT_MCS_SET_SIZE * sizeof(uint8_t); // vht_mcs_set
    class_size += sizeof(uint8_t); // preferred_channels_size
    return class_size;
}

bool cACTION_BACKHAUL_ENABLE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_iface_mac->struct_init(); }
    m_wire_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_wire_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_sta_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_sta_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_ssid = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::WIFI_SSID_MAX_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::WIFI_SSID_MAX_LENGTH) << ") Failed!";
        return false;
    }
    m_ssid_idx__  = beerocks::message::WIFI_SSID_MAX_LENGTH;
    m_pass = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::WIFI_PASS_MAX_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::WIFI_PASS_MAX_LENGTH) << ") Failed!";
        return false;
    }
    m_pass_idx__  = beerocks::message::WIFI_PASS_MAX_LENGTH;
    m_security_type = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_preferred_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_preferred_bssid->struct_init(); }
    m_wire_iface_type = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_wireless_iface_type = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_mem_only_psk = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_backhaul_preferred_radio_band = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_frequency_band = (beerocks::eFreqType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::eFreqType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::eFreqType) << ") Failed!";
        return false;
    }
    m_max_bandwidth = (beerocks::eWiFiBandwidth*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::eWiFiBandwidth))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::eWiFiBandwidth) << ") Failed!";
        return false;
    }
    m_ht_supported = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_ht_capability = (uint16_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_ht_mcs_set = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (beerocks::message::HT_MCS_SET_SIZE))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (beerocks::message::HT_MCS_SET_SIZE) << ") Failed!";
        return false;
    }
    m_ht_mcs_set_idx__  = beerocks::message::HT_MCS_SET_SIZE;
    m_vht_supported = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_vht_capability = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_vht_mcs_set = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (beerocks::message::VHT_MCS_SET_SIZE))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (beerocks::message::VHT_MCS_SET_SIZE) << ") Failed!";
        return false;
    }
    m_vht_mcs_set_idx__  = beerocks::message::VHT_MCS_SET_SIZE;
    m_preferred_channels_size = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_preferred_channels_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_preferred_channels = (beerocks::message::sWifiChannel*)m_buff_ptr__;
    uint8_t preferred_channels_size = *m_preferred_channels_size;
    m_preferred_channels_idx__ = preferred_channels_size;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sWifiChannel) * (preferred_channels_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sWifiChannel) * (preferred_channels_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CONNECTED_NOTIFICATION::cACTION_BACKHAUL_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CONNECTED_NOTIFICATION::cACTION_BACKHAUL_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CONNECTED_NOTIFICATION::~cACTION_BACKHAUL_CONNECTED_NOTIFICATION() {
}
sBackhaulParams& cACTION_BACKHAUL_CONNECTED_NOTIFICATION::params() {
    return (sBackhaulParams&)(*m_params);
}

void cACTION_BACKHAUL_CONNECTED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BACKHAUL_CONNECTED_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_CONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulParams); // params
    return class_size;
}

bool cACTION_BACKHAUL_CONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBackhaulParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sBackhaulParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBackhaulParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::~cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION() {
}
uint8_t& cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::stopped() {
    return (uint8_t&)(*m_stopped);
}

void cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // stopped
    return class_size;
}

bool cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_stopped = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ENABLE_APS_REQUEST::cACTION_BACKHAUL_ENABLE_APS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE_APS_REQUEST::cACTION_BACKHAUL_ENABLE_APS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE_APS_REQUEST::~cACTION_BACKHAUL_ENABLE_APS_REQUEST() {
}
uint8_t& cACTION_BACKHAUL_ENABLE_APS_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

uint32_t& cACTION_BACKHAUL_ENABLE_APS_REQUEST::bandwidth() {
    return (uint32_t&)(*m_bandwidth);
}

uint8_t& cACTION_BACKHAUL_ENABLE_APS_REQUEST::center_channel() {
    return (uint8_t&)(*m_center_channel);
}

void cACTION_BACKHAUL_ENABLE_APS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_bandwidth));
}

bool cACTION_BACKHAUL_ENABLE_APS_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_ENABLE_APS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(uint32_t); // bandwidth
    class_size += sizeof(uint8_t); // center_channel
    return class_size;
}

bool cACTION_BACKHAUL_ENABLE_APS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bandwidth = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_center_channel = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ROAM_REQUEST::cACTION_BACKHAUL_ROAM_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_REQUEST::cACTION_BACKHAUL_ROAM_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_REQUEST::~cACTION_BACKHAUL_ROAM_REQUEST() {
}
sBackhaulRoam& cACTION_BACKHAUL_ROAM_REQUEST::params() {
    return (sBackhaulRoam&)(*m_params);
}

void cACTION_BACKHAUL_ROAM_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BACKHAUL_ROAM_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_ROAM_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulRoam); // params
    return class_size;
}

bool cACTION_BACKHAUL_ROAM_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBackhaulRoam*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sBackhaulRoam))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBackhaulRoam) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ROAM_RESPONSE::cACTION_BACKHAUL_ROAM_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_RESPONSE::cACTION_BACKHAUL_ROAM_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_RESPONSE::~cACTION_BACKHAUL_ROAM_RESPONSE() {
}
uint8_t& cACTION_BACKHAUL_ROAM_RESPONSE::connected() {
    return (uint8_t&)(*m_connected);
}

void cACTION_BACKHAUL_ROAM_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_ROAM_RESPONSE::finalize()
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

size_t cACTION_BACKHAUL_ROAM_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // connected
    return class_size;
}

bool cACTION_BACKHAUL_ROAM_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_connected = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_RESET::cACTION_BACKHAUL_RESET(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_RESET::cACTION_BACKHAUL_RESET(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_RESET::~cACTION_BACKHAUL_RESET() {
}
void cACTION_BACKHAUL_RESET::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_RESET::finalize()
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

size_t cACTION_BACKHAUL_RESET::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BACKHAUL_RESET::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_4ADDR_CONNECTED::cACTION_BACKHAUL_4ADDR_CONNECTED(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_4ADDR_CONNECTED::cACTION_BACKHAUL_4ADDR_CONNECTED(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_4ADDR_CONNECTED::~cACTION_BACKHAUL_4ADDR_CONNECTED() {
}
sMacAddr& cACTION_BACKHAUL_4ADDR_CONNECTED::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_BACKHAUL_4ADDR_CONNECTED::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_BACKHAUL_4ADDR_CONNECTED::finalize()
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

size_t cACTION_BACKHAUL_4ADDR_CONNECTED::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_BACKHAUL_4ADDR_CONNECTED::init()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::~cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION() {
}
sBackhaulRssi& cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::params() {
    return (sBackhaulRssi&)(*m_params);
}

void cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulRssi); // params
    return class_size;
}

bool cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBackhaulRssi*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sBackhaulRssi))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBackhaulRssi) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::~cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST() {
}
uint32_t& cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::attempts() {
    return (uint32_t&)(*m_attempts);
}

void cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_attempts));
}

bool cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // attempts
    return class_size;
}

bool cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_attempts = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::~cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION() {
}
void cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BACKHAUL_ONBOARDING_FINISHED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST() {
}
sNodeRssiMeasurementRequest& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params() {
    return (sNodeRssiMeasurementRequest&)(*m_params);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurementRequest*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurementRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurementRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE() {
}
sNodeRssiMeasurement& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::finalize()
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

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurement*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurement) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE() {
}
sMacAddr& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::finalize()
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

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::~cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION() {
}
sVapsList& cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::params() {
    return (sVapsList&)(*m_params);
}

void cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVapsList); // params
    return class_size;
}

bool cACTION_BACKHAUL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sVapsList*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVapsList))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVapsList) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::~cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION() {
}
sMacAddr& cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint8_t* cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::association_frame(size_t idx) {
    if ( (m_association_frame_idx__ == 0) || (m_association_frame_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_association_frame[idx]);
}

bool cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::set_association_frame(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_association_frame received a null pointer.";
        return false;
    }
    if (!alloc_association_frame(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_association_frame);
    return true;
}
bool cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::alloc_association_frame(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list association_frame, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)m_association_frame;
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_association_frame_idx__ += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_client_mac->struct_swap();
    m_bssid->struct_swap();
}

bool cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // bssid
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_ASSOCIATED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_association_frame = (uint8_t*)m_buff_ptr__;
    m_association_frame_idx__ = getBuffRemainingBytes();
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::~cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION() {
}
sMacAddr& cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::bssid() {
    return (sMacAddr&)(*m_bssid);
}

void cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_client_mac->struct_swap();
    m_bssid->struct_swap();
}

bool cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::finalize()
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

size_t cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // bssid
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::~cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST() {
}
uint8_t& cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::sync() {
    return (uint8_t&)(*m_sync);
}

sMacAddr& cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

void cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    m_sta_mac->struct_swap();
}

bool cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // sync
    class_size += sizeof(sMacAddr); // sta_mac
    return class_size;
}

bool cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sync = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::~cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE() {
}
const uint16_t& cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::length() {
    return (const uint16_t&)(*m_length);
}

sMacAddr& cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

uint8_t& cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::bssid_info_list_length() {
    return (uint8_t&)(*m_bssid_info_list_length);
}

std::tuple<bool, sBssidInfo&> cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::bssid_info_list(size_t idx) {
    bool ret_success = ( (m_bssid_info_list_idx__ > 0) && (m_bssid_info_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_bssid_info_list[ret_idx]);
}

bool cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::alloc_bssid_info_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list bssid_info_list, abort!";
        return false;
    }
    size_t len = sizeof(sBssidInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_bssid_info_list[*m_bssid_info_list_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_bssid_info_list_idx__ += count;
    *m_bssid_info_list_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if(m_length){ (*m_length) += len; }
    if (!m_parse__) { 
        for (size_t i = m_bssid_info_list_idx__ - count; i < m_bssid_info_list_idx__; i++) { m_bssid_info_list[i].struct_init(); }
    }
    return true;
}

void cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_length));
    m_sta_mac->struct_swap();
    for (size_t i = 0; i < (size_t)*m_bssid_info_list_length; i++){
        m_bssid_info_list[i].struct_swap();
    }
}

bool cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::finalize()
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

size_t cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // length
    class_size += sizeof(sMacAddr); // sta_mac
    class_size += sizeof(uint8_t); // bssid_info_list_length
    return class_size;
}

bool cACTION_BACKHAUL_ASSOCIATED_STA_LINK_METRICS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_length = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(sMacAddr); }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    m_bssid_info_list_length = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_bssid_info_list_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if(m_length && !m_parse__){ (*m_length) += sizeof(uint8_t); }
    m_bssid_info_list = (sBssidInfo*)m_buff_ptr__;
    uint8_t bssid_info_list_length = *m_bssid_info_list_length;
    m_bssid_info_list_idx__ = bssid_info_list_length;
    if (!buffPtrIncrementSafe(sizeof(sBssidInfo) * (bssid_info_list_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sBssidInfo) * (bssid_info_list_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_START_WPS_PBC_REQUEST::cACTION_BACKHAUL_START_WPS_PBC_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_START_WPS_PBC_REQUEST::cACTION_BACKHAUL_START_WPS_PBC_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_START_WPS_PBC_REQUEST::~cACTION_BACKHAUL_START_WPS_PBC_REQUEST() {
}
void cACTION_BACKHAUL_START_WPS_PBC_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BACKHAUL), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BACKHAUL_START_WPS_PBC_REQUEST::finalize()
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

size_t cACTION_BACKHAUL_START_WPS_PBC_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BACKHAUL_START_WPS_PBC_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}


