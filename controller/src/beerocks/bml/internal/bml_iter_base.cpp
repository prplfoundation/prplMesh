/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_iter_base.h"
#include "bml_defs.h"

bml_iter_base::bml_iter_base(int elements_num, void *data_buffer)
    : m_iCurrElement(0), m_pCurrPos((char *)data_buffer), m_iNumOfElements(elements_num),
      m_pDataBuffer(data_buffer)
{
}

bml_iter_base::~bml_iter_base() {}

int bml_iter_base::first()
{
    // Reset internal state
    m_iCurrElement = 0;
    m_pCurrPos     = (char *)m_pDataBuffer;

    return (BML_RET_OK);
}

int bml_iter_base::next()
{
    // Reached the end of the buffer
    if (++m_iCurrElement >= m_iNumOfElements)
        return (-BML_RET_NO_DATA);

    return (BML_RET_OK);
}

void *bml_iter_base::data()
{
    // Reached the end of the buffer
    if (m_iCurrElement >= m_iNumOfElements)
        return (nullptr);

    return (m_pCurrPos);
}