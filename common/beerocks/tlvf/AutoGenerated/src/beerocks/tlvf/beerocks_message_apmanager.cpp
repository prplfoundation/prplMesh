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

#include <beerocks/tlvf/beerocks_message_apmanager.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_APMANAGER_4ADDR_STA_JOINED::cACTION_APMANAGER_4ADDR_STA_JOINED(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_4ADDR_STA_JOINED::cACTION_APMANAGER_4ADDR_STA_JOINED(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_4ADDR_STA_JOINED::~cACTION_APMANAGER_4ADDR_STA_JOINED() {
}
sMacAddr& cACTION_APMANAGER_4ADDR_STA_JOINED::src_mac() {
    return (sMacAddr&)(*m_src_mac);
}

sMacAddr& cACTION_APMANAGER_4ADDR_STA_JOINED::dst_mac() {
    return (sMacAddr&)(*m_dst_mac);
}

void cACTION_APMANAGER_4ADDR_STA_JOINED::class_swap()
{
    m_src_mac->struct_swap();
    m_dst_mac->struct_swap();
}

bool cACTION_APMANAGER_4ADDR_STA_JOINED::finalize()
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

size_t cACTION_APMANAGER_4ADDR_STA_JOINED::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // src_mac
    class_size += sizeof(sMacAddr); // dst_mac
    return class_size;
}

bool cACTION_APMANAGER_4ADDR_STA_JOINED::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_src_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_src_mac->struct_init(); }
    m_dst_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_dst_mac->struct_init(); }
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

void cACTION_APMANAGER_JOINED_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
    m_cs_params->struct_swap();
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
    return class_size;
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeHostap*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeHostap))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeHostap) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
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
    m_channel = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_bandwidth = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_center_channel = (uint8_t*)m_buff_ptr__;
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
    m_success = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_INIT_DONE_NOTIFICATION::cACTION_APMANAGER_INIT_DONE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_INIT_DONE_NOTIFICATION::cACTION_APMANAGER_INIT_DONE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_INIT_DONE_NOTIFICATION::~cACTION_APMANAGER_INIT_DONE_NOTIFICATION() {
}
void cACTION_APMANAGER_INIT_DONE_NOTIFICATION::class_swap()
{
}

bool cACTION_APMANAGER_INIT_DONE_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_INIT_DONE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_INIT_DONE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
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
    m_params = (sApSetRestrictedFailsafe*)m_buff_ptr__;
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
    m_success = (uint8_t*)m_buff_ptr__;
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
    m_vap_id = (int8_t*)m_buff_ptr__;
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
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_vap_info = (sVapInfo*)m_buff_ptr__;
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
    m_params = (sVapsList*)m_buff_ptr__;
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

void cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::class_swap()
{
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
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
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
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
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
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
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
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
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

std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::supported_channels_list(size_t idx) {
    bool ret_success = ( (m_supported_channels_list_idx__ > 0) && (m_supported_channels_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_supported_channels_list[ret_idx]);
}

void cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::class_swap()
{
    m_cs_params->struct_swap();
    for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++){
        m_supported_channels_list[i].struct_swap();
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
    class_size += beerocks::message::SUPPORTED_CHANNELS_LENGTH * sizeof(beerocks::message::sWifiChannel); // supported_channels_list
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sApChannelSwitch) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_supported_channels_list = (beerocks::message::sWifiChannel*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sWifiChannel) * (beerocks::message::SUPPORTED_CHANNELS_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sWifiChannel) * (beerocks::message::SUPPORTED_CHANNELS_LENGTH) << ") Failed!";
        return false;
    }
    m_supported_channels_list_idx__  = beerocks::message::SUPPORTED_CHANNELS_LENGTH;
    if (!m_parse__) {
        for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++) { m_supported_channels_list->struct_init(); }
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
    m_params = (sDfsCacCompleted*)m_buff_ptr__;
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
    m_params = (sDfsChannelAvailable*)m_buff_ptr__;
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
    m_mac = (sMacAddr*)m_buff_ptr__;
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
    m_mac = (sMacAddr*)m_buff_ptr__;
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
    m_params = (sNeighborSetParams11k*)m_buff_ptr__;
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
    m_params = (sNeighborRemoveParams11k*)m_buff_ptr__;
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
sClientAssociationParams& cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::params() {
    return (sClientAssociationParams&)(*m_params);
}

void cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
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
    class_size += sizeof(sClientAssociationParams); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientAssociationParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sClientAssociationParams))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sClientAssociationParams) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
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
    m_params = (sClientDisconnectionParams*)m_buff_ptr__;
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

uint32_t& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::type() {
    return (uint32_t&)(*m_type);
}

uint32_t& cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::reason() {
    return (uint32_t&)(*m_reason);
}

void cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    m_mac->struct_swap();
    tlvf_swap(32, reinterpret_cast<uint8_t*>(m_type));
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
    class_size += sizeof(uint32_t); // type
    class_size += sizeof(uint32_t); // reason
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(int8_t) << ") Failed!";
        return false;
    }
    m_type = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint32_t) << ") Failed!";
        return false;
    }
    m_reason = (uint32_t*)m_buff_ptr__;
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
    m_params = (sClientDisconnectResponse*)m_buff_ptr__;
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

void cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::class_swap()
{
    m_mac->struct_swap();
    m_bssid->struct_swap();
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
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_bssid->struct_init(); }
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
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
    m_bssid = (sMacAddr*)m_buff_ptr__;
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
    m_params = (sNodeRssiMeasurementRequest*)m_buff_ptr__;
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
    m_params = (sNodeRssiMeasurement*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sNodeRssiMeasurement) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse) :
    BaseClass(buff, buff_len, parse) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::~cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::mac() {
    return (sMacAddr&)(*m_mac);
}

void cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
}

bool cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::finalize()
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

size_t cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(sMacAddr) << ") Failed!";
        return false;
    }
    if (!m_parse__) { m_mac->struct_init(); }
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
    m_reason = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(uint8_t) << ") Failed!";
        return false;
    }
    m_sta_mac = (sMacAddr*)m_buff_ptr__;
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
    m_params = (sNodeBssSteerRequest*)m_buff_ptr__;
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
    m_params = (sNodeBssSteerResponse*)m_buff_ptr__;
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
    m_mac = (sMacAddr*)m_buff_ptr__;
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
    m_params = (sSteeringClientSetRequest*)m_buff_ptr__;
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
    m_params = (sSteeringClientSetResponse*)m_buff_ptr__;
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
    m_params = (sSteeringEvProbeReq*)m_buff_ptr__;
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
    m_params = (sSteeringEvAuthFail*)m_buff_ptr__;
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
    for (size_t i = 0; i < (size_t)*m_wifi_credentials_size; i++){
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
    m_wifi_credentials_size = (uint8_t*)m_buff_ptr__;
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
std::tuple<bool, beerocks::message::sWifiChannel&> cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::supported_channels_list(size_t idx) {
    bool ret_success = ( (m_supported_channels_list_idx__ > 0) && (m_supported_channels_list_idx__ > idx) );
    size_t ret_idx = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_supported_channels_list[ret_idx]);
}

void cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::class_swap()
{
    for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++){
        m_supported_channels_list[i].struct_swap();
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
    class_size += beerocks::message::SUPPORTED_CHANNELS_LENGTH * sizeof(beerocks::message::sWifiChannel); // supported_channels_list
    return class_size;
}

bool cACTION_APMANAGER_READ_ACS_REPORT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < get_initial_size()) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_supported_channels_list = (beerocks::message::sWifiChannel*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::message::sWifiChannel) * (beerocks::message::SUPPORTED_CHANNELS_LENGTH))) {
        LOG(ERROR) << "buffPtrIncrementSafe(" << std::dec << sizeof(beerocks::message::sWifiChannel) * (beerocks::message::SUPPORTED_CHANNELS_LENGTH) << ") Failed!";
        return false;
    }
    m_supported_channels_list_idx__  = beerocks::message::SUPPORTED_CHANNELS_LENGTH;
    if (!m_parse__) {
        for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++) { m_supported_channels_list->struct_init(); }
    }
    if (m_parse__) { class_swap(); }
    return true;
}


