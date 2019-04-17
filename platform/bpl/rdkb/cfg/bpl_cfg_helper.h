/*
 * INTEL CONFIDENTIAL
 * Copyright 2019 Intel Corporation All Rights Reserved.
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
#ifndef _BPL_CFG_HELPER_H
#define _BPL_CFG_HELPER_H

#include <string>

#include "../../include/bpl_cfg.h"

/**
 * Returns the index of interface from DB
 *
 * @param [in] inputIfName interface name string
 * @param [out] nIndex index of interface
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_index_from_interface(const std::string& inputIfName, int *nIndex);

/**
 * Returns the value of requested param from DB
 *
 * @param [in] param beerocks param key string
 * @param [out] buf buffer to get value of requested param
 * @param [in]  buf_len buffer length.
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_beerocks_param(const std::string& param, char *buf, size_t buf_len);

/**
 * Returns the value of requested integer type param from DB
 *
 * @param [in] param beerocks param key string
 * @param [out] buf buffer to get value of requested param
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_beerocks_param_int(const std::string& param, int *buf);

/**
 * Returns the value of ACS from DB
 *
 * @param [in] index interface index
 * @param [out] enable value of ACS configuration (1=enabled, 0=disabled)
 *
 * @return 0 on success or -1 on error.
 **/
int bpl_cfg_get_auto_channel_enable(int index, int *enable);

/**
 * Returns the value of ssid-advertisement from DB
 *
 * @param [in] index interface index
 * @param [out] enabled value of ssid advertisement configuration (1=enabled, 0=disabled)
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
int bpl_cfg_set_vap_credentials(int index, const char ssid[BPL_SSID_LEN], const char sec[BPL_SEC_LEN], const char key[BPL_PASS_LEN], const char psk[BPL_PASS_LEN]);

#endif // _BPL_CFG_HELPER_H