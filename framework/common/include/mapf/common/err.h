/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_COMMON_ERR_H__
#define __MAPF_COMMON_ERR_H__

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined __GNUC__
#define likely(x) __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

#define mapf_assert(x)                                                                             \
    do {                                                                                           \
        if (unlikely(!(x))) {                                                                      \
            fprintf(stderr, "Assertion failed: %s (%s:%d)\n", #x, __FILE__, __LINE__);             \
            fflush(stderr);                                                                        \
            abort();                                                                               \
        }                                                                                          \
    } while (false)

//  Provides convenient way to check for errno-style errors.
#define errno_assert(x)                                                                            \
    do {                                                                                           \
        if (unlikely(!(x))) {                                                                      \
            const char *errstr = strerror(errno);                                                  \
            fprintf(stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__);                           \
            fflush(stderr);                                                                        \
            abort();                                                                               \
        }                                                                                          \
    } while (false)

//  Provides convenient way to check for POSIX errors.
#define posix_assert(x)                                                                            \
    do {                                                                                           \
        if (unlikely(x)) {                                                                         \
            const char *errstr = strerror(x);                                                      \
            fprintf(stderr, "%s (%s:%d)\n", errstr, __FILE__, __LINE__);                           \
            fflush(stderr);                                                                        \
            abort();                                                                               \
        }                                                                                          \
    } while (false)

//  Provides convenient way to check whether memory allocation have succeeded.
#define alloc_assert(x)                                                                            \
    do {                                                                                           \
        if (unlikely(!x)) {                                                                        \
            fprintf(stderr, "FATAL ERROR: OUT OF MEMORY (%s:%d)\n", __FILE__, __LINE__);           \
            fflush(stderr);                                                                        \
            abort();                                                                               \
        }                                                                                          \
    } while (false)

#endif /* __MAPF_COMMON_ERR_H__ */
