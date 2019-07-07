/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/transport/ieee1905_transport_messages.h>

namespace mapf {

const std::string CmduRxMessage::kTopicPrefix = "cmdu.rx.";
const std::string CmduTxMessage::kTopicPrefix = "cmdu.tx.";

const std::string CmduTxConfirmationMessage::kTopicPrefix = "cmdu.conf.tx.";

const std::string InterfaceConfigurationQueryMessage::kTopicPrefix      = "if_config.qry.";
const std::string InterfaceConfigurationRequestMessage::kTopicPrefix    = "if_config.req.";
const std::string InterfaceConfigurationIndicationMessage::kTopicPrefix = "if_config.ind.";

}; // namespace mapf
