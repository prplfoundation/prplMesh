/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bpl_cfg_uci.h"
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

/**************************************************************************/
/*! \fn int uci_converter_get(char* path, char* value, size_t length)
 **************************************************************************
 *  \brief generic get from UCI database
 *  \param[in] char *path - variable full name to get (config file.device name.option) for example wireless.default_radio45.wps_state
 *  \param[out] char *value - value from UCI DB 
 *  \param[in] size_t length - size of value buffer
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get(char* path, char* value, size_t length)
{
	struct uci_ptr ptr;
	struct uci_context *cont = uci_alloc_context();

	if (!cont)
		return RETURN_ERR;

	if ((path == NULL) || (value == NULL)) {
		ERROR("%s, path or value is NULL!\n", __FUNCTION__);
		uci_free_context(cont);
		return RETURN_ERR;
	}

	if (uci_lookup_ptr(cont, &ptr, path, true) != UCI_OK || !ptr.o) {
		uci_free_context(cont);
		return RETURN_ERR;
	}

	strncpy_s(value, length, ptr.o->v.string, strnlen_s(ptr.o->v.string, length - 1));

	uci_free_context(cont);

	return RETURN_OK;
}

/**************************************************************************/
/*! \fn int uci_converter_get_str(enum paramType type, int index, const char param[], char *value)
 **************************************************************************
 *  \brief get a specific radio or VAP param string from UCI database
 *  \param[in] enum paramType type - TYPE_RADIO / TYPE_RADIO_VAP / TYPE_VAP
 *  \param[in] int index - radio or VAP index (regular index, not UCI index)
 *  \param[in] const char param[] - param name to get (not full path)
 *  \param[out] char *value - string value from UCI DB 
 *  \return 0 if success, negative if error / timeout
 ***************************************************************************/
int uci_converter_get_str(enum paramType type, int index, const char param[], char *value)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	char radio_str[] = "default_radio"; /* == TYPE_VAP / TYPE_RADIO_VAP */

	if ((param == NULL) || (value == NULL)) {
		ERROR("%s, param or value is NULL!\n", __FUNCTION__);
		return RETURN_ERR;
	}

	if (type == TYPE_RADIO)
		strncpy_s(radio_str, sizeof(radio_str), "radio", 5);

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.%s%d.%s", radio_str, rpc_to_uci_index(type, index), param);
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d param=%s\n", __func__, index, param);
		return RETURN_ERR;
	}

	status = uci_converter_get(path, value, MAX_UCI_BUF_LEN);
	if (status == RETURN_OK) {
		DEBUG("%s index=%d %s=%s\n", __func__, index, param, value);
	} else {
		INFO("%s option N/A. index=%d param=%s\n", __func__, index, param);
	}

	return status;
}

/**************************************************************************/
/*! \fn int dummy_to_radio_index(enum paramType iftype, int index)
 **************************************************************************
 *  \brief convert dummy vap uci index to radio uci index using uci-db
 *  \param[in] int index - The uci index of the dummy vap
 *  \return the index if success, negative if error
 ***************************************************************************/
static int dummy_to_radio_index(int index)
{
	char radio[MAX_UCI_BUF_LEN] = "";
	if (uci_converter_get_str(TYPE_RADIO_VAP, uci_to_rpc_index(TYPE_RADIO_VAP, index), "device", radio) == RETURN_ERR)
		return RETURN_ERR;

	return (radio[sizeof("radio") -1] - '0');

}

/**************************************************************************/
/*! \fn int uci_converter_is_dummy(int uci_index)
 **************************************************************************
 *  \brief get wheter vap is dummy or not based on uci index
 *  \param[in] int uci_index - The index of the vap entry
 *  \return true if dummy, false if not dummy / error
 ***************************************************************************/
static bool uci_converter_is_dummy(int uci_index)
{
	int status;
	char path[MAX_UCI_BUF_LEN] = "";
	char uci_ret[MAX_UCI_BUF_LEN] = "";

	status = sprintf_s(path, MAX_UCI_BUF_LEN, "wireless.radio_vap_rpc_indexes.index%d", uci_to_rpc_index(TYPE_RADIO_VAP, uci_index));
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d\n", __func__, uci_index);
		return false;
	}

	status = uci_converter_get(path, uci_ret, MAX_UCI_BUF_LEN);
	if ((status == RETURN_OK) && (std::stoi(uci_ret) == uci_index))
		return true;
	return false;
}

/**************************************************************************/
/*! \fn int rpc_to_uci_index(enum paramType iftype, int index)
 **************************************************************************
 *  \brief convert rpc index to uci index using uci-db
 *  \param[in] enum paramType ifType: TYPE_RADIO/TYPE_RADIO_VAP/TYPE_VAP
 *  \param[in] int index - The rpc index
 *  \return the index if success, negative if error
 ***************************************************************************/
int rpc_to_uci_index(enum paramType iftype, int index)
{
	char index_path[MAX_UCI_BUF_LEN] = "";
	char index_path_pre[MAX_UCI_BUF_LEN] = "";
	char value_str[MAX_UCI_BUF_LEN] = "";
	int status = 0;
	switch(iftype)
	{
		case TYPE_RADIO:
			status = sprintf_s(index_path_pre, MAX_UCI_BUF_LEN, "wireless.radio_rpc_indexes");
			break;
		case TYPE_RADIO_VAP:
			status = sprintf_s(index_path_pre, MAX_UCI_BUF_LEN, "wireless.radio_vap_rpc_indexes");
			break;
		case TYPE_VAP:
			status = sprintf_s(index_path_pre, MAX_UCI_BUF_LEN, "wireless.vap_rpc_indexes");
			break;
		default:
			ERROR("%s invalid iftype.", __func__);
			return RETURN_ERR;
	}
	if (status <= 0) {
		ERROR("%s failed sprintf_s.", __func__);
		return RETURN_ERR;
	}

	status = sprintf_s(index_path, MAX_UCI_BUF_LEN, "%s.index%d",
			   index_path_pre, index);
	if (status <= 0) {
		ERROR("%s failed sprintf_s.  index=%d", __func__, index);
		return RETURN_ERR;
	}

	status = uci_converter_get(index_path, value_str, MAX_UCI_BUF_LEN);
	if (status == RETURN_ERR)
		return RETURN_ERR;
	return std::stoi(value_str);
}

/**************************************************************************/
/*! \fn int uci_to_rpc_index(enum paramType iftype, int index)
 **************************************************************************
 *  \brief convert uci index to rpc index using uci-db
 *  \param[in] enum paramType ifType: TYPE_RADIO/TYPE_RADIO_VAP/TYPE_VAP
 *  \param[in] int index - The uci index
 *  \return the index if success, negative if error
 ***************************************************************************/
int uci_to_rpc_index(enum paramType iftype, int index)
{
	char index_path[MAX_UCI_BUF_LEN] = "";
	char index_path_pre[MAX_UCI_BUF_LEN] = "";
	char value_str[MAX_UCI_BUF_LEN] = "";
	int status = 0;
	switch(iftype)
	{
		case TYPE_RADIO:
			status = sprintf_s(index_path_pre, MAX_UCI_BUF_LEN, "wireless.radio");
			break;
		case TYPE_RADIO_VAP:
			/* fall through */
		case TYPE_VAP:
			status = sprintf_s(index_path_pre, MAX_UCI_BUF_LEN, "wireless.default_radio");
			break;
		default:
			ERROR("%s invalid iftype.", __func__);
			return RETURN_ERR;
	}
	if (status <= 0) {
		ERROR("%s failed sprintf_s.", __func__);
		return RETURN_ERR;
	}

	status = sprintf_s(index_path, MAX_UCI_BUF_LEN, "%s%d.rpc_index",
			   index_path_pre, index);
	if (status <= 0) {
		ERROR("%s failed sprintf_s. index=%d", __func__, index);
		return RETURN_ERR;
	}

	status = uci_converter_get(index_path, value_str, MAX_UCI_BUF_LEN);
	if (status == RETURN_ERR)
		return RETURN_ERR;
	return std::stoi(value_str);
}

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
                        rpc_to_uci_index(type, index), param);
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
