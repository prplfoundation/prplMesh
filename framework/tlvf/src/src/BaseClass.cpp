/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/BaseClass.h>
#include <tlvf/ClassList.h>
#include <tlvf/tlvflogging.h>

BaseClass::BaseClass(uint8_t *buff, const size_t buff_len, const bool parse)
    : m_buff__(buff), m_buff_ptr__(buff), m_buff_len__(buff_len), m_parse__(parse),
      m_finalized__(parse)
{
}
BaseClass::~BaseClass() {}

uint8_t *BaseClass::getBuffPtr() { return m_buff_ptr__; }
uint8_t *BaseClass::getStartBuffPtr() { return m_buff__; }
size_t BaseClass::getBuffRemainingBytes(void *start)
{
    return (m_buff_len__ -
            (size_t)(((uint8_t *)(start ? start : m_buff_ptr__)) - ((uint8_t *)m_buff__)));
}

size_t BaseClass::getLen() { return (size_t)((uint8_t *)m_buff_ptr__ - (uint8_t *)m_buff__); }

bool BaseClass::isInitialized() { return m_init_succeeded; }

/**
 * @brief Increments m_buff_ptr__ pointer while avoiding out-of-bounds accesses.

 * @param length size to increment by.
 * @return true if incrementation is possible.
 * @return false if incrementation is not possible and the process should be halted to avoid a segfault.
 */
bool BaseClass::buffPtrIncrementSafe(size_t length)
{
    if (getBuffRemainingBytes() < length) {
        return false;
    }
    m_buff_ptr__ += length;
    return true;
}
