/*
* INTEL CONFIDENTIAL
* Copyright 2018 Intel Corporation All Rights Reserved.
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

#ifndef _SWAP_H_
#define _SWAP_H_

#if __linux
#elif __unix
#elif __posix
#else
#ifndef IS_WINDOWS
#define IS_WINDOWS
#endif
#endif


#ifdef IS_WINDOWS
#include <winsock2.h>
#else // Linux
#include <stdint.h>
#include <arpa/inet.h>
#endif

inline uint64_t htonll(uint64_t x){
    return ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32));
}
// net message are in be, only swap be to le
inline void swap_64(int64_t &v)
{
   v = htonll(v);
}

inline void swap_64(uint64_t &v)
{
   v = htonll(v);
}

inline void swap_32(int32_t &v)
{
   v = htonl(v);
}

inline void swap_32(uint32_t &v)
{
   v = htonl(v);
}

inline void swap_16(int16_t &v)
{
   v = htons(v);
}

inline void swap_16(uint16_t &v)
{
   v = htons(v);
}

inline void tlvf_swap(uint8_t size, uint8_t* ptr)
{
   switch (size)
   {
       case 16:
       {
            uint16_t *tmp16 = reinterpret_cast<uint16_t*>(ptr);
            swap_16(*tmp16);
           break;
       }
       case 32:
       {
            uint32_t *tmp32 = reinterpret_cast<uint32_t*>(ptr);
            swap_32(*tmp32);
           break;
       }
       case 64:
       {
            uint64_t *tmp64 = reinterpret_cast<uint64_t*>(ptr);
            swap_64(*tmp64);
        break;
       }
       default:
        break;
   }
}

inline void tlvf_swap(uint8_t size, int8_t* ptr)
{
   switch (size)
   {
       case 16:
       {
            int16_t *tmp16 = reinterpret_cast<int16_t*>(ptr);
            swap_16(*tmp16);
           break;
       }
       case 32:
       {
            int32_t *tmp32 = reinterpret_cast<int32_t*>(ptr);
            swap_32(*tmp32);
           break;
       }
       case 64:
       {
            int64_t *tmp64 = reinterpret_cast<int64_t*>(ptr);
            swap_64(*tmp64);
        break;
       }
       default:
        break;
   }
}

#endif
