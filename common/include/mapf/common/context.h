#ifndef __MAPF_COMMON_CONTEXT_H__
#define __MAPF_COMMON_CONTEXT_H__

#include <mapf/common/logger.h>
#include <mapf/common/err.h>
#include <zmq.h>
#include <iostream>

namespace mapf
{

class Socket;

class Context
{
public:
	
	static Context& Instance()
	{
			   static Context instance;
			   return instance;
	}

	inline ~Context()
	{
		Close();
	}

	inline void* context() {return ctx_;}
	

private:
	Context() : ctx_(zmq_ctx_new())
	{
		mapf_assert(ctx_);
	}
	inline void Close()
	{
		if (ctx_ == nullptr)
			return;

		int rc = zmq_ctx_destroy(ctx_);
		mapf_assert(rc == 0);
		ctx_ = nullptr;
	}

	Context(const Context&) = delete;
	void operator = (const Context&) = delete;
	void *ctx_;
	
};

} /* namespace mapf */

#endif /* __MAPF_COMMON_CONTEXT_H__ */
