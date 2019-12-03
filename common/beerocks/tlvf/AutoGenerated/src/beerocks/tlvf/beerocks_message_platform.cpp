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
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
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

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::set_iface_name(const std::string& str) { return set_iface_name(str.c_str(), str.size()); }
bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_iface_name);
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
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
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
    if (!buffPtrIncrementSafe(sizeof(sPlatformSettings))) { return false; }
    if (!m_parse__) { m_platform_settings->struct_init(); }
    m_wlan_settings = (sWlanSettings*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWlanSettings))) { return false; }
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    m_valid = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(sArpMonitorData))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sWlanSettings))) { return false; }
    if (!m_parse__) { m_wlan_settings->struct_init(); }
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

bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::set_hostname(const std::string& str) { return set_hostname(str.c_str(), str.size()); }
bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::set_hostname(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_hostname received an empty string.";
        return false;
    }
    if (size > beerocks::message::NODE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_hostname);
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
    if (!buffPtrIncrementSafe(sizeof(eDHCPOp))) { return false; }
    m_op = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::net::sIpv4Addr))) { return false; }
    if (!m_parse__) { m_ipv4->struct_init(); }
    m_hostname = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::NODE_NAME_LENGTH))) { return false; }
    m_hostname_idx__  = beerocks::message::NODE_NAME_LENGTH;
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
    if (!buffPtrIncrementSafe(sizeof(sLoggingLevelChange))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sArpQuery))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sArpMonitorData))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sOnboarding))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sOnboarding))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
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

bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::set_iface_name(const std::string& str) { return set_iface_name(str.c_str(), str.size()); }
bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_iface_name);
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
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
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
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(sWifiCredentials))) { return false; }
    if (!m_parse__) { m_front_params->struct_init(); }
    m_back_params = (sWifiCredentials*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWifiCredentials))) { return false; }
    if (!m_parse__) { m_back_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(sAdminCredentials))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(sDeviceInfo))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(sVersions))) { return false; }
    if (!m_parse__) { m_versions->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sVersions))) { return false; }
    if (!m_parse__) { m_versions->struct_init(); }
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
    if (!buffPtrIncrementSafe(sizeof(sVersions))) { return false; }
    if (!m_parse__) { m_versions->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
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

bool cACTION_PLATFORM_ERROR_NOTIFICATION::set_data(const std::string& str) { return set_data(str.c_str(), str.size()); }
bool cACTION_PLATFORM_ERROR_NOTIFICATION::set_data(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_data received an empty string.";
        return false;
    }
    if (size > 256) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_data);
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
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    m_data = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(256))) { return false; }
    m_data_idx__  = 256;
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

bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_ap(const std::string& str) { return set_iface_name_ap(str.c_str(), str.size()); }
bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_ap(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name_ap received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_iface_name_ap);
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

bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_bh(const std::string& str) { return set_iface_name_bh(str.c_str(), str.size()); }
bool cACTION_PLATFORM_WIFI_INTERFACE_STATUS_NOTIFICATION::set_iface_name_bh(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_iface_name_bh received an empty string.";
        return false;
    }
    if (size > beerocks::message::IFACE_NAME_LENGTH) {
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    std::copy(str, str + size, m_iface_name_bh);
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
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_iface_name_ap_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_iface_name_bh = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char)*(beerocks::message::IFACE_NAME_LENGTH))) { return false; }
    m_iface_name_bh_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    m_status_ap = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_status_bh = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_status_bh_wired = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_is_bh_manager = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_status_operational = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
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
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


