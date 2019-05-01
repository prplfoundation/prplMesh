/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_BROKER_IF_H__
#define __MAPF_BROKER_IF_H__

#include "broker_config.h"
#include <mapf/common/context.h>
#include <mapf/common/socket.h>
#include <mapf/common/poller.h>
#include <mapf/common/message.h>

extern char *__progname;
namespace mapf
{

class BrokerInterface
{
public:
	BrokerInterface(Context& ctx, const std::string& cfg, std::string sync_topic = "hello")
		: config_(cfg)
		, sub_(ctx)
		, pub_(ctx)
		, kSyncTopic(sync_topic) {}

	~BrokerInterface() {}

	void Init();

	void PrintConfig();

	void Sync();

	SubSocket& subscriber() const { return sub_; }
	PubSocket& publisher() const { return pub_; }

protected:
	BrokerConfig config_;
	mutable SubSocket sub_;
	mutable PubSocket pub_;

private:
	BrokerInterface();
	const std::string kSyncTopic;
	static int syncCount;
	const std::string sync_topic(){ return kSyncTopic + std::string(__progname) + std::to_string(syncCount); }

	void SyncSend(Message& msg);	

	bool SyncRecv(const std::string& msg_topic);
};

} /* namespace mapf */
#endif /* __MAPF_BROKER_IF_H__ */
