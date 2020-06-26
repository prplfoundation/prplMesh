/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BEEROCKS_BACKPORT_H_
#define _BEEROCKS_BACKPORT_H_

#include <chrono>
#include <memory>
#include <type_traits>

#if __cplusplus < 201402L
namespace std {
// Backported from C++14
template <typename T, typename... Args> std::unique_ptr<T> make_unique(Args &&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
} // namespace std
#endif

// Until prplMesh will be built using C++20, there's no simple way to fail
// the built if a template argument is not derived from std::chrono::duration.
// This little trick allows using static_assert and fail the build in this case.
template <typename T> struct is_chrono_duration : std::false_type {
};

template <typename R, typename P>
struct is_chrono_duration<std::chrono::duration<R, P>> : std::true_type {
};

#endif // _BEEROCKS_BACKPORT_H_
