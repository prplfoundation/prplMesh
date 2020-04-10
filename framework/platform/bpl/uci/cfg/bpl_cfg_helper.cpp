/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "../../common/utils/utils.h"

#include "bpl_cfg_helper.h"
#include "bpl_cfg_uci.h"

#include "mapf/common/logger.h"

namespace beerocks {
namespace bpl {

int cfg_get_index_from_interface(const std::string &inputIfName, int *nIndex)
{
    char ifname[BPL_IFNAME_LEN] = {0};
    int rpcIndex                = -1;

    if (!nIndex) {
        return RETURN_ERR;
    }
    utils::copy_string(ifname, inputIfName.c_str(), BPL_IFNAME_LEN);

    const paramType ifType = (inputIfName.find('.') != std::string::npos) ? TYPE_VAP : TYPE_RADIO;

    if (cfg_uci_get_wireless_idx(ifname, &rpcIndex) == RETURN_OK) {
        *nIndex = UCI_RETURN_INDEX(ifType, rpcIndex);
    } else {
        return RETURN_ERR;
    }

    return RETURN_OK;
}

int cfg_get_prplmesh_param(const std::string &param, char *buf, size_t buf_len)
{
    char path[MAX_UCI_BUF_LEN] = {0};

    if (buf_len > MAX_UCI_BUF_LEN) {
        buf_len = MAX_UCI_BUF_LEN;
    }

    if (snprintf_s(path, MAX_UCI_BUF_LEN, "prplmesh.config.%s", param.c_str()) <= 0)
        return RETURN_ERR;

    return cfg_uci_get(path, buf, buf_len);
}

int cfg_get_prplmesh_radio_param(int radio_id, const std::string &radio_param, char *buf,
                                 size_t buf_len)
{
    char path[MAX_UCI_BUF_LEN] = {0};

    if (buf_len > MAX_UCI_BUF_LEN) {
        buf_len = MAX_UCI_BUF_LEN;
    }

    if (snprintf_s(path, MAX_UCI_BUF_LEN, "prplmesh.radio%d.%s", radio_id, radio_param.c_str()) <=
        0) {
        return RETURN_ERR;
    }

    return cfg_uci_get(path, buf, buf_len);
}

static int cfg_get_prplmesh_param_int_raw(const std::string &param, int *buf)
{
    int status;
    char val[MAX_UCI_BUF_LEN] = "";

    status = cfg_get_prplmesh_param(param, val, MAX_UCI_BUF_LEN);
    if (status == RETURN_ERR)
        return RETURN_ERR_NOT_FOUND;

    status = sscanf_s(val, "%d", buf);
    if (status != 1)
        return RETURN_ERR_PARSE;

    return RETURN_OK;
}

int cfg_get_prplmesh_param_int(const std::string &param, int *buf)
{
    int status = cfg_get_prplmesh_param_int_raw(param, buf);
    return status == RETURN_OK ? RETURN_OK : RETURN_ERR;
}

int cfg_get_prplmesh_param_int_default(const std::string &param, int *buf, int default_value)
{
    int status = cfg_get_prplmesh_param_int_raw(param, buf);
    switch (status) {
    case RETURN_OK:
        return RETURN_OK;
    case RETURN_ERR_NOT_FOUND:
        *buf = default_value;
        MAPF_INFO(__FUNCTION__ << ": missing parameter '" << param
                               << "', using default: " << default_value);
        return RETURN_OK;
    case RETURN_ERR_PARSE:
        MAPF_ERR(__FUNCTION__ << ": can't parse value for '" << param << "'");
        return RETURN_ERR;
    default:
        MAPF_ERR(__FUNCTION__ << ": cfg_get_prplmesh_param_int_raw returned unexpected value");
        return RETURN_ERR;
    }
}

int cfg_get_channel(const std::string &interface_name, int *channel)
{
    if (!channel) {
        return RETURN_ERR;
    }

    char channel_num[MAX_UCI_BUF_LEN] = {0};
    char ifname[BPL_IFNAME_LEN]       = {0};

    utils::copy_string(ifname, interface_name.c_str(), BPL_IFNAME_LEN);
    if (cfg_uci_get_wireless_from_ifname(TYPE_RADIO, ifname, "channel", channel_num) != RETURN_OK) {
        return RETURN_ERR;
    }

    std::string channel_str(channel_num);
    if (!channel_str.compare("auto")) {
        *channel = 0;
    } else {
        *channel = utils::stoi(channel_str);
    }

    return RETURN_OK;
}

int cfg_get_wep_key(const std::string &interface_name, int keyIndex, char *key)
{
    /*TODO: implement using d/s-pal apis*/
    return RETURN_OK;
}

} // namespace bpl
} // namespace beerocks
