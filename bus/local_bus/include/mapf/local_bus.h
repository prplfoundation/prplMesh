#ifndef __MAPF_LOCAL_BUS_API_H__
#define __MAPF_LOCAL_BUS_API_H__

#include <mapf/broker/broker_interface.h>
#include <mapf/common/config.h>

namespace mapf
{

static const std::string kLocalBusConf(std::string(MAPF_ROOT) + "/share/local_bus.conf");

class LocalBusInterface : public BrokerInterface
{
public:
	LocalBusInterface(Context& ctx) 
		: BrokerInterface(ctx, kLocalBusConf) {}
	~LocalBusInterface() {}
};

}

#endif /* __MAPF_LOCAL_BUS_API_H__ */
