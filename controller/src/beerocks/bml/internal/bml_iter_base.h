/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2019 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BML_INTER_BASE_H_
#define _BML_INTER_BASE_H_

class bml_iter_base {

public:
    bml_iter_base(int elements_num, void *data_buffer);
    virtual ~bml_iter_base();

    int first();
    void *data();

    // Overloaded by specific implementations
    virtual int next();

protected:
    int m_iCurrElement;
    char *m_pCurrPos;

private:
    int m_iNumOfElements;
    void *m_pDataBuffer;
};

#endif // _BML_INTER_BASE_H_
