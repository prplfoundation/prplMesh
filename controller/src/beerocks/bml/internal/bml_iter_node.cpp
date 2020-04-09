/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include "bml_iter_node.h"
#include "bml_defs.h"

#include <easylogging++.h>

bml_iter_node::bml_iter_node(int elements_num, void *data_buffer)
    : bml_iter_base(elements_num, data_buffer)
{
}

bml_iter_node::~bml_iter_node() {}

int bml_iter_node::next()
{
    // Reached the end of the buffer
    int ret = bml_iter_base::next();
    if (ret != BML_RET_OK)
        return (ret);

    // Advance the internal state
    struct BML_NODE *pCurrNode = (struct BML_NODE *)m_pCurrPos;

    switch (pCurrNode->type) {

    // GW or IRE node
    case BML_NODE_TYPE_GW:
    case BML_NODE_TYPE_IRE: {
        m_pCurrPos += sizeof(struct BML_NODE);

    } break;

    // Regular client
    case BML_NODE_TYPE_CLIENT: {
        m_pCurrPos += sizeof(struct BML_NODE) - sizeof(struct BML_NODE::N_DATA::N_GW_IRE);

    } break;

    // Unknown node type
    default: {
        LOG(WARNING) << "Invalid node type: " << int(pCurrNode->type);
        return (-BML_RET_INVALID_DATA);
    }
    }

    return (BML_RET_OK);
}
