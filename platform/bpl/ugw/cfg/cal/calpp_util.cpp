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

#include <mapf/common/logger.h>

#include "calpp_util.h"

namespace beerocks {
namespace bpl {

std::string mainop_to_string(uint32_t op)
{
    std::string retv;

    if (MOPT_GET & op) {
        retv += "MOPT_GET|";
    }
    if (MOPT_SET & op) {
        retv += "MOPT_SET|";
    }
    if (MOPT_INIT & op) {
        retv += "MOPT_INIT|";
    }
    if (MOPT_UNINIT & op) {
        retv += "MOPT_UNINIT|";
    }
    if (MOPT_NOTIFY & op) {
        retv += "MOPT_NOTIFY|";
    }
    if (MOPT_FACTORYRESET & op) {
        retv += "MOPT_FACTORYRESET|";
    }
    if (MOPT_BACKUP & op) {
        retv += "SOPT_MOPT_BACKUPDEFVAL|";
    }
    if (MOPT_REBOOT & op) {
        retv += "MOPT_REBOOT|";
    }
    if (MOPT_FWUPGRADE & op) {
        retv += "MOPT_FWUPGRADE|";
    }
    if (MOPT_ROLLBACK & op) {
        retv += "MOPT_ROLLBACK|";
    }
    if (MOPT_NOOPER & op) {
        retv += "MOPT_NOOPER|";
    }
    if (MOPT_LOGSET & op) {
        retv += "MOPT_LOGSET|";
    }
    if (MOPT_TIMERACTION & op) {
        retv += "MOPT_TIMERACTION|";
    }
    if (MOPT_ACTIVATE & op) {
        retv += "MOPT_ACTIVATE|";
    }

    if (retv.length() > 0) {
        retv = retv.substr(0, retv.length() - 1);
    } else {
        retv = "<NO_VALUE>";
    }
    return retv;
}

std::string subop_to_string(uint32_t subop)
{
    std::string retv;

    if (SOPT_ID & subop) {
        retv += "SOPT_ID|";
    }
    if (SOPT_OBJNAME & subop) {
        retv += "SOPT_OBJNAME|";
    }
    if (SOPT_OBJVALUE & subop) {
        retv += "SOPT_OBJVALUE|";
    }
    if (SOPT_OBJATTR & subop) {
        retv += "SOPT_OBJATTR|";
    }
    if (SOPT_OBJACSATTR & subop) {
        retv += "SOPT_OBJACSATTR|";
    }
    if (SOPT_VALIDATION & subop) {
        retv += "SOPT_VALIDATION|";
    }
    if (SOPT_DEFVAL & subop) {
        retv += "SOPT_DEFVAL|";
    }
    if (SOPT_NOSUBOPER & subop) {
        retv += "SOPT_NOSUBOPER|";
    }
    if (SOPT_LEAFNODE & subop) {
        retv += "SOPT_LEAFNODE|";
    }
    if (SOPT_ORGOWN & subop) {
        retv += "SOPT_ORGOWN|";
    }

    if (retv.length() > 0) {
        retv = retv.substr(0, retv.length() - 1);
    } else {
        retv = "<NO_VALUE>";
    }
    return retv;
}

std::string owner_to_string(uint32_t owner)
{
    std::string retv;

    if (OWN_TR69 & owner) {
        retv += "OWN_TR69|";
    }
    if (OWN_WEB & owner) {
        retv += "OWN_WEB|";
    }
    if (OWN_SERVD & owner) {
        retv += "OWN_SERVD|";
    }
    if (OWN_CLI & owner) {
        retv += "OWN_CLI|";
    }
    if (OWN_POLLD & owner) {
        retv += "OWN_POLLD|";
    }
    if (OWN_OTHER & owner) {
        retv += "OWN_OTHER|";
    }

    if (retv.length() > 0) {
        retv = retv.substr(0, retv.length() - 1);
    } else {
        retv = "<NO_VALUE>";
    }
    return retv;
}

void debug_log(cal_message& msg, const std::string& context)
{
    int indent = 0;

    MAPF_INFO(context);
    indent += 2;

    MAPF_INFO(std::string(indent, ' ') << "unMainOper: " << 
              mainop_to_string(static_cast<MsgHeader*>(msg)->unMainOper));

    MAPF_INFO(std::string(indent, ' ') << "unSubOper: " << 
              subop_to_string(static_cast<MsgHeader*>(msg)->unSubOper));

    MAPF_INFO(std::string(indent, ' ') << "unOwner: " << 
              owner_to_string(static_cast<MsgHeader*>(msg)->unOwner));
    
    MAPF_INFO(std::string(indent, ' ') << "unMsgSize: " << static_cast<MsgHeader*>(msg)->unMsgSize);

    for (auto& obj : msg) {
        MAPF_INFO(std::string(indent, ' ') << "objectName: " << obj.get_name());
        indent += 2;
        for (auto& param : obj.get_param_list()) {
            MAPF_INFO(std::string(indent, ' ') << param.get_name() << " = " << param.get_value());
        }
        indent -= 2;
    }
    indent -= 2;
}

} // namespace bpl
} // namespace beerocks