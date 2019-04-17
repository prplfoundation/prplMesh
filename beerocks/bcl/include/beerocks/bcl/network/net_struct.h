/*
* INTEL CONFIDENTIAL
* Copyright 2016 Intel Corporation All Rights Reserved.
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
