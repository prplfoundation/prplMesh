/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_iter_stat.h"
#include "bml_defs.h"

#include <easylogging++.h>

bml_iter_stat::bml_iter_stat(int elements_num, void *data_buffer)
    : bml_iter_base(elements_num, data_buffer)
{
}

bml_iter_stat::~bml_iter_stat() {}

int bml_iter_stat::next()
{
    // Reached the end of the buffer
    int ret = bml_iter_base::next();
    if (ret != BML_RET_OK)
        return (ret);

    // Advance the internal state
    BML_STATS *pCurrStat = (BML_STATS *)m_pCurrPos;

    m_pCurrPos += sizeof(BML_STATS) - sizeof(BML_STATS::S_TYPE);

    switch (pCurrStat->type) {

    case BML_STAT_TYPE_RADIO: {
        m_pCurrPos += sizeof(BML_STATS::S_TYPE::S_RADIO);
    } break;

    case BML_STAT_TYPE_VAP: {
        m_pCurrPos += sizeof(BML_STATS::S_TYPE::S_VAP);
    } break;

    case BML_STAT_TYPE_CLIENT: {
        m_pCurrPos += sizeof(BML_STATS::S_TYPE::S_CLIENT);
    } break;

    // Unknown type
    default: {
        LOG(WARNING) << "Invalid stats type: " << int(pCurrStat->type);
        return (-BML_RET_INVALID_DATA);
    }
    }

    return (BML_RET_OK);
}
