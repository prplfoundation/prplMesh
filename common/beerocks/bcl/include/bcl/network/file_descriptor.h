/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_FILE_DESCRIPTOR_H_
#define BCL_NETWORK_FILE_DESCRIPTOR_H_

namespace beerocks {
namespace net {

/**
 * This interface models OS resources implementing the file descriptor interface.
 */
class FileDescriptor {
public:
    static constexpr int invalid_descriptor = -1;

    virtual ~FileDescriptor() = default;

    /**
     * @brief Returns the file descriptor.
     *
     * A file descriptor is a number that uniquely identifies an open file in the OS.
     *
     * @return File descriptor.
     */
    virtual int fd() = 0;
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_FILE_DESCRIPTOR_H_ */
