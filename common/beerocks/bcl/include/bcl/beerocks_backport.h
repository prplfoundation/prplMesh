/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_BACKPORT_H_
#define _BEEROCKS_BACKPORT_H_

#include <memory>

#if __cplusplus < 201402L
namespace std {
// Backported from C++14
template <typename T, typename... Args> std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace std
#endif

#endif // _BEEROCKS_BACKPORT_H_
