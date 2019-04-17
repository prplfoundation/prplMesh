/*
 * INTEL CONFIDENTIAL
 * Copyright 2016 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its
 * suppliers or licensors.  Title to the Material remains with Intel
 * Corporation or its suppliers and licensors.  The Material contains trade
 * secrets and proprietary and confidential information of Intel or its
 * suppliers and licensors.  The Material is protected by worldwide copyright
 * and trade secret laws and treaty provisions. No part of the Material may
 * be used, copied, reproduced, modified, published, uploaded, posted,
 * transmitted, distributed, or disclosed in any way without Intel's prior
 * express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or
 * delivery of the Materials,  either expressly, by implication, inducement,
 * estoppel or otherwise.  Any license under such intellectual property
 * rights must be express and approved by Intel in writing.
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
}  // extern "C"

#include <cal_interface.h>

namespace beerocks {
namespace bpl {

int cal_interface::cal_getValue(MsgHeader *pxObj)
{
    return ::cal_getValue(pxObj);
}

int cal_interface::cal_setValue(MsgHeader *pxObjList)
{
    return ::cal_setValue(pxObjList);
}

int cal_interface::cal_getValueFromDb(MsgHeader *pxObj)
{
    return ::cal_getValueFromDb(pxObj);
}

int cal_interface::cal_setValueToDb(MsgHeader *pxObjList)
{
    return ::cal_setValueToDb(pxObjList);
}

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