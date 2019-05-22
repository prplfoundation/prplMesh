/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef _BWL_UGW_HELPER_H_
#define _BWL_UGW_HELPER_H_

#include <memory>

//////////////////////////////////////////////////////////////////////////////
////////////////////////// Local Module Definitions //////////////////////////
//////////////////////////////////////////////////////////////////////////////

// Wrapper for UGW helper functions - Add new object
#define UGW_OBJLIST_CREATE(objlist_name)                                                           \
    auto objlist_name =                                                                            \
        std::shared_ptr<ObjList>((ObjList *)HELP_CREATE_OBJ(SOPT_OBJVALUE), [](ObjList *obj) {     \
            if (obj)                                                                               \
                HELP_DELETE_OBJ(obj, SOPT_OBJVALUE, FREE_OBJLIST);                                 \
        })

// Wrapper for UGW helper functions - Wrap existing new object
#define UGW_OBJLIST_WRAP_EXTERNAL(objlist_name, objlist_ptr)                                       \
    auto objlist_name = std::shared_ptr<ObjList>((ObjList *)objlist_ptr, [](ObjList *obj) {        \
        /*if (obj) HELP_DELETE_OBJ(obj, SOPT_OBJVALUE, FREE_OBJLIST);*/                            \
    })

// Wrapper for UGW helper functions - Add new list
#define UGW_OBJLIST_ADD(pxObjList, pcObjName)                                                      \
    help_addObjList((ObjList *)pxObjList.get(), (char *)pcObjName, 0, 0, 0, 0)

// Wrapper for UGW helper functions - Add node to the list
#define UGW_OBJLIST_EDIT_NODE(pxDstObjList, pcObjname, pcParamName, pcParamValue, unParamId,       \
                              unParamFlag)                                                         \
    help_editNode((ObjList *)pxDstObjList.get(), (char *)pcObjname, (char *)pcParamName,           \
                  (char *)pcParamValue, unParamId, unParamFlag)

// Read value from a UGW object
#define UGW_READ_VALUE(pxObj, pcObjName, unInstance, pcParamName, pcVal)                           \
    help_getValueNameBased((ObjList *)pxObj.get(), (char *)pcObjName, unInstance,                  \
                           (char *)pcParamName, (char *)pcVal)

#endif // _BWL_UGW_HELPER_H_
