/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bpl_cfg_uci.h"
#include "../../common/utils/utils.h"

#include <string>

extern "C" {
#include <uci.h>
}

#define LOGF_LOG_CRIT(args...) PRINTF("CRIT", ##args)
#define LOGF_LOG_ERROR(args...) PRINTF("ERROR", ##args)
#define LOGF_LOG_WARN(args...) PRINTF("WARN", ##args)
#define LOGF_LOG_INFO(args...) PRINTF("INFO", ##args)
#define LOGF_LOG_DEBUG(args...) PRINTF("DEBUG", ##args)

#define PRINTF(LEVEL, fmt, args...) printf(LEVEL ":{%s, %d}:" fmt, __func__, __LINE__, ##args)

#ifdef BEEROCKS_RDKB
// NOTE:
// This function is defined as static function on swpal, so it is not possible to access it.
// Therefore the implementation has been copied from swpal.
// This is a temporary solution and will be removed.
/**
 * @brief convert dummy vap uci index to radio uci index using uci-db
 *
 * @param[in] index The uci index of the dummy vap
 * @return the index if success, negative if error
 */
static int dummy_to_radio_index(int index)
{
    char radio[MAX_UCI_BUF_LEN] = "";
    if (uci_converter_get_str(TYPE_RADIO_VAP, uci_to_rpc_index(TYPE_RADIO_VAP, index), "device",
                              radio) == RETURN_ERR)
        return RETURN_ERR;

    return (radio[sizeof("radio") - 1] - '0');
}

#endif

namespace beerocks {
namespace bpl {

int cfg_uci_get(char *path, char *value, size_t length)
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

int cfg_uci_get_wireless_int(enum paramType type, int index, const char param[], int *value)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = cfg_uci_get_wireless(type, index, param, val);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = sscanf_s(val, "%d", value);
    if (status != 1)
        return RETURN_ERR;

    return RETURN_OK;
}

int cfg_uci_get_radio_param(int index, const char param[], char *value, size_t buf_len)
{
    int status;
    char path[MAX_UCI_BUF_LEN] = "";

    status = snprintf_s(path, MAX_UCI_BUF_LEN, "beerocks.radio%d.%s", index, param);
    if (status <= 0) {
        ERROR("%s failed snprintf.  index=%d param=%s\n", __func__, index, param);
        return RETURN_ERR;
    }

    status = cfg_uci_get(path, value, buf_len);
    if (status == RETURN_OK) {
        // DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
    } else {
        ERROR("%s option N/A. index=%d param=%s\n", __func__, index, param);
    }

    return status;
}

int cfg_uci_get_radio_param_int(int index, const char param[], int *value)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = cfg_uci_get_radio_param(index, param, val, MAX_UCI_BUF_LEN);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = sscanf_s(val, "%d", value);
    if (status != 1)
        return RETURN_ERR;

    return RETURN_OK;
}

int cfg_uci_get_radio_param_ulong(int index, const char param[], unsigned long *value)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = cfg_uci_get_radio_param(index, param, val, MAX_UCI_BUF_LEN);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    status = sscanf_s(val, "%lu", value);
    if (status != 1)
        return RETURN_ERR;

    return RETURN_OK;
}

int cfg_uci_get_wireless(enum paramType type, int index, const char param[], char *value)
{
    int status;
    char path[MAX_UCI_BUF_LEN] = "";
    char radio_str[]           = "default_radio"; /* == TYPE_VAP */

    if (type == TYPE_RADIO) {
        strncpy_s(radio_str, sizeof(radio_str), "radio", 5);
    }

    status = snprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s", radio_str,
                        UCI_INDEX(type, index), param);
    if (status <= 0) {
        ERROR("%s failed snprintf.  index=%d param=%s\n", __func__, index, param);
        return RETURN_ERR;
    }

    status = cfg_uci_get(path, value, MAX_UCI_BUF_LEN);
    if (status == RETURN_OK) {
        // DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
    } else {
        ERROR("%s option N/A. index=%d param=%s\n", __func__, index, param);
    }

    return status;
}

int cfg_uci_get_wireless_from_ifname(enum paramType type, const char *interface_name,
                                     const char param[], char *value)
{
    struct uci_context *ctx = uci_alloc_context();
    if (!ctx) {
        ERROR("%s, uci alloc context failed!\n", __FUNCTION__);
        return RETURN_ERR;
    }

    char lookup_str[MAX_UCI_BUF_LEN] = "wireless";
    struct uci_ptr ptr;
    if ((uci_lookup_ptr(ctx, &ptr, lookup_str, true) != UCI_OK)) {
        ERROR("%s, uci lookup failed!\n", __FUNCTION__);
        uci_free_context(ctx);
        return RETURN_ERR;
    }

    if (!ptr.p) {
        ERROR("%s, returned pointer is null\n", __FUNCTION__);
        uci_free_context(ctx);
        return RETURN_ERR;
    }

    bool is_section_found = false;
    struct uci_package *p = ptr.p;
    struct uci_element *e = nullptr;
    struct uci_element *n = nullptr;
    struct uci_section *s = nullptr;
    // Iterate over all wireless sections in the UCI DB
    uci_foreach_element(&p->sections, e)
    {
        s = uci_to_section(e);

        if (strncmp(s->type, "wifi-iface", MAX_UCI_BUF_LEN))
            continue;

        // Iterate over all the options in the section
        uci_foreach_element(&s->options, n)
        {
            struct uci_option *o = uci_to_option(n);

            if (o->type != UCI_TYPE_STRING)
                continue;

            if (strncmp(n->name, "ifname", MAX_UCI_BUF_LEN))
                continue;

            if (strncmp(interface_name, o->v.string, MAX_UCI_BUF_LEN))
                continue;

            // We reached the section containing the requested ifname
            is_section_found = true;
            break;
        }

        if (is_section_found) {
            break;
        }
    }

    //if interface not found in wireless
    if (!is_section_found) {
        ERROR("%s, interface(%s) not found", __FUNCTION__, interface_name);
        uci_free_context(ctx);
        return RETURN_ERR;
    }

    if (type == TYPE_RADIO) {
        // create path to the param in the device: wireless.<device>.param
        bool device_option_exist = false;
        std::string path_str;

        uci_foreach_element(&s->options, n)
        {
            struct uci_option *o = uci_to_option(n);

            if (strncmp(n->name, "device", MAX_UCI_BUF_LEN) == 0 && o->type == UCI_TYPE_STRING) {
                path_str =
                    std::string("wireless." + std::string(o->v.string) + "." + std::string(param));
                device_option_exist = true;
                break;
            }
        }
        uci_free_context(ctx);

        if (device_option_exist) {
            // radio not found
            ERROR("%s device option not found\n", __func__);
            return RETURN_ERR;
        }

        char path[MAX_UCI_BUF_LEN] = "";
        utils::copy_string(path, path_str.c_str(), MAX_UCI_BUF_LEN);

        if (cfg_uci_get(path, value, MAX_UCI_BUF_LEN) != RETURN_OK) {
            ERROR("%s option N/A. path=%s\n", __func__, path);
        }

        return RETURN_OK;

    } else { //* type == TYPE_VAP *
        // read the value from the selected section
        // get param in the selected section
        uci_foreach_element(&s->options, n)
        {
            struct uci_option *o = uci_to_option(n);
            // if param is found in options
            if (strncmp(n->name, param, MAX_UCI_BUF_LEN) == 0 && o->type == UCI_TYPE_STRING) {
                strncpy_s(value, MAX_UCI_BUF_LEN, o->v.string, MAX_UCI_BUF_LEN - 1);
                uci_free_context(ctx);
                return RETURN_OK;
            }
        }

        // param not found in option
        ERROR("%s, interface(%s) found but param(%s) isn't configured\n", __FUNCTION__,
              interface_name, param);
        uci_free_context(ctx);
        return RETURN_ERR;
    }
}

int cfg_uci_get_wireless_radio_idx(const char *interfaceName, int *radio_index)
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

int cfg_uci_get_wireless_idx(char *interfaceName, int *rpc_index)
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

#ifdef BEEROCKS_RDKB
            if (scanf_res < 1 || *rpc_index < 0) {
                *rpc_index = -1;
                uci_free_context(ctx);
                return RETURN_ERR;
            }

            /* if it is dummy we want the radio index, not the dummy one */
            if (uci_converter_is_dummy(*rpc_index)) {
                *rpc_index = dummy_to_radio_index(*rpc_index);
                if (*rpc_index == RETURN_ERR) {
                    uci_free_context(ctx);
                    return RETURN_ERR;
                }
            }
#else
            if (scanf_res < 1 || *rpc_index < 0 ||
                *rpc_index > DUMMY_VAP_OFFSET + MAX_NUM_OF_RADIOS) {
                *rpc_index = -1;
                uci_free_context(ctx);
                return RETURN_ERR;
            }

            *rpc_index =
                (*rpc_index >= DUMMY_VAP_OFFSET) ? (*rpc_index - DUMMY_VAP_OFFSET) : *rpc_index;
#endif
            uci_free_context(ctx);
            return RETURN_OK;
        }
    }
    *rpc_index = -1;
    uci_free_context(ctx);
    return RETURN_ERR;
}

int cfg_uci_get_wireless_bool(enum paramType type, int index, const char param[], bool *value)
{
    int status;
    int res;

    status = cfg_uci_get_wireless_int(type, index, param, &res);
    if (status == RETURN_ERR)
        return RETURN_ERR;

    *value = (res != 0) ? true : false;

    return RETURN_OK;
}

} // namespace bpl
} // namespace beerocks
