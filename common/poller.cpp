#include <mapf/common/poller.h>
#include <mapf/common/logger.h>
#include <mapf/common/err.h>
#include <algorithm>

namespace mapf
{
void test(const Socket& socket)
{
	std::cout << socket.handle() << std::endl;
}

std::vector<zmq_pollitem_t>::iterator
Poller::Find(const Socket *socket)
{
	return std::find_if(items_.begin(), items_.end(), 
						[socket](const zmq_pollitem_t &item) { return item.socket == socket->handle(); });
}

std::vector<zmq_pollitem_t>::iterator
Poller::Find(int fd)
{
	return std::find_if(items_.begin(), items_.end(), 
						[&fd](const zmq_pollitem_t &item) {return item.fd == fd;});
}

// private add/modify/remove
int Poller::Add(const Socket* socket, int fd, short events)
{
	if (socket && socket->Pollable() == false) {
		MAPF_ERR("socket " << *socket << " not pollable!");
		errno = EINVAL;
		return -1;
	}

	auto it = socket ? Find(socket) : Find(fd);
	if (it == items_.end()) {
		items_.push_back({socket ? socket->handle() : nullptr, fd, events, 0});
		return 0;
	}

	errno = EEXIST;
	return -1;
}

int Poller::Add(int fd, short events)
{
	errno = 0;
	mapf_assert(fd >= 0);
	int rc = Add(nullptr, fd, events);
	if (errno == EEXIST)
		MAPF_ERR("fd " << fd << " already added, skipping...");
	return rc;
}

int Poller::Add(const Socket& socket, short events)
{
	errno = 0;
	int rc = Add(&socket, -1, events);
	if (errno == EEXIST)
		MAPF_ERR("socket " << socket << " already added, skipping...");
	return rc;
}

int Poller::Modify(const Socket* socket, int fd, short events)
{
	auto it = socket ? Find(socket) : Find(fd);
	if (it != items_.end()) {
		it->events = events;
		return 0;
	}

	errno = ENOENT;
	return -1;
}

int Poller::Modify(const Socket& socket, short events)
{
	errno = 0;
	int rc = Modify(&socket, -1, events);
	if (errno == ENOENT)
		MAPF_ERR("socket " << socket << " not added, skipping...");
	return rc;
}

int Poller::Modify(int fd, short events)
{
	errno = 0;
	int rc = Modify(nullptr, fd, events);
	if (errno == ENOENT)
		MAPF_ERR("fd " << fd << " not added, skipping...");
	return rc;
}

int Poller::Remove(const Socket *socket, int fd)
{
	auto it = socket ? Find(socket) : Find(fd);
	if (it != items_.end()) {
		items_.erase(it);
		return 0;
	}
	errno = ENOENT;
	return -1;
}

int Poller::Remove(const Socket &socket)
{
	errno = 0;
	int rc = Remove(&socket, -1);
	if (errno == ENOENT)
		MAPF_ERR("socket " << socket << " not added, skipping...");
	return rc;
}

int Poller::Remove(int fd)
{
	errno = 0;
	int rc = Remove(nullptr, fd);
	if (errno == ENOENT)
		MAPF_ERR("fd " << fd << " not added, skipping...");
	return rc;
}

int Poller::Poll(long timeout)
{
	if (items_.empty())
		return 0;
	errno = 0;
	return zmq_poll(items_.data(), items_.size(), timeout);
}

short Poller::CheckEvent(const Socket& socket)
{
	auto it = Find(&socket);
	if (it != items_.end())
		return (*it).revents;

	MAPF_ERR("socket " << socket << " not added, skipping...");
	errno = ENOENT;
	return -1;
}

short Poller::CheckEvent(int fd)
{
	auto it = Find(fd);
	if (it != items_.end())
		return (*it).revents;

	MAPF_ERR("fd " << fd << " not added, skipping...");
	errno = ENOENT;
	return -1;
}

void Poller::PrintItems() const
{
	MAPF_INFO("Poll Items:");
	for (auto item:items_)
		MAPF_INFO("socket: " << item.socket << ", fd: " << item.fd << ", events: " << item.events << ", revents: " << item.revents);
}

} /* namespace mapf */
