/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef LOGGING_ID
#define LOGGING_ID WIDAN_SLAVE
#endif

#ifndef PACKAGE_ID
#define PACKAGE_ID "0"
#endif

extern "C" {
#undef DEBUG
#include <cal.h>
} // extern "C"

#include <cal_interface.h>

namespace beerocks {
namespace bpl {

int cal_interface::cal_getValue(MsgHeader *pxObj) { return ::cal_getValue(pxObj); }

int cal_interface::cal_setValue(MsgHeader *pxObjList) { return ::cal_setValue(pxObjList); }

int cal_interface::cal_getValueFromDb(MsgHeader *pxObj) { return ::cal_getValueFromDb(pxObj); }

int cal_interface::cal_setValueToDb(MsgHeader *pxObjList) { return ::cal_setValueToDb(pxObjList); }

int cal_interface::cal_updatePeriodicValues(MsgHeader *pxMsg)
{
    return ::cal_updatePeriodicValues(pxMsg);
}

/*
// TODO: Determine if this is required. If not, this interface pulls in many fewer
//   dependencies making integration/testing easier.
int cal_interface::cal_sendMsgToPolld (MsgHeader *pxMsg, PolldTimeInfo *pPolldTime)
{
    return ::cal_sendMsgToPolld(pxMsg, pPolldTime);
}
*/

} // namespace bpl
} // namespace beerocks