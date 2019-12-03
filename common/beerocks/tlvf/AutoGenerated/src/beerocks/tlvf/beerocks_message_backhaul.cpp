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

#include <beerocks/tlvf/beerocks_message_backhaul.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_BACKHAUL_REGISTER_REQUEST::cACTION_BACKHAUL_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_REQUEST::cACTION_BACKHAUL_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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
    if( (m_sta_iface_idx__ <= 0) || (m_sta_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "sta_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_sta_iface);
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(const std::string& str) { return set_sta_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_sta_iface received an empty string.";
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
    if( (m_hostap_iface_idx__ <= 0) || (m_hostap_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "hostap_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_hostap_iface);
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(const std::string& str) { return set_hostap_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_hostap_iface received an empty string.";
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

void cACTION_BACKHAUL_REGISTER_REQUEST::class_swap()
{
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
    return class_size;
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sta_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_sta_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_hostap_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_hostap_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_local_master = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_local_gw = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_sta_iface_filter_low = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_onboarding = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_REGISTER_RESPONSE::cACTION_BACKHAUL_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_RESPONSE::cACTION_BACKHAUL_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_REGISTER_RESPONSE::~cACTION_BACKHAUL_REGISTER_RESPONSE() {
}
uint8_t& cACTION_BACKHAUL_REGISTER_RESPONSE::is_backhaul_manager() {
    return (uint8_t&)(*m_is_backhaul_manager);
}

void cACTION_BACKHAUL_REGISTER_RESPONSE::class_swap()
{
}

size_t cACTION_BACKHAUL_REGISTER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // is_backhaul_manager
    return class_size;
}

bool cACTION_BACKHAUL_REGISTER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_is_backhaul_manager = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_BUSY_NOTIFICATION::cACTION_BACKHAUL_BUSY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_BUSY_NOTIFICATION::cACTION_BACKHAUL_BUSY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_BUSY_NOTIFICATION::~cACTION_BACKHAUL_BUSY_NOTIFICATION() {
}
void cACTION_BACKHAUL_BUSY_NOTIFICATION::class_swap()
{
}

size_t cACTION_BACKHAUL_BUSY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BACKHAUL_BUSY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ENABLE::cACTION_BACKHAUL_ENABLE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE::cACTION_BACKHAUL_ENABLE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE::~cACTION_BACKHAUL_ENABLE() {
}
std::string cACTION_BACKHAUL_ENABLE::bridge_iface_str() {
    char *bridge_iface_ = bridge_iface();
    if (!bridge_iface_) { return std::string(); }
    return std::string(bridge_iface_, m_bridge_iface_idx__);
}

char* cACTION_BACKHAUL_ENABLE::bridge_iface(size_t length) {
    if( (m_bridge_iface_idx__ <= 0) || (m_bridge_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "bridge_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_bridge_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_bridge_iface(const std::string& str) { return set_bridge_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_bridge_iface(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_bridge_iface received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_bridge_iface);
    return true;
}
sMacAddr& cACTION_BACKHAUL_ENABLE::iface_mac() {
    return (sMacAddr&)(*m_iface_mac);
}

uint8_t& cACTION_BACKHAUL_ENABLE::iface_is_5ghz() {
    return (uint8_t&)(*m_iface_is_5ghz);
}

std::string cACTION_BACKHAUL_ENABLE::wire_iface_str() {
    char *wire_iface_ = wire_iface();
    if (!wire_iface_) { return std::string(); }
    return std::string(wire_iface_, m_wire_iface_idx__);
}

char* cACTION_BACKHAUL_ENABLE::wire_iface(size_t length) {
    if( (m_wire_iface_idx__ <= 0) || (m_wire_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "wire_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_wire_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_wire_iface(const std::string& str) { return set_wire_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_wire_iface(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_wire_iface received an empty string.";
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
    if( (m_sta_iface_idx__ <= 0) || (m_sta_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "sta_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_sta_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_sta_iface(const std::string& str) { return set_sta_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_sta_iface(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_sta_iface received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_sta_iface);
    return true;
}
std::string cACTION_BACKHAUL_ENABLE::ap_iface_str() {
    char *ap_iface_ = ap_iface();
    if (!ap_iface_) { return std::string(); }
    return std::string(ap_iface_, m_ap_iface_idx__);
}

char* cACTION_BACKHAUL_ENABLE::ap_iface(size_t length) {
    if( (m_ap_iface_idx__ <= 0) || (m_ap_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ap_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ap_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_ap_iface(const std::string& str) { return set_ap_iface(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_ap_iface(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_ap_iface received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_ap_iface);
    return true;
}
std::string cACTION_BACKHAUL_ENABLE::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* cACTION_BACKHAUL_ENABLE::ssid(size_t length) {
    if( (m_ssid_idx__ <= 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cACTION_BACKHAUL_ENABLE::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_ssid(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_ssid received an empty string.";
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
    if( (m_pass_idx__ <= 0) || (m_pass_idx__ < length) ) {
        TLVF_LOG(ERROR) << "pass length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_pass);
}

bool cACTION_BACKHAUL_ENABLE::set_pass(const std::string& str) { return set_pass(str.c_str(), str.size()); }
bool cACTION_BACKHAUL_ENABLE::set_pass(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_pass received an empty string.";
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

void cACTION_BACKHAUL_ENABLE::class_swap()
{
    m_iface_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_security_type));
    m_preferred_bssid->struct_swap();
}

size_t cACTION_BACKHAUL_ENABLE::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // bridge_iface
    class_size += sizeof(sMacAddr); // iface_mac
    class_size += sizeof(uint8_t); // iface_is_5ghz
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // wire_iface
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // sta_iface
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // ap_iface
    class_size += beerocks::message::WIFI_SSID_MAX_LENGTH * sizeof(char); // ssid
    class_size += beerocks::message::WIFI_PASS_MAX_LENGTH * sizeof(char); // pass
    class_size += sizeof(uint32_t); // security_type
    class_size += sizeof(sMacAddr); // preferred_bssid
    class_size += sizeof(uint8_t); // wire_iface_type
    class_size += sizeof(uint8_t); // wireless_iface_type
    class_size += sizeof(uint8_t); // mem_only_psk
    class_size += sizeof(uint8_t); // backhaul_preferred_radio_band
    return class_size;
}

bool cACTION_BACKHAUL_ENABLE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_bridge_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_bridge_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_iface_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_iface_mac->struct_init(); }
    m_iface_is_5ghz = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_wire_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_wire_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_sta_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_sta_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_ap_iface = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_ap_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_ssid = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::WIFI_SSID_MAX_LENGTH))) { return false; }
    m_ssid_idx__  = beerocks::message::WIFI_SSID_MAX_LENGTH;
    m_pass = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::WIFI_PASS_MAX_LENGTH))) { return false; }
    m_pass_idx__  = beerocks::message::WIFI_PASS_MAX_LENGTH;
    m_security_type = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    m_preferred_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_preferred_bssid->struct_init(); }
    m_wire_iface_type = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_wireless_iface_type = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_mem_only_psk = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_backhaul_preferred_radio_band = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CONNECTED_NOTIFICATION::cACTION_BACKHAUL_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CONNECTED_NOTIFICATION::cACTION_BACKHAUL_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CONNECTED_NOTIFICATION::~cACTION_BACKHAUL_CONNECTED_NOTIFICATION() {
}
sBackhaulParams& cACTION_BACKHAUL_CONNECTED_NOTIFICATION::params() {
    return (sBackhaulParams&)(*m_params);
}

void cACTION_BACKHAUL_CONNECTED_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BACKHAUL_CONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulParams); // params
    return class_size;
}

bool cACTION_BACKHAUL_CONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBackhaulParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sBackhaulParams))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::~cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION() {
}
uint8_t& cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::stopped() {
    return (uint8_t&)(*m_stopped);
}

void cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::class_swap()
{
}

size_t cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // stopped
    return class_size;
}

bool cACTION_BACKHAUL_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_stopped = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ENABLE_APS_REQUEST::cACTION_BACKHAUL_ENABLE_APS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ENABLE_APS_REQUEST::cACTION_BACKHAUL_ENABLE_APS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_bandwidth));
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
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_bandwidth = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    m_center_channel = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ROAM_REQUEST::cACTION_BACKHAUL_ROAM_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_REQUEST::cACTION_BACKHAUL_ROAM_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_REQUEST::~cACTION_BACKHAUL_ROAM_REQUEST() {
}
sBackhaulRoam& cACTION_BACKHAUL_ROAM_REQUEST::params() {
    return (sBackhaulRoam&)(*m_params);
}

void cACTION_BACKHAUL_ROAM_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BACKHAUL_ROAM_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulRoam); // params
    return class_size;
}

bool cACTION_BACKHAUL_ROAM_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBackhaulRoam*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sBackhaulRoam))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_ROAM_RESPONSE::cACTION_BACKHAUL_ROAM_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_RESPONSE::cACTION_BACKHAUL_ROAM_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_ROAM_RESPONSE::~cACTION_BACKHAUL_ROAM_RESPONSE() {
}
uint8_t& cACTION_BACKHAUL_ROAM_RESPONSE::connected() {
    return (uint8_t&)(*m_connected);
}

void cACTION_BACKHAUL_ROAM_RESPONSE::class_swap()
{
}

size_t cACTION_BACKHAUL_ROAM_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // connected
    return class_size;
}

bool cACTION_BACKHAUL_ROAM_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_connected = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_RESET::cACTION_BACKHAUL_RESET(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_RESET::cACTION_BACKHAUL_RESET(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_RESET::~cACTION_BACKHAUL_RESET() {
}
void cACTION_BACKHAUL_RESET::class_swap()
{
}

size_t cACTION_BACKHAUL_RESET::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BACKHAUL_RESET::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_4ADDR_CONNECTED::cACTION_BACKHAUL_4ADDR_CONNECTED(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_4ADDR_CONNECTED::cACTION_BACKHAUL_4ADDR_CONNECTED(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_4ADDR_CONNECTED::~cACTION_BACKHAUL_4ADDR_CONNECTED() {
}
sMacAddr& cACTION_BACKHAUL_4ADDR_CONNECTED::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_BACKHAUL_4ADDR_CONNECTED::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_BACKHAUL_4ADDR_CONNECTED::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_BACKHAUL_4ADDR_CONNECTED::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::~cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION() {
}
sBackhaulRssi& cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::params() {
    return (sBackhaulRssi&)(*m_params);
}

void cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulRssi); // params
    return class_size;
}

bool cACTION_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBackhaulRssi*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sBackhaulRssi))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::~cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST() {
}
uint32_t& cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::attempts() {
    return (uint32_t&)(*m_attempts);
}

void cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_attempts));
}

size_t cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // attempts
    return class_size;
}

bool cACTION_BACKHAUL_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_attempts = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST() {
}
sNodeRssiMeasurementRequest& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params() {
    return (sNodeRssiMeasurementRequest&)(*m_params);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurementRequest*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurementRequest))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE() {
}
sNodeRssiMeasurement& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurement*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::~cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE() {
}
sMacAddr& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


