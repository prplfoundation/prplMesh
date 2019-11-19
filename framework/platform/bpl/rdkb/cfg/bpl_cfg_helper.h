/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BPL_CFG_HELPER_H
#define _BPL_CFG_HELPER_H

#include <string>

#include <bpl/bpl_cfg.h>

/**
 * Returns the index of interface from DB
 *
 * @param [in] inputIfName interface name string
 * @param [out] nIndex index of interface
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_index_from_interface(const std::string &inputIfName, int *nIndex);

/**
 * Returns the value of requested param from DB
 *
 * @param [in] param beerocks param key string
 * @param [out] buf buffer to get value of requested param
 * @param [in]  buf_len buffer length.
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_beerocks_param(const std::string &param, char *buf, size_t buf_len);

/**
 * Returns the value of requested integer type param from DB
 *
 * @param [in] param beerocks param key string
 * @param [out] buf buffer to get value of requested param
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_beerocks_param_int(const std::string &param, int *buf);

/**
 * Returns the value of ACS from DB
 *
 * @param [in] index interface index
 * @param [out] channel channel number
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_channel(int index, int *channel);

/**
 * Returns the value of ssid-advertisement from DB
 *
 * @param [in] index interface index
 * @param [out] enabled value of ssid advertisement configuration (1=enabled,
 *0=disabled)
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_ssid_advertisement_enabled(int index, int *enabled);

/** API currently not implemented **/
int bpl_cfg_get_wep_key(int index, int keyIndex, char *key);

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

#endif // _BPL_CFG_HELPER_H
