/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __COMMON_CAL_INTERFACE_H__
#define __COMMON_CAL_INTERFACE_H__

extern "C" {
#include <ugw_structs.h>
} // extern "C"

namespace beerocks {
namespace bpl {

class cal_interface {
public:
    cal_interface()          = default;
    virtual ~cal_interface() = default;

    virtual int cal_getValue(MsgHeader *pxObj);
    virtual int cal_setValue(MsgHeader *pxObjList);
    virtual int cal_getValueFromDb(MsgHeader *pxObj);
    virtual int cal_setValueToDb(MsgHeader *pxObjList);
    virtual int cal_updatePeriodicValues(MsgHeader *pxMsg);

    // TODO: Determine if this is required. If not, this interface pulls in many fewer
    //   dependencies making integration/testing easier.
    // virtual int cal_sendMsgToPolld (MsgHeader *pxMsg, PolldTimeInfo *pPolldTime);
};

} // namespace bpl
} // namespace beerocks

#endif // __COMMON_CAL_INTERFACE_H__
