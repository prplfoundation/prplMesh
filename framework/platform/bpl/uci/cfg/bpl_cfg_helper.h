/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_CFG_HELPER_H
#define _BPL_CFG_HELPER_H

#include <string>

#include <bpl/bpl_cfg.h>

namespace beerocks {
namespace bpl {

/**
 * Returns the index of interface from DB
 *
 * @param [in] inputIfName interface name string
 * @param [out] nIndex index of interface
 *
 * @return 0 on success or -1 on error.
 **/
int cfg_get_index_from_interface(const std::string &inputIfName, int *nIndex);

/**
 * Returns the value of requested param from DB
 *
 * @param [in] param prplmesh param key string
 * @param [out] buf buffer to get value of requested param
 * @param [in]  buf_len buffer length.
 *
 * @return 0 on success or -1 on error.
 **/
int cfg_get_prplmesh_param(const std::string &param, char *buf, size_t buf_len);

/**
 * Returns the value of requested param from DB for the specified radio
 *
 * @param [in] radio_id prplmesh radio id
 * @param [in] param prplmesh param key string
 * @param [out] buf buffer to get value of requested param
 * @param [in]  buf_len buffer length.
 *
 * @return 0 on success or -1 on error.
 **/
int cfg_get_prplmesh_radio_param(int radio_id, const std::string &radio_param, char *buf,
                                 size_t buf_len);

/**
 * Returns the value of requested integer type param from DB
 *
 * @param [in] param prplmesh param key string
 * @param [out] buf buffer to get value of requested param
 *
 * @return 0 on success or -1 on error.
 **/
int cfg_get_prplmesh_param_int(const std::string &param, int *buf);

/**
 * Returns the value of requested integer type param from DB
 *
 * @param [in] param prplmesh param key string
 * @param [in] default_value value to return if parameter is missing in DB
 * @param [out] buf buffer to get value of requested param
 *
 * @return 0 on success or -1 on error.
 **/
int cfg_get_prplmesh_param_int_default(const std::string &param, int *buf, int default_value);

/**
 * Returns the value of ACS from DB
 *
 * @param [in] interface_name interface name string
 * @param [out] channel channel number
 *
 * @return 0 on success or -1 on error.
 **/
int cfg_get_channel(const std::string &interface_name, int *channel);

/** API currently not implemented **/
int cfg_get_wep_key(const std::string &interface_name, int keyIndex, char *key);

/**
 * set the VAP credentials
 *
 * @param [in] index interface index
 * @param [in] ssid SSID of the VAP
 * @param [in] sec security type
 * @param [in] key password
 * @param [in] psk pre-shared key
 *
 * @return 0 on success or -1 on error.
 **/

} // namespace bpl
} // namespace beerocks

#endif // _BPL_CFG_HELPER_H
