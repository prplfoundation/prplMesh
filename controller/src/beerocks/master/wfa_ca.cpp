#include "wfa_ca.h"

#include <beerocks/bcl/beerocks_string_utils.h>
#include <beerocks/bcl/network/network_utils.h>

#include <beerocks/tlvf/beerocks_message.h>
#include <beerocks/tlvf/beerocks_message_bml.h>

#include <easylogging++.h>

using namespace beerocks;

void wfa_ca::handle_wfa_ca_message(
    Socket *sd, std::shared_ptr<beerocks_message::cACTION_HEADER> beerocks_header,
    ieee1905_1::CmduMessageRx &cmdu_rx, ieee1905_1::CmduMessageTx &cmdu_tx, db &database,
    task_pool &tasks)
{
    auto request = cmdu_rx.addClass<beerocks_message::cACTION_BML_WFA_CA_CONTROLLER_REQUEST>();

    if (!request) {
        LOG(ERROR) << "addClass cACTION_BML_WFA_CA_CONTROLLER_REQUEST failed";
        return;
    }

    const char *command_ptr = request->command();

    LOG(DEBUG) << "Received ACTION_BML_WFA_CA_CONTROLLER_REQUEST, command=" << command_ptr;
}
