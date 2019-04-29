#ifndef __MAPF_COMMON_MSGLIB_H__
#define __MAPF_COMMON_MSGLIB_H__

#include <nng/nng.h>

namespace mapf
{

struct msglib_socket {
	nng_socket sd_;
};

} //namespace mapf

#endif /*__MAPF_COMMON_MSGLIB_H__ */