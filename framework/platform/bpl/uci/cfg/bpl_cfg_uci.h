#ifndef BPL_CFG_UCI_H_
#define BPL_CFG_UCI_H_

#ifdef __cplusplus
extern "C" {
#endif

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

#define RETURN_ERR_NOT_FOUND -2
#define RETURN_ERR -1
#define RETURN_OK 0

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
#include <uci_wrapper.h>
#endif

int bpl_cfg_uci_get_wireless_idx(char *interfaceName, int *rpc_index);
int bpl_cfg_uci_get(char *path, char *value, size_t length);
int bpl_cfg_uci_get_wireless(enum paramType type, int index, const char param[], char *value);
int bpl_cfg_uci_get_wireless_bool(enum paramType type, int index, const char param[], bool *value);

int bpl_cfg_uci_get_radio_param_int(int index, const char param[], int *value);
int bpl_cfg_uci_get_radio_param(int index, const char param[], char *value, size_t buf_len);
int bpl_cfg_uci_get_wireless_radio_idx(const char *interfaceName, int *radio_index);
int bpl_cfg_uci_get_radio_param_ulong(int index, const char param[], unsigned long *value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // BPL_CFG_UCI_H_c