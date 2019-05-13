/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_UTILS_H_
#define _BML_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BEEROCKS_RDKB
#include "rdkb/bml_rdkb_defs.h"
#else
#include "bml_defs.h"
#endif
/**
 * Convert BML_NODE to string, write result to buffer.
 * Returns the number of bytes copied to buffer
 */
int bml_utils_node_to_string(const struct BML_NODE *node, char *buffer, int buffer_len);

/**
 * Convert BML_STATS to string, write result to buffer.
 * Returns the number of bytes copied to buffer
 */
int bml_utils_stats_to_string(const struct BML_STATS *stats, char *buffer, int buffer_len);

/**
 * Convert BML_STATS to raw (unformatted) string, write result to buffer.
 * Returns the number of bytes copied to buffer.
 */
int bml_utils_stats_to_string_raw(const struct BML_STATS *stats, char *buffer, int buffer_len);

/**
 * Convert BML_EVENT to string, write result to buffer.
 * Returns the number of bytes copied to buffer
 */
int bml_utils_event_to_string(const struct BML_EVENT *event, char *buffer, int buffer_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _BML_UTILS_H_ */
