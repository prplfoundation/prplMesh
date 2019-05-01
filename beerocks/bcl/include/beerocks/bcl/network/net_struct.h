/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _NET_STRUCT_
#define _NET_STRUCT_

#include <stdint.h>
#include <cstring>

namespace beerocks {
namespace net {

enum eNetworkStructsConsts {
    MAC_ADDR_LEN = 6,
    IP_ADDR_LEN  = 4,
};

typedef struct sIpv4Addr {
    uint8_t oct[IP_ADDR_LEN];
    void struct_swap(){
    }
    void struct_init(){
        for(size_t i=0; i<IP_ADDR_LEN; i++){
            oct[i] = 0;
        }
    }
} sIpv4Addr;

typedef struct sMacAddr {
    uint8_t oct[MAC_ADDR_LEN];
    uint8_t channel;
    int8_t rssi;
    void struct_swap(){
    }
    void struct_init(){
        for(size_t i=0; i<MAC_ADDR_LEN; i++){
            oct[i] = 0;
        }
        channel = 0;
        rssi = 0;
    }
} sMacAddr;

}
}

#endif
