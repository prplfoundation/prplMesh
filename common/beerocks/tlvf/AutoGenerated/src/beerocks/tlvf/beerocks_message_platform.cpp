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

#include <beerocks/tlvf/beerocks_message_platform.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::~cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION() {
}
uint8_t& cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::is_backhaul_manager() {
    return (uint8_t&)(*m_is_backhaul_manager);
}

void cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::class_swap()
{
}

size_t cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // is_backhaul_manager
    return class_size;
}

bool cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::init()
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

cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::~cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST() {
}
std::string cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
void cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    return class_size;
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::~cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE() {
}
sPlatformSettings& cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::platform_settings() {
    return (sPlatformSettings&)(*m_platform_settings);
}

sWlanSettings& cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::wlan_settings() {
    return (sWlanSettings&)(*m_wlan_settings);
}

uint32_t& cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::valid() {
    return (uint32_t&)(*m_valid);
}

void cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::class_swap()
{
    m_platform_settings->struct_swap();
    m_wlan_settings->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_valid));
}

size_t cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sPlatformSettings); // platform_settings
    class_size += sizeof(sWlanSettings); // wlan_settings
    class_size += sizeof(uint32_t); // valid
    return class_size;
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_platform_settings = (sPlatformSettings*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sPlatformSettings) * 1;
    if (!m_parse__) { m_platform_settings->struct_init(); }
    m_wlan_settings = (sWlanSettings*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWlanSettings) * 1;
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    m_valid = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::~cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST() {
}
std::string cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
void cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    return class_size;
}

bool cACTION_PLATFORM_POST_INIT_CONFIG_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::~cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE() {
}
uint8_t& cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::result() {
    return (uint8_t&)(*m_result);
}

void cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result
    return class_size;
}

bool cACTION_PLATFORM_POST_INIT_CONFIG_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::~cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION() {
}
sArpMonitorData& cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sArpMonitorData*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sArpMonitorData) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::~cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION() {
}
sWlanSettings& cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::wlan_settings() {
    return (sWlanSettings&)(*m_wlan_settings);
}

void cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::class_swap()
{
    m_wlan_settings->struct_swap();
}

size_t cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sWlanSettings); // wlan_settings
    return class_size;
}

bool cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_wlan_settings = (sWlanSettings*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWlanSettings) * 1;
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::~cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION() {
}
eDHCPOp& cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::dhcp_op() {
    return (eDHCPOp&)(*m_dhcp_op);
}

uint32_t& cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::op() {
    return (uint32_t&)(*m_op);
}

sMacAddr& cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

beerocks::net::sIpv4Addr& cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_ipv4);
}

std::string cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::hostname_str() {
    char *hostname_ = hostname();
    if (!hostname_) { return std::string(); }
    return std::string(hostname_, m_hostname_idx__);
}

char* cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::hostname(size_t length) {
    if( (m_hostname_idx__ <= 0) || (m_hostname_idx__ < length) ) {
        TLVF_LOG(ERROR) << "hostname length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_hostname);
}

bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::set_hostname(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_hostname received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::NODE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_hostname, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::set_hostname(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_hostname received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::NODE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_hostname, str, size + 1);
    m_hostname[size] = '\0';
    return true;
}
void cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_op));
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

size_t cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(eDHCPOp); // dhcp_op
    class_size += sizeof(uint32_t); // op
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    class_size += beerocks::message::NODE_NAME_LENGTH * sizeof(char); // hostname
    return class_size;
}

bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_dhcp_op = (eDHCPOp*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(eDHCPOp) * 1;
    m_op = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sIpv4Addr) * 1;
    if (!m_parse__) { m_ipv4->struct_init(); }
    m_hostname = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::NODE_NAME_LENGTH);
    m_hostname_idx__  = beerocks::message::NODE_NAME_LENGTH;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::~cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL() {
}
sLoggingLevelChange& cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::params() {
    return (sLoggingLevelChange&)(*m_params);
}

void cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLoggingLevelChange); // params
    return class_size;
}

bool cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sLoggingLevelChange*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sLoggingLevelChange) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ARP_QUERY_REQUEST::cACTION_PLATFORM_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_REQUEST::cACTION_PLATFORM_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_REQUEST::~cACTION_PLATFORM_ARP_QUERY_REQUEST() {
}
sArpQuery& cACTION_PLATFORM_ARP_QUERY_REQUEST::params() {
    return (sArpQuery&)(*m_params);
}

void cACTION_PLATFORM_ARP_QUERY_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_ARP_QUERY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpQuery); // params
    return class_size;
}

bool cACTION_PLATFORM_ARP_QUERY_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sArpQuery*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sArpQuery) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ARP_QUERY_RESPONSE::cACTION_PLATFORM_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_RESPONSE::cACTION_PLATFORM_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_RESPONSE::~cACTION_PLATFORM_ARP_QUERY_RESPONSE() {
}
sArpMonitorData& cACTION_PLATFORM_ARP_QUERY_RESPONSE::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_PLATFORM_ARP_QUERY_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_ARP_QUERY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_PLATFORM_ARP_QUERY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sArpMonitorData*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sArpMonitorData) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::~cACTION_PLATFORM_ONBOARD_QUERY_REQUEST() {
}
void cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::init()
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

cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::~cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE() {
}
sOnboarding& cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::params() {
    return (sOnboarding&)(*m_params);
}

void cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sOnboarding); // params
    return class_size;
}

bool cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sOnboarding*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sOnboarding) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ONBOARD_SET_REQUEST::cACTION_PLATFORM_ONBOARD_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_SET_REQUEST::cACTION_PLATFORM_ONBOARD_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_SET_REQUEST::~cACTION_PLATFORM_ONBOARD_SET_REQUEST() {
}
sOnboarding& cACTION_PLATFORM_ONBOARD_SET_REQUEST::params() {
    return (sOnboarding&)(*m_params);
}

void cACTION_PLATFORM_ONBOARD_SET_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_ONBOARD_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sOnboarding); // params
    return class_size;
}

bool cACTION_PLATFORM_ONBOARD_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sOnboarding*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sOnboarding) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::~cACTION_PLATFORM_WPS_ONBOARDING_REQUEST() {
}
std::string cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
void cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    return class_size;
}

bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::~cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST() {
}
sWifiCredentials& cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::params() {
    return (sWifiCredentials&)(*m_params);
}

void cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sWifiCredentials); // params
    return class_size;
}

bool cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sWifiCredentials*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWifiCredentials) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::~cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE() {
}
uint32_t& cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_BEEROCKS_CREDENTIALS_UPDATE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::~cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST() {
}
std::string cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
std::string cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::ssid(size_t length) {
    if( (m_ssid_idx__ <= 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_ssid(const std::string& str) {
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
bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_ssid(const char str[], size_t size) {
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
std::string cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::pass_str() {
    char *pass_ = pass();
    if (!pass_) { return std::string(); }
    return std::string(pass_, m_pass_idx__);
}

char* cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::pass(size_t length) {
    if( (m_pass_idx__ <= 0) || (m_pass_idx__ < length) ) {
        TLVF_LOG(ERROR) << "pass length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_pass);
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_pass(const std::string& str) {
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
bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_pass(const char str[], size_t size) {
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
std::string cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::security_type_str() {
    char *security_type_ = security_type();
    if (!security_type_) { return std::string(); }
    return std::string(security_type_, m_security_type_idx__);
}

char* cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::security_type(size_t length) {
    if( (m_security_type_idx__ <= 0) || (m_security_type_idx__ < length) ) {
        TLVF_LOG(ERROR) << "security_type length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_security_type);
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_security_type(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_security_type received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_security_type, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::set_security_type(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_security_type received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_security_type, str, size + 1);
    m_security_type[size] = '\0';
    return true;
}
void cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    class_size += beerocks::message::WIFI_SSID_MAX_LENGTH * sizeof(char); // ssid
    class_size += beerocks::message::WIFI_PASS_MAX_LENGTH * sizeof(char); // pass
    class_size += beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH * sizeof(char); // security_type
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_ssid = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::WIFI_SSID_MAX_LENGTH);
    m_ssid_idx__  = beerocks::message::WIFI_SSID_MAX_LENGTH;
    m_pass = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::WIFI_PASS_MAX_LENGTH);
    m_pass_idx__  = beerocks::message::WIFI_PASS_MAX_LENGTH;
    m_security_type = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH);
    m_security_type_idx__  = beerocks::message::WIFI_SECURITY_TYPE_MAX_LENGTH;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::~cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE() {
}
std::string cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
uint8_t& cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_success = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::~cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST() {
}
uint32_t& cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::flag() {
    return (uint32_t&)(*m_flag);
}

void cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_flag));
}

size_t cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // flag
    return class_size;
}

bool cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_flag = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::~cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE() {
}
uint32_t& cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_ADVERTISE_SSID_FLAG_UPDATE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST::cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST::cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST::~cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST() {
}
void cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_GET_WLAN_READY_STATUS_REQUEST::init()
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

cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::~cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE() {
}
uint8_t& cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::result() {
    return (uint8_t&)(*m_result);
}

void cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result
    return class_size;
}

bool cACTION_PLATFORM_GET_WLAN_READY_STATUS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST() {
}
uint8_t& cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::vap_id() {
    return (uint8_t&)(*m_vap_id);
}

void cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // vap_id
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE() {
}
sWifiCredentials& cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::front_params() {
    return (sWifiCredentials&)(*m_front_params);
}

sWifiCredentials& cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::back_params() {
    return (sWifiCredentials&)(*m_back_params);
}

uint32_t& cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::class_swap()
{
    m_front_params->struct_swap();
    m_back_params->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sWifiCredentials); // front_params
    class_size += sizeof(sWifiCredentials); // back_params
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_front_params = (sWifiCredentials*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWifiCredentials) * 1;
    if (!m_parse__) { m_front_params->struct_init(); }
    m_back_params = (sWifiCredentials*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWifiCredentials) * 1;
    if (!m_parse__) { m_back_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST() {
}
void cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::init()
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

cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE() {
}
sAdminCredentials& cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::params() {
    return (sAdminCredentials&)(*m_params);
}

uint32_t& cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::class_swap()
{
    m_params->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sAdminCredentials); // params
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sAdminCredentials*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sAdminCredentials) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::~cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST() {
}
void cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::init()
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

cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::~cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE() {
}
sDeviceInfo& cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::params() {
    return (sDeviceInfo&)(*m_params);
}

uint32_t& cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::class_swap()
{
    m_params->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDeviceInfo); // params
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDeviceInfo*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sDeviceInfo) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::~cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST() {
}
void cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::init()
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

cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::~cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE() {
}
uint8_t& cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::local_master() {
    return (uint8_t&)(*m_local_master);
}

void cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // local_master
    return class_size;
}

bool cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_local_master = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::~cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST() {
}
std::string cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
int8_t& cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::iface_operation() {
    return (int8_t&)(*m_iface_operation);
}

void cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    class_size += sizeof(int8_t); // iface_operation
    return class_size;
}

bool cACTION_PLATFORM_WIFI_SET_IFACE_STATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_iface_operation = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::~cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE() {
}
std::string cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
int8_t& cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::iface_operation() {
    return (int8_t&)(*m_iface_operation);
}

uint8_t& cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    class_size += sizeof(int8_t); // iface_operation
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_PLATFORM_WIFI_SET_IFACE_STATE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_iface_operation = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    m_success = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::~cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST() {
}
std::string cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
uint8_t& cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::enable() {
    return (uint8_t&)(*m_enable);
}

void cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    class_size += sizeof(uint8_t); // enable
    return class_size;
}

bool cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_enable = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::~cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE() {
}
std::string cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::iface_name(size_t length) {
    if( (m_iface_name_idx__ <= 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::set_iface_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name, str, size + 1);
    m_iface_name[size] = '\0';
    return true;
}
uint8_t& cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::enable() {
    return (uint8_t&)(*m_enable);
}

uint8_t& cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    class_size += sizeof(uint8_t); // enable
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_PLATFORM_WIFI_SET_RADIO_TX_STATE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_enable = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_success = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::~cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION() {
}
sVersions& cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::versions() {
    return (sVersions&)(*m_versions);
}

void cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::class_swap()
{
    m_versions->struct_swap();
}

size_t cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    return class_size;
}

bool cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_versions = (sVersions*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sVersions) * 1;
    if (!m_parse__) { m_versions->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::~cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION() {
}
sVersions& cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::versions() {
    return (sVersions&)(*m_versions);
}

void cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::class_swap()
{
    m_versions->struct_swap();
}

size_t cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    return class_size;
}

bool cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_versions = (sVersions*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sVersions) * 1;
    if (!m_parse__) { m_versions->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST() {
}
void cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::init()
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

cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE() {
}
sVersions& cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::versions() {
    return (sVersions&)(*m_versions);
}

uint32_t& cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::class_swap()
{
    m_versions->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_versions = (sVersions*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sVersions) * 1;
    if (!m_parse__) { m_versions->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ERROR_NOTIFICATION::cACTION_PLATFORM_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ERROR_NOTIFICATION::cACTION_PLATFORM_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ERROR_NOTIFICATION::~cACTION_PLATFORM_ERROR_NOTIFICATION() {
}
uint32_t& cACTION_PLATFORM_ERROR_NOTIFICATION::code() {
    return (uint32_t&)(*m_code);
}

std::string cACTION_PLATFORM_ERROR_NOTIFICATION::data_str() {
    char *data_ = data();
    if (!data_) { return std::string(); }
    return std::string(data_, m_data_idx__);
}

char* cACTION_PLATFORM_ERROR_NOTIFICATION::data(size_t length) {
    if( (m_data_idx__ <= 0) || (m_data_idx__ < length) ) {
        TLVF_LOG(ERROR) << "data length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_data);
}

bool cACTION_PLATFORM_ERROR_NOTIFICATION::set_data(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_data received an empty string.";
        return false;
    }
    if (str_size + 1 > 256) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_data, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_ERROR_NOTIFICATION::set_data(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_data received an empty string.";
        return false;
    }
    if (size + 1 > 256) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_data, str, size + 1);
    m_data[size] = '\0';
    return true;
}
void cACTION_PLATFORM_ERROR_NOTIFICATION::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_code));
}

size_t cACTION_PLATFORM_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // code
    class_size += 256 * sizeof(char); // data
    return class_size;
}

bool cACTION_PLATFORM_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_code = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_data = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * 256);
    m_data_idx__  = 256;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::~cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION() {
}
std::string cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::iface_name_ap_str() {
    char *iface_name_ap_ = iface_name_ap();
    if (!iface_name_ap_) { return std::string(); }
    return std::string(iface_name_ap_, m_iface_name_ap_idx__);
}

char* cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::iface_name_ap(size_t length) {
    if( (m_iface_name_ap_idx__ <= 0) || (m_iface_name_ap_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name_ap length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name_ap);
}

bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_ap(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name_ap received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name_ap, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_ap(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name_ap received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name_ap, str, size + 1);
    m_iface_name_ap[size] = '\0';
    return true;
}
std::string cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::iface_name_bh_str() {
    char *iface_name_bh_ = iface_name_bh();
    if (!iface_name_bh_) { return std::string(); }
    return std::string(iface_name_bh_, m_iface_name_bh_idx__);
}

char* cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::iface_name_bh(size_t length) {
    if( (m_iface_name_bh_idx__ <= 0) || (m_iface_name_bh_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name_bh length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name_bh);
}

bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_bh(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name_bh received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name_bh, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_bh(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_iface_name_bh received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::IFACE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_iface_name_bh, str, size + 1);
    m_iface_name_bh[size] = '\0';
    return true;
}
uint8_t& cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::status_ap() {
    return (uint8_t&)(*m_status_ap);
}

uint8_t& cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::status_bh() {
    return (uint8_t&)(*m_status_bh);
}

uint8_t& cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::status_bh_wired() {
    return (uint8_t&)(*m_status_bh_wired);
}

uint8_t& cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::is_bh_manager() {
    return (uint8_t&)(*m_is_bh_manager);
}

uint8_t& cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::status_operational() {
    return (uint8_t&)(*m_status_operational);
}

void cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name_ap
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name_bh
    class_size += sizeof(uint8_t); // status_ap
    class_size += sizeof(uint8_t); // status_bh
    class_size += sizeof(uint8_t); // status_bh_wired
    class_size += sizeof(uint8_t); // is_bh_manager
    class_size += sizeof(uint8_t); // status_operational
    return class_size;
}

bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name_ap = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_ap_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_iface_name_bh = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::IFACE_NAME_LENGTH);
    m_iface_name_bh_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_status_ap = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_status_bh = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_status_bh_wired = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_is_bh_manager = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_status_operational = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::~cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST() {
}
uint8_t& cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::config_start() {
    return (uint8_t&)(*m_config_start);
}

void cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // config_start
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_config_start = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE::cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE::cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE::~cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE() {
}
void cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE::class_swap()
{
}

size_t cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CONFIGURATION_UPDATE_RESPONSE::init()
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

cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::cACTION_PLATFORM_OPERATIONAL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::cACTION_PLATFORM_OPERATIONAL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::~cACTION_PLATFORM_OPERATIONAL_NOTIFICATION() {
}
uint8_t& cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::operational() {
    return (uint8_t&)(*m_operational);
}

void cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::class_swap()
{
}

size_t cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // operational
    return class_size;
}

bool cACTION_PLATFORM_OPERATIONAL_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_operational = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


