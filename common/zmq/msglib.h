#ifndef __MAPF_COMMON_MSGLIB_H__
#define __MAPF_COMMON_MSGLIB_H__

#include <zmq.h>

namespace mapf
{

struct msglib_socket
{
	void *sd_;
};

} //namespace mapf

#endif /*__MAPF_COMMON_MSGLIB_H__ */