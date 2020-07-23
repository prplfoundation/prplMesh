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

#include <beerocks/tlvf/beerocks_message_bml.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_BML_PING_REQUEST::cACTION_BML_PING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_PING_REQUEST::cACTION_BML_PING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_PING_REQUEST::~cACTION_BML_PING_REQUEST() {
}
void cACTION_BML_PING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_PING_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_PING_RESPONSE::cACTION_BML_PING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_PING_RESPONSE::cACTION_BML_PING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_PING_RESPONSE::~cACTION_BML_PING_RESPONSE() {
}
void cACTION_BML_PING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_PING_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_NW_MAP_REQUEST::cACTION_BML_NW_MAP_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_REQUEST::cACTION_BML_NW_MAP_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_REQUEST::~cACTION_BML_NW_MAP_REQUEST() {
}
void cACTION_BML_NW_MAP_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_NW_MAP_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_NW_MAP_RESPONSE::cACTION_BML_NW_MAP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_RESPONSE::cACTION_BML_NW_MAP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_buffer_idx__ == 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_NW_MAP_RESPONSE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_NW_MAP_RESPONSE::set_buffer(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_buffer received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_node_num));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

bool cACTION_BML_NW_MAP_RESPONSE::finalize()
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
    m_node_num = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer_size = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_NW_MAP_UPDATE::cACTION_BML_NW_MAP_UPDATE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_NW_MAP_UPDATE::cACTION_BML_NW_MAP_UPDATE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_buffer_idx__ == 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_NW_MAP_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_NW_MAP_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_buffer received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_node_num));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

bool cACTION_BML_NW_MAP_UPDATE::finalize()
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
    m_node_num = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer_size = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STATS_UPDATE::cACTION_BML_STATS_UPDATE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STATS_UPDATE::cACTION_BML_STATS_UPDATE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_buffer_idx__ == 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_STATS_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_STATS_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_buffer received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_num_of_stats_bulks));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

bool cACTION_BML_STATS_UPDATE::finalize()
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
    m_num_of_stats_bulks = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer_size = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_EVENTS_UPDATE::cACTION_BML_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_EVENTS_UPDATE::cACTION_BML_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_buffer_idx__ == 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_EVENTS_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_EVENTS_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_buffer received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

bool cACTION_BML_EVENTS_UPDATE::finalize()
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
    m_buffer_size = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST() {
}
void cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::~cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE() {
}
void cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST() {
}
void cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::~cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE() {
}
void cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE() {
}
void cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST() {
}
void cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST() {
}
void cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::~cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE() {
}
void cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TO_EVENTS_UPDATES_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST() {
}
void cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::~cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE() {
}
void cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::~cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST() {
}
void cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::~cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE() {
}
void cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TO_STATS_UPDATES_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST() {
}
void cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::~cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE() {
}
void cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::~cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST() {
}
uint8_t& cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::~cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_REQUEST::~cACTION_BML_SET_CLIENT_ROAMING_REQUEST() {
}
uint8_t& cACTION_BML_SET_CLIENT_ROAMING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CLIENT_ROAMING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CLIENT_ROAMING_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::~cACTION_BML_SET_CLIENT_ROAMING_RESPONSE() {
}
void cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CLIENT_ROAMING_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_REQUEST::~cACTION_BML_GET_CLIENT_ROAMING_REQUEST() {
}
void cACTION_BML_GET_CLIENT_ROAMING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CLIENT_ROAMING_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::~cACTION_BML_GET_CLIENT_ROAMING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CLIENT_ROAMING_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_DFS_REENTRY_REQUEST::cACTION_BML_SET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_REQUEST::cACTION_BML_SET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_REQUEST::~cACTION_BML_SET_DFS_REENTRY_REQUEST() {
}
uint8_t& cACTION_BML_SET_DFS_REENTRY_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_DFS_REENTRY_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_DFS_REENTRY_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_DFS_REENTRY_RESPONSE::cACTION_BML_SET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_RESPONSE::cACTION_BML_SET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_DFS_REENTRY_RESPONSE::~cACTION_BML_SET_DFS_REENTRY_RESPONSE() {
}
void cACTION_BML_SET_DFS_REENTRY_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_DFS_REENTRY_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_DFS_REENTRY_REQUEST::cACTION_BML_GET_DFS_REENTRY_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_REQUEST::cACTION_BML_GET_DFS_REENTRY_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_REQUEST::~cACTION_BML_GET_DFS_REENTRY_REQUEST() {
}
void cACTION_BML_GET_DFS_REENTRY_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_DFS_REENTRY_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_DFS_REENTRY_RESPONSE::cACTION_BML_GET_DFS_REENTRY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_RESPONSE::cACTION_BML_GET_DFS_REENTRY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_DFS_REENTRY_RESPONSE::~cACTION_BML_GET_DFS_REENTRY_RESPONSE() {
}
uint8_t& cACTION_BML_GET_DFS_REENTRY_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_DFS_REENTRY_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_DFS_REENTRY_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST() {
}
uint8_t& cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::~cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE() {
}
void cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST() {
}
void cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::~cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::~cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST() {
}
uint8_t& cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::~cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE() {
}
void cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::~cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST() {
}
void cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::~cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_IRE_ROAMING_REQUEST::cACTION_BML_SET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_REQUEST::cACTION_BML_SET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_REQUEST::~cACTION_BML_SET_IRE_ROAMING_REQUEST() {
}
uint8_t& cACTION_BML_SET_IRE_ROAMING_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_IRE_ROAMING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_IRE_ROAMING_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_IRE_ROAMING_RESPONSE::cACTION_BML_SET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_RESPONSE::cACTION_BML_SET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_IRE_ROAMING_RESPONSE::~cACTION_BML_SET_IRE_ROAMING_RESPONSE() {
}
void cACTION_BML_SET_IRE_ROAMING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_IRE_ROAMING_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_IRE_ROAMING_REQUEST::cACTION_BML_GET_IRE_ROAMING_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_REQUEST::cACTION_BML_GET_IRE_ROAMING_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_REQUEST::~cACTION_BML_GET_IRE_ROAMING_REQUEST() {
}
void cACTION_BML_GET_IRE_ROAMING_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_IRE_ROAMING_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_IRE_ROAMING_RESPONSE::cACTION_BML_GET_IRE_ROAMING_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_RESPONSE::cACTION_BML_GET_IRE_ROAMING_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_IRE_ROAMING_RESPONSE::~cACTION_BML_GET_IRE_ROAMING_RESPONSE() {
}
uint8_t& cACTION_BML_GET_IRE_ROAMING_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_IRE_ROAMING_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_IRE_ROAMING_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LOAD_BALANCER_REQUEST::cACTION_BML_SET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_REQUEST::cACTION_BML_SET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_REQUEST::~cACTION_BML_SET_LOAD_BALANCER_REQUEST() {
}
uint8_t& cACTION_BML_SET_LOAD_BALANCER_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_LOAD_BALANCER_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_LOAD_BALANCER_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_LOAD_BALANCER_RESPONSE::cACTION_BML_SET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_RESPONSE::cACTION_BML_SET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_LOAD_BALANCER_RESPONSE::~cACTION_BML_SET_LOAD_BALANCER_RESPONSE() {
}
void cACTION_BML_SET_LOAD_BALANCER_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_LOAD_BALANCER_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LOAD_BALANCER_REQUEST::cACTION_BML_GET_LOAD_BALANCER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_REQUEST::cACTION_BML_GET_LOAD_BALANCER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_REQUEST::~cACTION_BML_GET_LOAD_BALANCER_REQUEST() {
}
void cACTION_BML_GET_LOAD_BALANCER_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_LOAD_BALANCER_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_LOAD_BALANCER_RESPONSE::cACTION_BML_GET_LOAD_BALANCER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_RESPONSE::cACTION_BML_GET_LOAD_BALANCER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_LOAD_BALANCER_RESPONSE::~cACTION_BML_GET_LOAD_BALANCER_RESPONSE() {
}
uint8_t& cACTION_BML_GET_LOAD_BALANCER_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_LOAD_BALANCER_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_LOAD_BALANCER_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::~cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST() {
}
uint8_t& cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::~cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE() {
}
void cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::~cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST() {
}
void cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::~cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE() {
}
uint8_t& cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST() {
}
sLoggingLevelChange& cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::params() {
    return (sLoggingLevelChange&)(*m_params);
}

void cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST::finalize()
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
    m_params = reinterpret_cast<sLoggingLevelChange*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sLoggingLevelChange))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sLoggingLevelChange) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::~cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE() {
}
void cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::~cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST() {
}
sMacAddr& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::al_mac() {
    return (sMacAddr&)(*m_al_mac);
}

uint16_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::authentication_type() {
    return (uint16_t&)(*m_authentication_type);
}

uint16_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::encryption_type() {
    return (uint16_t&)(*m_encryption_type);
}

uint8_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::fronthaul() {
    return (uint8_t&)(*m_fronthaul);
}

uint8_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::backhaul() {
    return (uint8_t&)(*m_backhaul);
}

uint8_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::ssid_size() {
    return (uint8_t&)(*m_ssid_size);
}

std::string cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::ssid_str() {
    char *ssid_ = ssid();
    if (!ssid_) { return std::string(); }
    return std::string(ssid_, m_ssid_idx__);
}

char* cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::ssid(size_t length) {
    if( (m_ssid_idx__ == 0) || (m_ssid_idx__ < length) ) {
        TLVF_LOG(ERROR) << "ssid length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_ssid);
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::set_ssid(const std::string& str) { return set_ssid(str.c_str(), str.size()); }
bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::set_ssid(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_ssid received a null pointer.";
        return false;
    }
    if (!alloc_ssid(size)) { return false; }
    std::copy(str, str + size, m_ssid);
    return true;
}
bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::alloc_ssid(size_t count) {
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
    uint8_t *src = (uint8_t *)&m_ssid[*m_ssid_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_network_key_size = (uint8_t *)((uint8_t *)(m_network_key_size) + len);
    m_network_key = (char *)((uint8_t *)(m_network_key) + len);
    m_operating_classes_size = (uint8_t *)((uint8_t *)(m_operating_classes_size) + len);
    m_operating_classes = (uint8_t *)((uint8_t *)(m_operating_classes) + len);
    m_ssid_idx__ += count;
    *m_ssid_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

uint8_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::network_key_size() {
    return (uint8_t&)(*m_network_key_size);
}

std::string cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::network_key_str() {
    char *network_key_ = network_key();
    if (!network_key_) { return std::string(); }
    return std::string(network_key_, m_network_key_idx__);
}

char* cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::network_key(size_t length) {
    if( (m_network_key_idx__ == 0) || (m_network_key_idx__ < length) ) {
        TLVF_LOG(ERROR) << "network_key length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_network_key);
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::set_network_key(const std::string& str) { return set_network_key(str.c_str(), str.size()); }
bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::set_network_key(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_network_key received a null pointer.";
        return false;
    }
    if (!alloc_network_key(size)) { return false; }
    std::copy(str, str + size, m_network_key);
    return true;
}
bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::alloc_network_key(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list network_key, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_network_key[*m_network_key_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_operating_classes_size = (uint8_t *)((uint8_t *)(m_operating_classes_size) + len);
    m_operating_classes = (uint8_t *)((uint8_t *)(m_operating_classes) + len);
    m_network_key_idx__ += count;
    *m_network_key_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

uint8_t& cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::operating_classes_size() {
    return (uint8_t&)(*m_operating_classes_size);
}

uint8_t* cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::operating_classes(size_t idx) {
    if ( (m_operating_classes_idx__ == 0) || (m_operating_classes_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_operating_classes[idx]);
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::set_operating_classes(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_operating_classes received a null pointer.";
        return false;
    }
    if (!alloc_operating_classes(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_operating_classes);
    return true;
}
bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::alloc_operating_classes(size_t count) {
    if (m_lock_order_counter__ > 2) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list operating_classes, abort!";
        return false;
    }
    size_t len = sizeof(uint8_t) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 2;
    uint8_t *src = (uint8_t *)&m_operating_classes[*m_operating_classes_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_operating_classes_idx__ += count;
    *m_operating_classes_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_al_mac->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_authentication_type));
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_encryption_type));
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::finalize()
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

size_t cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // al_mac
    class_size += sizeof(uint16_t); // authentication_type
    class_size += sizeof(uint16_t); // encryption_type
    class_size += sizeof(uint8_t); // fronthaul
    class_size += sizeof(uint8_t); // backhaul
    class_size += sizeof(uint8_t); // ssid_size
    class_size += sizeof(uint8_t); // network_key_size
    class_size += sizeof(uint8_t); // operating_classes_size
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_al_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    m_authentication_type = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_encryption_type = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    m_fronthaul = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_backhaul = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_ssid_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_ssid_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_ssid = (char*)m_buff_ptr__;
    uint8_t ssid_size = *m_ssid_size;
    m_ssid_idx__ = ssid_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (ssid_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (ssid_size) << ") Failed!";
        return false;
    }
    m_network_key_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_network_key_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_network_key = (char*)m_buff_ptr__;
    uint8_t network_key_size = *m_network_key_size;
    m_network_key_idx__ = network_key_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (network_key_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (network_key_size) << ") Failed!";
        return false;
    }
    m_operating_classes_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_operating_classes_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_operating_classes = (uint8_t*)m_buff_ptr__;
    uint8_t operating_classes_size = *m_operating_classes_size;
    m_operating_classes_idx__ = operating_classes_size;
    if (!buffPtrIncrementSafe(sizeof(uint8_t) * (operating_classes_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) * (operating_classes_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::~cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE() {
}
uint32_t& cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::finalize()
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

size_t cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::~cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST() {
}
sRestrictedChannels& cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::params() {
    return (sRestrictedChannels&)(*m_params);
}

void cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST::finalize()
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
    m_params = reinterpret_cast<sRestrictedChannels*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sRestrictedChannels))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRestrictedChannels) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::~cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE() {
}
uint32_t& cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::~cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST() {
}
sRestrictedChannels& cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::params() {
    return (sRestrictedChannels&)(*m_params);
}

void cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST::finalize()
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
    m_params = reinterpret_cast<sRestrictedChannels*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sRestrictedChannels))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRestrictedChannels) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::~cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE() {
}
sRestrictedChannels& cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::params() {
    return (sRestrictedChannels&)(*m_params);
}

void cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE::finalize()
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
    m_params = reinterpret_cast<sRestrictedChannels*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sRestrictedChannels))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sRestrictedChannels) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::cACTION_BML_SET_CERTIFICATION_MODE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::cACTION_BML_SET_CERTIFICATION_MODE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::~cACTION_BML_SET_CERTIFICATION_MODE_REQUEST() {
}
uint8_t& cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CERTIFICATION_MODE_REQUEST::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::~cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE() {
}
void cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_SET_CERTIFICATION_MODE_RESPONSE::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::cACTION_BML_GET_CERTIFICATION_MODE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::cACTION_BML_GET_CERTIFICATION_MODE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::~cACTION_BML_GET_CERTIFICATION_MODE_REQUEST() {
}
void cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CERTIFICATION_MODE_REQUEST::finalize()
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::~cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE() {
}
uint8_t& cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE::finalize()
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
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
    for (size_t i = 0; i < m_vap_list_idx__; i++){
        m_vap_list[i].struct_swap();
    }
}

bool cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST::finalize()
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
    m_result = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_vap_list_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::~cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE() {
}
uint32_t& cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE::finalize()
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
    m_result = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
    for (size_t i = 0; i < m_vap_list_idx__; i++){
        m_vap_list[i].struct_swap();
    }
}

bool cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE::finalize()
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
    m_result = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_vap_list_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
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
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::~cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST() {
}
uint32_t& cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST::finalize()
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
    m_result = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_SET_GROUP_REQUEST::cACTION_BML_STEERING_SET_GROUP_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_REQUEST::cACTION_BML_STEERING_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_cfg_2->struct_swap();
    m_cfg_5->struct_swap();
}

bool cACTION_BML_STEERING_SET_GROUP_REQUEST::finalize()
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
    m_steeringGroupIndex = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_cfg_2 = reinterpret_cast<sSteeringApConfig*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringApConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringApConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cfg_2->struct_init(); }
    m_cfg_5 = reinterpret_cast<sSteeringApConfig*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringApConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringApConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cfg_5->struct_init(); }
    m_remove = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_SET_GROUP_RESPONSE::cACTION_BML_STEERING_SET_GROUP_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_RESPONSE::cACTION_BML_STEERING_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_SET_GROUP_RESPONSE::~cACTION_BML_STEERING_SET_GROUP_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_SET_GROUP_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_SET_GROUP_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_STEERING_SET_GROUP_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<int32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_SET_REQUEST::cACTION_BML_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_REQUEST::cACTION_BML_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_bssid->struct_swap();
    m_client_mac->struct_swap();
    m_config->struct_swap();
}

bool cACTION_BML_STEERING_CLIENT_SET_REQUEST::finalize()
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
    m_steeringGroupIndex = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_client_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_config = reinterpret_cast<sSteeringClientConfig*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringClientConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_config->struct_init(); }
    m_remove = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_SET_RESPONSE::cACTION_BML_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_RESPONSE::cACTION_BML_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_SET_RESPONSE::~cACTION_BML_STEERING_CLIENT_SET_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_CLIENT_SET_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_CLIENT_SET_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_STEERING_CLIENT_SET_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<int32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST() {
}
uint8_t& cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::unregister() {
    return (uint8_t&)(*m_unregister);
}

void cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_REQUEST::finalize()
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
    m_unregister = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::~cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_STEERING_EVENT_REGISTER_UNREGISTER_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<int32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_bssid->struct_swap();
    m_client_mac->struct_swap();
    tlvf_swap(8*sizeof(eDisconnectType), reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
}

bool cACTION_BML_STEERING_CLIENT_DISCONNECT_REQUEST::finalize()
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
    m_steeringGroupIndex = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_client_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    m_type = reinterpret_cast<eDisconnectType*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(eDisconnectType))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(eDisconnectType) << ") Failed!";
        return false;
    }
    m_reason = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::~cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_STEERING_CLIENT_DISCONNECT_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<int32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_steeringGroupIndex));
    m_bssid->struct_swap();
    m_client_mac->struct_swap();
}

bool cACTION_BML_STEERING_CLIENT_MEASURE_REQUEST::finalize()
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
    m_steeringGroupIndex = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_client_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::~cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE() {
}
int32_t& cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::error_code() {
    return (int32_t&)(*m_error_code);
}

void cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_STEERING_CLIENT_MEASURE_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<int32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_STEERING_EVENTS_UPDATE::cACTION_BML_STEERING_EVENTS_UPDATE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_STEERING_EVENTS_UPDATE::cACTION_BML_STEERING_EVENTS_UPDATE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    if( (m_buffer_idx__ == 0) || (m_buffer_idx__ < length) ) {
        TLVF_LOG(ERROR) << "buffer length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_buffer);
}

bool cACTION_BML_STEERING_EVENTS_UPDATE::set_buffer(const std::string& str) { return set_buffer(str.c_str(), str.size()); }
bool cACTION_BML_STEERING_EVENTS_UPDATE::set_buffer(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_buffer received a null pointer.";
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_buffer_size));
}

bool cACTION_BML_STEERING_EVENTS_UPDATE::finalize()
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
    m_buffer_size = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!m_parse__) *m_buffer_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_buffer = (char*)m_buff_ptr__;
    uint32_t buffer_size = *m_buffer_size;
    if (m_parse__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&buffer_size)); }
    m_buffer_idx__ = buffer_size;
    if (!buffPtrIncrementSafe(sizeof(char) * (buffer_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (buffer_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_TRIGGER_TOPOLOGY_QUERY::cACTION_BML_TRIGGER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_TOPOLOGY_QUERY::cACTION_BML_TRIGGER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_TOPOLOGY_QUERY::~cACTION_BML_TRIGGER_TOPOLOGY_QUERY() {
}
sMacAddr& cACTION_BML_TRIGGER_TOPOLOGY_QUERY::al_mac() {
    return (sMacAddr&)(*m_al_mac);
}

void cACTION_BML_TRIGGER_TOPOLOGY_QUERY::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_al_mac->struct_swap();
}

bool cACTION_BML_TRIGGER_TOPOLOGY_QUERY::finalize()
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
    m_al_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_TOPOLOGY_RESPONSE::cACTION_BML_TOPOLOGY_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_TOPOLOGY_RESPONSE::cACTION_BML_TOPOLOGY_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_TOPOLOGY_RESPONSE::~cACTION_BML_TOPOLOGY_RESPONSE() {
}
sDeviceData& cACTION_BML_TOPOLOGY_RESPONSE::device_data() {
    return (sDeviceData&)(*m_device_data);
}

uint32_t& cACTION_BML_TOPOLOGY_RESPONSE::result() {
    return (uint32_t&)(*m_result);
}

void cACTION_BML_TOPOLOGY_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_device_data->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_result));
}

bool cACTION_BML_TOPOLOGY_RESPONSE::finalize()
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

size_t cACTION_BML_TOPOLOGY_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDeviceData); // device_data
    class_size += sizeof(uint32_t); // result
    return class_size;
}

bool cACTION_BML_TOPOLOGY_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_device_data = reinterpret_cast<sDeviceData*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sDeviceData))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDeviceData) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_device_data->struct_init(); }
    m_result = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_REGISTER_TOPOLOGY_QUERY::cACTION_BML_REGISTER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TOPOLOGY_QUERY::cACTION_BML_REGISTER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_REGISTER_TOPOLOGY_QUERY::~cACTION_BML_REGISTER_TOPOLOGY_QUERY() {
}
void cACTION_BML_REGISTER_TOPOLOGY_QUERY::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_REGISTER_TOPOLOGY_QUERY::finalize()
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

size_t cACTION_BML_REGISTER_TOPOLOGY_QUERY::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_REGISTER_TOPOLOGY_QUERY::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::cACTION_BML_UNREGISTER_TOPOLOGY_QUERY(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::cACTION_BML_UNREGISTER_TOPOLOGY_QUERY(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::~cACTION_BML_UNREGISTER_TOPOLOGY_QUERY() {
}
void cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::finalize()
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

size_t cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_UNREGISTER_TOPOLOGY_QUERY::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
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
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_al_mac->struct_swap();
    m_ruid->struct_swap();
}

bool cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST::finalize()
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
    m_al_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    m_ruid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_ruid->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST() {
}
sMacAddr& cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

sChannelScanRequestParams& cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::params() {
    return (sChannelScanRequestParams&)(*m_params);
}

void cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
    m_params->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(sChannelScanRequestParams); // params
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_params = reinterpret_cast<sChannelScanRequestParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sChannelScanRequestParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sChannelScanRequestParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE() {
}
uint8_t& cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

void cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // op_error_code
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_op_error_code = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST() {
}
sMacAddr& cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE() {
}
sChannelScanRequestParams& cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::params() {
    return (sChannelScanRequestParams&)(*m_params);
}

void cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sChannelScanRequestParams); // params
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sChannelScanRequestParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sChannelScanRequestParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sChannelScanRequestParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST() {
}
sMacAddr& cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

uint8_t& cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::~cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE() {
}
uint8_t& cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

void cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // op_error_code
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_op_error_code = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST() {
}
sMacAddr& cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

void cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::~cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE() {
}
uint8_t& cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::isEnable() {
    return (uint8_t&)(*m_isEnable);
}

void cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // isEnable
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_isEnable = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::~cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST() {
}
sTriggerChannelScanParams& cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::scan_params() {
    return (sTriggerChannelScanParams&)(*m_scan_params);
}

void cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_scan_params->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sTriggerChannelScanParams); // scan_params
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_scan_params = reinterpret_cast<sTriggerChannelScanParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sTriggerChannelScanParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sTriggerChannelScanParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_scan_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::~cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE() {
}
uint8_t& cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

void cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // op_error_code
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_op_error_code = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::~cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST() {
}
sMacAddr& cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::radio_mac() {
    return (sMacAddr&)(*m_radio_mac);
}

uint8_t& cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::scan_mode() {
    return (uint8_t&)(*m_scan_mode);
}

void cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_radio_mac->struct_swap();
}

bool cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // radio_mac
    class_size += sizeof(uint8_t); // scan_mode
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_radio_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_radio_mac->struct_init(); }
    m_scan_mode = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::~cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE() {
}
uint8_t& cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::result_status() {
    return (uint8_t&)(*m_result_status);
}

uint8_t& cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::op_error_code() {
    return (uint8_t&)(*m_op_error_code);
}

uint8_t& cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::last() {
    return (uint8_t&)(*m_last);
}

uint8_t& cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::results_size() {
    return (uint8_t&)(*m_results_size);
}

std::tuple<bool, sChannelScanResults&> cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::results(size_t idx) {
    bool ret_success = ( (m_results_idx__ > 0) && (m_results_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_results[ret_idx]);
}

bool cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::alloc_results(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list results, abort!";
        return false;
    }
    size_t len = sizeof(sChannelScanResults) * count;
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

void cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_results_idx__; i++){
        m_results[i].struct_swap();
    }
}

bool cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result_status
    class_size += sizeof(uint8_t); // op_error_code
    class_size += sizeof(uint8_t); // last
    class_size += sizeof(uint8_t); // results_size
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result_status = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_op_error_code = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_last = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_results_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_results_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_results = (sChannelScanResults*)m_buff_ptr__;
    uint8_t results_size = *m_results_size;
    m_results_idx__ = results_size;
    if (!buffPtrIncrementSafe(sizeof(sChannelScanResults) * (results_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sChannelScanResults) * (results_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::~cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST() {
}
void cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::~cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE() {
}
void cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::finalize()
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

size_t cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_CHANNEL_SCAN_DUMP_RESULTS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::~cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST() {
}
sMacAddr& cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::al_mac() {
    return (sMacAddr&)(*m_al_mac);
}

void cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_al_mac->struct_swap();
}

bool cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::finalize()
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

size_t cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // al_mac
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_al_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::~cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE() {
}
uint32_t& cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE::finalize()
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
    m_error_code = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::~cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST() {
}
sMacAddr& cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::al_mac() {
    return (sMacAddr&)(*m_al_mac);
}

void cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_al_mac->struct_swap();
}

bool cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::finalize()
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

size_t cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // al_mac
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_CLEAR_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_al_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_al_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::~cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE() {
}
uint32_t& cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::error_code() {
    return (uint32_t&)(*m_error_code);
}

void cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_error_code));
}

bool cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::finalize()
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

size_t cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_BML_WIFI_CREDENTIALS_CLEAR_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::~cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST() {
}
void cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::finalize()
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

size_t cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_BML_CLIENT_GET_CLIENT_LIST_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::~cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE() {
}
uint8_t& cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::result() {
    return (uint8_t&)(*m_result);
}

uint32_t& cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::client_list_size() {
    return (uint32_t&)(*m_client_list_size);
}

std::tuple<bool, sMacAddr&> cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::client_list(size_t idx) {
    bool ret_success = ( (m_client_list_idx__ > 0) && (m_client_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_client_list[ret_idx]);
}

bool cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::alloc_client_list(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list client_list, abort!";
        return false;
    }
    size_t len = sizeof(sMacAddr) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_client_list[*m_client_list_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_client_list_idx__ += count;
    *m_client_list_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_client_list_idx__ - count; i < m_client_list_idx__; i++) { m_client_list[i].struct_init(); }
    }
    return true;
}

void cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_client_list_size));
    for (size_t i = 0; i < m_client_list_idx__; i++){
        m_client_list[i].struct_swap();
    }
}

bool cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::finalize()
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

size_t cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result
    class_size += sizeof(uint32_t); // client_list_size
    return class_size;
}

bool cACTION_BML_CLIENT_GET_CLIENT_LIST_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_client_list_size = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!m_parse__) *m_client_list_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_client_list = (sMacAddr*)m_buff_ptr__;
    uint32_t client_list_size = *m_client_list_size;
    if (m_parse__) {  tlvf_swap(32, reinterpret_cast<uint8_t*>(&client_list_size)); }
    m_client_list_idx__ = client_list_size;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr) * (client_list_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) * (client_list_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CLIENT_SET_CLIENT_REQUEST::cACTION_BML_CLIENT_SET_CLIENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_SET_CLIENT_REQUEST::cACTION_BML_CLIENT_SET_CLIENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_SET_CLIENT_REQUEST::~cACTION_BML_CLIENT_SET_CLIENT_REQUEST() {
}
sMacAddr& cACTION_BML_CLIENT_SET_CLIENT_REQUEST::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

sClientConfig& cACTION_BML_CLIENT_SET_CLIENT_REQUEST::client_config() {
    return (sClientConfig&)(*m_client_config);
}

void cACTION_BML_CLIENT_SET_CLIENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_sta_mac->struct_swap();
    m_client_config->struct_swap();
}

bool cACTION_BML_CLIENT_SET_CLIENT_REQUEST::finalize()
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

size_t cACTION_BML_CLIENT_SET_CLIENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // sta_mac
    class_size += sizeof(sClientConfig); // client_config
    return class_size;
}

bool cACTION_BML_CLIENT_SET_CLIENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sta_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    m_client_config = reinterpret_cast<sClientConfig*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClientConfig))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientConfig) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client_config->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::cACTION_BML_CLIENT_SET_CLIENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::cACTION_BML_CLIENT_SET_CLIENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::~cACTION_BML_CLIENT_SET_CLIENT_RESPONSE() {
}
uint8_t& cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::result() {
    return (uint8_t&)(*m_result);
}

void cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::finalize()
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

size_t cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result
    return class_size;
}

bool cACTION_BML_CLIENT_SET_CLIENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CLIENT_GET_CLIENT_REQUEST::cACTION_BML_CLIENT_GET_CLIENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_REQUEST::cACTION_BML_CLIENT_GET_CLIENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_REQUEST::~cACTION_BML_CLIENT_GET_CLIENT_REQUEST() {
}
sMacAddr& cACTION_BML_CLIENT_GET_CLIENT_REQUEST::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

void cACTION_BML_CLIENT_GET_CLIENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_sta_mac->struct_swap();
}

bool cACTION_BML_CLIENT_GET_CLIENT_REQUEST::finalize()
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

size_t cACTION_BML_CLIENT_GET_CLIENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // sta_mac
    return class_size;
}

bool cACTION_BML_CLIENT_GET_CLIENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sta_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::cACTION_BML_CLIENT_GET_CLIENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::cACTION_BML_CLIENT_GET_CLIENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::~cACTION_BML_CLIENT_GET_CLIENT_RESPONSE() {
}
uint8_t& cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::result() {
    return (uint8_t&)(*m_result);
}

sClient& cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::client() {
    return (sClient&)(*m_client);
}

void cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_BML), reinterpret_cast<uint8_t*>(m_action_op));
    m_client->struct_swap();
}

bool cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::finalize()
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

size_t cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // result
    class_size += sizeof(sClient); // client
    return class_size;
}

bool cACTION_BML_CLIENT_GET_CLIENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_result = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_client = reinterpret_cast<sClient*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClient))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClient) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_client->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}


