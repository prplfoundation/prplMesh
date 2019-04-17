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

#ifndef __COMMON_CAL_INTERFACE_H__
#define __COMMON_CAL_INTERFACE_H__

extern "C" {
#include <ugw_structs.h>
}  // extern "C"

namespace beerocks {
namespace bpl {

class cal_interface
{
  public:
    cal_interface() = default;
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

#endif  // __COMMON_CAL_INTERFACE_H__
