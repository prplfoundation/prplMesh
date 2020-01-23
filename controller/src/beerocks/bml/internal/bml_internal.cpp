/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_internal.h"
#include "bml_defs.h"
#include "bml_iter_node.h"
#include "bml_iter_stat.h"

#include <bcl/beerocks_message_structs.h>
#include <bcl/beerocks_utils.h>
#include <bcl/network/network_utils.h>
#include <easylogging++.h>

#include <beerocks/tlvf/beerocks_message_bml.h>

using namespace beerocks;
using namespace net;

INITIALIZE_EASYLOGGINGPP

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

//These timeout definitions used throughout the code.
//Usually used for setting wait periods for promises,
//and sleeping until a partial or complete response arrives.
#define SELECT_TIMEOUT (500)             // equivelent of 0.5 seconds
#define RESPONSE_TIMEOUT (5000)          // equivelent of 5 seconds
#define DELAYED_RESPONSE_TIMEOUT (30000) // equivelent of 30 seconds

//////////////////////////////////////////////////////////////////////////////
//////////////////////// Static Members Initialization ///////////////////////
//////////////////////////////////////////////////////////////////////////////

bool bml_internal::s_fExtLogContext = false;

//////////////////////////////////////////////////////////////////////////////
/////////////////////////// Local Module Functions ///////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifdef BEEROCKS_DEBUG

static void config_logger(const std::string log_file = std::string())
{
    el::Configurations defaultConf;

    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Format,
                            "%level %datetime{%H:%m:%s} %fbase %line --> %msg");

    if (log_file.empty()) {
        defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
        defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
    } else {
        defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
        defaultConf.setGlobally(el::ConfigurationType::Filename, log_file.c_str());
        defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    }
    el::Loggers::reconfigureAllLoggers(defaultConf);
}

#endif // BEEROCKS_DEBUG

static void translate_channel_scan_results(const beerocks_message::sChannelScanResults &res_in,
                                           BML_NEIGHBOR_AP &res_out)
{
    string_utils::copy_string(res_out.ap_SSID, res_in.ssid,
                              beerocks::message::WIFI_SSID_MAX_LENGTH);
    std::copy_n(res_in.bssid.oct, BML_MAC_ADDR_LEN, res_out.ap_BSSID);
    std::copy_n(res_in.security_mode_enabled, beerocks::message::CHANNEL_SCAN_LIST_LENGTH,
                res_out.ap_SecurityModeEnabled);
    std::copy_n(res_in.encryption_mode, beerocks::message::CHANNEL_SCAN_LIST_LENGTH,
                res_out.ap_EncryptionMode);
    std::copy_n(res_in.supported_standards, beerocks::message::CHANNEL_SCAN_LIST_LENGTH,
                res_out.ap_SupportedStandards);
    std::copy_n(res_in.basic_data_transfer_rates_kbps, beerocks::message::CHANNEL_SCAN_LIST_LENGTH,
                res_out.ap_BasicDataTransferRates);
    std::copy_n(res_in.supported_data_transfer_rates_kbps,
                beerocks::message::CHANNEL_SCAN_LIST_LENGTH, res_out.ap_SupportedDataTransferRates);

    res_out.ap_Channel                   = res_in.channel;
    res_out.ap_SignalStrength            = res_in.signal_strength_dBm;
    res_out.ap_OperatingFrequencyBand    = res_in.operating_frequency_band;
    res_out.ap_OperatingStandards        = res_in.operating_standards;
    res_out.ap_OperatingChannelBandwidth = res_in.operating_channel_bandwidth;
    res_out.ap_BeaconPeriod              = res_in.beacon_period_ms;
    res_out.ap_Noise                     = res_in.noise_dBm;
    res_out.ap_DTIMPeriod                = res_in.dtim_period;
    res_out.ap_ChannelUtilization        = res_in.channel_utilization;
}

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////// Implementation ///////////////////////////////
//////////////////////////////////////////////////////////////////////////////

bml_internal::bml_internal()
{
#ifdef BEEROCKS_DEBUG

    if (!s_fExtLogContext) {
        LOG(INFO) << "bml_internal - calling config_logger()";
        config_logger();
    }

#endif
}

bml_internal::~bml_internal() {}

int bml_internal::send_bml_cmdu(int &result, uint8_t action_op)
{
    // initialize value
    result = false;

    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK)
            return iRet;
    }

    // Initialize the promise for receiving the response
    beerocks::promise<int> prmCliRes;
    std::unique_lock<std::mutex> lock(m_mtxLock);
    if (m_prmCliResponses.count(action_op) != 0) {
        LOG(ERROR) << "Duplicate message detected!";
        return (-BML_RET_OP_FAILED);
    }
    m_prmCliResponses.insert(std::pair<uint8_t, beerocks::promise<int> *>(action_op, &prmCliRes));
    lock.unlock();

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    // Wait for the duration of the RESPONSE_TIMEOUT for an answer
    if (!prmCliRes.wait_for(RESPONSE_TIMEOUT)) {
        LOG(WARNING) << "Timeout on getting message response from beerocks master";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the promise holder
    lock.lock();
    m_prmCliResponses.erase(action_op);
    lock.unlock();

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "got failed response from master";
        return (-BML_RET_OP_FAILED);
    }

    // Get response
    result = prmCliRes.get_value();

    return (iRet);
}

bool bml_internal::init()
{
    on_thread_stop();
    set_select_timeout(SELECT_TIMEOUT);

    return (true);
}

bool bml_internal::initialize(const std::string &beerocks_conf_path)
{
    // Store the beerocks path
    m_strBeerocksConfPath = beerocks_conf_path + "/";

    // Read the beerocks_agent config file
    std::string config_file_path = m_strBeerocksConfPath + std::string(BEEROCKS_AGENT) + ".conf";
    if (!beerocks::config_file::read_slave_config_file(config_file_path, m_sConfig)) {
        LOG(ERROR) << "initialize - Failed reading configuration file: " << config_file_path;
        return (false);
    }

#ifdef BEEROCKS_DEBUG

    if (!s_fExtLogContext) {
        // Reconfigure the logger to file output
        std::string log_file = m_sConfig.sLog.path + "bml_" +
                               std::string(program_invocation_short_name) + std::string("_") +
                               std::to_string(getpid()) + ".log";

        config_logger(log_file);
    }

#endif

    return (true);
}

int bml_internal::connect(const std::string beerocks_conf_path)
{
    // Read the config file
    if (!initialize(beerocks_conf_path)) {
        LOG(ERROR) << "connect - initialize " << beerocks_conf_path << " failed!";
        return (-BML_RET_INIT_FAIL);
    }

    // Connect to the platform manager
    if (!connect_to_platform()) {
        LOG(ERROR) << "connect - connect_to_platform failed!";
        return (-BML_RET_CONNECT_FAIL);
    }

    /*** get onboarding flag ***/

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmOnboard;
    m_prmOnboard = &prmOnboard;

    // Get onboarding flag using CMDU Vendor Specific messaging format
    auto header_onboarding =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ONBOARD_QUERY_REQUEST>(
            cmdu_tx);

    if (header_onboarding == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(WARNING) << "failed to send cmdu!";
        return (-BML_RET_OP_FAILED);
    }

    // Wait for the duration of the RESPONSE_TIMEOUT for an answer
    if (!prmOnboard.wait_for(RESPONSE_TIMEOUT)) {
        LOG(WARNING) << "connect - Timeout while waiting for onboard query response...";

        // Clear the promise holder
        m_prmOnboard = nullptr;
        return (-BML_RET_TIMEOUT);
    }

    /*** get local_master flag ***/

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmLocalMasterGet;
    m_prmLocalMasterGet = &prmLocalMasterGet;
    int iOpTimeout      = RESPONSE_TIMEOUT; // Default timeout

    // Get local_master flag using CMDU Vendor Specific messaging format
    auto header_local_master =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_LOCAL_MASTER_GET_REQUEST>(
            cmdu_tx);
    if (header_local_master == nullptr) {
        LOG(ERROR) << "Failed building message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmLocalMasterGet.wait_for(iOpTimeout)) {
        LOG(WARNING) << "connect - Timeout while waiting for local master get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the promise holder
    m_prmLocalMasterGet = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "connect - Configuration get failed!";
        return (iRet);
    }

    // Check if there's a local master
    if (is_local_master()) {
        int result = (connect_to_master() ? BML_RET_OK : -BML_RET_CONNECT_FAIL);
        if (BML_RET_OK != result) {
            LOG(ERROR) << "connect - connect_to_master failed!";
            return result;
        }
    }

    return (BML_RET_OK);
}

bool bml_internal::connect_to_master()
{
    // Clean previous connections
    if (m_sockMaster != nullptr) {
        remove_socket(m_sockMaster);
        delete m_sockMaster;
    }

    // Open a UDS connection with the master
    if (!(m_sockMaster =
              new SocketClient(m_sConfig.temp_path + std::string(BEEROCKS_MASTER_UDS)))) {

        LOG(ERROR) << "Failed creating UDS socket!";
        return (false);
    }

    std::string err = m_sockMaster->getError();
    if (!err.empty()) {
        LOG(ERROR) << "Failed connecting to Beerocks Master: " << err;
        return (false);
    }

    // Add the socket to the select
    add_socket(m_sockMaster);

    return (true);
}

bool bml_internal::connect_to_platform()
{
    // Clean previous connections
    if (m_sockPlatform != nullptr) {
        remove_socket(m_sockPlatform);
        delete m_sockPlatform;
    }

    // Open a UDS connection with the platform manager
    if (!(m_sockPlatform =
              new SocketClient(m_sConfig.temp_path + std::string(BEEROCKS_PLAT_MGR_UDS)))) {

        LOG(ERROR) << "Failed creating UDS socket!";
        return (false);
    }

    std::string err = m_sockPlatform->getError();
    if (!err.empty()) {
        LOG(ERROR) << "Failed connecting to Beerocks Platform Manager: " << err;
        return (false);
    }

    // Add the socket to the select
    add_socket(m_sockPlatform);

    return (true);
}

bool bml_internal::handle_nw_map_query_update(int elements_num, int last_node, void *data_buffer,
                                              bool is_query)
{
    // Exit gracefully is no callback function has been registered
    if ((is_query && !m_cbNetMapQuery) || (!is_query && !m_cbNetMapUpdate)) {
        return (true);
    }
    // Create an instance of the node iterator class
    bml_iter_node cNodeIter(elements_num, data_buffer);

    // Create an instance of the node iterator struct
    BML_NODE_ITER sNodeIter;

    // Initialize iterator members
    sNodeIter.ctx       = this;
    sNodeIter.nodes_num = elements_num;
    sNodeIter.last_node = last_node;

    // first()
    static __thread std::function<int()> node_iter_first_cb_wrapper;
    node_iter_first_cb_wrapper = [&]() -> int { return (cNodeIter.first()); };
    sNodeIter.first            = []() -> int { return (node_iter_first_cb_wrapper()); };

    // next()
    static __thread std::function<int()> node_iter_next_cb_wrapper;
    node_iter_next_cb_wrapper = [&]() -> int { return (cNodeIter.next()); };
    sNodeIter.next            = []() -> int { return (node_iter_next_cb_wrapper()); };

    // get_node()
    static __thread std::function<void *()> node_iter_get_node_cb_wrapper;
    node_iter_get_node_cb_wrapper = [&]() -> void * { return (cNodeIter.data()); };
    sNodeIter.get_node            = []() -> BML_NODE * {
        return ((BML_NODE *)node_iter_get_node_cb_wrapper());
    };

    // Execute the callback
    is_query ? m_cbNetMapQuery(&sNodeIter) : m_cbNetMapUpdate(&sNodeIter);

    return (true);
}

bool bml_internal::handle_stats_update(int elements_num, void *data_buffer)
{
    // Exit gracefully is no callback function has been registered
    if (!m_cbStatsUpdate) {
        return (true);
    }

    // Create an instance of the statistics iterator class
    bml_iter_stat cStatIter(elements_num, data_buffer);

    // Create an instance of the statistics iterator struct
    BML_STATS_ITER sStatIter;

    // Initialize iterator members
    sStatIter.ctx       = this;
    sStatIter.nodes_num = elements_num;

    // first()
    static __thread std::function<int()> stat_iter_first_cb_wrapper;
    stat_iter_first_cb_wrapper = [&]() -> int { return (cStatIter.first()); };
    sStatIter.first            = []() -> int { return (stat_iter_first_cb_wrapper()); };

    // next()
    static __thread std::function<int()> stat_iter_next_cb_wrapper;
    stat_iter_next_cb_wrapper = [&]() -> int { return (cStatIter.next()); };
    sStatIter.next            = []() -> int { return (stat_iter_next_cb_wrapper()); };

    // get_node()
    static __thread std::function<void *()> stat_iter_get_node_cb_wrapper;
    stat_iter_get_node_cb_wrapper = [&]() -> void * { return (cStatIter.data()); };
    sStatIter.get_node            = []() -> BML_STATS * {
        return ((BML_STATS *)stat_iter_get_node_cb_wrapper());
    };

    // Execute the callback
    m_cbStatsUpdate(&sStatIter);

    return (true);
}

bool bml_internal::handle_event_update(uint8_t *data_buffer)
{
    // Exit gracefully is no callback function has been registered
    if (!m_cbEvent) {
        return (true);
    }

    BML_EVENT *event = (BML_EVENT *)data_buffer;
    event->ctx       = this;

    auto size = sizeof(BML_EVENT);
    switch (event->type) {
    case BML_EVENT_TYPE_BSS_TM_REQ: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_BSS_TM_REQ, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_BH_ROAM_REQ: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_BH_ROAM_REQ, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_CLIENT_ALLOW_REQ: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_CLIENT_ALLOW_REQ, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_CLIENT_DISALLOW_REQ: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_CLIENT_DISALLOW_REQ, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_ACS_START: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_ACS_START, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_CSA_NOTIFICATION: {
        event->data = GET_MESSAGE_POINTER(BML_EVENT_CSA_NOTIFICATION, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_CAC_STATUS_CHANGED_NOTIFICATION: {
        event->data =
            GET_MESSAGE_POINTER(BML_EVENT_CAC_STATUS_CHANGED_NOTIFICATION, data_buffer, size);
        break;
    }
    case BML_EVENT_TYPE_BEACON_MEASUREMENT: {
        break;
    }
    default: {
        break;
    }
    }

    m_cbEvent(event);
    return (true);
}

void bml_internal::on_thread_stop()
{
    if (m_sockPlatform) {
        remove_socket(m_sockPlatform);
        delete m_sockPlatform;
        m_sockPlatform = nullptr;
    }

    if (m_sockMaster) {
        remove_socket(m_sockMaster);
        delete m_sockMaster;
        m_sockMaster = nullptr;
    }
}

bool bml_internal::socket_disconnected(Socket *sd)
{
    // TODO: Implement a more aggressive reconnect attempt?

    // Attempt reconnecting to the master
    if (sd == m_sockMaster) {
        LOG(INFO) << "Master socket disconnected. Reconnecting...";
        connect_to_master();
    } else if (sd == m_sockPlatform) {
        LOG(INFO) << "Platform Manager socket disconnected. Reconnecting...";
        connect_to_platform();
    }

    // Exit gracefully
    return (false);
}

std::string bml_internal::print_cmdu_types(const message::sUdsHeader *cmdu_header)
{
    return message_com::print_cmdu_types(cmdu_header);
}

bool bml_internal::handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx)
{

    auto beerocks_header = message_com::parse_intel_vs_message(cmdu_rx);
    if (!beerocks_header) {
        LOG(ERROR) << "Not a vendor specific message";
        return false;
    }

    if (process_cmdu_header(beerocks_header) != BML_RET_OK) {
        return false;
    }

    return true;
}

int bml_internal::process_cmdu_header(std::shared_ptr<beerocks_header> beerocks_header)
{

    // BML messages
    if (beerocks_header->action() == beerocks_message::ACTION_BML) {
        //uint32_t num_of_nodes;
        // Process BML messages
        switch (beerocks_header->action_op()) {
        // PING Response
        case beerocks_message::ACTION_BML_PING_RESPONSE: {
            // Signal any waiting threads
            if (m_prmPing) {
                m_prmPing->set_value(true);
                m_prmPing = nullptr;
            } else {
                LOG(WARNING) << "Received PING response, but no one is waiting...";
            }

        } break;
        // Network Map Response
        case beerocks_message::ACTION_BML_NW_MAP_RESPONSE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_NW_MAP_RESPONSE>();
            uint32_t num_of_nodes = response->node_num();
            char *firstNode       = (num_of_nodes > 0) ? response->buffer(0) : nullptr;

            // Process the message
            handle_nw_map_query_update(num_of_nodes, (int)beerocks_header->actionhdr()->last(),
                                       firstNode, true);

        } break;
        // Network map update
        case beerocks_message::ACTION_BML_NW_MAP_UPDATE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_NW_MAP_UPDATE>();
            uint32_t num_of_nodes = response->node_num();

            auto firstNode = response->buffer(0);
            // Process the message
            handle_nw_map_query_update(num_of_nodes, (int)beerocks_header->actionhdr()->last(),
                                       firstNode, false);
        } break;
        // statistics update
        case beerocks_message::ACTION_BML_STATS_UPDATE: {
            auto response = beerocks_header->addClass<beerocks_message::cACTION_BML_STATS_UPDATE>();
            uint32_t num_of_nodes = response->num_of_stats_bulks();

            auto firstNode = response->buffer(0);
            // Process the message
            handle_stats_update(num_of_nodes, firstNode);
        } break;
        // event update
        case beerocks_message::ACTION_BML_EVENTS_UPDATE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_EVENTS_UPDATE>();
            handle_event_update((uint8_t *)response->buffer(0));
        } break;
        case beerocks_message::ACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_CLIENT_BAND_STEERING_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_CLIENT_BAND_STEERING_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_CLIENT_ROAMING_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_RESPONSE>();

            // Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_CLIENT_ROAMING_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_LEGACY_CLIENT_ROAMING_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;

        case beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(
                    beerocks_message::ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST,
                    0)) {
                LOG(WARNING) << "Received "
                                "ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE "
                                "response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE: {
            auto response = beerocks_header->addClass<
                beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(
                    beerocks_message::ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST,
                    response->isEnable())) {
                LOG(WARNING) << "Received "
                                "ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_RESPONSE "
                                "response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_IRE_ROAMING_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_IRE_ROAMING_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_IRE_ROAMING_RESPONSE response, but no one "
                                "is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_IRE_ROAMING_RESPONSE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_GET_IRE_ROAMING_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_IRE_ROAMING_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_IRE_ROAMING_RESPONSE response, but no one "
                                "is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_LOAD_BALANCER_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_LOAD_BALANCER_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_LOAD_BALANCER_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_LOAD_BALANCER_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_LOAD_BALANCER_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_LOAD_BALANCER_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_LOAD_BALANCER_RESPONSE response, but no "
                                "one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_SERVICE_FAIRNESS_RESPONSE response, but "
                                "no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_SERVICE_FAIRNESS_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_SERVICE_FAIRNESS_RESPONSE response, but "
                                "no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_DFS_REENTRY_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_DFS_REENTRY_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_DFS_REENTRY_RESPONSE response, but no one "
                                "is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_DFS_REENTRY_RESPONSE: {
            auto response =
                beerocks_header->addClass<beerocks_message::cACTION_BML_GET_DFS_REENTRY_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_DFS_REENTRY_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_GET_DFS_REENTRY_RESPONSE response, but no one "
                                "is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_CERTIFICATION_MODE_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_CERTIFICATION_MODE_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_SET_CERTIFICATION_MODE_RESPONSE response, "
                                " but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_CERTIFICATION_MODE_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_CERTIFICATION_MODE_RESPONSE>();

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_GET_CERTIFICATION_MODE_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received cACTION_BML_GET_DFS_REENTRY_RESPONSE response, "
                                " but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE>();

            // Signal any waiting threads
            if (m_prmWiFiCredentialsUpdate) {
                m_prmWiFiCredentialsUpdate->set_value(response->error_code() == 0);
                m_prmWiFiCredentialsUpdate = nullptr;
            } else {
                LOG(WARNING)
                    << "Received CREDENTIALS_UPDATE_RESPONSE response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE: {
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST, 0)) {
                LOG(WARNING) << "Received ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_RESPONSE "
                                "response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE>();
            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST,
                         (response->error_code() == 0))) {
                LOG(WARNING) << "Received ACTION_BML_SET_RESTRICTED_CHANNELS_RESPONSE response, "
                                "but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_RESTRICTED_CHANNELS_RESPONSE>();
            // Signal any waiting threads
            if (m_prmRestrictedChannelsGet) {
                if (m_Restricted_channels != nullptr) {
                    *m_Restricted_channels = response->params();
                    m_prmRestrictedChannelsGet->set_value(0);
                }
                m_prmRestrictedChannelsGet = nullptr;
            } else {
                LOG(WARNING)
                    << "Received RESTRICTED_CHANNELS_RESPONSE response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE: {
            LOG(TRACE) << "ACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE";

            if (m_prmSetVapListCreds) {
                auto response = beerocks_header->addClass<
                    beerocks_message::cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE>();
                if (response == nullptr) {
                    LOG(ERROR) << "addClass cACTION_BML_SET_VAP_LIST_CREDENTIALS_RESPONSE failed";
                    return BML_RET_OP_FAILED;
                }

                if (response->result() != 0) { //if success
                    LOG(WARNING) << "SET_VAP_LIST_CREDENTIALS_REQUEST failed!";
                }

                // release the waiting thread.
                m_prmSetVapListCreds->set_value(response->result() == 0);
                m_prmSetVapListCreds = nullptr;
            } else {
                LOG(WARNING)
                    << "Received SET_VAP_LIST_CREDENTIALS response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE: {
            LOG(TRACE) << "ACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE";

            if (!m_prmGetVapListCreds) {
                LOG(WARNING)
                    << "Received GET_VAP_LIST_CREDENTIALS response, but no one is waiting...";
                break;
            }

            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_BML_GET_VAP_LIST_CREDENTIALS_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            auto release_waiting_thread = [&]() -> void {
                m_prmGetVapListCreds->set_value(response->result() == 0);
                m_prmGetVapListCreds = nullptr;
            };

            if (m_vaps == nullptr || m_pvaps_list_size == nullptr) {
                LOG(ERROR) << "The pointer to the user data buffer is null!";
                release_waiting_thread();
                break;
            }
            if (response->result() != 0) {
                LOG(ERROR) << "GET_VAP_LIST_CREDENTIALS_REQUEST failed with error: "
                           << response->result();
                release_waiting_thread();
                break;
            }
            auto vap_list_size = response->vap_list_size();
            LOG(INFO) << "Received " << (int)vap_list_size << " VAPs from the controller";
            //store the received data.
            if (vap_list_size == 0) {
                LOG(WARNING) << "got an empty vap list!";
                release_waiting_thread();
                break;
            }

            if (vap_list_size > *m_pvaps_list_size) {
                LOG(WARNING) << "Not enough space in input buffer, writing " << *m_pvaps_list_size
                             << "/" << vap_list_size << " VAPs";
            }
            // Copy the data from buffer to user
            uint8_t i; // needed after the below for loop
            uint8_t max_iteration = std::min(vap_list_size, *m_pvaps_list_size);
            for (i = 0; i < max_iteration; i++) {
                auto vap_list_tuple = response->vap_list(i);
                if (!std::get<0>(vap_list_tuple)) {
                    LOG(ERROR) << "vap list access fail!";
                    release_waiting_thread();
                    break;
                }
                auto &vap_element = std::get<1>(vap_list_tuple);
                m_vaps[i].type    = vap_element.type;
                m_vaps[i].auth    = vap_element.auth;
                m_vaps[i].enc     = vap_element.enc;
                std::copy_n(vap_element.al_mac, beerocks::net::MAC_ADDR_LEN, m_vaps[i].al_mac);
                std::copy_n(vap_element.ruid, beerocks::net::MAC_ADDR_LEN, m_vaps[i].ruid);
                std::copy_n(vap_element.bssid, beerocks::net::MAC_ADDR_LEN, m_vaps[i].bssid);
                std::copy_n(vap_element.ssid, beerocks::message::WIFI_SSID_MAX_LENGTH,
                            m_vaps[i].ssid);
                std::copy_n(vap_element.key, beerocks::message::WIFI_PASS_MAX_LENGTH,
                            m_vaps[i].key);
            }
            *m_pvaps_list_size = i;

        } break;
        case beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE: {
            auto response = beerocks_header->addClass<
                beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE>();
            if (!response) {
                LOG(ERROR)
                    << "addClass cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST,
                         response->isEnable())) {
                LOG(WARNING) << "Received ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_RESPONSE"
                             << " response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE: {
            auto response = beerocks_header->addClass<
                beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE>();
            if (!response) {
                LOG(ERROR)
                    << "addClass cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST,
                         response->op_error_code())) {
                LOG(WARNING) << "Received ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_RESPONSE"
                             << " response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE: {
            auto response = beerocks_header->addClass<
                beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE>();
            if (!response) {
                LOG(ERROR)
                    << "addClass cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (m_prmChannelScanParamsGet) {
                if (m_scan_params != nullptr) {
                    *m_scan_params = response->params();
                    m_prmChannelScanParamsGet->set_value(0);
                }
                m_prmChannelScanParamsGet = nullptr;
            } else {
                LOG(WARNING) << "Received ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_RESPONSE"
                             << " response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE: {
            auto response = beerocks_header->addClass<
                beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE>();
            if (!response) {
                LOG(ERROR)
                    << "addClass cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST,
                         response->op_error_code())) {
                LOG(WARNING) << "Received ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_RESPONSE"
                             << " response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE: {
            LOG(DEBUG) << "ACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE received";
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (m_prmChannelScanResultsGet) {

                if (m_scan_results && m_scan_results_maxsize && m_scan_results_status) {

                    LOG(DEBUG) << "Receiving Response";

                    uint8_t op_error_code  = response->op_error_code();
                    *m_scan_results_status = response->result_status();
                    auto scan_results_size = response->results_size();
                    uint8_t last           = response->last();

                    LOG(DEBUG) << "Opt code: " << int(op_error_code)
                               << ", status: " << int(*m_scan_results_status)
                               << ", size: " << int(scan_results_size);

                    if (scan_results_size > 0) {
                        LOG(DEBUG) << "currently with " << m_scan_results->size() << " results";

                        // Get results from CMDU
                        auto results = &std::get<1>(response->results(0));

                        // Cap results if no more room is avaliable
                        scan_results_size =
                            (m_scan_results->size() + scan_results_size > *m_scan_results_maxsize)
                                ? *m_scan_results_maxsize - m_scan_results->size()
                                : scan_results_size;

                        // Insert results
                        m_scan_results->insert(m_scan_results->end(), results,
                                               results + scan_results_size);
                        LOG(DEBUG) << scan_results_size << " results added";
                    }
                    LOG(DEBUG) << "last: " << (int)last;
                    if (last == 1) {
                        LOG(DEBUG) << "Results done";
                        m_prmChannelScanResultsGet->set_value(op_error_code);
                    } else {
                        LOG(DEBUG) << "Results cont";
                    }
                }
            } else {
                LOG(WARNING) << "Received ACTION_BML_CHANNEL_SCAN_GET_RESULTS_RESPONSE response, "
                             << "but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE: {
            LOG(DEBUG) << "ACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE received";
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE>();
            if (!response) {
                LOG(ERROR) << "addClass cACTION_BML_CHANNEL_SCAN_START_SCAN_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }

            //Signal any waiting threads
            if (!wake_up(beerocks_message::ACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST,
                         response->op_error_code())) {
                LOG(WARNING) << "Received ACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST"
                             << " response, but no one is waiting...";
            }
        } break;
        default: {
            LOG(WARNING) << "unhandled header BML action type 0x" << std::hex
                         << int(beerocks_header->action_op());
            return BML_RET_OP_NOT_SUPPORTED;
        }
        }
        // Platform manager messages
    } else if (beerocks_header->action() == beerocks_message::ACTION_PLATFORM) {

        // Process PLATFORM messages
        switch (beerocks_header->action_op()) {
        // Onboard Query Response
        case beerocks_message::ACTION_PLATFORM_ONBOARD_QUERY_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            // Store the value from the response message
            m_fOnboarding = (response->params().onboarding == 1);

            // Signal any waiting threads
            if (m_prmOnboard) {
                m_prmOnboard->set_value(m_fOnboarding);
                m_prmOnboard = nullptr;
            } else {
                LOG(WARNING) << "Received ONBOARD_QUERY response, but no one is waiting...";
            }

        } break;
        case beerocks_message::ACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_LOCAL_MASTER_GET_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_ONBOARD_QUERY_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            // Store the value from the repose message
            m_fLocal_Master = (response->local_master() == 1);

            // Signal any waiting threads
            if (m_prmLocalMasterGet) {
                m_prmLocalMasterGet->set_value(m_fLocal_Master);
                m_prmLocalMasterGet = nullptr;
            } else {
                LOG(WARNING) << "Received LOCAL_MASTER_GET response, but no one is waiting...";
            }

        } break;
        case beerocks_message::ACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_WIFI_CREDENTIALS_GET_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            // Signal any waiting threads
            if (m_prmWiFiCredentialsGet) {
                if (m_wifi_credentials != nullptr) {
                    *m_wifi_credentials = response->front_params();
                    m_prmWiFiCredentialsGet->set_value(response->result() == 0);
                } else {
                    m_prmWiFiCredentialsGet->set_value(0);
                }
                m_prmWiFiCredentialsGet = nullptr;
            } else {
                LOG(WARNING)
                    << "Received WIFI_CREDENTIALS_GET_RESPONSE response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            // Signal any waiting threads
            if (m_prmAdminCredentialsGet) {
                if (m_admin_credentials != nullptr) {
                    *m_admin_credentials = response->params();
                    m_prmAdminCredentialsGet->set_value(response->result() == 0);
                } else {
                    m_prmAdminCredentialsGet->set_value(0);
                }
                m_prmAdminCredentialsGet = nullptr;
            } else {
                LOG(WARNING)
                    << "Received ADMIN_CREDENTIALS_GET_RESPONSE response, but no one is waiting...";
            }
        } break;
        case beerocks_message::ACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE: {
            auto response =
                beerocks_header
                    ->addClass<beerocks_message::cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_DEVICE_INFO_GET_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            // Signal any waiting threads
            if (m_prmDeviceInfoGet) {
                if (m_device_info != nullptr) {
                    *m_device_info = response->params();
                    m_prmDeviceInfoGet->set_value(response->result() == 0);
                } else {
                    m_prmDeviceInfoGet->set_value(0);
                }
                m_prmDeviceInfoGet = nullptr;
            } else {
                LOG(WARNING)
                    << "Received SERIAL_NUMBER_GET_RESPONSE response, but no one is waiting...";
            }
            return BML_RET_OP_FAILED;
        } break;
        case beerocks_message::ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE: {
            auto response = beerocks_header->addClass<
                beerocks_message::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE>();
            if (response == nullptr) {
                LOG(ERROR) << "addClass cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_RESPONSE failed";
                return BML_RET_OP_FAILED;
            }
            // Signal any waiting threads
            if (m_prmMasterSlaveVersions) {
                if (m_master_slave_versions != nullptr) {
                    string_utils::copy_string(m_master_slave_versions->master_version,
                                              response->versions().master_version,
                                              message::VERSION_LENGTH);
                    string_utils::copy_string(m_master_slave_versions->slave_version,
                                              response->versions().slave_version,
                                              message::VERSION_LENGTH);
                    m_prmMasterSlaveVersions->set_value(response->result() == 0);
                } else {
                    LOG(DEBUG) << "m_master_slave_versions == nullptr !";
                    m_prmMasterSlaveVersions->set_value(0);
                }
                m_prmMasterSlaveVersions = nullptr;
            } else {
                LOG(WARNING)
                    << "Received MASTER_SLAVE_VERSIONS_RESPONSE response, but no one is waiting...";
            }
        } break;
        default: {
            LOG(WARNING) << "unhandled header platform action type 0x" << std::hex
                         << int(beerocks_header->action_op());
            return BML_RET_OP_NOT_SUPPORTED;
        }
        }
    } else {
        LOG(ERROR) << "header->action() != (beerocks_message::ACTION_BML || "
                      "beerocks_message::ACTION_PLATFORM) !";
        return BML_RET_OP_NOT_SUPPORTED;
    }

    return BML_RET_OK;
}

int bml_internal::bml_set_vap_list_credentials(const BML_VAP_INFO *vaps, const uint8_t vaps_num)
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    if (vaps == nullptr) {
        LOG(ERROR) << "vaps == nullptr";
        return (-BML_RET_INVALID_ARGS);
    }

    if (vaps_num > BML_NODE_MAX_VAPS) {
        LOG(ERROR) << "vap list length greater then allowed : " << int(BML_NODE_MAX_VAPS);
        return (-BML_RET_INVALID_ARGS);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            LOG(ERROR) << "Set VAP list - connect_to_master failed";
            return iRet;
        }
    }

    // CMDU Message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_SET_VAP_LIST_CREDENTIALS_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building SET VAP LIST message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!request->alloc_vap_list(vaps_num)) {
        LOG(ERROR) << "Failed TLV buffer allocation to size = " << int(vaps_num);
        return (-BML_RET_OP_FAILED);
    }

    for (uint8_t i = 0; i < vaps_num; i++) {
        auto vap_list_tuple = request->vap_list(i);
        if (!std::get<0>(vap_list_tuple)) {
            LOG(ERROR) << "vap list access fail!";
            return false;
        }
        auto &vap_element = std::get<1>(vap_list_tuple);
        vap_element.type  = vaps[i].type;
        vap_element.auth  = vaps[i].auth;
        vap_element.enc   = vaps[i].enc;
        std::copy_n(vaps[i].al_mac, beerocks::net::MAC_ADDR_LEN, vap_element.al_mac);
        std::copy_n(vaps[i].ruid, beerocks::net::MAC_ADDR_LEN, vap_element.ruid);
        std::copy_n(vaps[i].bssid, beerocks::net::MAC_ADDR_LEN, vap_element.bssid);
        std::copy_n(vaps[i].ssid, beerocks::message::WIFI_SSID_MAX_LENGTH, vap_element.ssid);
        std::copy_n(vaps[i].key, beerocks::message::WIFI_PASS_MAX_LENGTH, vap_element.key);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmSetVapListCreds;
    m_prmSetVapListCreds = &prmSetVapListCreds;

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending SET VAP LIST message!";
        // Clear the promise holder
        m_prmSetVapListCreds = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    // Wait for the duration of the RESPONSE_TIMEOUT for an answer
    if (!prmSetVapListCreds.wait_for(RESPONSE_TIMEOUT)) {
        LOG(WARNING) << "Timeout on set vap list to beerocks master";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the promise holder
    m_prmSetVapListCreds = nullptr;

    if (iRet != BML_RET_OK || !prmSetVapListCreds.get_value()) {
        LOG(ERROR) << "Sending VAPs list to master failed!";
        return (-BML_RET_OP_FAILED);
    }

    return (iRet);
}

int bml_internal::bml_get_vap_list_credentials(BML_VAP_INFO *vaps, uint8_t &vaps_num)
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    if (vaps == nullptr) {
        LOG(ERROR) << "Uninitialized vaps pointer!";
        return (-BML_RET_INVALID_ARGS);
    }

    if (vaps_num == 0) {
        LOG(ERROR) << "VAPs list length is zero!";
        return (-BML_RET_INVALID_ARGS);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            LOG(ERROR) << "get VAP list - connect_to_master failed";
            return iRet;
        }
    }

    // Store the user arguments in local data members for the RX handling method
    m_vaps            = vaps;
    m_pvaps_list_size = &vaps_num;

    // CMDU Message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_GET_VAP_LIST_CREDENTIALS_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building GET VAP LIST message!";
        return (-BML_RET_OP_FAILED);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmGetVapListCreds;
    m_prmGetVapListCreds = &prmGetVapListCreds;

    int iRet = BML_RET_OK;

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending GET VAP LIST message!";
        m_prmGetVapListCreds = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    // Wait for the duration of the RESPONSE_TIMEOUT for an answer
    if (!prmGetVapListCreds.wait_for(RESPONSE_TIMEOUT)) {
        LOG(WARNING) << "Timeout on get VAP list from master";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the promise holder
    m_prmGetVapListCreds = nullptr;

    // Clear local data members
    m_vaps            = nullptr;
    m_pvaps_list_size = nullptr;

    if (iRet != BML_RET_OK || !prmGetVapListCreds.get_value()) {
        LOG(ERROR) << "Failed getting the VAP list from the master!";
        return (-BML_RET_OP_FAILED);
    }

    return (iRet);
}

int bml_internal::set_dcs_continuous_scan_enable(const sMacAddr &mac, int enable)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST>(cmdu_tx);

    if (!request) {
        LOG(ERROR)
            << "Failed building cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->radio_mac() = mac;
    request->isEnable()  = enable;

    int result = 0;
    if (send_bml_cmdu(result, request->get_action_op()) != BML_RET_OK) {
        LOG(ERROR) << "Send ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST failed";
        return (-BML_RET_OP_FAILED);
    }

    if (result != int(eChannelScanOpErrCode::CHANNEL_SCAN_OP_SUCCESS)) {
        LOG(ERROR) << "ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_ENABLE_REQUEST returned error code:"
                   << result;
        return result;
    }

    return BML_RET_OK;
}

int bml_internal::get_dcs_continuous_scan_enable(const sMacAddr &mac, int &enable)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (!m_sockMaster) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            return iRet;
        }
    }

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST>(cmdu_tx);

    if (!request) {
        LOG(ERROR)
            << "Failed building ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_ENABLE_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->radio_mac() = mac;

    return send_bml_cmdu(enable, request->get_action_op());
}

int bml_internal::set_dcs_continuous_scan_params(const sMacAddr &mac, int dwell_time,
                                                 int interval_time, unsigned int *channel_pool,
                                                 int channel_pool_size)
{
    if (dwell_time == BML_CHANNEL_SCAN_INVALID_PARAM &&
        interval_time == BML_CHANNEL_SCAN_INVALID_PARAM &&
        channel_pool_size == BML_CHANNEL_SCAN_INVALID_PARAM) {
        LOG(ERROR) << "Function is called, but no data is being set!";
        return (-BML_RET_INVALID_DATA);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (!m_sockMaster) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            return iRet;
        }
    }

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST>(cmdu_tx);

    if (!request) {
        LOG(ERROR)
            << "Failed building cACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->radio_mac()                = mac;
    request->params().dwell_time_ms     = dwell_time;
    request->params().interval_time_sec = interval_time;
    request->params().channel_pool_size = channel_pool_size;
    if (channel_pool_size > 0 && channel_pool_size <= BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE &&
        channel_pool) {
        std::copy_n(channel_pool, channel_pool_size, request->params().channel_pool);
    }

    int result = 0;
    if (send_bml_cmdu(result, request->get_action_op()) != BML_RET_OK) {
        LOG(ERROR) << "Send ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST failed";
        return (-BML_RET_OP_FAILED);
    }

    if (result != int(eChannelScanOpErrCode::CHANNEL_SCAN_OP_SUCCESS)) {
        LOG(ERROR) << "ACTION_BML_CHANNEL_SCAN_SET_CONTINUOUS_PARAMS_REQUEST returned error code:"
                   << result;
        return result;
    }

    return BML_RET_OK;
}

int bml_internal::get_dcs_continuous_scan_params(const sMacAddr &mac, int *dwell_time,
                                                 int *interval_time, unsigned int *channel_pool,
                                                 int *channel_pool_size)
{
    if (!dwell_time && !interval_time && !channel_pool_size) {
        LOG(ERROR) << "Function is called, but no data is being requested!";
        return (-BML_RET_INVALID_DATA);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (!m_sockMaster) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            return iRet;
        }
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmChannelScanParamsGet;
    m_prmChannelScanParamsGet = &prmChannelScanParamsGet;
    int iOpTimeout            = RESPONSE_TIMEOUT; // Default timeout

    beerocks_message::sChannelScanRequestParams ScanParams;
    m_scan_params = &ScanParams;

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST>(cmdu_tx);

    if (!request) {
        LOG(ERROR)
            << "Failed building ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->radio_mac() = mac;
    // Build and send the message
    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending param get message!";
        m_prmChannelScanParamsGet = nullptr;
        m_scan_params             = nullptr;
        return (-BML_RET_OP_FAILED);
    }
    LOG(DEBUG) << "ACTION_BML_CHANNEL_SCAN_GET_CONTINUOUS_PARAMS_REQUEST sent";

    int iRet = BML_RET_OK;

    if (!m_prmChannelScanParamsGet->wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for param get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the scan params member
    m_scan_params = nullptr;

    // Clear the promise holder
    m_prmChannelScanParamsGet = nullptr;

    if (dwell_time) {
        *dwell_time = ScanParams.dwell_time_ms;
    }
    if (interval_time) {
        *interval_time = ScanParams.interval_time_sec;
    }
    if (channel_pool_size && channel_pool) {
        *channel_pool_size = ScanParams.channel_pool_size;
        if (*channel_pool_size > BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE) {
            LOG(WARNING) << "Channel pool size is too big for the allocated channel pool...";
            *channel_pool_size = BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE;
            /* Currently this simply cuts the given channel pool to the maximal size
               If this is considered an error uncomment the following line */
            // iRet = -BML_RET_OP_FAILED;
        }
        std::copy_n(ScanParams.channel_pool, *channel_pool_size, channel_pool);
    }

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Params get failed!";
        return (iRet);
    }

    return (iRet);
}

int bml_internal::get_dcs_scan_results(const sMacAddr &mac, BML_NEIGHBOR_AP **results,
                                       unsigned int &results_size,
                                       const unsigned int max_results_size, uint8_t &result_status,
                                       bool is_single_scan)
{
    if (!results || !results_size || !result_status) {
        LOG(ERROR) << "Function is called, but no data is being requested!";
        return (-BML_RET_INVALID_DATA);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (!m_sockMaster) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            return iRet;
        }
    }

    // Initialize the promise for receiving the response
    beerocks::promise<int> prmChannelScanResultsGet;
    m_prmChannelScanResultsGet    = &prmChannelScanResultsGet;
    int iOpTimeout                = DELAYED_RESPONSE_TIMEOUT; // Default timeout
    auto scan_results             = std::list<beerocks_message::sChannelScanResults>();
    uint32_t scan_results_maxsize = uint32_t(max_results_size);

    m_scan_results         = &scan_results;
    m_scan_results_maxsize = &scan_results_maxsize;
    m_scan_results_status  = &result_status;

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST>(cmdu_tx);

    if (!request) {
        LOG(ERROR) << "Failed building cACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->radio_mac() = mac;
    request->scan_mode() = (is_single_scan) ? 1 : 0;
    // Build and send the message
    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending param get message!";
        m_prmChannelScanResultsGet = nullptr;
        m_scan_results             = nullptr;
        m_scan_results_maxsize     = nullptr;
        m_scan_results_status      = nullptr;
        return (-BML_RET_OP_FAILED);
    }
    LOG(DEBUG) << "ACTION_BML_CHANNEL_SCAN_GET_RESULTS_REQUEST sent";

    int iRet = BML_RET_OK;

    if (!m_prmChannelScanResultsGet->wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for results get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the scan results members
    m_scan_results         = nullptr;
    m_scan_results_maxsize = nullptr;
    m_scan_results_status  = nullptr;

    // Clear the promise holder
    m_prmChannelScanResultsGet = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Results get failed!";
        return (iRet);
    }

    iRet = prmChannelScanResultsGet.get_value();
    if (iRet != int(eChannelScanOpErrCode::CHANNEL_SCAN_OP_SUCCESS)) {
        LOG(ERROR) << "Results returned with error code:" << iRet << ". Aborting!";
        return iRet;
    }

    //output_results_size will be set to the number of actual returning results
    results_size = 0;
    for (auto &res : scan_results) {
        auto &out = ((*results)[results_size]);
        translate_channel_scan_results(res, out);
        results_size += 1;
    }

    return BML_RET_OK;
}

int bml_internal::start_dcs_single_scan(const sMacAddr &mac, int dwell_time_ms,
                                        unsigned int *channel_pool, int channel_pool_size)
{
    LOG(DEBUG) << "start_single_channel_scan";

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST>(cmdu_tx);

    if (!request) {
        LOG(ERROR) << "Failed building cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->scan_params().radio_mac         = mac;
    request->scan_params().dwell_time_ms     = dwell_time_ms;
    request->scan_params().channel_pool_size = channel_pool_size;
    if (channel_pool && channel_pool_size > 0 &&
        channel_pool_size <= BML_CHANNEL_SCAN_MAX_CHANNEL_POOL_SIZE) {
        std::copy_n(channel_pool, channel_pool_size, request->scan_params().channel_pool);
    }

    LOG(DEBUG) << "mac:" << mac << ", dwell_time:" << request->scan_params().dwell_time_ms
               << ", channel_pool_size:" << (int)request->scan_params().channel_pool_size << ".";

    int result = 0;
    if (send_bml_cmdu(result, request->get_action_op()) != BML_RET_OK) {
        LOG(ERROR) << "Send cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST failed";
        return (-BML_RET_OP_FAILED);
    }

    if (result != int(eChannelScanOpErrCode::CHANNEL_SCAN_OP_SUCCESS)) {
        LOG(ERROR) << "cACTION_BML_CHANNEL_SCAN_START_SCAN_REQUEST returned error code:" << result;
        return result;
    }

    return BML_RET_OK;
}

int bml_internal::ping()
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            LOG(ERROR) << "ping - connect_to_master failed";
            return iRet;
        }
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmPing;
    m_prmPing = &prmPing;

    //CMDU Message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_PING_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building PING message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending PING message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    // Wait for the duration of the RESPONSE_TIMEOUT for an answer
    // std::unique_lock<std::mutex> lk(m_mtxLock);
    if (!prmPing.wait_for(RESPONSE_TIMEOUT)) {
        LOG(WARNING) << "Timeout on PING to beerocks master";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the promise holder
    m_prmPing = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "PING to master failed!";
        return (-BML_RET_OP_FAILED);
    }

    return (iRet);
}

void bml_internal::register_nw_map_query_cb(BML_NW_MAP_QUERY_CB pCB) { m_cbNetMapQuery = pCB; }

int bml_internal::register_nw_map_update_cb(BML_NW_MAP_QUERY_CB pCB)
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK)
            return iRet;
    }

    if ((m_cbNetMapUpdate == nullptr) && (pCB == nullptr)) {
        LOG(WARNING) << "Network map update callback function was NOT registered...";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    m_cbNetMapUpdate = pCB;

    // Build and send the message
    if (m_cbNetMapUpdate) {
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }

        if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
            LOG(ERROR) << "Failed sending ACTION_BML_REGISTER_TO_NW_MAP_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }
    } else {
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR)
                << "Failed building ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }

        if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
            LOG(ERROR)
                << "Failed sending ACTION_BML_UNREGISTER_FROM_NW_MAP_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }
    }

    return (BML_RET_OK);
}

int bml_internal::nw_map_query()
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK)
            return iRet;
    }

    // TODO: Fail if a callback was NOT registered?
    if (!m_cbNetMapQuery) {
        LOG(WARNING) << "Network map callback function was NOT registered...";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // Build and send the NW_MAP_REQUEST message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_NW_MAP_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_NW_MAP_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending NW_MAP_QUERY message!";
        return (false);
    }

    return (BML_RET_OK);
}

int bml_internal::register_stats_cb(BML_STATS_UPDATE_CB pCB)
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK)
            return iRet;
    }

    if ((m_cbStatsUpdate == nullptr) && (pCB == nullptr)) {
        LOG(WARNING) << "Statistics update callback function was NOT registered...";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    m_cbStatsUpdate = pCB;

    //CMDU message
    if (m_cbStatsUpdate) {
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building REGISTER_TO_STATS_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }
    } else {
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNREGISTER_FROM_STATS_UPDATES_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building UNREGISTER_TO_STATS_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }
    }

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending ACTION_BML_REGISTER_TO_STATS_UPDATES_REQUEST message!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    return (BML_RET_OK);
}

int bml_internal::register_event_cb(BML_EVENT_CB pCB)
{
    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK)
            return iRet;
    }

    if ((m_cbEvent == nullptr) && (pCB == nullptr)) {
        LOG(WARNING) << "Event callback function was NOT registered...";
        return (-BML_RET_OP_FAILED);
    }

    m_cbEvent = pCB;

    //CMDU message
    if (m_cbEvent) {
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_BML_REGISTER_TO_EVENTS_UPDATES_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building REGISTER_TO_EVENTS_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }
    } else {
        auto request = message_com::create_vs_message<
            beerocks_message::cACTION_BML_UNREGISTER_FROM_EVENTS_UPDATES_REQUEST>(cmdu_tx);

        if (request == nullptr) {
            LOG(ERROR) << "Failed building UNREGISTER_TO_EVENTS_UPDATES_REQUEST message!";
            return (-BML_RET_OP_FAILED);
        }
    }

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR)
            << "Failed sending ACTION_BML_REGISTER/UNREGISTER_TO_EVENTS_UPDATES_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    return (BML_RET_OK);
}

int bml_internal::set_wifi_credentials(const std::string ssid, const std::string pass, int sec,
                                       int vap_id, int force)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Validate SSID length
    // Workaround for SSID containing consecutive spaces or start/ end with spaces
    if (ssid[0] == ' ' || ssid[ssid.length() - 1] == ' ') {
        LOG(ERROR) << "Invalid SSID. SSID cannot start or end with space";
        return (-BML_RET_INVALID_ARGS);
    }
    if (ssid.find("  ") != std::string::npos) {
        LOG(ERROR) << "Invalid SSID. SSID cannot contain consecutive spaces";
        return (-BML_RET_INVALID_ARGS);
    }

    // Max length should be 31 chars, need last byte for null char to avoid overflow
    if (ssid.length() == 0 || ssid.length() >= message::WIFI_SSID_MAX_LENGTH) {
        LOG(ERROR) << "Invalid SSID (Len = " << int(ssid.length()) << "): " << ssid;
        return (-BML_RET_INVALID_ARGS);
    }

    // Validate Pass length
    // Max length should be 63 chars, need last byte for null char to avoid overflow
    if (pass.length() == 0 || pass.length() >= message::WIFI_PASS_MAX_LENGTH) {
        LOG(ERROR) << "Invalid PASS (Len = " << int(pass.length()) << "): " << pass;
        return (-BML_RET_INVALID_ARGS);
    }

    // Workaround for password containing consecutive spaces or start/ end with spaces
    if (pass[0] == ' ' || pass[pass.length() - 1] == ' ') {
        LOG(ERROR) << "Invalid PASS. Password cannot start or end with space";
        return (-BML_RET_INVALID_ARGS);
    }
    if (pass.find("  ") != std::string::npos) {
        LOG(ERROR) << "Invalid PASS. Password cannot contain consecutive spaces";
        return (-BML_RET_INVALID_ARGS);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmWiFiCredentialsUpdate;
    m_prmWiFiCredentialsUpdate = &prmWiFiCredentialsUpdate;
    int iOpTimeout             = 60000; // Default timeout

    // Send to master for credential update
    LOG(TRACE) << "BML:: Config update prepareing message to send to master";

    // Increase the operation timeout to 3 minutes (for global update)
    iOpTimeout = 180000;

    // Command supported only on local master
    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        // Clear the promise holder
        m_prmWiFiCredentialsUpdate = nullptr;
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK) {
            // Clear the promise holder
            m_prmWiFiCredentialsUpdate = nullptr;
            return iRet;
        }
    }

    auto config = message_com::create_vs_message<
        beerocks_message::cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST>(cmdu_tx);

    if (config == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST message!";
        // Clear the promise holder
        m_prmWiFiCredentialsUpdate = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    string_utils::copy_string(config->params().ssid, ssid.c_str(), message::WIFI_SSID_MAX_LENGTH);
    string_utils::copy_string(config->params().pass, pass.c_str(), message::WIFI_PASS_MAX_LENGTH);

    switch (sec) {
    case BML_WLAN_SEC_NONE:
        config->params().sec = beerocks_message::eWiFiSec_None;
        break;

    case BML_WLAN_SEC_WEP64:
        config->params().sec = beerocks_message::eWiFiSec_WEP64;
        break;

    case BML_WLAN_SEC_WEP128:
        config->params().sec = beerocks_message::eWiFiSec_WEP128;
        break;

    case BML_WLAN_SEC_WPA_PSK:
        config->params().sec = beerocks_message::eWiFiSec_WPA_PSK;
        break;

    case BML_WLAN_SEC_WPA2_PSK:
        config->params().sec = beerocks_message::eWiFiSec_WPA2_PSK;
        break;

    case BML_WLAN_SEC_WPA_WPA2_PSK:
        config->params().sec = beerocks_message::eWiFiSec_WPA_WPA2_PSK;
        break;

    default: {
        LOG(WARNING) << "Unsupported Wi-Fi security: " << sec;
        // Clear the promise holder
        m_prmWiFiCredentialsUpdate = nullptr;
        return (false);
    }
    }

    // Set the rest of the message values
    config->params().vap_id = uint8_t(vap_id);
    config->params().force  = (force) ? 1 : 0;

    // Build and send the message
    LOG(TRACE) << "Sending config update message to master";

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending configuration update message!";
        // Clear the promise holder
        m_prmWiFiCredentialsUpdate = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmWiFiCredentialsUpdate.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration update response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the promise holder
    m_prmWiFiCredentialsUpdate = nullptr;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration update failed!";
        return (-BML_RET_OP_FAILED);
    }

    return (iRet);
}

int bml_internal::get_wifi_credentials(int vap_id, char *ssid, char *pass, int *sec)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Validate ssid
    if (ssid == nullptr) {
        LOG(ERROR) << "Invalid ssid ptr";
        return (-BML_RET_INVALID_ARGS);
    }

    // Validate ssid
    if (sec == nullptr) {
        LOG(ERROR) << "Invalid sec ptr";
        return (-BML_RET_INVALID_ARGS);
    }

    // Validate vap_id
    if (vap_id < 0 || vap_id >= BML_NODE_MAX_VAPS) {
        LOG(ERROR) << "Invalid vap_id=" << vap_id;
        return (-BML_RET_INVALID_ARGS);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmWiFiCredentialsGet;
    m_prmWiFiCredentialsGet = &prmWiFiCredentialsGet;
    int iOpTimeout          = RESPONSE_TIMEOUT; // Default timeout

    //CMDU message
    beerocks_message::sWifiCredentials sWifiCredentials;
    m_wifi_credentials = &sWifiCredentials;

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->vap_id() = vap_id;

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending cACTION_PLATFORM_WIFI_CREDENTIALS_GET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmWiFiCredentialsGet.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the credentials member
    m_wifi_credentials = nullptr;

    // Clear the promise holder
    m_prmWiFiCredentialsGet = nullptr;

    string_utils::copy_string(ssid, sWifiCredentials.ssid, BML_NODE_SSID_LEN);
    if (pass != nullptr) {
        string_utils::copy_string(pass, sWifiCredentials.pass, BML_NODE_PASS_LEN);
    }
    *sec = sWifiCredentials.sec;

    //clear the memory with password in it.
    memset(&sWifiCredentials.pass, 0, sizeof(sWifiCredentials.pass));
    //memset might be optimized and compiler might not set it 0 if its not used after memset
    *(volatile char *)sWifiCredentials.pass = *(volatile char *)sWifiCredentials.pass;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
        return (iRet);
    }

    return (iRet);
}

int bml_internal::get_onboarding_state(int *enable)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmOnboard;
    m_prmOnboard = &prmOnboard;

    //CMDU message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ONBOARD_QUERY_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_PLATFORM_ONBOARD_QUERY_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending ACTION_PLATFORM_ONBOARD_QUERY_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    // Wait for the duration of the RESPONSE_TIMEOUT for an answer
    if (!prmOnboard.wait_for(RESPONSE_TIMEOUT)) {
        LOG(WARNING) << "connect - Timeout on ONBOARD REQUEST request!";

        iRet = (-BML_RET_TIMEOUT);
    }

    if (iRet == BML_RET_OK) {
        *enable = (int)m_fOnboarding;
    }

    m_prmOnboard = nullptr;
    return iRet;
}

int bml_internal::set_onboarding_state(int enable)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    if (enable < 0)
        enable = 0;
    else if (enable > 1)
        enable = 1;

    // Query the platform manager about the onboarding state
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_ONBOARD_SET_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_PLATFORM_ONBOARD_SET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->params().onboarding = enable;

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending ACTION_PLATFORM_ONBOARD_SET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    // We are not expected to receive response
    m_prmOnboard = nullptr;
    return (BML_RET_OK);
}

int bml_internal::bml_wps_onboarding(const char *iface)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }
    // Query the platform manager about the onboarding state
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_WPS_ONBOARDING_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_PLATFORM_WPS_ONBOARDING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    string_utils::copy_string(request->iface_name(message::IFACE_NAME_LENGTH), iface,
                              message::IFACE_NAME_LENGTH);

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending ACTION_PLATFORM_WPS_ONBOARDING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    // We are not expected to receive response
    m_prmOnboard = nullptr;
    return (BML_RET_OK);
}

int bml_internal::get_administrator_credentials(char *user_password)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Validate user_password
    if (user_password == nullptr) {
        LOG(ERROR) << "Invalid user_password ptr";
        return (-BML_RET_INVALID_ARGS);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmAdminCredentialsGet;
    m_prmAdminCredentialsGet = &prmAdminCredentialsGet;
    int iOpTimeout           = RESPONSE_TIMEOUT; // Default timeout

    beerocks_message::sAdminCredentials AdminCredentials;
    m_admin_credentials = &AdminCredentials;

    // Build and send the message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_PLATFORM_ADMIN_CREDENTIALS_GET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending configuration get message!";
        m_prmAdminCredentialsGet = nullptr;
        m_admin_credentials      = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmAdminCredentialsGet.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the credentials member
    m_admin_credentials = nullptr;

    // Clear the promise holder
    m_prmAdminCredentialsGet = nullptr;

    string_utils::copy_string(user_password, AdminCredentials.user_password,
                              BML_NODE_USER_PASS_LEN);

    //clear the memory with password in it.
    memset(&AdminCredentials.user_password, 0, sizeof(AdminCredentials.user_password));
    //memset might be optimized and compiler might not set it 0 if its not used after memset
    *(volatile char *)AdminCredentials.user_password =
        *(volatile char *)AdminCredentials.user_password;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
        return (iRet);
    }

    return (iRet);
}

int bml_internal::get_device_info(BML_DEVICE_INFO &device_info)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmDeviceInfoGet;
    m_prmDeviceInfoGet = &prmDeviceInfoGet;
    int iOpTimeout     = RESPONSE_TIMEOUT; // Default timeout

    beerocks_message::sDeviceInfo DeviceInfo;
    m_device_info = &DeviceInfo;

    //CMDU message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_PLATFORM_DEVICE_INFO_GET_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_PLATFORM_DEVICE_INFO_GET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending ACTION_PLATFORM_DEVICE_INFO_GET_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmDeviceInfoGet.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for device info get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the credentials member
    m_device_info = nullptr;

    // Clear the promise holder
    m_prmDeviceInfoGet = nullptr;

    string_utils::copy_string(device_info.manufacturer, DeviceInfo.manufacturer, BML_DEV_INFO_LEN);
    string_utils::copy_string(device_info.model_name, DeviceInfo.model_name, BML_DEV_INFO_LEN);
    string_utils::copy_string(device_info.serial_number, DeviceInfo.serial_number,
                              BML_DEV_INFO_LEN);

    // LAN
    string_utils::copy_string(device_info.lan_iface_name, DeviceInfo.lan_iface_name,
                              BML_IFACE_NAME_LEN);
    device_info.lan_ip_address   = DeviceInfo.lan_ip_address;
    device_info.lan_network_mask = DeviceInfo.lan_network_mask;

    // WAN
    string_utils::copy_string(device_info.wan_iface_name, DeviceInfo.wan_iface_name,
                              BML_IFACE_NAME_LEN);
    device_info.wan_ip_address   = DeviceInfo.wan_ip_address;
    device_info.wan_network_mask = DeviceInfo.wan_network_mask;

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Device information get failed!";
        return (iRet);
    }

    return (iRet);
}

int bml_internal::set_client_roaming(bool enable)
{
    //CMDU message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_SET_CLIENT_ROAMING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_client_roaming(int &result)
{
    //CMDU message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_CLIENT_ROAMING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_legacy_client_roaming(bool enable)
{
    //CMDU message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_legacy_client_roaming(int &result)
{
    //CMDU message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_LEGACY_CLIENT_ROAMING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_client_band_steering(bool enable)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_SET_CLIENT_BAND_STEERING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_client_band_steering(int &result)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_CLIENT_BAND_STEERING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_client_roaming_prefer_signal_strength(bool enable)
{
    //CMDU message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building "
                      "ACTION_BML_SET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_client_roaming_prefer_signal_strength(int &result)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building "
                      "ACTION_BML_GET_CLIENT_ROAMING_PREFER_SIGNAL_STRENGTH_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_ire_roaming(bool enable)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_SET_IRE_ROAMING_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_SET_IRE_ROAMING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_ire_roaming(int &result)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_GET_IRE_ROAMING_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_IRE_ROAMING_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_load_balancer(bool enable)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_SET_LOAD_BALANCER_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_SET_LOAD_BALANCER_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_load_balancer(int &result)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_GET_LOAD_BALANCER_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_LOAD_BALANCER_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_service_fairness(bool enable)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_SET_SERVICE_FAIRNESS_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_SET_SERVICE_FAIRNESS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_service_fairness(int &result)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_GET_SERVICE_FAIRNESS_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_SERVICE_FAIRNESS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_dfs_reentry(int &result)
{
    //CMDU message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_GET_DFS_REENTRY_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_GET_DFS_REENTRY_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_dfs_reentry(bool enable)
{
    //CMDU message
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_SET_DFS_REENTRY_REQUEST>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_SET_DFS_REENTRY_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_certification_mode(int &result)
{
    //CMDU message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_GET_CERTIFICATION_MODE_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_GET_CERTIFICATION_MODE_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_certification_mode(bool enable)
{
    //CMDU message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_SET_CERTIFICATION_MODE_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_SET_CERTIFICATION_MODE_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->isEnable() = enable;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::set_log_level(const std::string module_name, const std::string log_level,
                                uint8_t on, const std::string mac)
{
    // Build and send the SET_LOG_LEVEL_REQUEST message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->params().mac = network_utils::mac_from_string(mac);
    if (log_level == "i")
        request->params().log_level = LOG_LEVEL_INFO;
    else if (log_level == "d")
        request->params().log_level = LOG_LEVEL_DEBUG;
    else if (log_level == "e")
        request->params().log_level = LOG_LEVEL_ERROR;
    else if (log_level == "f")
        request->params().log_level = LOG_LEVEL_FATAL;
    else if (log_level == "t")
        request->params().log_level = LOG_LEVEL_TRACE;
    else if (log_level == "w")
        request->params().log_level = LOG_LEVEL_WARNING;
    else if (log_level == "a")
        request->params().log_level = LOG_LEVEL_ALL;
    else {
        LOG(ERROR) << "log_level not valid";
        return -BML_RET_INVALID_ARGS;
    }

    if (module_name == "master")
        request->params().module_name = BEEROCKS_PROCESS_MASTER;
    else if (module_name == "slave")
        request->params().module_name = BEEROCKS_PROCESS_SLAVE;
    else if (module_name == "monitor")
        request->params().module_name = BEEROCKS_PROCESS_MONITOR;
    else if (module_name == "platform")
        request->params().module_name = BEEROCKS_PROCESS_MONITOR;
    else if (module_name == "all")
        request->params().module_name = BEEROCKS_PROCESS_ALL;
    else {
        LOG(ERROR) << "module_name not valid";
        return -BML_RET_INVALID_ARGS;
    }
    request->params().enable = on;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    return (iRet);
}

int bml_internal::get_master_slave_versions(char *master_version, char *slave_version)
{
    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockPlatform == nullptr && !connect_to_platform()) {
        return (-BML_RET_CONNECT_FAIL);
    }

    // Validate master_version
    if (master_version == nullptr) {
        LOG(ERROR) << "Invalid master_version ptr";
        return (-BML_RET_INVALID_ARGS);
    }

    // Validate slave_version
    if (slave_version == nullptr) {
        LOG(ERROR) << "Invalid slave_version ptr";
        return (-BML_RET_INVALID_ARGS);
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmMasterSlaveVersions;
    m_prmMasterSlaveVersions = &prmMasterSlaveVersions;
    int iOpTimeout           = RESPONSE_TIMEOUT; // Default timeout

    beerocks_message::sVersions sVersion;
    m_master_slave_versions = &sVersion;

    // Build and send the message
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_PLATFORM_GET_MASTER_SLAVE_VERSIONS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!message_com::send_cmdu(m_sockPlatform, cmdu_tx)) {
        LOG(ERROR) << "Failed sending master_slave_versions request!";
        m_prmMasterSlaveVersions = nullptr;
        m_master_slave_versions  = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmMasterSlaveVersions.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for master version response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the versions member
    m_master_slave_versions = nullptr;

    // Clear the promise holder
    m_prmMasterSlaveVersions = nullptr;

    string_utils::copy_string(master_version, sVersion.master_version, message::VERSION_LENGTH);
    string_utils::copy_string(slave_version, sVersion.slave_version, message::VERSION_LENGTH);

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "master_slave_versions request failed!";
        return (iRet);
    }

    return (iRet);
}

int bml_internal::set_log_context(void *log_ctx)
{
    if (!log_ctx) {
        LOG(ERROR) << "Invalid log context!";
        return (-BML_RET_INVALID_ARGS);
    }

    el::base::type::StoragePointer *log_storage =
        static_cast<el::base::type::StoragePointer *>(log_ctx);

    el::Helpers::setStorage(*log_storage);
    s_fExtLogContext = true;

    return (BML_RET_OK);
}

int bml_internal::set_restricted_channels(const uint8_t *restricted_channels, const std::string mac,
                                          uint8_t is_global, uint8_t size)
{
    // // If the socket is not valid, attempt to re-establish the connection
    // if (m_sockPlatform == nullptr && !connect_to_platform()) {
    //     return (-BML_RET_CONNECT_FAIL);
    // }
    LOG(DEBUG) << "bml_internal::set_restricted_channels entry";
    // Validate restricted_channels
    if (restricted_channels == nullptr) {
        LOG(ERROR) << "Invalid restricted_channels ptr";
        return (-BML_RET_INVALID_ARGS);
    }

    // Validate mac
    if (!is_global) {
        if (mac.empty()) {
            LOG(ERROR) << "Invalid mac string";
            return (-BML_RET_INVALID_ARGS);
        }
    }

    // Validate size
    if (!size || size > BML_NODE_RESTRICTED_CHANNELS_LEN) {
        LOG(ERROR) << "Invalid size ";
        return (-BML_RET_INVALID_ARGS);
    }
    LOG(DEBUG) << "bml_internal::set_restricted_channels after sanity";

    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_SET_RESTRICTED_CHANNELS_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    std::copy_n(restricted_channels, size, request->params().restricted_channels);
    request->params().is_global = is_global;
    if (!is_global) {
        request->params().hostap_mac = network_utils::mac_from_string(mac);
    }
    LOG(DEBUG) << "mac = " << mac;

    int result;
    int iRet = send_bml_cmdu(result, request->get_action_op());

    LOG(DEBUG) << "bml_internal::set_restricted_channels after send_bml_message";
    return (iRet);
}

int bml_internal::get_restricted_channels(uint8_t *restricted_channels, const std::string mac,
                                          uint8_t is_global)
{

    if (!is_local_master()) {
        LOG(ERROR) << "Command supported only on local master!";
        return (-BML_RET_OP_NOT_SUPPORTED);
    }

    // If the socket is not valid, attempt to re-establish the connection
    if (m_sockMaster == nullptr) {
        int iRet = connect_to_master();
        if (iRet != BML_RET_OK)
            return iRet;
    }

    // Validate mac
    if (!is_global) {
        if (mac.empty()) {
            LOG(ERROR) << "Invalid mac string";
            return (-BML_RET_INVALID_ARGS);
        }
    }

    // Initialize the promise for receiving the response
    beerocks::promise<bool> prmRestrictedChannelsGet;
    m_prmRestrictedChannelsGet = &prmRestrictedChannelsGet;
    int iOpTimeout             = RESPONSE_TIMEOUT; // Default timeout

    beerocks_message::sRestrictedChannels sRestrictedChannels{};
    m_Restricted_channels = &sRestrictedChannels;

    // Check whether the credentials should be applied locally in onboarding or forwarded to master
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_GET_RESTRICTED_CHANNELS_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building ACTION_BML_GET_RESTRICTED_CHANNELS_REQUES message!";
        return (-BML_RET_OP_FAILED);
    }

    if (!is_global) {
        request->params().hostap_mac = network_utils::mac_from_string(mac);
    }
    request->params().is_global = is_global;

    // Build and send the message
    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending configuration get message!";
        m_prmRestrictedChannelsGet = nullptr;
        m_Restricted_channels      = nullptr;
        return (-BML_RET_OP_FAILED);
    }

    int iRet = BML_RET_OK;

    if (!prmRestrictedChannelsGet.wait_for(iOpTimeout)) {
        LOG(WARNING) << "Timeout while waiting for configuration get response...";
        iRet = -BML_RET_TIMEOUT;
    }

    // Clear the credentials member
    m_Restricted_channels = nullptr;

    // Clear the promise holder
    m_prmRestrictedChannelsGet = nullptr;

    std::copy_n(sRestrictedChannels.restricted_channels, BML_NODE_RESTRICTED_CHANNELS_LEN,
                restricted_channels);

    if (iRet != BML_RET_OK) {
        LOG(ERROR) << "Configuration get failed!";
        return (iRet);
    }

    return (iRet);
}

int bml_internal::topology_discovery(const char *al_mac)
{
    auto request =
        message_com::create_vs_message<beerocks_message::cACTION_BML_TRIGGER_TOPOLOGY_QUERY>(
            cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_TRIGGER_TOPOLOGY_QUERY message";
        return (-BML_RET_OP_FAILED);
    }

    request->al_mac() = network_utils::mac_from_string(al_mac);

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending CMDU TRIGGER_TOPOLOGY_QUERY message";
        return (-BML_RET_OP_FAILED);
    }

    return BML_RET_OK;
}

int bml_internal::channel_selection(const char *al_mac, const char *ruid)
{
    auto request = message_com::create_vs_message<
        beerocks_message::cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST>(cmdu_tx);

    if (request == nullptr) {
        LOG(ERROR) << "Failed building cACTION_BML_TRIGGER_CHANNEL_SELECTION_REQUEST message!";
        return (-BML_RET_OP_FAILED);
    }

    request->al_mac() = network_utils::mac_from_string(al_mac);
    request->ruid()   = network_utils::mac_from_string(ruid);

    if (!message_com::send_cmdu(m_sockMaster, cmdu_tx)) {
        LOG(ERROR) << "Failed sending message!";
        return (-BML_RET_OP_FAILED);
    }

    return BML_RET_OK;
}

bool bml_internal::wake_up(uint8_t action_opcode, int value)
{
    std::unique_lock<std::mutex> lock(m_mtxLock);
    if (m_prmCliResponses.count(action_opcode) == 0) {
        return false;
    }
    m_prmCliResponses[action_opcode]->set_value(value);
    return true;
}
