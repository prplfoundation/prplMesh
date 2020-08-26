/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef BCL_NETWORK_FILE_DESCRIPTOR_IMPL_H_
#define BCL_NETWORK_FILE_DESCRIPTOR_IMPL_H_

#include "file_descriptor.h"

#include <unistd.h>

#include <easylogging++.h>

namespace beerocks {
namespace net {

/**
 * File descriptor implementation.
 * This class is basically a wrapper around an `int fd` that automatically closes the descriptor
 * on destructor and prevents from having multiple copies of the file descriptor to avoid
 * programming errors.
 *
 * This class will be aggregated by all classes modeling OS resources implementing the file
 * descriptor interface.
 */
class FileDescriptorImpl : public FileDescriptor {
public:
    /**
     * @brief Class constructor.
     *
     * @param fd File descriptor value
     */
    explicit FileDescriptorImpl(int fd) : m_fd(fd)
    {
        if (invalid_descriptor == fd) {
            LOG(ERROR) << "Invalid file descriptor: " << strerror(errno);
        }
    }

    /**
     * @brief Copy constructor
     *
     * Delete copy constructor to avoid having multiple copies of the file descriptor
     */
    FileDescriptorImpl(const FileDescriptorImpl &) = delete;

    /**
     * @brief Move constructor.
     *
     * A move constructor allows the resources owned by an rvalue object to be moved into an
     * lvalue without creating its copy.
     */
    FileDescriptorImpl(FileDescriptorImpl &&obj)
    {
        this->m_fd = obj.m_fd;
        obj.m_fd   = FileDescriptor::invalid_descriptor;
    }

    /**
     * @brief Assignment operator
     *
     * Delete assignment operator to avoid having multiple copies of the file descriptor
     */
    FileDescriptorImpl &operator=(const FileDescriptorImpl &) = delete;

    /**
     * @brief Move assignment operator
     *
     * The move assignment operator is used to transfer ownership of a file descriptor
     */
    FileDescriptorImpl &operator=(FileDescriptorImpl &&obj)
    {
        // Self-assignment detection
        if (&obj == this) {
            return *this;
        }

        // Release any resource we're holding
        close();

        // Transfer ownership of obj.m_fd to m_fd
        this->m_fd = obj.m_fd;

        return *this;
    }

    /**
     * @brief Class destructor.
     *
     * Close socket on destructor (if it is still open)
     */
    ~FileDescriptorImpl() override { close(); }

    /**
     * @brief Gets file descriptor.
     *
     * File descriptor can be used for example in:
     * - select(), poll() or epoll() to wait for events on this descriptor.
     * - send() family of functions to send data through the socket connection.
     * - recv() family of functions to receive data from the socket connection.
     *
     * @return File descriptor value.
     */
    int fd() override { return m_fd; }

private:
    /**
     * File descriptor value
     */
    int m_fd;

    /**
     * @brief Closes file descriptor.
     *
     * If valid, closes file descriptor and then invalidates it.
     *
     * @return True on success and false otherwise (for example, if it was already closed)
     */
    bool close()
    {
        if (FileDescriptor::invalid_descriptor == m_fd) {
            return false;
        }

        int rc = ::close(m_fd);
        if (0 != rc) {
            LOG(ERROR) << "Unable to close descriptor: " << strerror(errno);
        }

        m_fd = FileDescriptor::invalid_descriptor;

        return (0 == rc);
    }
};

} // namespace net
} // namespace beerocks

#endif /* BCL_NETWORK_FILE_DESCRIPTOR_IMPL_H_ */
