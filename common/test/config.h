/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_TESTS_CONFIG_H__
#define __MAPF_COMMON_TESTS_CONFIG_H__

#include <iostream>
#include <mapf/common/config.h>

static const std::string kTopic       = "test.topic1";
static const std::string kTopic2      = "test.topic2";
static const std::string kData        = "0123456789";
static const std::string kData2       = "abcdef";
static constexpr const char *kSubAddr = "ipc://" TMP_PATH "/subscribers";
static constexpr const char *kPubAddr = "ipc://" TMP_PATH "/publishers";

#endif // __MAPF_COMMON_TESTS_CONFIG_H__
