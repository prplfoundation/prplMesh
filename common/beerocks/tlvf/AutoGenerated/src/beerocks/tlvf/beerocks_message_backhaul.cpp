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
char* cACTION_BACKHAUL_REGISTER_REQUEST::sta_iface(size_t length) {
    if( (m_sta_iface_idx__ <= 0) || (m_sta_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "sta_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_sta_iface);
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(std::string& str) {
    return set_sta_iface(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_sta_iface received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_sta_iface, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_sta_iface(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_sta_iface received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_sta_iface, str, size + 1);
    m_sta_iface[size] = '\0';
    return true;
}
char* cACTION_BACKHAUL_REGISTER_REQUEST::hostap_iface(size_t length) {
    if( (m_hostap_iface_idx__ <= 0) || (m_hostap_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "hostap_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_hostap_iface);
}

bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(std::string& str) {
    return set_hostap_iface(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_hostap_iface received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_hostap_iface, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_REGISTER_REQUEST::set_hostap_iface(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_hostap_iface received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_hostap_iface, str, size + 1);
    m_hostap_iface[size] = '\0';
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
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_sta_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_hostap_iface = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_hostap_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_local_master = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_local_gw = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_sta_iface_filter_low = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_onboarding = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
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
char* cACTION_BACKHAUL_ENABLE::bridge_iface(size_t length) {
    if( (m_bridge_iface_idx__ <= 0) || (m_bridge_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "bridge_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_bridge_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_bridge_iface(std::string& str) {
    return set_bridge_iface(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_ENABLE::set_bridge_iface(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_bridge_iface received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_bridge_iface, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_ENABLE::set_bridge_iface(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_bridge_iface received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_bridge_iface, str, size + 1);
    m_bridge_iface[size] = '\0';
    return true;
}
beerocks::net::sMacAddr& cACTION_BACKHAUL_ENABLE::iface_mac() {
    return (beerocks::net::sMacAddr&)(*m_iface_mac);
}

uint8_t& cACTION_BACKHAUL_ENABLE::iface_is_5ghz() {
    return (uint8_t&)(*m_iface_is_5ghz);
}

char* cACTION_BACKHAUL_ENABLE::wire_iface(size_t length) {
    if( (m_wire_iface_idx__ <= 0) || (m_wire_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "wire_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_wire_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_wire_iface(std::string& str) {
    return set_wire_iface(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_ENABLE::set_wire_iface(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_wire_iface received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_wire_iface, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_ENABLE::set_wire_iface(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_wire_iface received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_wire_iface, str, size + 1);
    m_wire_iface[size] = '\0';
    return true;
}
char* cACTION_BACKHAUL_ENABLE::sta_iface(size_t length) {
    if( (m_sta_iface_idx__ <= 0) || (m_sta_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "sta_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_sta_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_sta_iface(std::string& str) {
    return set_sta_iface(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_ENABLE::set_sta_iface(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_sta_iface received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_sta_iface, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_ENABLE::set_sta_iface(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_sta_iface received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_sta_iface, str, size + 1);
    m_sta_iface[size] = '\0';
    return true;
}
char* cACTION_BACKHAUL_ENABLE::ap_iface(size_t length) {
    if( (m_ap_iface_idx__ <= 0) || (m_ap_iface_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ap_iface length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ap_iface);
}

bool cACTION_BACKHAUL_ENABLE::set_ap_iface(std::string& str) {
    return set_ap_iface(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_ENABLE::set_ap_iface(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_ap_iface received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_ap_iface, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_ENABLE::set_ap_iface(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_ap_iface received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_ap_iface, str, size + 1);
    m_ap_iface[size] = '\0';
    return true;
}
char* cACTION_BACKHAUL_ENABLE::ssid(size_t length) {
    if( (m_ssid_idx__ <= 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cACTION_BACKHAUL_ENABLE::set_ssid(std::string& str) {
    return set_ssid(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_ENABLE::set_ssid(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_ssid received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::WIFI_SSID_MAX_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_ssid, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_ENABLE::set_ssid(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_ssid received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::WIFI_SSID_MAX_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_ssid, str, size + 1);
    m_ssid[size] = '\0';
    return true;
}
char* cACTION_BACKHAUL_ENABLE::pass(size_t length) {
    if( (m_pass_idx__ <= 0) || (m_pass_idx__ < length) ) {
        TLVF_LOG(ERROR) << "pass length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_pass);
}

bool cACTION_BACKHAUL_ENABLE::set_pass(std::string& str) {
    return set_pass(const_cast<std::string&>(str));
}
bool cACTION_BACKHAUL_ENABLE::set_pass(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_pass received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::WIFI_PASS_MAX_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_pass, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_BACKHAUL_ENABLE::set_pass(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_pass received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::WIFI_PASS_MAX_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_pass, str, size + 1);
    m_pass[size] = '\0';
    return true;
}
uint32_t& cACTION_BACKHAUL_ENABLE::security_type() {
    return (uint32_t&)(*m_security_type);
}

beerocks::net::sMacAddr& cACTION_BACKHAUL_ENABLE::preferred_bssid() {
    return (beerocks::net::sMacAddr&)(*m_preferred_bssid);
}

uint8_t& cACTION_BACKHAUL_ENABLE::wire_iface_type() {
    return (uint8_t&)(*m_wire_iface_type);
}

uint8_t& cACTION_BACKHAUL_ENABLE::wireless_iface_type() {
    return (uint8_t&)(*m_wireless_iface_type);
}

uint8_t& cACTION_BACKHAUL_ENABLE::wired_backhaul() {
    return (uint8_t&)(*m_wired_backhaul);
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
    class_size += sizeof(beerocks::net::sMacAddr); // iface_mac
    class_size += sizeof(uint8_t); // iface_is_5ghz
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // wire_iface
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // sta_iface
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // ap_iface
    class_size += beerocks::message::WIFI_SSID_MAX_LENGTH * sizeof(char); // ssid
    class_size += beerocks::message::WIFI_PASS_MAX_LENGTH * sizeof(char); // pass
    class_size += sizeof(uint32_t); // security_type
    class_size += sizeof(beerocks::net::sMacAddr); // preferred_bssid
    class_size += sizeof(uint8_t); // wire_iface_type
    class_size += sizeof(uint8_t); // wireless_iface_type
    class_size += sizeof(uint8_t); // wired_backhaul
    return class_size;
}

bool cACTION_BACKHAUL_ENABLE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_bridge_iface = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_bridge_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_iface_mac = (beerocks::net::sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) { m_iface_mac->struct_init(); }
    m_iface_is_5ghz = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_wire_iface = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_wire_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_sta_iface = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_sta_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_ap_iface = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_ap_iface_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_ssid = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::WIFI_SSID_MAX_LENGTH);
    m_ssid_idx__  = beerocks::message::WIFI_SSID_MAX_LENGTH;
    m_pass = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::WIFI_PASS_MAX_LENGTH);
    m_pass_idx__  = beerocks::message::WIFI_PASS_MAX_LENGTH;
    m_security_type = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_preferred_bssid = (beerocks::net::sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) { m_preferred_bssid->struct_init(); }
    m_wire_iface_type = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_wireless_iface_type = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_wired_backhaul = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(sBackhaulParams) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(sBackhaulRoam) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
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
beerocks::net::sMacAddr& cACTION_BACKHAUL_4ADDR_CONNECTED::mac() {
    return (beerocks::net::sMacAddr&)(*m_mac);
}

void cACTION_BACKHAUL_4ADDR_CONNECTED::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_BACKHAUL_4ADDR_CONNECTED::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_BACKHAUL_4ADDR_CONNECTED::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(sBackhaulRssi) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(sNodeRssiMeasurementRequest) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
    m_buff_ptr__ += sizeof(sNodeRssiMeasurement) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
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
beerocks::net::sMacAddr& cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (beerocks::net::sMacAddr&)(*m_mac);
}

void cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_BACKHAUL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


