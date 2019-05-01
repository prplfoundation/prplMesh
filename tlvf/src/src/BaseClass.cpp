/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <tlvf/BaseClass.h>

BaseClass::BaseClass(uint8_t* buff, const size_t buff_len, const bool parse, const bool swap_needed) :
    m_buff__(buff), m_buff_ptr__(buff), m_buff_len__(buff_len), m_parse__(parse), m_swap__(swap_needed), m_init_succeeded(false) {
}
BaseClass::~BaseClass(){
}

uint8_t* BaseClass::getBuffPtr(){
    return m_buff_ptr__;
}
uint8_t* BaseClass::getStartBuffPtr(){
	return m_buff__;
}
size_t BaseClass::getBuffRemainingBytes(){
    return (m_buff_len__ - (size_t)( ((uint8_t*)m_buff_ptr__) - ((uint8_t*)m_buff__)) );
}
size_t BaseClass::getLen(){
    return (size_t)((uint8_t*)m_buff_ptr__ - (uint8_t*)m_buff__);
}

bool BaseClass::isInitialized(){
    return m_init_succeeded;
}
