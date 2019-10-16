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

cACTION_APMANAGER_4ADDR_STA_JOINED::cACTION_APMANAGER_4ADDR_STA_JOINED(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_4ADDR_STA_JOINED::cACTION_APMANAGER_4ADDR_STA_JOINED(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_4ADDR_STA_JOINED::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // src_mac
    class_size += sizeof(sMacAddr); // dst_mac
    return class_size;
}

bool cACTION_APMANAGER_4ADDR_STA_JOINED::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_src_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_src_mac->struct_init(); }
    m_dst_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_dst_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_JOINED_NOTIFICATION::cACTION_APMANAGER_JOINED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_JOINED_NOTIFICATION::cACTION_APMANAGER_JOINED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_JOINED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeHostap); // params
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeHostap*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeHostap))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) { return false; }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_INIT_DONE_NOTIFICATION::cACTION_APMANAGER_INIT_DONE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_INIT_DONE_NOTIFICATION::cACTION_APMANAGER_INIT_DONE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_APMANAGER_INIT_DONE_NOTIFICATION::~cACTION_APMANAGER_INIT_DONE_NOTIFICATION() {
}
void cACTION_APMANAGER_INIT_DONE_NOTIFICATION::class_swap()
{
}

size_t cACTION_APMANAGER_INIT_DONE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_INIT_DONE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApSetRestrictedFailsafe); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sApSetRestrictedFailsafe*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApSetRestrictedFailsafe))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // success
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_RESTRICTED_FAILSAFE_CHANNEL_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_success = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_AP_DISABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    class_size += sizeof(sVapInfo); // vap_info
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_AP_ENABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) { return false; }
    m_vap_info = (sVapInfo*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVapInfo))) { return false; }
    if (!m_parse__) { m_vap_info->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::~cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST() {
}
void cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::class_swap()
{
}

size_t cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sVapsList); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_VAPS_LIST_UPDATE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sVapsList*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sVapsList))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CHANNEL_SWITCH_ACS_START::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) { return false; }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CSA_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) { return false; }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_CSA_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) { return false; }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ACS_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) { return false; }
    if (!m_parse__) { m_cs_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::~cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION() {
}
sApChannelSwitch& cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::cs_params() {
    return (sApChannelSwitch&)(*m_cs_params);
}

std::tuple<bool, sWifiChannel&> cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::supported_channels_list(size_t idx) {
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

size_t cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApChannelSwitch); // cs_params
    class_size += beerocks::message::SUPPORTED_CHANNELS_LENGTH * sizeof(sWifiChannel); // supported_channels_list
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ACS_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_cs_params = (sApChannelSwitch*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sApChannelSwitch))) { return false; }
    if (!m_parse__) { m_cs_params->struct_init(); }
    m_supported_channels_list = (sWifiChannel*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sWifiChannel)*(beerocks::message::SUPPORTED_CHANNELS_LENGTH))) { return false; }
    m_supported_channels_list_idx__  = beerocks::message::SUPPORTED_CHANNELS_LENGTH;
    if (!m_parse__) {
        for (size_t i = 0; i < beerocks::message::SUPPORTED_CHANNELS_LENGTH; i++) { m_supported_channels_list->struct_init(); }
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsCacCompleted); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CAC_COMPLETED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDfsCacCompleted*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sDfsCacCompleted))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sDfsChannelAvailable); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DFS_CHANNEL_AVAILABLE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sDfsChannelAvailable*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sDfsChannelAvailable))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_ADD_4ADDR_STA_UPDATE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_DEL_4ADDR_STA_UPDATE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborSetParams11k); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_SET_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNeighborSetParams11k*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNeighborSetParams11k))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNeighborRemoveParams11k); // params
    return class_size;
}

bool cACTION_APMANAGER_HOSTAP_REMOVE_NEIGHBOR_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNeighborRemoveParams11k*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNeighborRemoveParams11k))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientAssociationParams); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ASSOCIATED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientAssociationParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sClientAssociationParams))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectionParams); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientDisconnectionParams*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sClientDisconnectionParams))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST::cACTION_APMANAGER_CLIENT_DISCONNECT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    m_vap_id = (int8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(int8_t))) { return false; }
    m_type = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    m_reason = (uint32_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint32_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientDisconnectResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISCONNECT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientDisconnectResponse*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sClientDisconnectResponse))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::~cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

uint8_t& cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::reject_sta() {
    return (uint8_t&)(*m_reject_sta);
}

void cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(uint8_t); // reject_sta
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_DISALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    m_reject_sta = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::cACTION_APMANAGER_CLIENT_ALLOW_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::~cACTION_APMANAGER_CLIENT_ALLOW_REQUEST() {
}
sMacAddr& cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::mac() {
    return (sMacAddr&)(*m_mac);
}

beerocks::net::sIpv4Addr& cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::ipv4() {
    return (beerocks::net::sIpv4Addr&)(*m_ipv4);
}

void cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::class_swap()
{
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

size_t cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_ALLOW_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    m_ipv4 = (beerocks::net::sIpv4Addr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(beerocks::net::sIpv4Addr))) { return false; }
    if (!m_parse__) { m_ipv4->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurementRequest*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurementRequest))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurement*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeRssiMeasurement))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_IRE_CONNECTED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_ACK::cACTION_APMANAGER_ACK(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_ACK::cACTION_APMANAGER_ACK(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_ACK::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // reason
    class_size += sizeof(sMacAddr); // sta_mac
    return class_size;
}

bool cACTION_APMANAGER_ACK::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_reason = (uint8_t*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(uint8_t))) { return false; }
    m_sta_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_sta_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeBssSteerRequest*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeBssSteerRequest))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeBssSteerResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_BSS_STEER_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeBssSteerResponse*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sNodeBssSteerResponse))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sMacAddr); // mac
    return class_size;
}

bool cACTION_APMANAGER_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (sMacAddr*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sMacAddr))) { return false; }
    if (!m_parse__) { m_mac->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetRequest); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringClientSetRequest*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientSetRequest))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetResponse); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_CLIENT_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringClientSetResponse*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringClientSetResponse))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvProbeReq); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_EVENT_PROBE_REQ_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvProbeReq*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvProbeReq))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
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

size_t cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvAuthFail); // params
    return class_size;
}

bool cACTION_APMANAGER_STEERING_EVENT_AUTH_FAIL_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvAuthFail*)m_buff_ptr__;
    if (!buffPtrIncrementSafe(sizeof(sSteeringEvAuthFail))) { return false; }
    if (!m_parse__) { m_params->struct_init(); }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}

cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed) :
    BaseClass(buff, buff_len, parse, swap_needed) {
    m_init_succeeded = init();
}
cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::cACTION_APMANAGER_HEARTBEAT_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse, bool swap_needed) :
BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed){
    m_init_succeeded = init();
}
cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::~cACTION_APMANAGER_HEARTBEAT_NOTIFICATION() {
}
void cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::class_swap()
{
}

size_t cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_APMANAGER_HEARTBEAT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) { class_swap(); }
    return true;
}


