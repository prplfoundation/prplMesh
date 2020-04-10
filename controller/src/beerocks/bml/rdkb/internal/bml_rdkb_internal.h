/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_RDKB_INTERNAL_H_
#define _BML_RDKB_INTERNAL_H_

#include <bcl/beerocks_config_file.h>
#include <bcl/beerocks_promise.h>
#include <bcl/beerocks_socket_thread.h>

#include <beerocks/tlvf/beerocks_message_common.h>

#include "../../internal/bml_internal.h"
#include "../bml_rdkb_defs.h"

#include <list>
#include <map>
#include <mutex>

class bml_rdkb_internal : public bml_internal {

public:
    /*
    * A steering group defines a group of apIndex's which can have steering done
    * between them.
    * To remove a group configuration call with NULL as cfg_2 and cfg_5.
    * @param[in] steeringGroupIndex  Wifi Steering Group index
    * @param[in] cfg_2               2.4G apConfig
    * @param[in] cfg_5               5G apConfig
    *
    * @return BML_RET_OK on success.
    *
    * @warning All apIndex's provided within a group must have the same SSID,
    * encryption, and passphrase configured for steering to function properly.
    *
    */
    int steering_set_group(uint32_t steeringGroupIndex, BML_STEERING_AP_CONFIG *cfg_2,
                           BML_STEERING_AP_CONFIG *cfg_5);
    /**
     * Call this function to add/modify per-client configuration config of client_mac.
     * To remove a client configuration call with NULL as config.
     * @param[in] steeringGroupIndex   Wifi Steering Group index
     * @param[in] bssid                AP bssid.
     * @param[in] client_mac           The Client's MAC address.
     * @param[in] config               The client configuration
     * 
     * @return RETURN_OK on success.
     */
    int steering_client_set(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                            const BML_MAC_ADDR client_mac, BML_STEERING_CLIENT_CONFIG *config);
    /**
     * Call this function to register/unregister the callback function.
     *
     * @param[in] pCB  a callback function pointer or NULL to unregister.
     * 
     * @return RETURN_OK on success.
     */
    int steering_event_register(BML_EVENT_CB pCB);
    /** 
    *
    * @param[in] steeringGroupIndex  Wifi Steering Group index
    * @param[in] bssid               AP bssid.   
    * @param[in] client_mac           The Client's MAC address.
    *
    * @return BML_RET_OK on success.
    *
    */
    int steering_client_measure(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                const BML_MAC_ADDR client_mac);
    /**Initiate a Client Disconnect.
     *
     * This is used to kick off a client, for steering purposes.
     *
     * @param[in]  steeringgroupIndex  Wifi Steering Group index
     * @param[in]  bssid               AP bssid.
     * @param[in]  client_mac          The Client's MAC address
     * @param[in]  type                Disconnect Type
     * @param[in]  reason              Reason code to provide in deauth/disassoc frame.
     *
     * @return BML_RET_OK on success.
     */
    int steering_client_disconnect(uint32_t steeringGroupIndex, const BML_MAC_ADDR bssid,
                                   const BML_MAC_ADDR client_mac, BML_DISCONNECT_TYPE type,
                                   uint32_t reason);

protected:
    virtual bool handle_cmdu(Socket *sd, ieee1905_1::CmduMessageRx &cmdu_rx) override;
    virtual int process_cmdu_header(std::shared_ptr<beerocks::beerocks_header> beerocks_header);

private:
    bool handle_steering_event_update(uint8_t *data_buffer);
    beerocks::promise<int> *m_prmRdkbWlan = nullptr;
    BML_EVENT_CB m_cbSteeringEvent        = nullptr;
};

#endif /* _BML_RDKB_INTERNAL_H_ */
