///////////////////////////////////////
// AUTO GENERATED FILE - DO NOT EDIT //
///////////////////////////////////////

/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <beerocks/tlvf/beerocks_message_platform.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::~cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION() {
}
uint8_t& cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::is_backhaul_manager() {
    return (uint8_t&)(*m_is_backhaul_manager);
}

void cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::finalize()
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

size_t cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // is_backhaul_manager
    return class_size;
}

bool cACTION_PLATFORM_SON_SLAVE_BACKHAUL_CONNECTION_COMPLETE_NOTIFICATION::init()
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

cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_iface_name_idx__ == 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::set_iface_name(const std::string& str) { return set_iface_name(str.c_str(), str.size()); }
bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_iface_name received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::finalize()
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

size_t cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    return class_size;
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_platform_settings->struct_swap();
    m_wlan_settings->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_valid));
}

bool cACTION_PLATFORM_SON_SLAVE_REGISTER_RESPONSE::finalize()
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
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_platform_settings = (sPlatformSettings*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sPlatformSettings))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sPlatformSettings) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_platform_settings->struct_init(); }
    m_wlan_settings = (sWlanSettings*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWlanSettings))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWlanSettings) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    m_valid = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::~cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION() {
}
sArpMonitorData& cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::finalize()
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

size_t cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_PLATFORM_ARP_MONITOR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sArpMonitorData*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sArpMonitorData))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sArpMonitorData) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::~cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION() {
}
sWlanSettings& cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::wlan_settings() {
    return (sWlanSettings&)(*m_wlan_settings);
}

void cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_wlan_settings->struct_swap();
}

bool cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::finalize()
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

size_t cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sWlanSettings); // wlan_settings
    return class_size;
}

bool cACTION_PLATFORM_WLAN_PARAMS_CHANGED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_wlan_settings = (sWlanSettings*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWlanSettings))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWlanSettings) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_wlan_settings->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_hostname_idx__ == 0) || (m_hostname_idx__ < length) ) {
        TLVF_LOG(ERROR) << "hostname length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_hostname);
}

bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::set_hostname(const std::string& str) { return set_hostname(str.c_str(), str.size()); }
bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::set_hostname(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_hostname received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(8*sizeof(eDHCPOp), reinterpret_cast<uint8_t*>(m_dhcp_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_op));
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

bool cACTION_PLATFORM_DHCP_MONITOR_NOTIFICATION::finalize()
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
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_dhcp_op = (eDHCPOp*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eDHCPOp))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eDHCPOp) << ") Failed!";
        return false;
    }
    m_op = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::net::sIpv4Addr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::net::sIpv4Addr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_ipv4->struct_init(); }
    m_hostname = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::NODE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::NODE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_hostname_idx__  = beerocks::message::NODE_NAME_LENGTH;
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::~cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL() {
}
sLoggingLevelChange& cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::params() {
    return (sLoggingLevelChange&)(*m_params);
}

void cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::finalize()
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

size_t cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLoggingLevelChange); // params
    return class_size;
}

bool cACTION_PLATFORM_CHANGE_MODULE_LOGGING_LEVEL::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sLoggingLevelChange*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sLoggingLevelChange))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sLoggingLevelChange) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ARP_QUERY_REQUEST::cACTION_PLATFORM_ARP_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_REQUEST::cACTION_PLATFORM_ARP_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_REQUEST::~cACTION_PLATFORM_ARP_QUERY_REQUEST() {
}
sArpQuery& cACTION_PLATFORM_ARP_QUERY_REQUEST::params() {
    return (sArpQuery&)(*m_params);
}

void cACTION_PLATFORM_ARP_QUERY_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_PLATFORM_ARP_QUERY_REQUEST::finalize()
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

size_t cACTION_PLATFORM_ARP_QUERY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpQuery); // params
    return class_size;
}

bool cACTION_PLATFORM_ARP_QUERY_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sArpQuery*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sArpQuery))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sArpQuery) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ARP_QUERY_RESPONSE::cACTION_PLATFORM_ARP_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_RESPONSE::cACTION_PLATFORM_ARP_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ARP_QUERY_RESPONSE::~cACTION_PLATFORM_ARP_QUERY_RESPONSE() {
}
sArpMonitorData& cACTION_PLATFORM_ARP_QUERY_RESPONSE::params() {
    return (sArpMonitorData&)(*m_params);
}

void cACTION_PLATFORM_ARP_QUERY_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_PLATFORM_ARP_QUERY_RESPONSE::finalize()
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

size_t cACTION_PLATFORM_ARP_QUERY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sArpMonitorData); // params
    return class_size;
}

bool cACTION_PLATFORM_ARP_QUERY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sArpMonitorData*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sArpMonitorData))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sArpMonitorData) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::cACTION_PLATFORM_ONBOARD_QUERY_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::~cACTION_PLATFORM_ONBOARD_QUERY_REQUEST() {
}
void cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::finalize()
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

size_t cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_ONBOARD_QUERY_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::~cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE() {
}
sOnboarding& cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::params() {
    return (sOnboarding&)(*m_params);
}

void cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::finalize()
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

size_t cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sOnboarding); // params
    return class_size;
}

bool cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sOnboarding*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sOnboarding))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sOnboarding) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ONBOARD_SET_REQUEST::cACTION_PLATFORM_ONBOARD_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_SET_REQUEST::cACTION_PLATFORM_ONBOARD_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ONBOARD_SET_REQUEST::~cACTION_PLATFORM_ONBOARD_SET_REQUEST() {
}
sOnboarding& cACTION_PLATFORM_ONBOARD_SET_REQUEST::params() {
    return (sOnboarding&)(*m_params);
}

void cACTION_PLATFORM_ONBOARD_SET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_PLATFORM_ONBOARD_SET_REQUEST::finalize()
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

size_t cACTION_PLATFORM_ONBOARD_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sOnboarding); // params
    return class_size;
}

bool cACTION_PLATFORM_ONBOARD_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sOnboarding*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sOnboarding))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sOnboarding) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::cACTION_PLATFORM_WPS_ONBOARDING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_iface_name_idx__ == 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::set_iface_name(const std::string& str) { return set_iface_name(str.c_str(), str.size()); }
bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::set_iface_name(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_iface_name received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::finalize()
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

size_t cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += beerocks::message::IFACE_NAME_LENGTH * sizeof(char); // iface_name
    return class_size;
}

bool cACTION_PLATFORM_WPS_ONBOARDING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (beerocks::message::IFACE_NAME_LENGTH) << ") Failed!";
        return false;
    }
    m_iface_name_idx__  = beerocks::message::IFACE_NAME_LENGTH;
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::~cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST() {
}
uint8_t& cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::vap_id() {
    return (uint8_t&)(*m_vap_id);
}

void cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::finalize()
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

size_t cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // vap_id
    return class_size;
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_front_params->struct_swap();
    m_back_params->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE::finalize()
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
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_front_params = (sWifiCredentials*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWifiCredentials))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWifiCredentials) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_front_params->struct_init(); }
    m_back_params = (sWifiCredentials*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWifiCredentials))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWifiCredentials) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_back_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::~cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST() {
}
void cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::finalize()
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

size_t cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::finalize()
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

size_t cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sAdminCredentials); // params
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sAdminCredentials*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sAdminCredentials))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sAdminCredentials) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::~cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST() {
}
void cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::finalize()
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

size_t cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::finalize()
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

size_t cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDeviceInfo); // params
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDeviceInfo*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sDeviceInfo))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDeviceInfo) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::~cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST() {
}
void cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::finalize()
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

size_t cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::~cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE() {
}
uint8_t& cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::local_master() {
    return (uint8_t&)(*m_local_master);
}

void cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::finalize()
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

size_t cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // local_master
    return class_size;
}

bool cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_local_master = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::~cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION() {
}
sVersions& cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::versions() {
    return (sVersions&)(*m_versions);
}

void cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_versions->struct_swap();
}

bool cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::finalize()
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

size_t cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    return class_size;
}

bool cACTION_PLATFORM_VERSION_MISMATCH_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_versions = (sVersions*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVersions))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVersions) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_versions->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::~cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION() {
}
sVersions& cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::versions() {
    return (sVersions&)(*m_versions);
}

void cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_versions->struct_swap();
}

bool cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::finalize()
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

size_t cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    return class_size;
}

bool cACTION_PLATFORM_MASTER_SLAVE_VERSIONS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_versions = (sVersions*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVersions))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVersions) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_versions->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::~cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST() {
}
void cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::finalize()
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

size_t cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    m_versions->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::finalize()
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

size_t cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVersions); // versions
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_versions = (sVersions*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVersions))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVersions) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_versions->struct_init(); }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_PLATFORM_ERROR_NOTIFICATION::cACTION_PLATFORM_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_PLATFORM_ERROR_NOTIFICATION::cACTION_PLATFORM_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_data_idx__ == 0) || (m_data_idx__ < length) ) {
        TLVF_LOG(ERROR) << "data length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_data);
}

bool cACTION_PLATFORM_ERROR_NOTIFICATION::set_data(const std::string& str) { return set_data(str.c_str(), str.size()); }
bool cACTION_PLATFORM_ERROR_NOTIFICATION::set_data(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_data received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_PLATFORM), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_code));
}

bool cACTION_PLATFORM_ERROR_NOTIFICATION::finalize()
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

size_t cACTION_PLATFORM_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // code
    class_size += 256 * sizeof(char); // data
    return class_size;
}

bool cACTION_PLATFORM_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_code = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_data = (char*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(char) * (256))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (256) << ") Failed!";
        return false;
    }
    m_data_idx__  = 256;
    if (m_parse__) { class_swap(); }
    return true;
}


