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

#include <beerocks/tlvf/beerocks_message_control.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::~cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION() {
}
char* cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::slave_version(size_t length) {
    if( (m_slave_version_idx__ <= 0) || (m_slave_version_idx__ < length) ) {
        TLVF_LOG(ERROR) << "slave_version length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_slave_version);
}

bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::set_slave_version(std::string& str) {
    return set_slave_version(const_cast<std::string&>(str));
}
bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::set_slave_version(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_slave_version received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::VERSION_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_slave_version, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::set_slave_version(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_slave_version received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::VERSION_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_slave_version, str, size + 1);
    m_slave_version[size] = '\0';
    return true;
}
sPlatformSettings& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::platform_settings() {
    return (sPlatformSettings&)(*m_platform_settings);
}

sWlanSettings& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::wlan_settings() {
    return (sWlanSettings&)(*m_wlan_settings);
}

sBackhaulParams& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::backhaul_params() {
    return (sBackhaulParams&)(*m_backhaul_params);
}

sNodeHostap& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::hostap() {
    return (sNodeHostap&)(*m_hostap);
}

sApChannelSwitch& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

uint8_t& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::platform() {
    return (uint8_t&)(*m_platform);
}

uint8_t& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::low_pass_filter_on() {
    return (uint8_t&)(*m_low_pass_filter_on);
}

uint8_t& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::enable_repeater_mode() {
    return (uint8_t&)(*m_enable_repeater_mode);
}

sMacAddr& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::radio_identifier() {
    return (sMacAddr&)(*m_radio_identifier);
}

uint8_t& cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::is_slave_reconf() {
    return (uint8_t&)(*m_is_slave_reconf);
}

void cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::class_swap()
{
    m_platform_settings->struct_swap();
    m_wlan_settings->struct_swap();
    m_backhaul_params->struct_swap();
    m_hostap->struct_swap();
    m_cs_params->struct_swap();
    m_radio_identifier->struct_swap();
}

size_t cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::VERSION_LENGTH * sizeof(char); // slave_version
    class_size += sizeof(sPlatformSettings); // platform_settings
    class_size += sizeof(sWlanSettings); // wlan_settings
    class_size += sizeof(sBackhaulParams); // backhaul_params
    class_size += sizeof(sNodeHostap); // hostap
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(uint8_t); // platform
    class_size += sizeof(uint8_t); // low_pass_filter_on
    class_size += sizeof(uint8_t); // enable_repeater_mode
    class_size += sizeof(sMacAddr); // radio_identifier
    class_size += sizeof(uint8_t); // is_slave_reconf
    return class_size;
}

bool cACTION_CONTROL_SLAVE_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_slave_version = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::VERSION_LENGTH);
    m_slave_version_idx__  = beerocks::message::VERSION_LENGTH;
    m_platform_settings = (sPlatformSettings*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sPlatformSettings) * 1;
    if (!m_parse__) { m_platform_settings->struct_init(); }
    m_wlan_settings = (sWlanSettings*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sWlanSettings) * 1;
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    m_backhaul_params = (sBackhaulParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sBackhaulParams) * 1;
    if (!m_parse__) { m_backhaul_params->struct_init(); }
    m_hostap = (sNodeHostap*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeHostap) * 1;
    if (!m_parse__) { m_hostap->struct_init(); }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_platform = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_low_pass_filter_on = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_enable_repeater_mode = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_radio_identifier = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_radio_identifier->struct_init(); }
    m_is_slave_reconf = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_SLAVE_JOINED_RESPONSE::cACTION_CONTROL_SLAVE_JOINED_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_RESPONSE::cACTION_CONTROL_SLAVE_JOINED_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_RESPONSE::~cACTION_CONTROL_SLAVE_JOINED_RESPONSE() {
}
char* cACTION_CONTROL_SLAVE_JOINED_RESPONSE::master_version(size_t length) {
    if( (m_master_version_idx__ <= 0) || (m_master_version_idx__ < length) ) {
        TLVF_LOG(ERROR) << "master_version length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_master_version);
}

bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::set_master_version(std::string& str) {
    return set_master_version(const_cast<std::string&>(str));
}
bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::set_master_version(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_master_version received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::VERSION_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_master_version, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::set_master_version(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_master_version received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::VERSION_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_master_version, str, size + 1);
    m_master_version[size] = '\0';
    return true;
}
uint8_t& cACTION_CONTROL_SLAVE_JOINED_RESPONSE::err_code() {
    return (uint8_t&)(*m_err_code);
}

sSonConfig& cACTION_CONTROL_SLAVE_JOINED_RESPONSE::config() {
    return (sSonConfig&)(*m_config);
}

void cACTION_CONTROL_SLAVE_JOINED_RESPONSE::class_swap()
{
    m_config->struct_swap();
}

size_t cACTION_CONTROL_SLAVE_JOINED_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::VERSION_LENGTH * sizeof(char); // master_version
    class_size += sizeof(uint8_t); // err_code
    class_size += sizeof(sSonConfig); // config
    return class_size;
}

bool cACTION_CONTROL_SLAVE_JOINED_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_master_version = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::VERSION_LENGTH);
    m_master_version_idx__  = beerocks::message::VERSION_LENGTH;
    m_err_code = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_config = (sSonConfig*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSonConfig) * 1;
    if (!m_parse__) { m_config->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::~cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::backhaul_iface_mac() {
    return (sMacAddr&)(*m_backhaul_iface_mac);
}

beerocks::net::sIpv4Addr& cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::backhaul_ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_backhaul_ipv4);
}

sMacAddr& cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::bridge_iface_mac() {
    return (sMacAddr&)(*m_bridge_iface_mac);
}

beerocks::net::sIpv4Addr& cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::bridge_ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_bridge_ipv4);
}

sNodeHostap& cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::hostap() {
    return (sNodeHostap&)(*m_hostap);
}

void cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::class_swap()
{
    m_backhaul_iface_mac->struct_swap();
    m_backhaul_ipv4->struct_swap();
    m_bridge_iface_mac->struct_swap();
    m_bridge_ipv4->struct_swap();
    m_hostap->struct_swap();
}

size_t cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // backhaul_iface_mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // backhaul_ipv4
    class_size += sizeof(sMacAddr); // bridge_iface_mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // bridge_ipv4
    class_size += sizeof(sNodeHostap); // hostap
    return class_size;
}

bool cACTION_CONTROL_SLAVE_JOINED_4ADDR_MODE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_backhaul_iface_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_backhaul_iface_mac->struct_init(); }
    m_backhaul_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sIpv4Addr) * 1;
    if (!m_parse__) { m_backhaul_ipv4->struct_init(); }
    m_bridge_iface_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bridge_iface_mac->struct_init(); }
    m_bridge_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sIpv4Addr) * 1;
    if (!m_parse__) { m_bridge_ipv4->struct_init(); }
    m_hostap = (sNodeHostap*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeHostap) * 1;
    if (!m_parse__) { m_hostap->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_SON_CONFIG_UPDATE::cACTION_CONTROL_SON_CONFIG_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_SON_CONFIG_UPDATE::cACTION_CONTROL_SON_CONFIG_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_SON_CONFIG_UPDATE::~cACTION_CONTROL_SON_CONFIG_UPDATE() {
}
sSonConfig& cACTION_CONTROL_SON_CONFIG_UPDATE::config() {
    return (sSonConfig&)(*m_config);
}

void cACTION_CONTROL_SON_CONFIG_UPDATE::class_swap()
{
    m_config->struct_swap();
}

size_t cACTION_CONTROL_SON_CONFIG_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSonConfig); // config
    return class_size;
}

bool cACTION_CONTROL_SON_CONFIG_UPDATE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_config = (sSonConfig*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSonConfig) * 1;
    if (!m_parse__) { m_config->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CONTROLLER_PING_REQUEST::cACTION_CONTROL_CONTROLLER_PING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CONTROLLER_PING_REQUEST::cACTION_CONTROL_CONTROLLER_PING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CONTROLLER_PING_REQUEST::~cACTION_CONTROL_CONTROLLER_PING_REQUEST() {
}
uint16_t& cACTION_CONTROL_CONTROLLER_PING_REQUEST::total() {
    return (uint16_t&)(*m_total);
}

uint16_t& cACTION_CONTROL_CONTROLLER_PING_REQUEST::seq() {
    return (uint16_t&)(*m_seq);
}

uint16_t& cACTION_CONTROL_CONTROLLER_PING_REQUEST::size() {
    return (uint16_t&)(*m_size);
}

std::tuple<bool, uint8_t&> cACTION_CONTROL_CONTROLLER_PING_REQUEST::data(size_t idx) {
    bool ret_success = ( (m_data_idx__ > 0) && (m_data_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_data[ret_idx]);
}

bool cACTION_CONTROL_CONTROLLER_PING_REQUEST::alloc_data(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_data_idx__ += count;
    m_buff_ptr__ += len;
    return true;
}

void cACTION_CONTROL_CONTROLLER_PING_REQUEST::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_total));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_seq));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
}

size_t cACTION_CONTROL_CONTROLLER_PING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // total
    class_size += sizeof(uint16_t); // seq
    class_size += sizeof(uint16_t); // size
    return class_size;
}

bool cACTION_CONTROL_CONTROLLER_PING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_total = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_seq = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_data = (uint8_t*)m_buff_ptr__;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CONTROLLER_PING_RESPONSE::cACTION_CONTROL_CONTROLLER_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CONTROLLER_PING_RESPONSE::cACTION_CONTROL_CONTROLLER_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CONTROLLER_PING_RESPONSE::~cACTION_CONTROL_CONTROLLER_PING_RESPONSE() {
}
uint16_t& cACTION_CONTROL_CONTROLLER_PING_RESPONSE::total() {
    return (uint16_t&)(*m_total);
}

uint16_t& cACTION_CONTROL_CONTROLLER_PING_RESPONSE::seq() {
    return (uint16_t&)(*m_seq);
}

uint16_t& cACTION_CONTROL_CONTROLLER_PING_RESPONSE::size() {
    return (uint16_t&)(*m_size);
}

std::tuple<bool, uint8_t&> cACTION_CONTROL_CONTROLLER_PING_RESPONSE::data(size_t idx) {
    bool ret_success = ( (m_data_idx__ > 0) && (m_data_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_data[ret_idx]);
}

bool cACTION_CONTROL_CONTROLLER_PING_RESPONSE::alloc_data(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_data_idx__ += count;
    m_buff_ptr__ += len;
    return true;
}

void cACTION_CONTROL_CONTROLLER_PING_RESPONSE::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_total));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_seq));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
}

size_t cACTION_CONTROL_CONTROLLER_PING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // total
    class_size += sizeof(uint16_t); // seq
    class_size += sizeof(uint16_t); // size
    return class_size;
}

bool cACTION_CONTROL_CONTROLLER_PING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_total = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_seq = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_data = (uint8_t*)m_buff_ptr__;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_AGENT_PING_REQUEST::cACTION_CONTROL_AGENT_PING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_AGENT_PING_REQUEST::cACTION_CONTROL_AGENT_PING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_AGENT_PING_REQUEST::~cACTION_CONTROL_AGENT_PING_REQUEST() {
}
uint16_t& cACTION_CONTROL_AGENT_PING_REQUEST::total() {
    return (uint16_t&)(*m_total);
}

uint16_t& cACTION_CONTROL_AGENT_PING_REQUEST::seq() {
    return (uint16_t&)(*m_seq);
}

uint16_t& cACTION_CONTROL_AGENT_PING_REQUEST::size() {
    return (uint16_t&)(*m_size);
}

std::tuple<bool, uint8_t&> cACTION_CONTROL_AGENT_PING_REQUEST::data(size_t idx) {
    bool ret_success = ( (m_data_idx__ > 0) && (m_data_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_data[ret_idx]);
}

bool cACTION_CONTROL_AGENT_PING_REQUEST::alloc_data(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_data_idx__ += count;
    m_buff_ptr__ += len;
    return true;
}

void cACTION_CONTROL_AGENT_PING_REQUEST::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_total));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_seq));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
}

size_t cACTION_CONTROL_AGENT_PING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // total
    class_size += sizeof(uint16_t); // seq
    class_size += sizeof(uint16_t); // size
    return class_size;
}

bool cACTION_CONTROL_AGENT_PING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_total = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_seq = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_data = (uint8_t*)m_buff_ptr__;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_AGENT_PING_RESPONSE::cACTION_CONTROL_AGENT_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_AGENT_PING_RESPONSE::cACTION_CONTROL_AGENT_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_AGENT_PING_RESPONSE::~cACTION_CONTROL_AGENT_PING_RESPONSE() {
}
uint16_t& cACTION_CONTROL_AGENT_PING_RESPONSE::total() {
    return (uint16_t&)(*m_total);
}

uint16_t& cACTION_CONTROL_AGENT_PING_RESPONSE::seq() {
    return (uint16_t&)(*m_seq);
}

uint16_t& cACTION_CONTROL_AGENT_PING_RESPONSE::size() {
    return (uint16_t&)(*m_size);
}

std::tuple<bool, uint8_t&> cACTION_CONTROL_AGENT_PING_RESPONSE::data(size_t idx) {
    bool ret_success = ( (m_data_idx__ > 0) && (m_data_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_data[ret_idx]);
}

bool cACTION_CONTROL_AGENT_PING_RESPONSE::alloc_data(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_data_idx__ += count;
    m_buff_ptr__ += len;
    return true;
}

void cACTION_CONTROL_AGENT_PING_RESPONSE::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_total));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_seq));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
}

size_t cACTION_CONTROL_AGENT_PING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // total
    class_size += sizeof(uint16_t); // seq
    class_size += sizeof(uint16_t); // size
    return class_size;
}

bool cACTION_CONTROL_AGENT_PING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_total = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_seq = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_data = (uint8_t*)m_buff_ptr__;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_ARP_QUERY_REQUEST::cACTION_CONTROL_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_REQUEST::cACTION_CONTROL_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_REQUEST::~cACTION_CONTROL_ARP_QUERY_REQUEST() {
}
sArpQuery& cACTION_CONTROL_ARP_QUERY_REQUEST::params() {
    return (sArpQuery&)(*m_params);
}

void cACTION_CONTROL_ARP_QUERY_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_ARP_QUERY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpQuery); // params
    return class_size;
}

bool cACTION_CONTROL_ARP_QUERY_REQUEST::init()
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

cACTION_CONTROL_ARP_QUERY_RESPONSE::cACTION_CONTROL_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_RESPONSE::cACTION_CONTROL_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_ARP_QUERY_RESPONSE::~cACTION_CONTROL_ARP_QUERY_RESPONSE() {
}
sArpMonitorData& cACTION_CONTROL_ARP_QUERY_RESPONSE::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_CONTROL_ARP_QUERY_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_ARP_QUERY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_CONTROL_ARP_QUERY_RESPONSE::init()
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

cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::~cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION() {
}
sVersions& cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::versions() {
    return (sVersions&)(*m_versions);
}

void cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::class_swap()
{
    m_versions->struct_swap();
}

size_t cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    return class_size;
}

bool cACTION_CONTROL_VERSION_MISMATCH_NOTIFICATION::init()
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

cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::~cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::bridge_mac() {
    return (sMacAddr&)(*m_bridge_mac);
}

uint8_t& cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::operational() {
    return (uint8_t&)(*m_operational);
}

void cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::class_swap()
{
    m_bridge_mac->struct_swap();
}

size_t cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // bridge_mac
    class_size += sizeof(uint8_t); // operational
    return class_size;
}

bool cACTION_CONTROL_PLATFORM_OPERATIONAL_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_bridge_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bridge_mac->struct_init(); }
    m_operational = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::~cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION() {
}
sBackhaulRssi& cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::params() {
    return (sBackhaulRssi&)(*m_params);
}

void cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulRssi); // params
    return class_size;
}

bool cACTION_CONTROL_BACKHAUL_DL_RSSI_REPORT_NOTIFICATION::init()
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

cACTION_CONTROL_BACKHAUL_RESET::cACTION_CONTROL_BACKHAUL_RESET(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_BACKHAUL_RESET::cACTION_CONTROL_BACKHAUL_RESET(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_BACKHAUL_RESET::~cACTION_CONTROL_BACKHAUL_RESET() {
}
void cACTION_CONTROL_BACKHAUL_RESET::class_swap()
{
}

size_t cACTION_CONTROL_BACKHAUL_RESET::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_BACKHAUL_RESET::init()
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

cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::cACTION_CONTROL_BACKHAUL_ROAM_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::cACTION_CONTROL_BACKHAUL_ROAM_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::~cACTION_CONTROL_BACKHAUL_ROAM_REQUEST() {
}
sBackhaulRoam& cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::params() {
    return (sBackhaulRoam&)(*m_params);
}

void cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBackhaulRoam); // params
    return class_size;
}

bool cACTION_CONTROL_BACKHAUL_ROAM_REQUEST::init()
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

cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::~cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL() {
}
sLoggingLevelChange& cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::params() {
    return (sLoggingLevelChange&)(*m_params);
}

void cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLoggingLevelChange); // params
    return class_size;
}

bool cACTION_CONTROL_CHANGE_MODULE_LOGGING_LEVEL::init()
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

cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::~cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST() {
}
sWifiCredentials& cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::params() {
    return (sWifiCredentials&)(*m_params);
}

void cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sWifiCredentials); // params
    return class_size;
}

bool cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_REQUEST::init()
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

cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::~cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE() {
}
uint32_t& cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PREPARE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST::~cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST() {
}
void cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST::class_swap()
{
}

size_t cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_REQUEST::init()
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

cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::~cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE() {
}
uint32_t& cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_PRE_COMMIT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST::~cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST() {
}
void cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST::class_swap()
{
}

size_t cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_COMMIT_REQUEST::init()
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

cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST::cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST::~cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST() {
}
void cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST::class_swap()
{
}

size_t cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_WIFI_CREDENTIALS_UPDATE_ABORT_REQUEST::init()
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

cACTION_CONTROL_HOSTAP_TX_ON_REQUEST::cACTION_CONTROL_HOSTAP_TX_ON_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_TX_ON_REQUEST::cACTION_CONTROL_HOSTAP_TX_ON_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_TX_ON_REQUEST::~cACTION_CONTROL_HOSTAP_TX_ON_REQUEST() {
}
void cACTION_CONTROL_HOSTAP_TX_ON_REQUEST::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_TX_ON_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_TX_ON_REQUEST::init()
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

cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST::cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST::cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST::~cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST() {
}
void cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_TX_OFF_REQUEST::init()
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

cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE::cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE::cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE::~cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE() {
}
void cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_TX_ON_RESPONSE::init()
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

cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::~cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::class_swap()
{
    m_cs_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CSA_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::~cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::class_swap()
{
    m_cs_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CSA_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::~cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::class_swap()
{
    m_cs_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_ACS_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::~cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

std::tuple<bool, sWifiChannel&> cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::supported_channels(size_t idx) {
    bool ret_success = ( (m_supported_channels_idx__ > 0) && (m_supported_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_supported_channels[ret_idx]);
}

void cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::class_swap()
{
    m_cs_params->struct_swap();
    for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++){
        m_supported_channels[i].struct_swap();
    }
}

size_t cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += beerocks::message::SUPPORTED_CHANNELS_LENGTH * sizeof(sWifiChannel); // supported_channels
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_ACS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_supported_channels = (sWifiChannel*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(sWifiChannel) * beerocks::message::SUPPORTED_CHANNELS_LENGTH);
    m_supported_channels_idx__  = beerocks::message::SUPPORTED_CHANNELS_LENGTH;
        if (!m_parse__) { 
            for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++) { m_supported_channels->struct_init(); }
        }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::~cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION() {
}
sDfsCacCompleted& cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::params() {
    return (sDfsCacCompleted&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsCacCompleted); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDfsCacCompleted*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sDfsCacCompleted) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::~cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION() {
}
sDfsChannelAvailable& cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::params() {
    return (sDfsChannelAvailable&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsChannelAvailable); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDfsChannelAvailable*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sDfsChannelAvailable) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::~cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST() {
}
sApSetRestrictedFailsafe& cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::params() {
    return (sApSetRestrictedFailsafe&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApSetRestrictedFailsafe); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sApSetRestrictedFailsafe*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApSetRestrictedFailsafe) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::~cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE() {
}
uint8_t& cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::~cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::class_swap()
{
    m_cs_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_ACS_START::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::~cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST() {
}
uint32_t& cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::attempts() {
    return (uint32_t&)(*m_attempts);
}

void cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_attempts));
}

size_t cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // attempts
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_UPDATE_STOP_ON_FAILURE_ATTEMPTS_REQUEST::init()
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

cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER::cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER::cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER::~cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER() {
}
void cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_DISABLED_BY_MASTER::init()
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

cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::~cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST() {
}
sApChannelSwitch& cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::class_swap()
{
    m_cs_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_CHANNEL_SWITCH_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApChannelSwitch) * 1;
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::~cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST() {
}
uint8_t& cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::sync() {
    return (uint8_t&)(*m_sync);
}

void cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // sync
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sync = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::~cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE() {
}
sApStatsParams& cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::ap_stats() {
    return (sApStatsParams&)(*m_ap_stats);
}

uint8_t& cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::sta_stats_size() {
    return (uint8_t&)(*m_sta_stats_size);
}

std::tuple<bool, sStaStatsParams&> cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::sta_stats(size_t idx) {
    bool ret_success = ( (m_sta_stats_idx__ > 0) && (m_sta_stats_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_sta_stats[ret_idx]);
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::alloc_sta_stats(size_t count) {
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sStaStatsParams) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    if (!m_parse__)
        std::memmove(m_buff_ptr__ + len, m_buff_ptr__, getBuffRemainingBytes() - len);
    m_sta_stats_idx__ += count;
    *m_sta_stats_size += count;
    m_buff_ptr__ += len;
    if (!m_parse__) { 
        for (size_t i = m_sta_stats_idx__ - count; i < m_sta_stats_idx__; i++) { m_sta_stats[i].struct_init(); }
    }
    return true;
}

void cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::class_swap()
{
    m_ap_stats->struct_swap();
    for (size_t i = 0; i < (size_t)*m_sta_stats_size; i++){
        m_sta_stats[i].struct_swap();
    }
}

size_t cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApStatsParams); // ap_stats
    class_size += sizeof(uint8_t); // sta_stats_size
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_STATS_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_stats = (sApStatsParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApStatsParams) * 1;
    if (!m_parse__) { m_ap_stats->struct_init(); }
    m_sta_stats_size = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_sta_stats_size = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_sta_stats = (sStaStatsParams*)m_buff_ptr__;
    m_sta_stats_idx__ = *m_sta_stats_size;
    m_buff_ptr__ += sizeof(sStaStatsParams)*(*m_sta_stats_size);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::~cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION() {
}
sApLoadNotificationParams& cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::params() {
    return (sApLoadNotificationParams&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApLoadNotificationParams); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sApLoadNotificationParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApLoadNotificationParams) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::~cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST() {
}
sNeighborSetParams11k& cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborSetParams11k&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborSetParams11k); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_SET_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNeighborSetParams11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNeighborSetParams11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::~cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST() {
}
sNeighborRemoveParams11k& cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborRemoveParams11k&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborRemoveParams11k); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNeighborRemoveParams11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNeighborRemoveParams11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::~cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION() {
}
sApActivityNotificationParams& cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::params() {
    return (sApActivityNotificationParams&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApActivityNotificationParams); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sApActivityNotificationParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApActivityNotificationParams) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::~cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION() {
}
sVapsList& cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::params() {
    return (sVapsList&)(*m_params);
}

void cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVapsList); // params
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sVapsList*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sVapsList) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::~cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION() {
}
int8_t& cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

void cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::class_swap()
{
}

size_t cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_AP_DISABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::~cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION() {
}
int8_t& cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

sVapInfo& cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::vap_info() {
    return (sVapInfo&)(*m_vap_info);
}

void cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::class_swap()
{
    m_vap_info->struct_swap();
}

size_t cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(sVapInfo); // vap_info
    return class_size;
}

bool cACTION_CONTROL_HOSTAP_AP_ENABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    m_vap_info = (sVapInfo*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sVapInfo) * 1;
    if (!m_parse__) { m_vap_info->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::~cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST() {
}
sClientMonitoringParams& cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::params() {
    return (sClientMonitoringParams&)(*m_params);
}

void cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientMonitoringParams); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_START_MONITORING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientMonitoringParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sClientMonitoringParams) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::~cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST() {
}
sMacAddr& cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_STOP_MONITORING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST() {
}
sNodeRssiMeasurementRequest& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params() {
    return (sNodeRssiMeasurementRequest&)(*m_params);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
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

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE() {
}
sNodeRssiMeasurement& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
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

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE() {
}
sMacAddr& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::~cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION() {
}
sNodeRssiMeasurement& cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::init()
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

cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::~cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_NO_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::~cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_NO_RESPONSE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::~cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION() {
}
sClientAssociationParams& cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::params() {
    return (sClientAssociationParams&)(*m_params);
}

void cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientAssociationParams); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_ASSOCIATED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientAssociationParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sClientAssociationParams) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::~cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION() {
}
sClientDisconnectionParams& cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::params() {
    return (sClientDisconnectionParams&)(*m_params);
}

void cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectionParams); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientDisconnectionParams*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sClientDisconnectionParams) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::cACTION_CONTROL_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::cACTION_CONTROL_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::~cACTION_CONTROL_CLIENT_DISALLOW_REQUEST() {
}
sMacAddr& cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

uint8_t& cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::reject_sta() {
    return (uint8_t&)(*m_reject_sta);
}

void cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(uint8_t); // reject_sta
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    m_reject_sta = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_ALLOW_REQUEST::cACTION_CONTROL_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ALLOW_REQUEST::cACTION_CONTROL_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ALLOW_REQUEST::~cACTION_CONTROL_CLIENT_ALLOW_REQUEST() {
}
sMacAddr& cACTION_CONTROL_CLIENT_ALLOW_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

beerocks::net::sIpv4Addr& cACTION_CONTROL_CLIENT_ALLOW_REQUEST::ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_ipv4);
}

void cACTION_CONTROL_CLIENT_ALLOW_REQUEST::class_swap()
{
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_ALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    return class_size;
}

bool cACTION_CONTROL_CLIENT_ALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sIpv4Addr) * 1;
    if (!m_parse__) { m_ipv4->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::~cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST() {
}
sMacAddr& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

int8_t& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::vap_id() {
    return (int8_t&)(*m_vap_id);
}

uint32_t& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::type() {
    return (uint32_t&)(*m_type);
}

uint32_t& cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

void cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    m_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
}

size_t cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(uint32_t); // type
    class_size += sizeof(uint32_t); // reason
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    m_vap_id = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    m_type = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_reason = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::~cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE() {
}
sClientDisconnectResponse& cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::params() {
    return (sClientDisconnectResponse&)(*m_params);
}

void cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectResponse); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DISCONNECT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientDisconnectResponse*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sClientDisconnectResponse) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::~cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST() {
}
sNodeBssSteerRequest& cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::params() {
    return (sNodeBssSteerRequest&)(*m_params);
}

void cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerRequest); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_BSS_STEER_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeBssSteerRequest*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeBssSteerRequest) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::~cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE() {
}
sNodeBssSteerResponse& cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::params() {
    return (sNodeBssSteerResponse&)(*m_params);
}

void cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerResponse); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_BSS_STEER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeBssSteerResponse*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeBssSteerResponse) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::~cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION() {
}
sMacAddr& cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

beerocks::net::sIpv4Addr& cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_ipv4);
}

char* cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::name(size_t length) {
    if( (m_name_idx__ <= 0) || (m_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_name);
}

bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::set_name(std::string& str) {
    return set_name(const_cast<std::string&>(str));
}
bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::set_name(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_name received an empty string.";
        return false;
    }
    if (str_size + 1 > beerocks::message::NODE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_name, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::set_name(char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_name received an empty string.";
        return false;
    }
    if (size + 1 > beerocks::message::NODE_NAME_LENGTH) { // +1 for null terminator
        TLVF_LOG(ERROR) << "Received buffer size is smaller than string length";
        return false;
    }
    tlvf_copy_string(m_name, str, size + 1);
    m_name[size] = '\0';
    return true;
}
void cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    class_size += beerocks::message::NODE_NAME_LENGTH * sizeof(char); // name
    return class_size;
}

bool cACTION_CONTROL_CLIENT_DHCP_COMPLETE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sIpv4Addr) * 1;
    if (!m_parse__) { m_ipv4->struct_init(); }
    m_name = (char*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(char) * beerocks::message::NODE_NAME_LENGTH);
    m_name_idx__  = beerocks::message::NODE_NAME_LENGTH;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::~cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION() {
}
sArpMonitorData& cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_ARP_MONITOR_NOTIFICATION::init()
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

cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::~cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST() {
}
sBeaconRequest11k& cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::params() {
    return (sBeaconRequest11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBeaconRequest11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_BEACON_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBeaconRequest11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sBeaconRequest11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::~cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE() {
}
sBeaconResponse11k& cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::params() {
    return (sBeaconResponse11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBeaconResponse11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_BEACON_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBeaconResponse11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sBeaconResponse11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::~cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST() {
}
sStaChannelLoadRequest11k& cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::params() {
    return (sStaChannelLoadRequest11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStaChannelLoadRequest11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStaChannelLoadRequest11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStaChannelLoadRequest11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::~cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE() {
}
sStaChannelLoadResponse11k& cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::params() {
    return (sStaChannelLoadResponse11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStaChannelLoadResponse11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_CHANNEL_LOAD_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStaChannelLoadResponse11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStaChannelLoadResponse11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::~cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST() {
}
sStatisticsRequest11k& cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::params() {
    return (sStatisticsRequest11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStatisticsRequest11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_STATISTICS_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStatisticsRequest11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStatisticsRequest11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::~cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE() {
}
sStatisticsResponse11k& cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::params() {
    return (sStatisticsResponse11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStatisticsResponse11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_STATISTICS_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStatisticsResponse11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStatisticsResponse11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::~cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST() {
}
sMacAddr& cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CONTROL_CLIENT_LINK_MEASUREMENT_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::~cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE() {
}
sLinkMeasurementsResponse11k& cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::params() {
    return (sLinkMeasurementsResponse11k&)(*m_params);
}

void cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLinkMeasurementsResponse11k); // params
    return class_size;
}

bool cACTION_CONTROL_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sLinkMeasurementsResponse11k*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sLinkMeasurementsResponse11k) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::~cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST() {
}
sSteeringSetGroupRequest& cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::params() {
    return (sSteeringSetGroupRequest&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringSetGroupRequest); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringSetGroupRequest*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringSetGroupRequest) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::~cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE() {
}
sSteeringSetGroupResponse& cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::params() {
    return (sSteeringSetGroupResponse&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringSetGroupResponse); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_GROUP_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringSetGroupResponse*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringSetGroupResponse) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::~cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST() {
}
sSteeringClientSetRequest& cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::params() {
    return (sSteeringClientSetRequest&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetRequest); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringClientSetRequest*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringClientSetRequest) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::~cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE() {
}
sSteeringClientSetResponse& cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::params() {
    return (sSteeringClientSetResponse&)(*m_params);
}

void cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetResponse); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_CLIENT_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringClientSetResponse*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringClientSetResponse) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION() {
}
sSteeringEvActivity& cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::params() {
    return (sSteeringEvActivity&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvActivity); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvActivity*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringEvActivity) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION() {
}
sSteeringEvSnrXing& cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::params() {
    return (sSteeringEvSnrXing&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvSnrXing); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_SNR_XING_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvSnrXing*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringEvSnrXing) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION() {
}
sSteeringEvProbeReq& cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::params() {
    return (sSteeringEvProbeReq&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvProbeReq); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_PROBE_REQ_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvProbeReq*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringEvProbeReq) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::~cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION() {
}
sSteeringEvAuthFail& cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::params() {
    return (sSteeringEvAuthFail&)(*m_params);
}

void cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvAuthFail); // params
    return class_size;
}

bool cACTION_CONTROL_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvAuthFail*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringEvAuthFail) * 1;
    if (!m_parse__) { m_params->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


