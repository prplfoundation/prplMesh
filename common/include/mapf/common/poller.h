#ifndef __MAPF_COMMON_POLLER_H__
#define __MAPF_COMMON_POLLER_H__

#include "socket.h"
#include "message.h"
#include <iostream>
#include <vector>
#include <zmq.h>

#define MAPF_POLLIN ZMQ_POLLIN
#define MAPF_POLLOUT ZMQ_POLLOUT
#define MAPF_POLLERR ZMQ_POLLERR
#define MAPF_POLLPRI ZMQ_POLLPRI

namespace mapf
{

class Poller
{
public:
	int Add(const Socket& socket, short events = MAPF_POLLIN);
	int Modify(const Socket& socket, short events);
	int Remove(const Socket& socket);
	int Add(int fd, short events = MAPF_POLLIN);
	int Modify(int fd, short events);
	int Remove(int fd);

	int Poll(long timeout = -1);
	short CheckEvent(const Socket& socket);
	short CheckEvent(int fd);

	void PrintItems() const;

private:
	int Add(const Socket* socket, int fd, short events);
	int Modify(const Socket* socket, int fd, short events);
	int Remove(const Socket* socket, int fd);
	std::vector<zmq_pollitem_t>::iterator Find(const Socket* socket);
	std::vector<zmq_pollitem_t>::iterator Find(int fd);

	std::vector<zmq_pollitem_t> items_;
};

} /* namespace mapf */
#endif /* __MAPF_COMMON_POLLER_H__ */
