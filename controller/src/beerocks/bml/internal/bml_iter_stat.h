/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */
#ifndef _BML_INTER_STAT_H_
#define _BML_INTER_STAT_H_

#include "bml_iter_base.h"

class bml_iter_stat : public bml_iter_base {

public:
    bml_iter_stat(int elements_num, void *data_buffer);
    virtual ~bml_iter_stat();

    virtual int next() override;
};

#endif // _BML_INTER_STAT_H_
