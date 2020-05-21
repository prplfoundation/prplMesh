/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

/**
 * @brief This file helps to define and use a custom logger id which is required for using
 * easylogging with more than one log files on a single thread.
 * 
 * To use custom logger id:
 * 1. Create beerock::logging object with custom logging id string: "logging_id_str"
 * 2. Define BEEROCKS_CUSTOM_LOGGER_ID macro as logging_id_str":
 *    #define BEEROCKS_CUSTOM_LOGGER_ID "logging_id_str"
 * 3. Include this file where there is a need to write to the non-default logger id.
 */

#ifndef BEEROCKS_CUSTOM_LOGGER_ID
#error "beerocks_logging_custom.h is not included after BEEROCKS_CUSTOM_LOGGER_ID macro definition"

// gcc stops when it can't find the include file, without the following #include, the compilation
// log continues and it is hard to find the error message.
#include <stophere>
#endif

#include <easylogging++.h>
#undef LOG
#define LOG(LEVEL) CLOG(LEVEL, BEEROCKS_CUSTOM_LOGGER_ID)

#undef LOG_IF
#define LOG_IF(condition, LEVEL) CLOG_IF(condition, LEVEL, BEEROCKS_CUSTOM_LOGGER_ID)
