/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * Copyright (c) 2016-2019 Intel Corporation
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#ifndef __MAPF_BROKER_H__
#define __MAPF_BROKER_H__

#include <mapf/broker/broker_config.h>
#include <mapf/common/config.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <iostream>


namespace mapf
{
struct msglib_socket;

enum class BrokerSocket:int {FRONTEND, BACKEND};

class Broker
{
public:
	Broker();

	Broker(const std::string& cfg);

	~Broker();

	void Bind(BrokerSocket socket_type, const std::string& endpoint);
	void Run();
	void PrintConfig();

private:
	void Init(const std::string& cfg);
	void Bind();

	BrokerConfig config_;

protected:
	msglib_socket *frontend_;
	msglib_socket *backend_;
	void *context_;
};

} /* namespace mapf */

#endif /* __MAPF_BROKER_H__ */
