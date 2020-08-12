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

#include <beerocks/tlvf/beerocks_message_apmanager.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_APMANAGER_UP_NOTIFICATION::cACTION_APMANAGER_UP_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_UP_NOTIFICATION::cACTION_APMANAGER_UP_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_UP_NOTIFICATION::~cACTION_APMANAGER_UP_NOTIFICATION() {
}
uint8_t& cACTION_APMANAGER_UP_NOTIFICATION::iface_name_length() {
    return (uint8_t&)(*m_iface_name_length);
}

std::string cACTION_APMANAGER_UP_NOTIFICATION::iface_name_str() {
    char *iface_name_ = iface_name();
    if (!iface_name_) { return std::string(); }
    return std::string(iface_name_, m_iface_name_idx__);
}

char* cACTION_APMANAGER_UP_NOTIFICATION::iface_name(size_t length) {
    if( (m_iface_name_idx__ == 0) || (m_iface_name_idx__ < length) ) {
        TLVF_LOG(ERROR) << "iface_name length is smaller than requested length";
        return nullptr;
    }
    return ((char*)m_iface_name);
}

bool cACTION_APMANAGER_UP_NOTIFICATION::set_iface_name(const std::string& str) { return set_iface_name(str.c_str(), str.size()); }
bool cACTION_APMANAGER_UP_NOTIFICATION::set_iface_name(const char str[], size_t size) {
    if (str == nullptr) {
        TLVF_LOG(WARNING) << "set_iface_name received a null pointer.";
        return false;
    }
    if (!alloc_iface_name(size)) { return false; }
    std::copy(str, str + size, m_iface_name);
    return true;
}
bool cACTION_APMANAGER_UP_NOTIFICATION::alloc_iface_name(size_t count) {
    if (m_lock_order_counter__ > 0) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list iface_name, abort!";
        return false;
    }
    size_t len = sizeof(char) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 0;
    uint8_t *src = (uint8_t *)&m_iface_name[*m_iface_name_length];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_iface_name_idx__ += count;
    *m_iface_name_length += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    return true;
}

void cACTION_APMANAGER_UP_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_UP_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_UP_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // iface_name_length
    return class_size;
}

bool cACTION_APMANAGER_UP_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_iface_name_length = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_iface_name_length = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_iface_name = (char*)m_buff_ptr__;
    uint8_t iface_name_length = *m_iface_name_length;
    m_iface_name_idx__ = iface_name_length;
    if (!buffPtrIncrementSafe(sizeof(char) * (iface_name_length))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(char) * (iface_name_length) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CONFIGURE::cACTION_APMANAGER_CONFIGURE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CONFIGURE::cACTION_APMANAGER_CONFIGURE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CONFIGURE::~cACTION_APMANAGER_CONFIGURE() {
}
uint8_t& cACTION_APMANAGER_CONFIGURE::channel() {
    return (uint8_t&)(*m_channel);
}

void cACTION_APMANAGER_CONFIGURE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_CONFIGURE::finalize()
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

size_t cACTION_APMANAGER_CONFIGURE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // channel
    return class_size;
}

bool cACTION_APMANAGER_CONFIGURE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_JOINED_NOTIFICATION::cACTION_APMANAGER_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_JOINED_NOTIFICATION::cACTION_APMANAGER_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_JOINED_NOTIFICATION::~cACTION_APMANAGER_JOINED_NOTIFICATION() {
}
sNodeHostap& cACTION_APMANAGER_JOINED_NOTIFICATION::params() {
    return (sNodeHostap&)(*m_params);
}

sApChannelSwitch& cACTION_APMANAGER_JOINED_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

uint8_t& cACTION_APMANAGER_JOINED_NOTIFICATION::preferred_channels_size() {
    return (uint8_t&)(*m_preferred_channels_size);
}

std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_APMANAGER_JOINED_NOTIFICATION::preferred_channels(size_t idx) {
    bool ret_success = ( (m_preferred_channels_idx__ > 0) && (m_preferred_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_preferred_channels[ret_idx]);
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::alloc_preferred_channels(size_t count) {
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
    m_supported_channels_size = (uint8_t *)((uint8_t *)(m_supported_channels_size) + len);
    m_supported_channels = (beerocks::message::sWifiChannel *)((uint8_t *)(m_supported_channels) + len);
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

uint8_t& cACTION_APMANAGER_JOINED_NOTIFICATION::supported_channels_size() {
    return (uint8_t&)(*m_supported_channels_size);
}

std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_APMANAGER_JOINED_NOTIFICATION::supported_channels(size_t idx) {
    bool ret_success = ( (m_supported_channels_idx__ > 0) && (m_supported_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_supported_channels[ret_idx]);
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::alloc_supported_channels(size_t count) {
    if (m_lock_order_counter__ > 1) {;
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list supported_channels, abort!";
        return false;
    }
    size_t len = sizeof(beerocks::message::sWifiChannel) * count;
    if(getBuffRemainingBytes() < len )  {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    m_lock_order_counter__ = 1;
    uint8_t *src = (uint8_t *)&m_supported_channels[*m_supported_channels_size];
    uint8_t *dst = src + len;
    if (!m_parse__) {
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    m_supported_channels_idx__ += count;
    *m_supported_channels_size += count;
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    if (!m_parse__) { 
        for (size_t i = m_supported_channels_idx__ - count; i < m_supported_channels_idx__; i++) { m_supported_channels[i].struct_init(); }
    }
    return true;
}

void cACTION_APMANAGER_JOINED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
    m_cs_params->struct_swap();
    for (size_t i = 0; i < m_preferred_channels_idx__; i++){
        m_preferred_channels[i].struct_swap();
    }
    for (size_t i = 0; i < m_supported_channels_idx__; i++){
        m_supported_channels[i].struct_swap();
    }
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_JOINED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeHostap); // params
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(uint8_t); // preferred_channels_size
    class_size += sizeof(uint8_t); // supported_channels_size
    return class_size;
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeHostap*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeHostap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeHostap) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_preferred_channels_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
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
    m_supported_channels_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_supported_channels_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_supported_channels = (beerocks::message::sWifiChannel*)m_buff_ptr__;
    uint8_t supported_channels_size = *m_supported_channels_size;
    m_supported_channels_idx__ = supported_channels_size;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sWifiChannel) * (supported_channels_size))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sWifiChannel) * (supported_channels_size) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_ENABLE_APS_REQUEST::cACTION_APMANAGER_ENABLE_APS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_REQUEST::cACTION_APMANAGER_ENABLE_APS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_REQUEST::~cACTION_APMANAGER_ENABLE_APS_REQUEST() {
}
uint8_t& cACTION_APMANAGER_ENABLE_APS_REQUEST::channel() {
    return (uint8_t&)(*m_channel);
}

uint32_t& cACTION_APMANAGER_ENABLE_APS_REQUEST::bandwidth() {
    return (uint32_t&)(*m_bandwidth);
}

uint8_t& cACTION_APMANAGER_ENABLE_APS_REQUEST::center_channel() {
    return (uint8_t&)(*m_center_channel);
}

void cACTION_APMANAGER_ENABLE_APS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_bandwidth));
}

bool cACTION_APMANAGER_ENABLE_APS_REQUEST::finalize()
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

size_t cACTION_APMANAGER_ENABLE_APS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // channel
    class_size += sizeof(uint32_t); // bandwidth
    class_size += sizeof(uint8_t); // center_channel
    return class_size;
}

bool cACTION_APMANAGER_ENABLE_APS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bandwidth = reinterpret_cast<uint32_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_center_channel = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_ENABLE_APS_RESPONSE::cACTION_APMANAGER_ENABLE_APS_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_RESPONSE::cACTION_APMANAGER_ENABLE_APS_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_ENABLE_APS_RESPONSE::~cACTION_APMANAGER_ENABLE_APS_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_ENABLE_APS_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_APMANAGER_ENABLE_APS_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_ENABLE_APS_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_ENABLE_APS_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_ENABLE_APS_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::~cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST() {
}
sApSetRestrictedFailsafe& cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::params() {
    return (sApSetRestrictedFailsafe&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApSetRestrictedFailsafe); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sApSetRestrictedFailsafe*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApSetRestrictedFailsafe))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApSetRestrictedFailsafe) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::~cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::success() {
    return (uint8_t&)(*m_success);
}

void cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION() {
}
int8_t& cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

void cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION() {
}
int8_t& cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

sVapInfo& cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::vap_info() {
    return (sVapInfo&)(*m_vap_info);
}

void cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_vap_info->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(sVapInfo); // vap_info
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_vap_info = reinterpret_cast<sVapInfo*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sVapInfo))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVapInfo) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_vap_info->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST() {
}
void cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::~cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST() {
}
void cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_GENERATE_CLIENT_ASSOCIATION_NOTIFICATIONS_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION() {
}
sVapsList& cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::params() {
    return (sVapsList&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVapsList); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sVapsList*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sVapsList))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sVapsList) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::~cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

int8_t& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::tx_limit() {
    return (int8_t&)(*m_tx_limit);
}

uint8_t& cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::tx_limit_valid() {
    return (uint8_t&)(*m_tx_limit_valid);
}

void cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(int8_t); // tx_limit
    class_size += sizeof(uint8_t); // tx_limit_valid
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_tx_limit = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_tx_limit_valid = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

void cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

uint8_t& cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::preferred_channels_size() {
    return (uint8_t&)(*m_preferred_channels_size);
}

std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::preferred_channels(size_t idx) {
    bool ret_success = ( (m_preferred_channels_idx__ > 0) && (m_preferred_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_preferred_channels[ret_idx]);
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::alloc_preferred_channels(size_t count) {
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

void cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_cs_params->struct_swap();
    for (size_t i = 0; i < m_preferred_channels_idx__; i++){
        m_preferred_channels[i].struct_swap();
    }
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += sizeof(uint8_t); // preferred_channels_size
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = reinterpret_cast<sApChannelSwitch*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_preferred_channels_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
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

cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION() {
}
sDfsCacCompleted& cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::params() {
    return (sDfsCacCompleted&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsCacCompleted); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sDfsCacCompleted*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sDfsCacCompleted))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDfsCacCompleted) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION() {
}
sDfsChannelAvailable& cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::params() {
    return (sDfsChannelAvailable&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsChannelAvailable); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sDfsChannelAvailable*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sDfsChannelAvailable))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sDfsChannelAvailable) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::~cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE() {
}
sMacAddr& cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::~cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE() {
}
sMacAddr& cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::~cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST() {
}
sNeighborSetParams11k& cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborSetParams11k&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborSetParams11k); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNeighborSetParams11k*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNeighborSetParams11k))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNeighborSetParams11k) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::~cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST() {
}
sNeighborRemoveParams11k& cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::params() {
    return (sNeighborRemoveParams11k&)(*m_params);
}

void cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::finalize()
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

size_t cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborRemoveParams11k); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNeighborRemoveParams11k*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNeighborRemoveParams11k))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNeighborRemoveParams11k) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::~cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

sMacAddr& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::bssid() {
    return (sMacAddr&)(*m_bssid);
}

beerocks::message::sRadioCapabilities& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::capabilities() {
    return (beerocks::message::sRadioCapabilities&)(*m_capabilities);
}

int8_t& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::vap_id() {
    return (int8_t&)(*m_vap_id);
}

uint8_t* cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::association_frame(size_t idx) {
    if ( (m_association_frame_idx__ == 0) || (m_association_frame_idx__ <= idx) ) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
        return nullptr;
    }
    return &(m_association_frame[idx]);
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::set_association_frame(const void* buffer, size_t size) {
    if (buffer == nullptr) {
        TLVF_LOG(WARNING) << "set_association_frame received a null pointer.";
        return false;
    }
    if (!alloc_association_frame(size)) { return false; }
    std::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_association_frame);
    return true;
}
bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::alloc_association_frame(size_t count) {
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

void cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_bssid->struct_swap();
    m_capabilities->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(beerocks::message::sRadioCapabilities); // capabilities
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_capabilities = reinterpret_cast<beerocks::message::sRadioCapabilities*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sRadioCapabilities))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sRadioCapabilities) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_capabilities->struct_init(); }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_association_frame = (uint8_t*)m_buff_ptr__;
    m_association_frame_idx__ = getBuffRemainingBytes();
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::~cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION() {
}
sClientDisconnectionParams& cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::params() {
    return (sClientDisconnectionParams&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectionParams); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sClientDisconnectionParams*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClientDisconnectionParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientDisconnectionParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::~cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

int8_t& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::vap_id() {
    return (int8_t&)(*m_vap_id);
}

eDisconnectType& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::type() {
    return (eDisconnectType&)(*m_type);
}

uint32_t& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

void cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    tlvf_swap(8*sizeof(eDisconnectType), reinterpret_cast<uint8_t*>(m_type));
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_reason));
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(eDisconnectType); // type
    class_size += sizeof(uint32_t); // reason
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_vap_id = reinterpret_cast<int8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
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

cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::~cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE() {
}
sClientDisconnectResponse& cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::params() {
    return (sClientDisconnectResponse&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sClientDisconnectResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sClientDisconnectResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientDisconnectResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::~cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

sMacAddr& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

uint16_t& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::validity_period_sec() {
    return (uint16_t&)(*m_validity_period_sec);
}

void cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_bssid->struct_swap();
    tlvf_swap(16, reinterpret_cast<uint8_t*>(m_validity_period_sec));
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sMacAddr); // bssid
    class_size += sizeof(uint16_t); // validity_period_sec
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    m_validity_period_sec = reinterpret_cast<uint16_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint16_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint16_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::~cACTION_APMANAGER_CLIENT_ALLOW_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

sMacAddr& cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::bssid() {
    return (sMacAddr&)(*m_bssid);
}

void cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
    m_bssid->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(sMacAddr); // bssid
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST() {
}
sNodeRssiMeasurementRequest& cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params() {
    return (sNodeRssiMeasurementRequest&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeRssiMeasurementRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurementRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurementRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE() {
}
sNodeRssiMeasurement& cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params() {
    return (sNodeRssiMeasurement&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeRssiMeasurement*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurement) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_ACK::cACTION_APMANAGER_ACK(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ACK::cACTION_APMANAGER_ACK(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_ACK::~cACTION_APMANAGER_ACK() {
}
uint8_t& cACTION_APMANAGER_ACK::reason() {
    return (uint8_t&)(*m_reason);
}

sMacAddr& cACTION_APMANAGER_ACK::sta_mac() {
    return (sMacAddr&)(*m_sta_mac);
}

void cACTION_APMANAGER_ACK::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_sta_mac->struct_swap();
}

bool cACTION_APMANAGER_ACK::finalize()
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

size_t cACTION_APMANAGER_ACK::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // reason
    class_size += sizeof(sMacAddr); // sta_mac
    return class_size;
}

bool cACTION_APMANAGER_ACK::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_reason = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
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

cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::~cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST() {
}
sNodeBssSteerRequest& cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::params() {
    return (sNodeBssSteerRequest&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::finalize()
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

size_t cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeBssSteerRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeBssSteerRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeBssSteerRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::~cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE() {
}
sNodeBssSteerResponse& cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::params() {
    return (sNodeBssSteerResponse&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sNodeBssSteerResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sNodeBssSteerResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeBssSteerResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::~cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_mac->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = reinterpret_cast<sMacAddr*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::~cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST() {
}
sSteeringClientSetRequest& cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::params() {
    return (sSteeringClientSetRequest&)(*m_params);
}

void cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::finalize()
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

size_t cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringClientSetRequest*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientSetRequest))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringClientSetRequest) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::~cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE() {
}
sSteeringClientSetResponse& cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::params() {
    return (sSteeringClientSetResponse&)(*m_params);
}

void cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringClientSetResponse*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientSetResponse))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringClientSetResponse) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::~cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION() {
}
sSteeringEvProbeReq& cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::params() {
    return (sSteeringEvProbeReq&)(*m_params);
}

void cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvProbeReq); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringEvProbeReq*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvProbeReq))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringEvProbeReq) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::~cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION() {
}
sSteeringEvAuthFail& cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::params() {
    return (sSteeringEvAuthFail&)(*m_params);
}

void cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    m_params->struct_swap();
}

bool cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvAuthFail); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = reinterpret_cast<sSteeringEvAuthFail*>(m_buff_ptr__);
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvAuthFail))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sSteeringEvAuthFail) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::~cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST() {
}
uint8_t& cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::wifi_credentials_size() {
    return (uint8_t&)(*m_wifi_credentials_size);
}

std::tuple<bool, WSC::cConfigData&> cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::wifi_credentials(size_t idx) {
    bool ret_success = ( (m_wifi_credentials_idx__ > 0) && (m_wifi_credentials_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, *(m_wifi_credentials_vector[ret_idx]));
}

std::shared_ptr<WSC::cConfigData> cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::create_wifi_credentials() {
    if (m_lock_order_counter__ > 0) {
        TLVF_LOG(ERROR) << "Out of order allocation for variable length list wifi_credentials, abort!";
        return nullptr;
    }
    size_t len = WSC::cConfigData::get_initial_size();
    if (m_lock_allocation__ || getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return nullptr;
    }
    m_lock_order_counter__ = 0;
    m_lock_allocation__ = true;
    uint8_t *src = (uint8_t *)m_wifi_credentials;
    if (m_wifi_credentials_idx__ > 0) {
        src = (uint8_t *)m_wifi_credentials_vector[m_wifi_credentials_idx__ - 1]->getBuffPtr();
    }
    if (!m_parse__) {
        uint8_t *dst = src + len;
        size_t move_length = getBuffRemainingBytes(src) - len;
        std::copy_n(src, move_length, dst);
    }
    return std::make_shared<WSC::cConfigData>(src, getBuffRemainingBytes(src), m_parse__);
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::add_wifi_credentials(std::shared_ptr<WSC::cConfigData> ptr) {
    if (ptr == nullptr) {
        TLVF_LOG(ERROR) << "Received entry is nullptr";
        return false;
    }
    if (m_lock_allocation__ == false) {
        TLVF_LOG(ERROR) << "No call to create_wifi_credentials was called before add_wifi_credentials";
        return false;
    }
    uint8_t *src = (uint8_t *)m_wifi_credentials;
    if (m_wifi_credentials_idx__ > 0) {
        src = (uint8_t *)m_wifi_credentials_vector[m_wifi_credentials_idx__ - 1]->getBuffPtr();
    }
    if (ptr->getStartBuffPtr() != src) {
        TLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";
        return false;
    }
    if (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;
        TLVF_LOG(ERROR) << "Not enough available space on buffer";
        return false;
    }
    m_wifi_credentials_idx__++;
    if (!m_parse__) { (*m_wifi_credentials_size)++; }
    size_t len = ptr->getLen();
    m_wifi_credentials_vector.push_back(ptr);
    if (!buffPtrIncrementSafe(len)) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << len << ") Failed!";
        return false;
    }
    m_lock_allocation__ = false;
    return true;
}

void cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_wifi_credentials_idx__; i++){
        std::get<1>(wifi_credentials(i)).class_swap();
    }
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // wifi_credentials_size
    return class_size;
}

bool cACTION_APMANAGER_WIFI_CREDENTIALS_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_wifi_credentials_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
    if (!m_parse__) *m_wifi_credentials_size = 0;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_wifi_credentials = (WSC::cConfigData*)m_buff_ptr__;
    uint8_t wifi_credentials_size = *m_wifi_credentials_size;
    m_wifi_credentials_idx__ = 0;
    for (size_t i = 0; i < wifi_credentials_size; i++) {
        auto wifi_credentials = create_wifi_credentials();
        if (!wifi_credentials) {
            TLVF_LOG(ERROR) << "create_wifi_credentials() failed";
            return false;
        }
        if (!add_wifi_credentials(wifi_credentials)) {
            TLVF_LOG(ERROR) << "add_wifi_credentials() failed";
            return false;
        }
        // swap back since wifi_credentials will be swapped as part of the whole class swap
        wifi_credentials->class_swap();
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_START_WPS_PBC_REQUEST::cACTION_APMANAGER_START_WPS_PBC_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_START_WPS_PBC_REQUEST::cACTION_APMANAGER_START_WPS_PBC_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_START_WPS_PBC_REQUEST::~cACTION_APMANAGER_START_WPS_PBC_REQUEST() {
}
void cACTION_APMANAGER_START_WPS_PBC_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_START_WPS_PBC_REQUEST::finalize()
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

size_t cACTION_APMANAGER_START_WPS_PBC_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_START_WPS_PBC_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_RADIO_DISABLE_REQUEST::cACTION_APMANAGER_RADIO_DISABLE_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_RADIO_DISABLE_REQUEST::cACTION_APMANAGER_RADIO_DISABLE_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_RADIO_DISABLE_REQUEST::~cACTION_APMANAGER_RADIO_DISABLE_REQUEST() {
}
void cACTION_APMANAGER_RADIO_DISABLE_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_RADIO_DISABLE_REQUEST::finalize()
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

size_t cACTION_APMANAGER_RADIO_DISABLE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_RADIO_DISABLE_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::~cACTION_APMANAGER_HEARTBEAT_NOTIFICATION() {
}
void cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::cACTION_APMANAGER_READ_ACS_REPORT_REQUEST(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::cACTION_APMANAGER_READ_ACS_REPORT_REQUEST(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::~cACTION_APMANAGER_READ_ACS_REPORT_REQUEST() {
}
void cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
}

bool cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::finalize()
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

size_t cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_READ_ACS_REPORT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::~cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE() {
}
uint8_t& cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::preferred_channels_size() {
    return (uint8_t&)(*m_preferred_channels_size);
}

std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::preferred_channels(size_t idx) {
    bool ret_success = ( (m_preferred_channels_idx__ > 0) && (m_preferred_channels_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_preferred_channels[ret_idx]);
}

bool cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::alloc_preferred_channels(size_t count) {
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

void cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::class_swap()
{
    tlvf_swap(8*sizeof(eActionOp_APMANAGER), reinterpret_cast<uint8_t*>(m_action_op));
    for (size_t i = 0; i < m_preferred_channels_idx__; i++){
        m_preferred_channels[i].struct_swap();
    }
}

bool cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::finalize()
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

size_t cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // preferred_channels_size
    return class_size;
}

bool cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_preferred_channels_size = reinterpret_cast<uint8_t*>(m_buff_ptr__);
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


