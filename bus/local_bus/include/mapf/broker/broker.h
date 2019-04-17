#ifndef __MAPF_BROKER_H__
#define __MAPF_BROKER_H__

#include "broker_config.h"
#include <mapf/common/config.h>
#include <mapf/common/context.h>
#include <mapf/common/err.h>
#include <mapf/common/logger.h>
#include <iostream>
#include <zmq.h>

namespace mapf
{

enum class BrokerSocket:int {FRONTEND, BACKEND, CAPTURE};

class Broker
{
public:
	Broker(void *ctx)
		: context_(ctx)
		, frontend_(zmq_socket(context_, ZMQ_XSUB))
		, backend_(zmq_socket(context_, ZMQ_XPUB))
		, capture_(zmq_socket(context_, ZMQ_PUB)) {}

	Broker(Context& ctx) : Broker(ctx.context()) {}

	Broker() : Broker(zmq_ctx_new()) {}

	Broker(Context& ctx, const std::string& cfg)
		: Broker(ctx) { Init(cfg); }

	Broker(const std::string& cfg)
		: Broker() { Init(cfg); }

	~Broker() {}

	void Bind(BrokerSocket socket_type, const std::string& endpoint);
	void Run();
	void PrintConfig();

private:
	void Init(const std::string& cfg);
	void Bind();
	void Sanity();

	void Setsockopt(void *socket, int option, const void *optval, size_t optvallen)
	{
		int rc = zmq_setsockopt(socket, option, optval, optvallen);
		mapf_assert(rc == 0);
	}

	template<typename T> void Setsockopt(void *socket, int option, T const& optval)
	{
		Setsockopt(socket, option, &optval, sizeof(T));
	}

	void Getsockopt(void *socket, int option, void *optval, size_t *optvallen) const
	{
		int rc = zmq_getsockopt(socket, option, optval, optvallen);
		mapf_assert(rc == 0);
	}

	template<typename T> T Getsockopt(int option) const
	{
		T optval;
		size_t optlen = sizeof(T);
		getsockopt(option, &optval, &optlen);
		return optval;
	}

	BrokerConfig config_;

protected:
	void* context_;
	void* frontend_;
	void* backend_;
	void* capture_;
};

} /* namespace mapf */

#endif /* __MAPF_BROKER_H__ */
