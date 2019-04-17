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

#ifndef _BWL_UGW_HELPER_H_
#define _BWL_UGW_HELPER_H_

#include <memory>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Wrapper for UGW helper functions - Add new object
#define UGW_OBJLIST_CREATE(objlist_name)                                    \
auto objlist_name = std::shared_ptr<ObjList>(                               \
    (ObjList*)HELP_CREATE_OBJ(SOPT_OBJVALUE),                               \
    [](ObjList* obj) {                                                      \
        if (obj) HELP_DELETE_OBJ(obj, SOPT_OBJVALUE, FREE_OBJLIST);         \
    })

// Wrapper for UGW helper functions - Wrap existing new object
#define UGW_OBJLIST_WRAP_EXTERNAL(objlist_name, objlist_ptr)                \
auto objlist_name = std::shared_ptr<ObjList>(                               \
    (ObjList*)objlist_ptr,                                                  \
    [](ObjList* obj) {                                                      \
        /*if (obj) HELP_DELETE_OBJ(obj, SOPT_OBJVALUE, FREE_OBJLIST);*/     \
    })
    
// Wrapper for UGW helper functions - Add new list
#define UGW_OBJLIST_ADD(pxObjList, pcObjName)                               \
help_addObjList((ObjList*)pxObjList.get(), (char*)pcObjName, 0, 0, 0, 0)

// Wrapper for UGW helper functions - Add node to the list
#define UGW_OBJLIST_EDIT_NODE(pxDstObjList,pcObjname,pcParamName,           \
                          pcParamValue,unParamId,unParamFlag)               \
help_editNode((ObjList*)pxDstObjList.get(), (char*)pcObjname,               \
             (char*)pcParamName, (char*)pcParamValue,                       \
             unParamId, unParamFlag)

// Read value from a UGW object
#define UGW_READ_VALUE(pxObj, pcObjName, unInstance, pcParamName, pcVal)    \
help_getValueNameBased((ObjList*)pxObj.get(), (char*)pcObjName, unInstance, \
                       (char*)pcParamName, (char*)pcVal)

#endif // _BWL_UGW_HELPER_H_
