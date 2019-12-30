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

#include <beerocks/tlvf/beerocks_message_bml.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_BML_PING_REQUEST::cACTION_BML_PING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_PING_REQUEST::cACTION_BML_PING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_PING_REQUEST::~cACTION_BML_PING_REQUEST() {
}
void cACTION_BML_PING_REQUEST::class_swap()
{
}

size_t cACTION_BML_PING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_PING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_PING_RESPONSE::cACTION_BML_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_PING_RESPONSE::cACTION_BML_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_PING_RESPONSE::~cACTION_BML_PING_RESPONSE() {
}
void cACTION_BML_PING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_PING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_PING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_NW_MAP_REQUEST::cACTION_BML_NW_MAP_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_REQUEST::cACTION_BML_NW_MAP_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_REQUEST::~cACTION_BML_NW_MAP_REQUEST() {
}
void cACTION_BML_NW_MAP_REQUEST::class_swap()
{
}

size_t cACTION_BML_NW_MAP_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_NW_MAP_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_NW_MAP_RESPONSE::cACTION_BML_NW_MAP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_RESPONSE::cACTION_BML_NW_MAP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_RESPONSE::~cACTION_BML_NW_MAP_RESPONSE() {
}
uint32_t& cACTION_BML_NW_MAP_RESPONSE::node_num() {
    return (uint32_t&)(*m_node_num);
}

uint32_t& cACTION_BML_NW_MAP_RESPONSE::buffer_size() {
    return (uint32_t&)(*m_buffer_size);
}

std::string cACTION_BML_NW_MAP_RESPONSE::buffer_str() {
    char *buffer_ = buffer();
    if (!buffer_) { return std::string(); }
    return std::string(buffer_, m_buffer_idx__);
}

char* cACTION_BML_NW_MAP_RESPONSE::buffer(size_t length) {
    if( (m_buffer_idx__ <= 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_NW_MAP_RESPONSE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_NW_MAP_RESPONSE::set_buffer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(size)) { return false; }
    std::copy(str, str + size, m_buffer);
    return true;
}
bool cACTION_BML_NW_MAP_RESPONSE::alloc_buffer(size_t count) {
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BML_NW_MAP_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_node_num));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

size_t cACTION_BML_NW_MAP_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // node_num
    class_size += sizeof(uint32_t); // buffer_size
    return class_size;
}

bool cACTION_BML_NW_MAP_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_node_num = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_NW_MAP_UPDATE::cACTION_BML_NW_MAP_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_UPDATE::cACTION_BML_NW_MAP_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_UPDATE::~cACTION_BML_NW_MAP_UPDATE() {
}
uint32_t& cACTION_BML_NW_MAP_UPDATE::node_num() {
    return (uint32_t&)(*m_node_num);
}

uint32_t& cACTION_BML_NW_MAP_UPDATE::buffer_size() {
    return (uint32_t&)(*m_buffer_size);
}

std::string cACTION_BML_NW_MAP_UPDATE::buffer_str() {
    char *buffer_ = buffer();
    if (!buffer_) { return std::string(); }
    return std::string(buffer_, m_buffer_idx__);
}

char* cACTION_BML_NW_MAP_UPDATE::buffer(size_t length) {
    if( (m_buffer_idx__ <= 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_NW_MAP_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_NW_MAP_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(size)) { return false; }
    std::copy(str, str + size, m_buffer);
    return true;
}
bool cACTION_BML_NW_MAP_UPDATE::alloc_buffer(size_t count) {
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BML_NW_MAP_UPDATE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_node_num));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

size_t cACTION_BML_NW_MAP_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // node_num
    class_size += sizeof(uint32_t); // buffer_size
    return class_size;
}

bool cACTION_BML_NW_MAP_UPDATE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_node_num = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STATS_UPDATE::cACTION_BML_STATS_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STATS_UPDATE::cACTION_BML_STATS_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STATS_UPDATE::~cACTION_BML_STATS_UPDATE() {
}
uint32_t& cACTION_BML_STATS_UPDATE::num_of_stats_bulks() {
    return (uint32_t&)(*m_num_of_stats_bulks);
}

uint32_t& cACTION_BML_STATS_UPDATE::buffer_size() {
    return (uint32_t&)(*m_buffer_size);
}

std::string cACTION_BML_STATS_UPDATE::buffer_str() {
    char *buffer_ = buffer();
    if (!buffer_) { return std::string(); }
    return std::string(buffer_, m_buffer_idx__);
}

char* cACTION_BML_STATS_UPDATE::buffer(size_t length) {
    if( (m_buffer_idx__ <= 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_STATS_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_STATS_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(size)) { return false; }
    std::copy(str, str + size, m_buffer);
    return true;
}
bool cACTION_BML_STATS_UPDATE::alloc_buffer(size_t count) {
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BML_STATS_UPDATE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_num_of_stats_bulks));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

size_t cACTION_BML_STATS_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // num_of_stats_bulks
    class_size += sizeof(uint32_t); // buffer_size
    return class_size;
}

bool cACTION_BML_STATS_UPDATE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_num_of_stats_bulks = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_EVENTS_UPDATE::cACTION_BML_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_EVENTS_UPDATE::cACTION_BML_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_EVENTS_UPDATE::~cACTION_BML_EVENTS_UPDATE() {
}
uint32_t& cACTION_BML_EVENTS_UPDATE::buffer_size() {
    return (uint32_t&)(*m_buffer_size);
}

std::string cACTION_BML_EVENTS_UPDATE::buffer_str() {
    char *buffer_ = buffer();
    if (!buffer_) { return std::string(); }
    return std::string(buffer_, m_buffer_idx__);
}

char* cACTION_BML_EVENTS_UPDATE::buffer(size_t length) {
    if( (m_buffer_idx__ <= 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_EVENTS_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_EVENTS_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(size)) { return false; }
    std::copy(str, str + size, m_buffer);
    return true;
}
bool cACTION_BML_EVENTS_UPDATE::alloc_buffer(size_t count) {
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BML_EVENTS_UPDATE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

size_t cACTION_BML_EVENTS_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // buffer_size
    return class_size;
}

bool cACTION_BML_EVENTS_UPDATE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_buffer_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST() {
}
void cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::class_swap()
{
}

size_t cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE() {
}
void cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::class_swap()
{
}

size_t cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST() {
}
void cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::class_swap()
{
}

size_t cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE() {
}
void cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::class_swap()
{
}

size_t cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE() {
}
void cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST() {
}
void cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST() {
}
void cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::class_swap()
{
}

size_t cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE() {
}
void cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::class_swap()
{
}

size_t cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST() {
}
void cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::class_swap()
{
}

size_t cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE() {
}
void cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::class_swap()
{
}

size_t cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::~cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST() {
}
void cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::class_swap()
{
}

size_t cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::~cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE() {
}
void cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::class_swap()
{
}

size_t cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST() {
}
void cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::class_swap()
{
}

size_t cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE() {
}
void cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::class_swap()
{
}

size_t cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST() {
}
uint8_t& cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_REQUEST::~cACTION_BML_SET_CLIENT_ROAMING_REQUEST() {
}
uint8_t& cACTION_BML_SET_CLIENT_ROAMING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CLIENT_ROAMING_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_CLIENT_ROAMING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_CLIENT_ROAMING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::~cACTION_BML_SET_CLIENT_ROAMING_RESPONSE() {
}
void cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_REQUEST::~cACTION_BML_GET_CLIENT_ROAMING_REQUEST() {
}
void cACTION_BML_GET_CLIENT_ROAMING_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_CLIENT_ROAMING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_CLIENT_ROAMING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::~cACTION_BML_GET_CLIENT_ROAMING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_DFS_REENTRY_REQUEST::cACTION_BML_SET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_REQUEST::cACTION_BML_SET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_REQUEST::~cACTION_BML_SET_DFS_REENTRY_REQUEST() {
}
uint8_t& cACTION_BML_SET_DFS_REENTRY_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_DFS_REENTRY_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_DFS_REENTRY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_DFS_REENTRY_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_DFS_REENTRY_RESPONSE::cACTION_BML_SET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_RESPONSE::cACTION_BML_SET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_RESPONSE::~cACTION_BML_SET_DFS_REENTRY_RESPONSE() {
}
void cACTION_BML_SET_DFS_REENTRY_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_DFS_REENTRY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_DFS_REENTRY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_DFS_REENTRY_REQUEST::cACTION_BML_GET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_REQUEST::cACTION_BML_GET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_REQUEST::~cACTION_BML_GET_DFS_REENTRY_REQUEST() {
}
void cACTION_BML_GET_DFS_REENTRY_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_DFS_REENTRY_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_DFS_REENTRY_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_DFS_REENTRY_RESPONSE::cACTION_BML_GET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_RESPONSE::cACTION_BML_GET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_RESPONSE::~cACTION_BML_GET_DFS_REENTRY_RESPONSE() {
}
uint8_t& cACTION_BML_GET_DFS_REENTRY_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_DFS_REENTRY_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_DFS_REENTRY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_DFS_REENTRY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST() {
}
uint8_t& cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE() {
}
void cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST() {
}
void cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::~cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST() {
}
uint8_t& cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::~cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE() {
}
void cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::~cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST() {
}
void cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::~cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_IRE_ROAMING_REQUEST::cACTION_BML_SET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_REQUEST::cACTION_BML_SET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_REQUEST::~cACTION_BML_SET_IRE_ROAMING_REQUEST() {
}
uint8_t& cACTION_BML_SET_IRE_ROAMING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_IRE_ROAMING_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_IRE_ROAMING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_IRE_ROAMING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_IRE_ROAMING_RESPONSE::cACTION_BML_SET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_RESPONSE::cACTION_BML_SET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_RESPONSE::~cACTION_BML_SET_IRE_ROAMING_RESPONSE() {
}
void cACTION_BML_SET_IRE_ROAMING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_IRE_ROAMING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_IRE_ROAMING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_IRE_ROAMING_REQUEST::cACTION_BML_GET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_REQUEST::cACTION_BML_GET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_REQUEST::~cACTION_BML_GET_IRE_ROAMING_REQUEST() {
}
void cACTION_BML_GET_IRE_ROAMING_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_IRE_ROAMING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_IRE_ROAMING_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_IRE_ROAMING_RESPONSE::cACTION_BML_GET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_RESPONSE::cACTION_BML_GET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_RESPONSE::~cACTION_BML_GET_IRE_ROAMING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_IRE_ROAMING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_IRE_ROAMING_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_IRE_ROAMING_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_IRE_ROAMING_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LOAD_BALANCER_REQUEST::cACTION_BML_SET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_REQUEST::cACTION_BML_SET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_REQUEST::~cACTION_BML_SET_LOAD_BALANCER_REQUEST() {
}
uint8_t& cACTION_BML_SET_LOAD_BALANCER_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_LOAD_BALANCER_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_LOAD_BALANCER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_LOAD_BALANCER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LOAD_BALANCER_RESPONSE::cACTION_BML_SET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_RESPONSE::cACTION_BML_SET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_RESPONSE::~cACTION_BML_SET_LOAD_BALANCER_RESPONSE() {
}
void cACTION_BML_SET_LOAD_BALANCER_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_LOAD_BALANCER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_LOAD_BALANCER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LOAD_BALANCER_REQUEST::cACTION_BML_GET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_REQUEST::cACTION_BML_GET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_REQUEST::~cACTION_BML_GET_LOAD_BALANCER_REQUEST() {
}
void cACTION_BML_GET_LOAD_BALANCER_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_LOAD_BALANCER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_LOAD_BALANCER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LOAD_BALANCER_RESPONSE::cACTION_BML_GET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_RESPONSE::cACTION_BML_GET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_RESPONSE::~cACTION_BML_GET_LOAD_BALANCER_RESPONSE() {
}
uint8_t& cACTION_BML_GET_LOAD_BALANCER_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_LOAD_BALANCER_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_LOAD_BALANCER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_LOAD_BALANCER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::~cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST() {
}
uint8_t& cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::~cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE() {
}
void cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::~cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST() {
}
void cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::~cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE() {
}
uint8_t& cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST() {
}
sLoggingLevelChange& cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::params() {
    return (sLoggingLevelChange&)(*m_params);
}

void cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLoggingLevelChange); // params
    return class_size;
}

bool cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::init()
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
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE() {
}
void cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::class_swap()
{
}

size_t cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::~cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST() {
}
sWifiCredentials& cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::params() {
    return (sWifiCredentials&)(*m_params);
}

void cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sWifiCredentials); // params
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sWifiCredentials*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWifiCredentials))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sWifiCredentials) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::~cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE() {
}
uint32_t& cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::~cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST() {
}
sRestrictedChannels& cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::params() {
    return (sRestrictedChannels&)(*m_params);
}

void cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sRestrictedChannels); // params
    return class_size;
}

bool cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sRestrictedChannels*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sRestrictedChannels))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRestrictedChannels) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::~cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE() {
}
uint32_t& cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::~cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST() {
}
sRestrictedChannels& cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::params() {
    return (sRestrictedChannels&)(*m_params);
}

void cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sRestrictedChannels); // params
    return class_size;
}

bool cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sRestrictedChannels*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sRestrictedChannels))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRestrictedChannels) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::~cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE() {
}
sRestrictedChannels& cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::params() {
    return (sRestrictedChannels&)(*m_params);
}

void cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sRestrictedChannels); // params
    return class_size;
}

bool cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sRestrictedChannels*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sRestrictedChannels))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRestrictedChannels) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::cACTION_BML_SET_CERTIFICATION_MODE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::cACTION_BML_SET_CERTIFICATION_MODE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::~cACTION_BML_SET_CERTIFICATION_MODE_REQUEST() {
}
uint8_t& cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::class_swap()
{
}

size_t cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::~cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE() {
}
void cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::class_swap()
{
}

size_t cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::cACTION_BML_GET_CERTIFICATION_MODE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::cACTION_BML_GET_CERTIFICATION_MODE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::~cACTION_BML_GET_CERTIFICATION_MODE_REQUEST() {
}
void cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::class_swap()
{
}

size_t cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::~cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::class_swap()
{
}

size_t cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::~cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST() {
}
uint32_t& cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::result() {
    return (uint32_t&)(*m_result);
}

uint8_t& cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::vap_list_size() {
    return (uint8_t&)(*m_vap_list_size);
}

std::tuple<bool, sConfigVapInfo&> cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::vap_list(size_t idx) {
    bool ret_success = ( (m_vap_list_idx__ > 0) && (m_vap_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_vap_list[ret_idx]);
}

bool cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::alloc_vap_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list vap_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sConfigVapInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_vap_list[*m_vap_list_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_vap_list_idx__ += count;
    *m_vap_list_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_vap_list_idx__ - count; i < m_vap_list_idx__; i++) { m_vap_list[i].struct_init(); }
    }
    return true;
}

void cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
    for (size_t i = 0; i < (size_t)*m_vap_list_size; i++){
        m_vap_list[i].struct_swap();
    }
}

size_t cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // result
    class_size += sizeof(uint8_t); // vap_list_size
    return class_size;
}

bool cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_vap_list_size = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_vap_list_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_vap_list = (sConfigVapInfo*)m_buff_ptr__;
    uint8_t vap_list_size = *m_vap_list_size;
    m_vap_list_idx__ = vap_list_size;
    if (!buffPtrIncrementSafe(sizeof(sConfigVapInfo) * (vap_list_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sConfigVapInfo) * (vap_list_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::~cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE() {
}
uint32_t& cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::~cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE() {
}
uint32_t& cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

uint8_t& cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::vap_list_size() {
    return (uint8_t&)(*m_vap_list_size);
}

std::tuple<bool, sConfigVapInfo&> cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::vap_list(size_t idx) {
    bool ret_success = ( (m_vap_list_idx__ > 0) && (m_vap_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_vap_list[ret_idx]);
}

bool cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::alloc_vap_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list vap_list, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sConfigVapInfo) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_vap_list[*m_vap_list_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_vap_list_idx__ += count;
    *m_vap_list_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_vap_list_idx__ - count; i < m_vap_list_idx__; i++) { m_vap_list[i].struct_init(); }
    }
    return true;
}

void cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
    for (size_t i = 0; i < (size_t)*m_vap_list_size; i++){
        m_vap_list[i].struct_swap();
    }
}

size_t cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // result
    class_size += sizeof(uint8_t); // vap_list_size
    return class_size;
}

bool cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_vap_list_size = (uint8_t*)m_buff_ptr__;
    if (!m_parse__) *m_vap_list_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_vap_list = (sConfigVapInfo*)m_buff_ptr__;
    uint8_t vap_list_size = *m_vap_list_size;
    m_vap_list_idx__ = vap_list_size;
    if (!buffPtrIncrementSafe(sizeof(sConfigVapInfo) * (vap_list_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sConfigVapInfo) * (vap_list_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::~cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST() {
}
uint32_t& cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

size_t cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_SET_GROUP_REQUEST::cACTION_BML_STEERING_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_REQUEST::cACTION_BML_STEERING_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_REQUEST::~cACTION_BML_STEERING_SET_GROUP_REQUEST() {
}
uint32_t& cACTION_BML_STEERING_SET_GROUP_REQUEST::steeringGroupIndex() {
    return (uint32_t&)(*m_steeringGroupIndex);
}

sSteeringApConfig& cACTION_BML_STEERING_SET_GROUP_REQUEST::cfg_2() {
    return (sSteeringApConfig&)(*m_cfg_2);
}

sSteeringApConfig& cACTION_BML_STEERING_SET_GROUP_REQUEST::cfg_5() {
    return (sSteeringApConfig&)(*m_cfg_5);
}

uint8_t& cACTION_BML_STEERING_SET_GROUP_REQUEST::remove() {
    return (uint8_t&)(*m_remove);
}

void cACTION_BML_STEERING_SET_GROUP_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_cfg_2->struct_swap();
    m_cfg_5->struct_swap();
}

size_t cACTION_BML_STEERING_SET_GROUP_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // steeringGroupIndex
    class_size += sizeof(sSteeringApConfig); // cfg_2
    class_size += sizeof(sSteeringApConfig); // cfg_5
    class_size += sizeof(uint8_t); // remove
    return class_size;
}

bool cACTION_BML_STEERING_SET_GROUP_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_steeringGroupIndex = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_cfg_2 = (sSteeringApConfig*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringApConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringApConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cfg_2->struct_init(); }
    m_cfg_5 = (sSteeringApConfig*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringApConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringApConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cfg_5->struct_init(); }
    m_remove = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_SET_GROUP_RESPONSE::cACTION_BML_STEERING_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_RESPONSE::cACTION_BML_STEERING_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_RESPONSE::~cACTION_BML_STEERING_SET_GROUP_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_SET_GROUP_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_SET_GROUP_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_STEERING_SET_GROUP_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int32_t); // error_code
    return class_size;
}

bool cACTION_BML_STEERING_SET_GROUP_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (int32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_SET_REQUEST::cACTION_BML_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_REQUEST::cACTION_BML_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_REQUEST::~cACTION_BML_STEERING_CLIENT_SET_REQUEST() {
}
uint32_t& cACTION_BML_STEERING_CLIENT_SET_REQUEST::steeringGroupIndex() {
    return (uint32_t&)(*m_steeringGroupIndex);
}

sMacAddr& cACTION_BML_STEERING_CLIENT_SET_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

sMacAddr& cACTION_BML_STEERING_CLIENT_SET_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

sSteeringClientConfig& cACTION_BML_STEERING_CLIENT_SET_REQUEST::config() {
    return (sSteeringClientConfig&)(*m_config);
}

uint8_t& cACTION_BML_STEERING_CLIENT_SET_REQUEST::remove() {
    return (uint8_t&)(*m_remove);
}

void cACTION_BML_STEERING_CLIENT_SET_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_bssid->struct_swap();
    m_client_mac->struct_swap();
    m_config->struct_swap();
}

size_t cACTION_BML_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // steeringGroupIndex
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(sSteeringClientConfig); // config
    class_size += sizeof(uint8_t); // remove
    return class_size;
}

bool cACTION_BML_STEERING_CLIENT_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_steeringGroupIndex = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_config = (sSteeringClientConfig*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringClientConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_config->struct_init(); }
    m_remove = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_SET_RESPONSE::cACTION_BML_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_RESPONSE::cACTION_BML_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_RESPONSE::~cACTION_BML_STEERING_CLIENT_SET_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_CLIENT_SET_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_CLIENT_SET_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int32_t); // error_code
    return class_size;
}

bool cACTION_BML_STEERING_CLIENT_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (int32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST() {
}
uint8_t& cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::unregister() {
    return (uint8_t&)(*m_unregister);
}

void cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::class_swap()
{
}

size_t cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // unregister
    return class_size;
}

bool cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_unregister = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int32_t); // error_code
    return class_size;
}

bool cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (int32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::~cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST() {
}
uint32_t& cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::steeringGroupIndex() {
    return (uint32_t&)(*m_steeringGroupIndex);
}

sMacAddr& cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

sMacAddr& cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

eDisconnectType& cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::type() {
    return (eDisconnectType&)(*m_type);
}

uint32_t& cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

void cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_bssid->struct_swap();
    m_client_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
}

size_t cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // steeringGroupIndex
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(sMacAddr); // client_mac
    class_size += sizeof(eDisconnectType); // type
    class_size += sizeof(uint32_t); // reason
    return class_size;
}

bool cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_steeringGroupIndex = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_type = (eDisconnectType*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(eDisconnectType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eDisconnectType) << ") Failed!";
        return false;
    }
    m_reason = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::~cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int32_t); // error_code
    return class_size;
}

bool cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (int32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::~cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST() {
}
uint32_t& cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::steeringGroupIndex() {
    return (uint32_t&)(*m_steeringGroupIndex);
}

sMacAddr& cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

sMacAddr& cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::client_mac() {
    return (sMacAddr&)(*m_client_mac);
}

void cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_bssid->struct_swap();
    m_client_mac->struct_swap();
}

size_t cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // steeringGroupIndex
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(sMacAddr); // client_mac
    return class_size;
}

bool cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_steeringGroupIndex = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_client_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::~cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

size_t cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int32_t); // error_code
    return class_size;
}

bool cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (int32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_EVENTS_UPDATE::cACTION_BML_STEERING_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENTS_UPDATE::cACTION_BML_STEERING_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENTS_UPDATE::~cACTION_BML_STEERING_EVENTS_UPDATE() {
}
uint32_t& cACTION_BML_STEERING_EVENTS_UPDATE::buffer_size() {
    return (uint32_t&)(*m_buffer_size);
}

std::string cACTION_BML_STEERING_EVENTS_UPDATE::buffer_str() {
    char *buffer_ = buffer();
    if (!buffer_) { return std::string(); }
    return std::string(buffer_, m_buffer_idx__);
}

char* cACTION_BML_STEERING_EVENTS_UPDATE::buffer(size_t length) {
    if( (m_buffer_idx__ <= 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_STEERING_EVENTS_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_STEERING_EVENTS_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr || size == 0) {
        TLVF_LOG(WARNING) << "set_buffer received an empty string.";
        return false;
    }
    if (!alloc_buffer(size)) { return false; }
    std::copy(str, str + size, m_buffer);
    return true;
}
bool cACTION_BML_STEERING_EVENTS_UPDATE::alloc_buffer(size_t count) {
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
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BML_STEERING_EVENTS_UPDATE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

size_t cACTION_BML_STEERING_EVENTS_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // buffer_size
    return class_size;
}

bool cACTION_BML_STEERING_EVENTS_UPDATE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_buffer_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_TRIGGER_TOPOLOGY_QUERY::cACTION_BML_TRIGGER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_TOPOLOGY_QUERY::cACTION_BML_TRIGGER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_TOPOLOGY_QUERY::~cACTION_BML_TRIGGER_TOPOLOGY_QUERY() {
}
sMacAddr& cACTION_BML_TRIGGER_TOPOLOGY_QUERY::al_mac() {
    return (sMacAddr&)(*m_al_mac);
}

void cACTION_BML_TRIGGER_TOPOLOGY_QUERY::class_swap()
{
    m_al_mac->struct_swap();
}

size_t cACTION_BML_TRIGGER_TOPOLOGY_QUERY::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // al_mac
    return class_size;
}

bool cACTION_BML_TRIGGER_TOPOLOGY_QUERY::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_al_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::~cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST() {
}
sMacAddr& cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::al_mac() {
    return (sMacAddr&)(*m_al_mac);
}

sMacAddr& cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::ruid() {
    return (sMacAddr&)(*m_ruid);
}

void cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::class_swap()
{
    m_al_mac->struct_swap();
    m_ruid->struct_swap();
}

size_t cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // al_mac
    class_size += sizeof(sMacAddr); // ruid
    return class_size;
}

bool cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_al_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    m_ruid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_ruid->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::~cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST() {
}
sMacAddr& cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

sDcsScanRequestParams& cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::params() {
    return (sDcsScanRequestParams&)(*m_params);
}

void cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::class_swap()
{
    m_radio_mac->struct_swap();
    m_params->struct_swap();
}

size_t cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(sDcsScanRequestParams); // params
    return class_size;
}

bool cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_params = (sDcsScanRequestParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sDcsScanRequestParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDcsScanRequestParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::~cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE() {
}
uint8_t& cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

void cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::class_swap()
{
}

size_t cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // op_error_code
    return class_size;
}

bool cACTION_BML_DCS_SET_CONTINUOUS_PARAMS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_op_error_code = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::~cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST() {
}
sMacAddr& cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::class_swap()
{
    m_radio_mac->struct_swap();
}

size_t cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::~cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE() {
}
sDcsScanRequestParams& cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::params() {
    return (sDcsScanRequestParams&)(*m_params);
}

void cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDcsScanRequestParams); // params
    return class_size;
}

bool cACTION_BML_DCS_GET_CONTINUOUS_PARAMS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDcsScanRequestParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sDcsScanRequestParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDcsScanRequestParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::~cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST() {
}
sMacAddr& cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

uint8_t& cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::class_swap()
{
    m_radio_mac->struct_swap();
}

size_t cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::~cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE() {
}
uint8_t& cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

void cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::class_swap()
{
}

size_t cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // op_error_code
    return class_size;
}

bool cACTION_BML_DCS_SET_CONTINUOUS_SCAN_ENABLE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_op_error_code = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::~cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST() {
}
sMacAddr& cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::class_swap()
{
    m_radio_mac->struct_swap();
}

size_t cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::~cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE() {
}
uint8_t& cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::class_swap()
{
}

size_t cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_DCS_GET_CONTINUOUS_SCAN_ENABLE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::~cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST() {
}
sMacAddr& cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

uint8_t& cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::scan_mode() {
    return (uint8_t&)(*m_scan_mode);
}

void cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::class_swap()
{
    m_radio_mac->struct_swap();
}

size_t cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(uint8_t); // scan_mode
    return class_size;
}

bool cACTION_BML_DCS_GET_SCAN_RESULTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_scan_mode = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::~cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE() {
}
uint8_t& cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::result_status() {
    return (uint8_t&)(*m_result_status);
}

uint8_t& cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

uint8_t& cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::last() {
    return (uint8_t&)(*m_last);
}

uint32_t& cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::results_size() {
    return (uint32_t&)(*m_results_size);
}

std::tuple<bool, sDcsChannelScanResults&> cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::results(size_t idx) {
    bool ret_success = ( (m_results_idx__ > 0) && (m_results_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_results[ret_idx]);
}

bool cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::alloc_results(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list results, abort!";
        return false;
    }
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sDcsChannelScanResults) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_results[*m_results_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_results_idx__ += count;
    *m_results_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_results_idx__ - count; i < m_results_idx__; i++) { m_results[i].struct_init(); }
    }
    return true;
}

void cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_results_size));
    for (size_t i = 0; i < (size_t)*m_results_size; i++){
        m_results[i].struct_swap();
    }
}

size_t cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result_status
    class_size += sizeof(uint8_t); // op_error_code
    class_size += sizeof(uint8_t); // last
    class_size += sizeof(uint32_t); // results_size
    return class_size;
}

bool cACTION_BML_DCS_GET_SCAN_RESULTS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result_status = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_op_error_code = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_last = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_results_size = (uint32_t*)m_buff_ptr__;
    if (!m_parse__) *m_results_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_results = (sDcsChannelScanResults*)m_buff_ptr__;
    uint32_t results_size = *m_results_size;
    if (m_parse__ && m_swap__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&results_size)); }
    m_results_idx__ = results_size;
    if (!buffPtrIncrementSafe(sizeof(sDcsChannelScanResults) * (results_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDcsChannelScanResults) * (results_size) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_START_SCAN_REQUEST::cACTION_BML_DCS_START_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_START_SCAN_REQUEST::cACTION_BML_DCS_START_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_START_SCAN_REQUEST::~cACTION_BML_DCS_START_SCAN_REQUEST() {
}
sDcsTriggerScanParams& cACTION_BML_DCS_START_SCAN_REQUEST::scan_params() {
    return (sDcsTriggerScanParams&)(*m_scan_params);
}

void cACTION_BML_DCS_START_SCAN_REQUEST::class_swap()
{
    m_scan_params->struct_swap();
}

size_t cACTION_BML_DCS_START_SCAN_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDcsTriggerScanParams); // scan_params
    return class_size;
}

bool cACTION_BML_DCS_START_SCAN_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_scan_params = (sDcsTriggerScanParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sDcsTriggerScanParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDcsTriggerScanParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_scan_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_START_SCAN_RESPONSE::cACTION_BML_DCS_START_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_START_SCAN_RESPONSE::cACTION_BML_DCS_START_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_START_SCAN_RESPONSE::~cACTION_BML_DCS_START_SCAN_RESPONSE() {
}
uint8_t& cACTION_BML_DCS_START_SCAN_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

void cACTION_BML_DCS_START_SCAN_RESPONSE::class_swap()
{
}

size_t cACTION_BML_DCS_START_SCAN_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // op_error_code
    return class_size;
}

bool cACTION_BML_DCS_START_SCAN_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_op_error_code = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST::cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST::cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST::~cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST() {
}
void cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST::class_swap()
{
}

size_t cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_DCS_DUMP_SCAN_RESULTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE::cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE::cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE::~cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE() {
}
void cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE::class_swap()
{
}

size_t cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_DCS_DUMP_SCAN_RESULTS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


