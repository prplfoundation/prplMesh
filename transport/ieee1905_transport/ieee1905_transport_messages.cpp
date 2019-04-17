#include <mapf/transport/ieee1905_transport_messages.h>

namespace mapf {

const std::string CmduRxMessage::kTopicPrefix = "cmdu.rx.";
const std::string CmduTxMessage::kTopicPrefix = "cmdu.tx.";

const std::string CmduTxConfirmationMessage::kTopicPrefix = "cmdu.conf.tx.";

const std::string InterfaceConfigurationQueryMessage::kTopicPrefix = "if_config.qry.";
const std::string InterfaceConfigurationRequestMessage::kTopicPrefix = "if_config.req.";
const std::string InterfaceConfigurationIndicationMessage::kTopicPrefix = "if_config.ind.";

}; //namespace
