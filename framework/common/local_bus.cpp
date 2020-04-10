/* SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 * SPDX-FileCopyrightText: 2016-2020 the prplMesh contributors (see AUTHORS.md)
 *
 * This code is subject to the terms of the BSD+Patent license.
 * See LICENSE file for more details.
 */

#include <mapf/broker/broker.h>
#include <mapf/common/context.h>
#include <mapf/common/logger.h>
#include <sstream>
#include <unistd.h>

void usage()
{
    std::cout << "usage: broker -[cfbdsh]" << std::endl;
    std::cout << "   c <path>     - broker.conf path" << std::endl;
    std::cout << "   f <endpoint> - extra frontend endpoints (comma seperated list)" << std::endl;
    std::cout << "   b <endpoint> - extra backend endpoints (comma seperated list)" << std::endl;
    std::cout << "   h            - show this help menu" << std::endl;
}
int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("local_bus");
    std::string conf = std::string(MAPF_ROOT) + "/share/local_bus.conf";
    std::string endpoint, frontend, backend;
    int opt;

    while ((opt = getopt(argc, argv, "c:f:b:h")) != EOF) {
        switch (opt) {
        case 'c':
            conf = optarg;
            std::cout << "conf path: " << conf << std::endl;
            break;
        case 'f':
            frontend = optarg;
            break;
        case 'b':
            backend = optarg;
            break;
        case 'h':
        default:
            usage();
            return -1;
        }
    }

    mapf::Broker broker(conf);
    std::istringstream f_endpoints(frontend);
    while (std::getline(f_endpoints, endpoint, ','))
        broker.Bind(mapf::BrokerSocket::FRONTEND, endpoint);

    std::istringstream b_endpoints(backend);
    while (std::getline(b_endpoints, endpoint, ','))
        broker.Bind(mapf::BrokerSocket::BACKEND, endpoint);

    broker.PrintConfig();
    broker.Run();

    return 0;
}
