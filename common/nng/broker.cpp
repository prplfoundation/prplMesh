#include "msglib.h"
#include <mapf/broker/broker.h>
#include <mapf/common/logger.h>
#include <errno.h>
#include <nng/nng.h>
#include <nng/protocol/pubsub0/pub.h>
#include <nng/protocol/pubsub0/sub.h>

namespace mapf
{

Broker::Broker():
	frontend_(new msglib_socket), backend_(new msglib_socket)
{
	int rc = nng_pub0_open_raw(&backend_->sd_);
	if (rc) {
		fprintf (stderr, "nng: %s\n", nng_strerror (rc));
		mapf_assert(0);
	}
	rc = nng_sub0_open_raw(&frontend_->sd_);
	if (rc) {
		fprintf (stderr, "nng: %s\n", nng_strerror (rc));
		mapf_assert(0);
	}
}

Broker::Broker(const std::string& cfg)
	: Broker() { Init(cfg); }

Broker::~Broker() 
{
	delete frontend_;
	delete backend_;
}

void Broker::PrintConfig()
{
	MAPF_INFO("Broker configuration");
	config_.Print();
}

void Broker::Init(const std::string& cfg)
{
	config_ = std::move(BrokerConfig(cfg));
	config_.Parse();
	Bind();
}

void Broker::Bind()
{
	for (auto& frontend:config_.frontend)
		Bind(BrokerSocket::FRONTEND, frontend);
	for (auto& backend:config_.backend)
		Bind(BrokerSocket::BACKEND, backend);
}

void Broker::Bind(BrokerSocket socket_type, const std::string& endpoint)
{
	int rc;

	switch (socket_type) {
	case BrokerSocket::FRONTEND:
		if (config_.security.enable) {
			//Not supported
		}
		if (config_.verbose)
			MAPF_INFO("bind frontend: " + endpoint);
		std::cout << "bind sub frontend: " << endpoint <<std::endl;
		config_.frontend.insert(endpoint);
		rc = nng_listen(frontend_->sd_, endpoint.c_str(), nullptr, 0);
		if (rc) {
			fprintf (stderr, "nng: %s\n", nng_strerror (rc));
			mapf_assert(0);
		}
		break;
	case BrokerSocket::BACKEND:
		if (config_.security.enable) {
			//Not supported
		}
		if (config_.verbose)
			MAPF_INFO("bind backend: " + endpoint);
		std::cout << "bind pub frontend: " << endpoint << std::endl;
		config_.backend.insert(endpoint);
		rc = nng_listen(backend_->sd_, endpoint.c_str(), nullptr, 0);
		if (rc) {
			fprintf (stderr, "nng: %s\n", nng_strerror (rc));
			mapf_assert(0);
		}
		break;
	default:
		mapf_assert(0);
	}
}

void Broker::Run()
{
	if (config_.verbose)
		MAPF_DBG("start proxy (secure=" << std::boolalpha << config_.security.enable << ")");
	int rc = nng_device(frontend_->sd_, backend_->sd_);
	if (rc) {
		fprintf (stderr, "nng: %s\n", nng_strerror (rc));
		mapf_assert(0);
	}
}

} /* namespace multiap */
