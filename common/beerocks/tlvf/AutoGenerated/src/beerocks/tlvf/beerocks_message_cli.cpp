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

#include <beerocks/tlvf/beerocks_message_cli.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::~cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS() {
}
int8_t& cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::isEnable() {
    return (int8_t&)(*m_isEnable);
}

void cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::class_swap()
{
}

size_t cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // isEnable
    return class_size;
}

bool cACTION_CLI_ENABLE_DIAGNOSTICS_MEASUREMENTS::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_ENABLE_LOAD_BALANCER::cACTION_CLI_ENABLE_LOAD_BALANCER(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_ENABLE_LOAD_BALANCER::cACTION_CLI_ENABLE_LOAD_BALANCER(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_ENABLE_LOAD_BALANCER::~cACTION_CLI_ENABLE_LOAD_BALANCER() {
}
int8_t& cACTION_CLI_ENABLE_LOAD_BALANCER::isEnable() {
    return (int8_t&)(*m_isEnable);
}

void cACTION_CLI_ENABLE_LOAD_BALANCER::class_swap()
{
}

size_t cACTION_CLI_ENABLE_LOAD_BALANCER::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // isEnable
    return class_size;
}

bool cACTION_CLI_ENABLE_LOAD_BALANCER::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_ENABLE_DEBUG::cACTION_CLI_ENABLE_DEBUG(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_ENABLE_DEBUG::cACTION_CLI_ENABLE_DEBUG(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_ENABLE_DEBUG::~cACTION_CLI_ENABLE_DEBUG() {
}
int8_t& cACTION_CLI_ENABLE_DEBUG::isEnable() {
    return (int8_t&)(*m_isEnable);
}

void cACTION_CLI_ENABLE_DEBUG::class_swap()
{
}

size_t cACTION_CLI_ENABLE_DEBUG::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // isEnable
    return class_size;
}

bool cACTION_CLI_ENABLE_DEBUG::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::~cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS() {
}
int32_t& cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::attempts() {
    return (int32_t&)(*m_attempts);
}

void cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_attempts));
}

size_t cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int32_t); // attempts
    return class_size;
}

bool cACTION_CLI_SET_SLAVES_STOP_ON_FAILURE_ATTEMPTS::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_attempts = (int32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_RESPONSE_INT::cACTION_CLI_RESPONSE_INT(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_RESPONSE_INT::cACTION_CLI_RESPONSE_INT(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_RESPONSE_INT::~cACTION_CLI_RESPONSE_INT() {
}
uint8_t& cACTION_CLI_RESPONSE_INT::isOK() {
    return (uint8_t&)(*m_isOK);
}

int8_t& cACTION_CLI_RESPONSE_INT::currentValue() {
    return (int8_t&)(*m_currentValue);
}

void cACTION_CLI_RESPONSE_INT::class_swap()
{
}

size_t cACTION_CLI_RESPONSE_INT::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isOK
    class_size += sizeof(int8_t); // currentValue
    return class_size;
}

bool cACTION_CLI_RESPONSE_INT::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isOK = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_currentValue = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_RESPONSE_STR::cACTION_CLI_RESPONSE_STR(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_RESPONSE_STR::cACTION_CLI_RESPONSE_STR(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_RESPONSE_STR::~cACTION_CLI_RESPONSE_STR() {
}
uint32_t& cACTION_CLI_RESPONSE_STR::buffer_size() {
    return (uint32_t&)(*m_buffer_size);
}

std::string cACTION_CLI_RESPONSE_STR::buffer_str() {
    char *buffer_ = buffer();
    if (!buffer_) { return std::string(); }
    return std::string(buffer_, m_buffer_idx__);
}

char* cACTION_CLI_RESPONSE_STR::buffer(size_t length) {
    if( (m_buffer_idx__ <= 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_CLI_RESPONSE_STR::set_buffer(const std::string& str) {
    size_t str_size = str.size();
    if (str_size == 0) {
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(str_size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_buffer, str.c_str(), str_size + 1);
    return true;
}
bool cACTION_CLI_RESPONSE_STR::set_buffer(const char str[], size_t size) {
    if (str == nullptr || size == 0) { 
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(size + 1)) { return false; } // +1 for null terminator
    tlvf_copy_string(m_buffer, str, size + 1);
    m_buffer[size] = '\0';
    return true;
}
bool cACTION_CLI_RESPONSE_STR::alloc_buffer(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list buffer, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_buffer[*m_buffer_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_buffer_idx__ += count;
    *m_buffer_size += count;
    m_buff_ptr__ += len;
    return true;
}

void cACTION_CLI_RESPONSE_STR::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

size_t cACTION_CLI_RESPONSE_STR::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // buffer_size
    return class_size;
}

bool cACTION_CLI_RESPONSE_STR::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_buffer_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_buffer_size = 0;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    m_buff_ptr__ += sizeof(char)*(buffer_size);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::~cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT() {
}
sMacAddr& cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::hostap_mac() {
    return (sMacAddr&)(*m_hostap_mac);
}

uint16_t& cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::center_frequency() {
    return (uint16_t&)(*m_center_frequency);
}

void cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::class_swap()
{
    m_client_mac->struct_swap();
    m_hostap_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_center_frequency));
}

size_t cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // hostap_mac
    class_size += sizeof(uint16_t); // center_frequency
    return class_size;
}

bool cACTION_CLI_CROSS_RX_RSSI_MEASUREMENT::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_hostap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_hostap_mac->struct_init(); }
    m_center_frequency = (uint16_t*)m_buff_ptr__;
    if (!m_parse__) *m_center_frequency = 0x0;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_OPTIMAL_PATH_TASK::cACTION_CLI_OPTIMAL_PATH_TASK(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_OPTIMAL_PATH_TASK::cACTION_CLI_OPTIMAL_PATH_TASK(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_OPTIMAL_PATH_TASK::~cACTION_CLI_OPTIMAL_PATH_TASK() {
}
sMacAddr& cACTION_CLI_OPTIMAL_PATH_TASK::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

void cACTION_CLI_OPTIMAL_PATH_TASK::class_swap()
{
    m_client_mac->struct_swap();
}

size_t cACTION_CLI_OPTIMAL_PATH_TASK::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    return class_size;
}

bool cACTION_CLI_OPTIMAL_PATH_TASK::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_LOAD_BALANCER_TASK::cACTION_CLI_LOAD_BALANCER_TASK(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_LOAD_BALANCER_TASK::cACTION_CLI_LOAD_BALANCER_TASK(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_LOAD_BALANCER_TASK::~cACTION_CLI_LOAD_BALANCER_TASK() {
}
sMacAddr& cACTION_CLI_LOAD_BALANCER_TASK::ap_mac() {
    return (sMacAddr&)(*m_ap_mac);
}

void cACTION_CLI_LOAD_BALANCER_TASK::class_swap()
{
    m_ap_mac->struct_swap();
}

size_t cACTION_CLI_LOAD_BALANCER_TASK::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // ap_mac
    return class_size;
}

bool cACTION_CLI_LOAD_BALANCER_TASK::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_ap_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK::cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK::cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK::~cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK() {
}
void cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK::class_swap()
{
}

size_t cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_CLI_IRE_NETWORK_OPTIMIZATION_TASK::init()
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

cACTION_CLI_DUMP_NODE_INFO::cACTION_CLI_DUMP_NODE_INFO(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_DUMP_NODE_INFO::cACTION_CLI_DUMP_NODE_INFO(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_DUMP_NODE_INFO::~cACTION_CLI_DUMP_NODE_INFO() {
}
sMacAddr& cACTION_CLI_DUMP_NODE_INFO::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CLI_DUMP_NODE_INFO::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CLI_DUMP_NODE_INFO::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CLI_DUMP_NODE_INFO::init()
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

cACTION_CLI_PING_SLAVE_REQUEST::cACTION_CLI_PING_SLAVE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_PING_SLAVE_REQUEST::cACTION_CLI_PING_SLAVE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_PING_SLAVE_REQUEST::~cACTION_CLI_PING_SLAVE_REQUEST() {
}
sMacAddr& cACTION_CLI_PING_SLAVE_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

uint16_t& cACTION_CLI_PING_SLAVE_REQUEST::num_of_req() {
    return (uint16_t&)(*m_num_of_req);
}

uint16_t& cACTION_CLI_PING_SLAVE_REQUEST::size() {
    return (uint16_t&)(*m_size);
}

void cACTION_CLI_PING_SLAVE_REQUEST::class_swap()
{
    m_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_num_of_req));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
}

size_t cACTION_CLI_PING_SLAVE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(uint16_t); // num_of_req
    class_size += sizeof(uint16_t); // size
    return class_size;
}

bool cACTION_CLI_PING_SLAVE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
    m_num_of_req = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_PING_ALL_SLAVES_REQUEST::cACTION_CLI_PING_ALL_SLAVES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_PING_ALL_SLAVES_REQUEST::cACTION_CLI_PING_ALL_SLAVES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_PING_ALL_SLAVES_REQUEST::~cACTION_CLI_PING_ALL_SLAVES_REQUEST() {
}
uint16_t& cACTION_CLI_PING_ALL_SLAVES_REQUEST::num_of_req() {
    return (uint16_t&)(*m_num_of_req);
}

uint16_t& cACTION_CLI_PING_ALL_SLAVES_REQUEST::size() {
    return (uint16_t&)(*m_size);
}

void cACTION_CLI_PING_ALL_SLAVES_REQUEST::class_swap()
{
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_num_of_req));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_size));
}

size_t cACTION_CLI_PING_ALL_SLAVES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint16_t); // num_of_req
    class_size += sizeof(uint16_t); // size
    return class_size;
}

bool cACTION_CLI_PING_ALL_SLAVES_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_num_of_req = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_size = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_BACKHAUL_SCAN_RESULTS::cACTION_CLI_BACKHAUL_SCAN_RESULTS(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_BACKHAUL_SCAN_RESULTS::cACTION_CLI_BACKHAUL_SCAN_RESULTS(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_BACKHAUL_SCAN_RESULTS::~cACTION_CLI_BACKHAUL_SCAN_RESULTS() {
}
sMacAddr& cACTION_CLI_BACKHAUL_SCAN_RESULTS::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_CLI_BACKHAUL_SCAN_RESULTS::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_CLI_BACKHAUL_SCAN_RESULTS::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_CLI_BACKHAUL_SCAN_RESULTS::init()
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

cACTION_CLI_BACKHAUL_ROAM_REQUEST::cACTION_CLI_BACKHAUL_ROAM_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_BACKHAUL_ROAM_REQUEST::cACTION_CLI_BACKHAUL_ROAM_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_BACKHAUL_ROAM_REQUEST::~cACTION_CLI_BACKHAUL_ROAM_REQUEST() {
}
sMacAddr& cACTION_CLI_BACKHAUL_ROAM_REQUEST::slave_mac() {
    return (sMacAddr&)(*m_slave_mac);
}

sMacAddr& cACTION_CLI_BACKHAUL_ROAM_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

void cACTION_CLI_BACKHAUL_ROAM_REQUEST::class_swap()
{
    m_slave_mac->struct_swap();
    m_bssid->struct_swap();
}

size_t cACTION_CLI_BACKHAUL_ROAM_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // slave_mac
    class_size += sizeof(sMacAddr); // bssid
    return class_size;
}

bool cACTION_CLI_BACKHAUL_ROAM_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_slave_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_slave_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bssid->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_ALLOW_REQUEST::cACTION_CLI_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_ALLOW_REQUEST::cACTION_CLI_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_ALLOW_REQUEST::~cACTION_CLI_CLIENT_ALLOW_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_ALLOW_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_CLI_CLIENT_ALLOW_REQUEST::hostap_mac() {
    return (sMacAddr&)(*m_hostap_mac);
}

void cACTION_CLI_CLIENT_ALLOW_REQUEST::class_swap()
{
    m_client_mac->struct_swap();
    m_hostap_mac->struct_swap();
}

size_t cACTION_CLI_CLIENT_ALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // hostap_mac
    return class_size;
}

bool cACTION_CLI_CLIENT_ALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_hostap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_hostap_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_DISALLOW_REQUEST::cACTION_CLI_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_DISALLOW_REQUEST::cACTION_CLI_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_DISALLOW_REQUEST::~cACTION_CLI_CLIENT_DISALLOW_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_DISALLOW_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_CLI_CLIENT_DISALLOW_REQUEST::hostap_mac() {
    return (sMacAddr&)(*m_hostap_mac);
}

void cACTION_CLI_CLIENT_DISALLOW_REQUEST::class_swap()
{
    m_client_mac->struct_swap();
    m_hostap_mac->struct_swap();
}

size_t cACTION_CLI_CLIENT_DISALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // hostap_mac
    return class_size;
}

bool cACTION_CLI_CLIENT_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_hostap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_hostap_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_DISCONNECT_REQUEST::cACTION_CLI_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_DISCONNECT_REQUEST::cACTION_CLI_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_DISCONNECT_REQUEST::~cACTION_CLI_CLIENT_DISCONNECT_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_DISCONNECT_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

uint32_t& cACTION_CLI_CLIENT_DISCONNECT_REQUEST::type() {
    return (uint32_t&)(*m_type);
}

uint32_t& cACTION_CLI_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

void cACTION_CLI_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    m_client_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
}

size_t cACTION_CLI_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(uint32_t); // type
    class_size += sizeof(uint32_t); // reason
    return class_size;
}

bool cACTION_CLI_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
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

cACTION_CLI_CLIENT_BSS_STEER_REQUEST::cACTION_CLI_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_BSS_STEER_REQUEST::cACTION_CLI_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_BSS_STEER_REQUEST::~cACTION_CLI_CLIENT_BSS_STEER_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_BSS_STEER_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_CLI_CLIENT_BSS_STEER_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint32_t& cACTION_CLI_CLIENT_BSS_STEER_REQUEST::disassoc_timer() {
    return (uint32_t&)(*m_disassoc_timer);
}

void cACTION_CLI_CLIENT_BSS_STEER_REQUEST::class_swap()
{
    m_client_mac->struct_swap();
    m_bssid->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_disassoc_timer));
}

size_t cACTION_CLI_CLIENT_BSS_STEER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint32_t); // disassoc_timer
    return class_size;
}

bool cACTION_CLI_CLIENT_BSS_STEER_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bssid->struct_init(); }
    m_disassoc_timer = (uint32_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::~cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::hostap_mac() {
    return (sMacAddr&)(*m_hostap_mac);
}

sMacAddr& cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

void cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::class_swap()
{
    m_hostap_mac->struct_swap();
    m_client_mac->struct_swap();
}

size_t cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // hostap_mac
    class_size += sizeof(sMacAddr); // client_mac
    return class_size;
}

bool cACTION_CLI_CLIENT_LINK_MEASUREMENT_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_hostap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_hostap_mac->struct_init(); }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::~cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::hostap_mac() {
    return (sMacAddr&)(*m_hostap_mac);
}

sMacAddr& cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

uint8_t& cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

void cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::class_swap()
{
    m_hostap_mac->struct_swap();
    m_client_mac->struct_swap();
}

size_t cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // hostap_mac
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(uint8_t); // channel
    return class_size;
}

bool cACTION_CLI_CLIENT_CHANNEL_LOAD_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_hostap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_hostap_mac->struct_init(); }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_channel = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_BEACON_11K_REQUEST::cACTION_CLI_CLIENT_BEACON_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_BEACON_11K_REQUEST::cACTION_CLI_CLIENT_BEACON_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_BEACON_11K_REQUEST::~cACTION_CLI_CLIENT_BEACON_11K_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

std::tuple<bool, uint8_t&> cACTION_CLI_CLIENT_BEACON_11K_REQUEST::ssid(size_t idx) {
    bool ret_success = ( (m_ssid_idx__ > 0) && (m_ssid_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_ssid[ret_idx]);
}

uint8_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::use_optional_ssid() {
    return (uint8_t&)(*m_use_optional_ssid);
}

uint8_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

uint8_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::measurement_mode() {
    return (uint8_t&)(*m_measurement_mode);
}

uint16_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::duration() {
    return (uint16_t&)(*m_duration);
}

uint16_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::rand_ival() {
    return (uint16_t&)(*m_rand_ival);
}

uint16_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::repeats() {
    return (uint16_t&)(*m_repeats);
}

int16_t& cACTION_CLI_CLIENT_BEACON_11K_REQUEST::op_class() {
    return (int16_t&)(*m_op_class);
}

void cACTION_CLI_CLIENT_BEACON_11K_REQUEST::class_swap()
{
    m_client_mac->struct_swap();
    m_bssid->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_duration));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_rand_ival));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_repeats));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_op_class));
}

size_t cACTION_CLI_CLIENT_BEACON_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += beerocks::message::WIFI_SSID_MAX_LENGTH * sizeof(uint8_t); // ssid
    class_size += sizeof(uint8_t); // use_optional_ssid
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(uint8_t); // measurement_mode
    class_size += sizeof(uint16_t); // duration
    class_size += sizeof(uint16_t); // rand_ival
    class_size += sizeof(uint16_t); // repeats
    class_size += sizeof(int16_t); // op_class
    return class_size;
}

bool cACTION_CLI_CLIENT_BEACON_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bssid->struct_init(); }
    m_ssid = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += (sizeof(uint8_t) * beerocks::message::WIFI_SSID_MAX_LENGTH);
    m_ssid_idx__  = beerocks::message::WIFI_SSID_MAX_LENGTH;
    m_use_optional_ssid = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_channel = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_measurement_mode = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_duration = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_rand_ival = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_repeats = (uint16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint16_t) * 1;
    m_op_class = (int16_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int16_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::~cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST() {
}
sMacAddr& cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::hostap_mac() {
    return (sMacAddr&)(*m_hostap_mac);
}

sMacAddr& cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sMacAddr& cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::peer_mac() {
    return (sMacAddr&)(*m_peer_mac);
}

uint8_t& cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::group_identity() {
    return (uint8_t&)(*m_group_identity);
}

void cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::class_swap()
{
    m_hostap_mac->struct_swap();
    m_client_mac->struct_swap();
    m_peer_mac->struct_swap();
}

size_t cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // hostap_mac
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sMacAddr); // peer_mac
    class_size += sizeof(uint8_t); // group_identity
    return class_size;
}

bool cACTION_CLI_CLIENT_STATISTICS_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_hostap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_hostap_mac->struct_init(); }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_peer_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_peer_mac->struct_init(); }
    m_group_identity = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::~cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST() {
}
sMacAddr& cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

sApChannelSwitch& cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::class_swap()
{
    m_mac->struct_swap();
    m_cs_params->struct_swap();
}

size_t cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_CLI_HOSTAP_CHANNEL_SWITCH_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_mac->struct_init(); }
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

cACTION_CLI_HOSTAP_TX_ON_REQUEST::cACTION_CLI_HOSTAP_TX_ON_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_TX_ON_REQUEST::cACTION_CLI_HOSTAP_TX_ON_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_TX_ON_REQUEST::~cACTION_CLI_HOSTAP_TX_ON_REQUEST() {
}
sMacAddr& cACTION_CLI_HOSTAP_TX_ON_REQUEST::ap_mac() {
    return (sMacAddr&)(*m_ap_mac);
}

void cACTION_CLI_HOSTAP_TX_ON_REQUEST::class_swap()
{
    m_ap_mac->struct_swap();
}

size_t cACTION_CLI_HOSTAP_TX_ON_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // ap_mac
    return class_size;
}

bool cACTION_CLI_HOSTAP_TX_ON_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_ap_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_HOSTAP_TX_OFF_REQUEST::cACTION_CLI_HOSTAP_TX_OFF_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_TX_OFF_REQUEST::cACTION_CLI_HOSTAP_TX_OFF_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_TX_OFF_REQUEST::~cACTION_CLI_HOSTAP_TX_OFF_REQUEST() {
}
sMacAddr& cACTION_CLI_HOSTAP_TX_OFF_REQUEST::ap_mac() {
    return (sMacAddr&)(*m_ap_mac);
}

void cACTION_CLI_HOSTAP_TX_OFF_REQUEST::class_swap()
{
    m_ap_mac->struct_swap();
}

size_t cACTION_CLI_HOSTAP_TX_OFF_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // ap_mac
    return class_size;
}

bool cACTION_CLI_HOSTAP_TX_OFF_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_ap_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::~cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST() {
}
sMacAddr& cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::ap_mac() {
    return (sMacAddr&)(*m_ap_mac);
}

sMacAddr& cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint8_t& cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

int8_t& cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::vap_id() {
    return (int8_t&)(*m_vap_id);
}

void cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::class_swap()
{
    m_ap_mac->struct_swap();
    m_bssid->struct_swap();
}

size_t cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // ap_mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_CLI_HOSTAP_SET_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_ap_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bssid->struct_init(); }
    m_channel = (uint8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_vap_id = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::~cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST() {
}
sMacAddr& cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::ap_mac() {
    return (sMacAddr&)(*m_ap_mac);
}

sMacAddr& cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

int8_t& cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::vap_id() {
    return (int8_t&)(*m_vap_id);
}

void cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::class_swap()
{
    m_ap_mac->struct_swap();
    m_bssid->struct_swap();
}

size_t cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // ap_mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_CLI_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_ap_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_bssid->struct_init(); }
    m_vap_id = (int8_t*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_CLI_HOSTAP_STATS_MEASUREMENT::cACTION_CLI_HOSTAP_STATS_MEASUREMENT(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_STATS_MEASUREMENT::cACTION_CLI_HOSTAP_STATS_MEASUREMENT(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_CLI_HOSTAP_STATS_MEASUREMENT::~cACTION_CLI_HOSTAP_STATS_MEASUREMENT() {
}
sMacAddr& cACTION_CLI_HOSTAP_STATS_MEASUREMENT::ap_mac() {
    return (sMacAddr&)(*m_ap_mac);
}

void cACTION_CLI_HOSTAP_STATS_MEASUREMENT::class_swap()
{
    m_ap_mac->struct_swap();
}

size_t cACTION_CLI_HOSTAP_STATS_MEASUREMENT::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // ap_mac
    return class_size;
}

bool cACTION_CLI_HOSTAP_STATS_MEASUREMENT::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_mac = (sMacAddr*)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sMacAddr) * 1;
    if (!m_parse__) { m_ap_mac->struct_init(); }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


