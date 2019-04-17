/*
* INTEL CONFIDENTIAL
* Copyright 2019 Intel Corporation All Rights Reserved.
*
* The source code contained or described herein and all documents related to
* the source code ("Material") are owned by Intel Corporation or its
* suppliers or licensors.  Title to the Material remains with Intel
* Corporation or its suppliers and licensors.  The Material contains trade
* secrets and proprietary and confidential information of Intel or its
* suppliers and licensors.  The Material is protected by worldwide copyright
* and trade secret laws and treaty provisions. No part of the Material may
* be used, copied, reproduced, modified, published, uploaded, posted,
* transmitted, distributed, or disclosed in any way without Intel's prior
* express written permission.
*
* No license under any patent, copyright, trade secret or other intellectual
* property right is granted to or conferred upon you by disclosure or
* delivery of the Materials,  either expressly, by implication, inducement,
* estoppel or otherwise.  Any license under such intellectual property
* rights must be express and approved by Intel in writing.
*/

#include "btl.h"

#include <beerocks/bcl/network/network_utils.h>
#include <easylogging++.h>

using namespace beerocks::btl;
using namespace beerocks::net;

const std::string transport_socket_thread::MULTICAST_MAC_ADDR = "01:80:c2:00:00:13";

transport_socket_thread::transport_socket_thread(const std::string& unix_socket_path_) :
    socket_thread(unix_socket_path_)
{
    set_select_timeout(DEFAULT_SELECT_TIMEOUT_MS);
}

transport_socket_thread::~transport_socket_thread()
{
}

bool transport_socket_thread::init()
{
    bus_init();
    if(!socket_thread::init()) {
        THREAD_LOG(ERROR) << "Failed to init socket_thread";
        return false;
    }

    return true;    
}

void transport_socket_thread::set_select_timeout(unsigned msec)
{
    poll_timeout_ms = msec;
}

bool transport_socket_thread::send_cmdu_to_bus(ieee1905_1::CmduMessageTx& cmdu_tx, const std::string& dst_mac, const std::string& src_mac)
{
    if (!cmdu_tx.finalize(true)) {
        THREAD_LOG(ERROR) << "finalize failed";
        return false;
    }

    return send_cmdu_to_bus(cmdu_tx, dst_mac, src_mac, cmdu_tx.getMessageLength());
}

bool transport_socket_thread::send_cmdu_to_bus(ieee1905_1::CmduMessage& cmdu, const std::string& dst_mac, const std::string& src_mac, uint16_t length)
{
    // This method should be used by Message Routers only. It is used to forward CMDU messages from UDS socket to the BUS.
    LOG_IF(!bus, FATAL) << "Bus is not allocated!";
    
    // set bridge address
    if (src_mac.empty()) {
        THREAD_LOG(ERROR) << "src_mac is empty!";  
        return false;
    }

    if (dst_mac.empty()) {
        THREAD_LOG(ERROR) << "dst_mac is empty!";  
        return false;
    }
    
    return bus_send(cmdu, dst_mac, src_mac, length);
}
