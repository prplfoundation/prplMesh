/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BPL_CFG_UCI_H_
#define BPL_CFG_UCI_H_

#ifdef BEEROCKS_UGW

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define restrict __restrict
#include <libsafec/safe_str_lib.h>
#undef snprintf_s
#define snprintf_s snprintf

#ifndef u_int_32
#define u_int_32 unsigned int
#endif

#ifndef _cplusplus
#include <stdbool.h>
#endif

#define CRIT(fmt, args...) LOGF_LOG_CRIT(fmt, ##args)
#define ERROR(fmt, args...) LOGF_LOG_ERROR(fmt, ##args)
#define WARN(fmt, args...) LOGF_LOG_WARN(fmt, ##args)
#define INFO(fmt, args...) LOGF_LOG_INFO(fmt, ##args)
#define DEBUG(fmt, args...) LOGF_LOG_DEBUG(fmt, ##args)

#define MAX_UCI_BUF_LEN 64
#define DUMMY_VAP_OFFSET 100
#define MAX_NUM_OF_RADIOS 3

/* use only even phy numbers since odd phy's are used for station interfaces */
#define RADIO_INDEX_SKIP 2
#define VAP_RPC_IDX_OFFSET 10
#define MAX_VAPS_PER_RADIO 16

#define UCI_INDEX(iftype, index)                                                                   \
    iftype == TYPE_RADIO                                                                           \
        ? (RADIO_INDEX_SKIP * index)                                                               \
        : (VAP_RPC_IDX_OFFSET + (MAX_VAPS_PER_RADIO * RADIO_INDEX_SKIP * (index % 2)) +            \
           ((index - (index % 2)) / 2))

#define UCI_RETURN_INDEX(iftype, rpcIndex)                                                         \
    iftype == TYPE_RADIO                                                                           \
        ? (rpcIndex / RADIO_INDEX_SKIP)                                                            \
        : (2 * (rpcIndex -                                                                         \
                (VAP_RPC_IDX_OFFSET +                                                              \
                 MAX_VAPS_PER_RADIO * ((rpcIndex - VAP_RPC_IDX_OFFSET) / MAX_VAPS_PER_RADIO))) +   \
           (((rpcIndex - VAP_RPC_IDX_OFFSET) / MAX_VAPS_PER_RADIO) / RADIO_INDEX_SKIP))

enum paramType { TYPE_RADIO = 0, TYPE_VAP };

#elif BEEROCKS_RDKB

#include <slibc/stdio.h>
#include <slibc/string.h>

extern "C" {
#include <uci_wrapper.h>

#define UCI_INDEX(iftype, rpc_index) rpc_to_uci_index(iftype, rpc_index)
#define UCI_RETURN_INDEX(iftype, uci_idx) uci_to_rpc_index(ifType, uci_idx)
}

#endif

#define RETURN_ERR_PARSE -3
#define RETURN_ERR_NOT_FOUND -2
#define RETURN_ERR -1
#define RETURN_OK 0

namespace beerocks {
namespace bpl {

int cfg_uci_get_wireless_idx(char *interfaceName, int *rpc_index);
int cfg_uci_get(char *path, char *value, size_t length);
int cfg_uci_get_wireless(enum paramType type, int index, const char param[], char *value);
int cfg_uci_get_wireless_bool(enum paramType type, const char *interface_name, const char param[],
                              bool *value);

int cfg_uci_get_radio_param_int(int index, const char param[], int *value);
int cfg_uci_get_radio_param(int index, const char param[], char *value, size_t buf_len);
int cfg_uci_get_wireless_radio_idx(const char *interfaceName, int *radio_index);
int cfg_uci_get_radio_param_ulong(int index, const char param[], unsigned long *value);

int cfg_uci_get_wireless_from_ifname(enum paramType type, const char *interface_name,
                                     const char param[], char *value);

} // namespace bpl
} // namespace beerocks

#endif // BPL_CFG_UCI_H
