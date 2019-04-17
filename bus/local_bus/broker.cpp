#include <mapf/broker/broker.h>
#include <mapf/common/logger.h>

namespace mapf
{

void Broker::PrintConfig()
{
	MAPF_INFO("Borker configuration");
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
	for (auto& capture:config_.capture)
		Bind(BrokerSocket::CAPTURE, capture);
}

void Broker::Bind(BrokerSocket socket_type, const std::string& endpoint)
{
	int rc;

	switch (socket_type) {
	case BrokerSocket::FRONTEND:
		if (config_.security.enable) {
			Setsockopt(frontend_, ZMQ_CURVE_SERVERKEY, config_.security.pub_public.c_str(), config_.security.key_len);
			Setsockopt(frontend_, ZMQ_CURVE_PUBLICKEY, config_.security.xsub_public.c_str(), config_.security.key_len);
			Setsockopt(frontend_, ZMQ_CURVE_SECRETKEY, config_.security.xsub_secret.c_str(), config_.security.key_len);
		}
		if (config_.verbose)
			MAPF_DBG("bind frontend: " + endpoint);
		config_.frontend.insert(endpoint);
		rc = zmq_bind(frontend_, endpoint.c_str());
		mapf_assert(rc == 0);
		break;
	case BrokerSocket::BACKEND:
		if (config_.security.enable) {
			int as_server = 1;
			Setsockopt(backend_, ZMQ_CURVE_SERVER, &as_server, sizeof (int));
			Setsockopt(backend_, ZMQ_CURVE_SECRETKEY, config_.security.xpub_secret.c_str(), config_.security.key_len);
		}
		if (config_.verbose)
			MAPF_DBG("bind backend: " + endpoint);
		config_.backend.insert(endpoint);
		rc = zmq_bind(backend_, endpoint.c_str());
		mapf_assert(rc == 0);
		break;
	case BrokerSocket::CAPTURE:
		if (config_.verbose)
			MAPF_DBG("bind capture: " + endpoint);
		config_.capture.insert(endpoint);
		rc = zmq_bind(capture_, endpoint.c_str());
		mapf_assert(rc == 0);
		break;
	default:
		mapf_assert(0);
	}
}

void Broker::Sanity()
{
	/* check that frontend and backend are binded to at least one endpoint each */
	char endpoint[128];
	size_t len = sizeof(endpoint);

	Getsockopt(frontend_, ZMQ_LAST_ENDPOINT, endpoint, &len);
	if (std::string(endpoint) == "") {
		MAPF_ERR("broker '" + config_.name + "' frontend socket not binded!");
		mapf_assert(0);
	}

	len = sizeof(endpoint);
	Getsockopt(backend_, ZMQ_LAST_ENDPOINT, endpoint, &len);
	if (std::string(endpoint) == "") {
		MAPF_ERR("broker '" + config_.name + "' backend socket not binded!");
		mapf_assert(0);
	}
}

void Broker::Run()
{
	Sanity();
	if (config_.verbose)
		MAPF_DBG("start proxy (secure=" << std::boolalpha << config_.security.enable << ")");
	zmq_proxy(frontend_, backend_, capture_);
}

} /* namespace multiap */
