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

#include <beerocks/tlvf/beerocks_message_monitor.h>
#include <tlvf/tlvflogging.h>

using namespace beerocks_message;

cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::~cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION()
{
}
int8_t &cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::vap_id() { return (int8_t &)(*m_vap_id); }

void cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::class_swap() {}

size_t cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // vap_id
    return class_size;
}

bool cACTION_MONITOR_HOSTAP_AP_DISABLED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_vap_id = (int8_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_JOINED_NOTIFICATION::cACTION_MONITOR_JOINED_NOTIFICATION(uint8_t *buff,
                                                                         size_t buff_len,
                                                                         bool parse,
                                                                         bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_JOINED_NOTIFICATION::cACTION_MONITOR_JOINED_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_JOINED_NOTIFICATION::~cACTION_MONITOR_JOINED_NOTIFICATION() {}
void cACTION_MONITOR_JOINED_NOTIFICATION::class_swap() {}

size_t cACTION_MONITOR_JOINED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_MONITOR_JOINED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_SON_CONFIG_UPDATE::cACTION_MONITOR_SON_CONFIG_UPDATE(uint8_t *buff, size_t buff_len,
                                                                     bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_SON_CONFIG_UPDATE::cACTION_MONITOR_SON_CONFIG_UPDATE(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_SON_CONFIG_UPDATE::~cACTION_MONITOR_SON_CONFIG_UPDATE() {}
sSonConfig &cACTION_MONITOR_SON_CONFIG_UPDATE::config() { return (sSonConfig &)(*m_config); }

void cACTION_MONITOR_SON_CONFIG_UPDATE::class_swap() { m_config->struct_swap(); }

size_t cACTION_MONITOR_SON_CONFIG_UPDATE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSonConfig); // config
    return class_size;
}

bool cACTION_MONITOR_SON_CONFIG_UPDATE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_config = (sSonConfig *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSonConfig) * 1;
    if (!m_parse__) {
        m_config->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::~cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL() {}
sLoggingLevelChange &cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::params()
{
    return (sLoggingLevelChange &)(*m_params);
}

void cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLoggingLevelChange); // params
    return class_size;
}

bool cACTION_MONITOR_CHANGE_MODULE_LOGGING_LEVEL::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sLoggingLevelChange *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sLoggingLevelChange) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_ERROR_NOTIFICATION::cACTION_MONITOR_ERROR_NOTIFICATION(uint8_t *buff,
                                                                       size_t buff_len, bool parse,
                                                                       bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_ERROR_NOTIFICATION::cACTION_MONITOR_ERROR_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_ERROR_NOTIFICATION::~cACTION_MONITOR_ERROR_NOTIFICATION() {}
uint32_t &cACTION_MONITOR_ERROR_NOTIFICATION::error_code() { return (uint32_t &)(*m_error_code); }

void cACTION_MONITOR_ERROR_NOTIFICATION::class_swap()
{
    tlvf_swap(32, reinterpret_cast<uint8_t *>(m_error_code));
}

size_t cACTION_MONITOR_ERROR_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint32_t); // error_code
    return class_size;
}

bool cACTION_MONITOR_ERROR_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_error_code = (uint32_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint32_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_ERROR_NOTIFICATION_ACK::cACTION_MONITOR_ERROR_NOTIFICATION_ACK(uint8_t *buff,
                                                                               size_t buff_len,
                                                                               bool parse,
                                                                               bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_ERROR_NOTIFICATION_ACK::cACTION_MONITOR_ERROR_NOTIFICATION_ACK(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_ERROR_NOTIFICATION_ACK::~cACTION_MONITOR_ERROR_NOTIFICATION_ACK() {}
void cACTION_MONITOR_ERROR_NOTIFICATION_ACK::class_swap() {}

size_t cACTION_MONITOR_ERROR_NOTIFICATION_ACK::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_MONITOR_ERROR_NOTIFICATION_ACK::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_HEARTBEAT_NOTIFICATION::cACTION_MONITOR_HEARTBEAT_NOTIFICATION(uint8_t *buff,
                                                                               size_t buff_len,
                                                                               bool parse,
                                                                               bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HEARTBEAT_NOTIFICATION::cACTION_MONITOR_HEARTBEAT_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HEARTBEAT_NOTIFICATION::~cACTION_MONITOR_HEARTBEAT_NOTIFICATION() {}
void cACTION_MONITOR_HEARTBEAT_NOTIFICATION::class_swap() {}

size_t cACTION_MONITOR_HEARTBEAT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    return class_size;
}

bool cACTION_MONITOR_HEARTBEAT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::~cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST()
{
}
sClientMonitoringParams &cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::params()
{
    return (sClientMonitoringParams &)(*m_params);
}

void cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sClientMonitoringParams); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_START_MONITORING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sClientMonitoringParams *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sClientMonitoringParams) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::~cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST() {}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

void cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::class_swap() { m_mac->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_MONITOR_CLIENT_STOP_MONITORING_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(uint8_t *buff, size_t buff_len, bool parse,
                                                       bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST(std::shared_ptr<BaseClass> base, bool parse,
                                                       bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::
    ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST()
{
}
sNodeRssiMeasurementRequest &cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::params()
{
    return (sNodeRssiMeasurementRequest &)(*m_params);
}

void cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurementRequest); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurementRequest *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeRssiMeasurementRequest) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::~cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST() {}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

beerocks::net::sIpv4Addr &cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::ipv4()
{
    return (beerocks::net::sIpv4Addr &)(*m_ipv4);
}

uint8_t &cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::channel() { return (uint8_t &)(*m_channel); }

void cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::class_swap()
{
    m_mac->struct_swap();
    m_ipv4->struct_swap();
}

size_t cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr);  // mac
    class_size += sizeof(beerocks::net::sIpv4Addr); // ipv4
    class_size += sizeof(uint8_t);                  // channel
    return class_size;
}

bool cACTION_MONITOR_CLIENT_DISCONNECT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    m_ipv4 = (beerocks::net::sIpv4Addr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sIpv4Addr) * 1;
    if (!m_parse__) {
        m_ipv4->struct_init();
    }
    m_channel = (uint8_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(uint8_t *buff, size_t buff_len,
                                                            bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base,
                                                            bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::
    ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION()
{
}
sNodeRssiMeasurement &cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::params()
{
    return (sNodeRssiMeasurement &)(*m_params);
}

void cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurement *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeRssiMeasurement) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(uint8_t *buff, size_t buff_len, bool parse,
                                                        bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse,
                                                        bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::
    ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE()
{
}
sNodeRssiMeasurement &cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::params()
{
    return (sNodeRssiMeasurement &)(*m_params);
}

void cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sNodeRssiMeasurement); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sNodeRssiMeasurement *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sNodeRssiMeasurement) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::~cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION()
{
}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

void cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::class_swap() { m_mac->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_MONITOR_CLIENT_NO_RESPONSE_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(uint8_t *buff, size_t buff_len,
                                                                  bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION(std::shared_ptr<BaseClass> base,
                                                                  bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::
    ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION()
{
}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

void cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::class_swap()
{
    m_mac->struct_swap();
}

size_t cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_START_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(uint8_t *buff, size_t buff_len,
                                                            bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::
    cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE(std::shared_ptr<BaseClass> base,
                                                            bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::
    ~cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE()
{
}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

void cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::class_swap() { m_mac->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_MONITOR_CLIENT_RX_RSSI_MEASUREMENT_CMD_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::~cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION()
{
}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

void cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::class_swap() { m_mac->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_MONITOR_CLIENT_NO_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::~cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION() {}
sApActivityNotificationParams &cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::params()
{
    return (sApActivityNotificationParams &)(*m_params);
}

void cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApActivityNotificationParams); // params
    return class_size;
}

bool cACTION_MONITOR_HOSTAP_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sApActivityNotificationParams *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApActivityNotificationParams) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::
    ~cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST()
{
}
uint8_t &cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::sync() { return (uint8_t &)(*m_sync); }

void cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::class_swap() {}

size_t cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(uint8_t); // sync
    return class_size;
}

bool cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_sync = (uint8_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::
    cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION(uint8_t *buff, size_t buff_len, bool parse,
                                                       bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::
    cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION(std::shared_ptr<BaseClass> base, bool parse,
                                                       bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::
    ~cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION()
{
}
int8_t &cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::new_tx_state()
{
    return (int8_t &)(*m_new_tx_state);
}

int8_t &cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::new_hostap_enabled_state()
{
    return (int8_t &)(*m_new_hostap_enabled_state);
}

void cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::class_swap() {}

size_t cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(int8_t); // new_tx_state
    class_size += sizeof(int8_t); // new_hostap_enabled_state
    return class_size;
}

bool cACTION_MONITOR_HOSTAP_STATUS_CHANGED_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_new_tx_state = (int8_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    m_new_hostap_enabled_state = (int8_t *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(int8_t) * 1;
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::
    cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE(uint8_t *buff, size_t buff_len, bool parse,
                                                      bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::
    cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE(std::shared_ptr<BaseClass> base, bool parse,
                                                      bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::
    ~cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE()
{
}
sApStatsParams &cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::ap_stats()
{
    return (sApStatsParams &)(*m_ap_stats);
}

uint8_t &cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::sta_stats_size()
{
    return (uint8_t &)(*m_sta_stats_size);
}

std::tuple<bool, sStaStatsParams &>
cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::sta_stats(size_t idx)
{
    bool ret_success = ((m_sta_stats_idx__ > 0) && (m_sta_stats_idx__ > idx));
    size_t ret_idx   = ret_success ? idx : 0;
    if (!ret_success) {
        TLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";
    }
    return std::forward_as_tuple(ret_success, m_sta_stats[ret_idx]);
}

bool cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::alloc_sta_stats(size_t count)
{
    if (count == 0) {
        TLVF_LOG(WARNING) << "can't allocate 0 bytes";
        return false;
    }
    size_t len = sizeof(sStaStatsParams) * count;
    if (getBuffRemainingBytes() < len) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer - can't allocate";
        return false;
    }
    //TLVF_TODO: enable call to memmove
    m_sta_stats_idx__ += count;
    *m_sta_stats_size += count;
    m_buff_ptr__ += len;
    if (!m_parse__) {
        for (size_t i = m_sta_stats_idx__ - count; i < m_sta_stats_idx__; i++) {
            m_sta_stats[i].struct_init();
        }
    }
    return true;
}

void cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::class_swap()
{
    m_ap_stats->struct_swap();
    for (size_t i = 0; i < (size_t)*m_sta_stats_size; i++) {
        m_sta_stats[i].struct_swap();
    }
}

size_t cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApStatsParams); // ap_stats
    class_size += sizeof(uint8_t);        // sta_stats_size
    return class_size;
}

bool cACTION_MONITOR_HOSTAP_STATS_MEASUREMENT_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_ap_stats = (sApStatsParams *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApStatsParams) * 1;
    if (!m_parse__) {
        m_ap_stats->struct_init();
    }
    m_sta_stats_size = (uint8_t *)m_buff_ptr__;
    if (!m_parse__)
        *m_sta_stats_size = 0;
    m_buff_ptr__ += sizeof(uint8_t) * 1;
    m_sta_stats       = (sStaStatsParams *)m_buff_ptr__;
    m_sta_stats_idx__ = *m_sta_stats_size;
    m_buff_ptr__ += sizeof(sStaStatsParams) * (*m_sta_stats_size);
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::
    cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(uint8_t *buff, size_t buff_len, bool parse,
                                                         bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::
    cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION(std::shared_ptr<BaseClass> base,
                                                         bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::
    ~cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION()
{
}
sApLoadNotificationParams &cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::params()
{
    return (sApLoadNotificationParams &)(*m_params);
}

void cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sApLoadNotificationParams); // params
    return class_size;
}

bool cACTION_MONITOR_HOSTAP_LOAD_MEASUREMENT_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sApLoadNotificationParams *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sApLoadNotificationParams) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::~cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST() {}
sBeaconRequest11k &cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::params()
{
    return (sBeaconRequest11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBeaconRequest11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBeaconRequest11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sBeaconRequest11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::~cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE() {}
sBeaconResponse11k &cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::params()
{
    return (sBeaconResponse11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sBeaconResponse11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_BEACON_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sBeaconResponse11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sBeaconResponse11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::~cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST()
{
}
sStaChannelLoadRequest11k &cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::params()
{
    return (sStaChannelLoadRequest11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStaChannelLoadRequest11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStaChannelLoadRequest11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStaChannelLoadRequest11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::
    ~cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE()
{
}
sStaChannelLoadResponse11k &cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::params()
{
    return (sStaChannelLoadResponse11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStaChannelLoadResponse11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_CHANNEL_LOAD_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStaChannelLoadResponse11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStaChannelLoadResponse11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::~cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST() {}
sStatisticsRequest11k &cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::params()
{
    return (sStatisticsRequest11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStatisticsRequest11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_STATISTICS_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStatisticsRequest11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStatisticsRequest11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::~cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE() {}
sStatisticsResponse11k &cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::params()
{
    return (sStatisticsResponse11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sStatisticsResponse11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_STATISTICS_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sStatisticsResponse11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sStatisticsResponse11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::
    cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST(uint8_t *buff, size_t buff_len, bool parse,
                                                        bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::
    cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST(std::shared_ptr<BaseClass> base, bool parse,
                                                        bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::
    ~cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST()
{
}
beerocks::net::sMacAddr &cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::mac()
{
    return (beerocks::net::sMacAddr &)(*m_mac);
}

void cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::class_swap() { m_mac->struct_swap(); }

size_t cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(beerocks::net::sMacAddr); // mac
    return class_size;
}

bool cACTION_MONITOR_CLIENT_LINK_MEASUREMENT_11K_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_mac = (beerocks::net::sMacAddr *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(beerocks::net::sMacAddr) * 1;
    if (!m_parse__) {
        m_mac->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::
    cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE(uint8_t *buff, size_t buff_len,
                                                          bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::
    cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE(std::shared_ptr<BaseClass> base,
                                                          bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::
    ~cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE()
{
}
sLinkMeasurementsResponse11k &cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::params()
{
    return (sLinkMeasurementsResponse11k &)(*m_params);
}

void cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sLinkMeasurementsResponse11k); // params
    return class_size;
}

bool cACTION_MONITOR_CLIENT_LINK_MEASUREMENTS_11K_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sLinkMeasurementsResponse11k *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sLinkMeasurementsResponse11k) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::
    cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST(uint8_t *buff, size_t buff_len, bool parse,
                                                      bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::
    cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST(std::shared_ptr<BaseClass> base, bool parse,
                                                      bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::
    ~cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST()
{
}
sSteeringSetGroupRequest &cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::params()
{
    return (sSteeringSetGroupRequest &)(*m_params);
}

void cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringSetGroupRequest); // params
    return class_size;
}

bool cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringSetGroupRequest *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringSetGroupRequest) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::
    cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE(uint8_t *buff, size_t buff_len, bool parse,
                                                       bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::
    cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE(std::shared_ptr<BaseClass> base, bool parse,
                                                       bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::
    ~cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE()
{
}
sSteeringSetGroupResponse &cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::params()
{
    return (sSteeringSetGroupResponse &)(*m_params);
}

void cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringSetGroupResponse); // params
    return class_size;
}

bool cACTION_MONITOR_STEERING_CLIENT_SET_GROUP_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringSetGroupResponse *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringSetGroupResponse) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::~cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST() {}
sSteeringClientSetRequest &cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::params()
{
    return (sSteeringClientSetRequest &)(*m_params);
}

void cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetRequest); // params
    return class_size;
}

bool cACTION_MONITOR_STEERING_CLIENT_SET_REQUEST::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringClientSetRequest *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringClientSetRequest) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE(
    uint8_t *buff, size_t buff_len, bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE(
    std::shared_ptr<BaseClass> base, bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::~cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE() {}
sSteeringClientSetResponse &cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::params()
{
    return (sSteeringClientSetResponse &)(*m_params);
}

void cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringClientSetResponse); // params
    return class_size;
}

bool cACTION_MONITOR_STEERING_CLIENT_SET_RESPONSE::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringClientSetResponse *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringClientSetResponse) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::
    cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(uint8_t *buff, size_t buff_len,
                                                                bool parse, bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::
    cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION(std::shared_ptr<BaseClass> base,
                                                                bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::
    ~cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION()
{
}
sSteeringEvActivity &cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::params()
{
    return (sSteeringEvActivity &)(*m_params);
}

void cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::class_swap()
{
    m_params->struct_swap();
}

size_t cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvActivity); // params
    return class_size;
}

bool cACTION_MONITOR_STEERING_EVENT_CLIENT_ACTIVITY_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvActivity *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringEvActivity) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}

cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::
    cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION(uint8_t *buff, size_t buff_len, bool parse,
                                                         bool swap_needed)
    : BaseClass(buff, buff_len, parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::
    cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION(std::shared_ptr<BaseClass> base,
                                                         bool parse, bool swap_needed)
    : BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse, swap_needed)
{
    m_init_succeeded = init();
}
cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::
    ~cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION()
{
}
sSteeringEvSnrXing &cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::params()
{
    return (sSteeringEvSnrXing &)(*m_params);
}

void cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::class_swap() { m_params->struct_swap(); }

size_t cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::get_initial_size()
{
    size_t class_size = 0;
    class_size += sizeof(sSteeringEvSnrXing); // params
    return class_size;
}

bool cACTION_MONITOR_STEERING_EVENT_SNR_XING_NOTIFICATION::init()
{
    if (getBuffRemainingBytes() < kMinimumLength) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    m_params = (sSteeringEvSnrXing *)m_buff_ptr__;
    m_buff_ptr__ += sizeof(sSteeringEvSnrXing) * 1;
    if (!m_parse__) {
        m_params->struct_init();
    }
    if (m_buff_ptr__ - m_buff__ > ssize_t(m_buff_len__)) {
        TLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";
        return false;
    }
    if (m_parse__ && m_swap__) {
        class_swap();
    }
    return true;
}
