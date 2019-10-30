
/*##################################################################################################
# "Copyright (c) 2013 Intel Corporation                                                            #
# DISTRIBUTABLE AS SAMPLE SOURCE SOFTWARE                                                          #
# This Distributable As Sample Source Software is subject to the terms and conditions              #
# of the Intel Software License Agreement for the Intel(R) Cable and GW Software Development Kit"  #
##################################################################################################*/

/*! \file 	uci_wrapper.c
	\brief 	This file implements a wrapper over uci library.
	\todo 	Add license header
	\todo   Add null pointer verifications and clean-up
 */

#include <uci.h>

#include "../uci/bpl_cfg_uci.h"

#include <slibc/stdio.h>
#include <slibc/string.h>

// #define CRIT(fmt, args...) LOGF_LOG_CRIT(fmt, ##args)
// #define ERROR(fmt, args...) LOGF_LOG_ERROR(fmt, ##args)
// #define WARN(fmt, args...) LOGF_LOG_WARN(fmt, ##args)
// #define INFO(fmt, args...) LOGF_LOG_INFO(fmt, ##args)
// #define DEBUG(fmt, args...) LOGF_LOG_DEBUG(fmt, ##args)

#define LOGF_LOG_CRIT(args...) PRINTF("CRIT", ##args)
#define LOGF_LOG_ERROR(args...) PRINTF("ERROR", ##args)
#define LOGF_LOG_WARN(args...) PRINTF("WARN", ##args)
#define LOGF_LOG_INFO(args...) PRINTF("INFO", ##args)
#define LOGF_LOG_DEBUG(args...) PRINTF("DEBUG", ##args)

#define PRINTF(LEVEL, fmt, args...) printf(LEVEL ":{%s, %d}:" fmt, __func__, __LINE__, ##args)

int bpl_cfg_uci_get(char *path, char *value, size_t length)
{
    struct uci_ptr ptr;
    struct uci_context *cont = uci_alloc_context();

    if (!cont)
        return RETURN_ERR;

    if (uci_lookup_ptr(cont, &ptr, path, true) != UCI_OK || !ptr.o) {
        uci_free_context(cont);
        return RETURN_ERR;
    }

    strncpy_s(value, length, ptr.o->v.string, length - 1);

    uci_free_context(cont);

    return RETURN_OK;
}

int bpl_cfg_uci_get_wireless_int(enum paramType type, int index, const char param[], int *value)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = bpl_cfg_uci_get_wireless(type, index, param, val);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = sscanf_s(val, "%d", value);
    if (status != 1)
        return RETURN_ERR;

    return RETURN_OK;
}

int bpl_cfg_uci_get_radio_param(int index, const char param[], char *value, size_t buf_len)
{
    int status;
    char path[MAX_UCI_BUF_LEN] = "";

    status = snprintf_s(path, MAX_UCI_BUF_LEN, "beerocks.radio%d.%s", index, param);
    if (status <= 0) {
        ERROR("%s failed snprintf.  index=%d param=%s\n", __func__, index, param);
        return RETURN_ERR;
    }

    status = bpl_cfg_uci_get(path, value, buf_len);
    if (status == RETURN_OK) {
        // DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
    } else {
        ERROR("%s option N/A. index=%d param=%s\n", __func__, index, param);
    }

    return status;
}

int bpl_cfg_uci_get_radio_param_int(int index, const char param[], int *value)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = bpl_cfg_uci_get_radio_param(index, param, val, MAX_UCI_BUF_LEN);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = sscanf_s(val, "%d", value);
    if (status != 1)
        return RETURN_ERR;

    return RETURN_OK;
}

int bpl_cfg_uci_get_radio_param_ulong(int index, const char param[], unsigned long *value)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = bpl_cfg_uci_get_radio_param(index, param, val, MAX_UCI_BUF_LEN);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = sscanf_s(val, "%lu", value);
    if (status != 1)
        return RETURN_ERR;

    return RETURN_OK;
}

int bpl_cfg_uci_get_wireless(enum paramType type, int index, const char param[], char *value)
{
    int status;
    char path[MAX_UCI_BUF_LEN] = "";
    char radio_str[]           = "default_radio"; /* == TYPE_VAP */

    if (type == TYPE_RADIO) {
        strncpy_s(radio_str, sizeof(radio_str), "radio", sizeof(radio_str) - 1);
    }

    status = snprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s", radio_str,
                        UCI_INDEX(type, index), param);
    if (status <= 0) {
        ERROR("%s failed snprintf.  index=%d param=%s\n", __func__, index, param);
        return RETURN_ERR;
    }

    status = bpl_cfg_uci_get(path, value, MAX_UCI_BUF_LEN);
    if (status == RETURN_OK) {
        // DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
    } else {
        ERROR("%s option N/A. index=%d param=%s\n", __func__, index, param);
    }

    return status;
}

int bpl_cfg_uci_get_wireless_radio_idx(const char *interfaceName, int *radio_index)
{
    struct uci_ptr ptr;
    struct uci_context *ctx;
    struct uci_package *p = NULL;
    struct uci_element *e;
    struct uci_section *s;
    char lookup_str[MAX_UCI_BUF_LEN] = "wireless";
    char tmp_deviceName[MAX_UCI_BUF_LEN];
    char tmp_ifaceName[MAX_UCI_BUF_LEN];

    ctx = uci_alloc_context();
    if (!ctx)
        return RETURN_ERR;

    if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
        ERROR("%s, uci lookup failed!\n", __FUNCTION__);
        uci_free_context(ctx);
        return RETURN_ERR;
    }

    p = ptr.p;

    uci_foreach_element(&p->sections, e)
    {
        struct uci_element *n;
        int scanf_res;

        s = uci_to_section(e);

        if (strncmp(s->type, "wifi-iface", MAX_UCI_BUF_LEN))
            continue;

        tmp_deviceName[0] = '\0';
        tmp_ifaceName[0]  = '\0';
        uci_foreach_element(&s->options, n)
        {
            struct uci_option *o = uci_to_option(n);

            if (strncmp(n->name, "device", MAX_UCI_BUF_LEN) == 0 && o->type == UCI_TYPE_STRING) {
                strncpy_s(tmp_deviceName, MAX_UCI_BUF_LEN, o->v.string, MAX_UCI_BUF_LEN - 1);
                continue;
            }

            if (strncmp(n->name, "ifname", MAX_UCI_BUF_LEN) == 0 && o->type == UCI_TYPE_STRING) {
                strncpy_s(tmp_ifaceName, MAX_UCI_BUF_LEN, o->v.string, MAX_UCI_BUF_LEN - 1);
                continue;
            }
        }

        if (tmp_deviceName[0] != '\0' && tmp_ifaceName[0] != '\0' &&
            strncmp(interfaceName, tmp_ifaceName, MAX_UCI_BUF_LEN) == 0) {

            uci_free_context(ctx);

            scanf_res = sscanf_s(tmp_deviceName, "radio%d", radio_index);

            if (scanf_res < 1 || *radio_index < 0 || *radio_index > MAX_NUM_OF_RADIOS) {
                *radio_index = -1;
                return RETURN_ERR;
            }

            return RETURN_OK;
        }
    }

    *radio_index = -1;
    uci_free_context(ctx);
    return RETURN_ERR;
}

int bpl_cfg_uci_get_wireless_idx(char *interfaceName, int *rpc_index)
{
    struct uci_ptr ptr;
    struct uci_context *ctx;
    struct uci_package *p = NULL;
    struct uci_element *e;
    struct uci_section *s;
    char lookup_str[MAX_UCI_BUF_LEN] = "wireless";

    ctx = uci_alloc_context();
    if (!ctx)
        return RETURN_ERR;

    if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
        ERROR("%s, uci lookup failed!\n", __FUNCTION__);
        uci_free_context(ctx);
        return RETURN_ERR;
    }

    p = ptr.p;

    uci_foreach_element(&p->sections, e)
    {
        struct uci_element *n;
        int scanf_res;

        s = uci_to_section(e);

        if (strncmp(s->type, "wifi-iface", MAX_UCI_BUF_LEN))
            continue;

        uci_foreach_element(&s->options, n)
        {
            struct uci_option *o = uci_to_option(n);

            if (strncmp(n->name, "ifname", MAX_UCI_BUF_LEN))
                continue;

            if (o->type != UCI_TYPE_STRING)
                continue;

            if (strncmp(interfaceName, o->v.string, MAX_UCI_BUF_LEN))
                continue;

            scanf_res = sscanf_s(s->e.name, "default_radio%d", rpc_index);

            if (scanf_res < 1 || *rpc_index < 0 ||
                *rpc_index > DUMMY_VAP_OFFSET + MAX_NUM_OF_RADIOS) {
                *rpc_index = -1;
                uci_free_context(ctx);
                return RETURN_ERR;
            }

            *rpc_index =
                (*rpc_index >= DUMMY_VAP_OFFSET) ? (*rpc_index - DUMMY_VAP_OFFSET) : *rpc_index;
            uci_free_context(ctx);
            return RETURN_OK;
        }
    }
    *rpc_index = -1;
    uci_free_context(ctx);
    return RETURN_ERR;
}

int bpl_cfg_uci_get_wireless_bool(enum paramType type, int index, const char param[], bool *value)
{
    int status;
    int res;

    status = bpl_cfg_uci_get_wireless_int(type, index, param, &res);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    *value = (res != 0) ? true : false;

    return RETURN_OK;
}

//UCI HELPER ENDS
