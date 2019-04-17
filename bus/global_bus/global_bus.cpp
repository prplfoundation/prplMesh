#include "multiap.h"
#include "broker.h"

using namespace std;
using namespace multiap;

void usage()
{
    cout << "usage: broker -[cfbdsh]" << endl;
    cout << "   c <path>     - broker.conf path" << endl;
    cout << "   f <endpoint> - extra frontend endpoints (comma seperated list)" << endl;
    cout << "   b <endpoint> - extra backend endpoints (comma seperated list)" << endl;
    cout << "   d <endpoint> - extra debug (capture) endpoints (comma seperated list)" << endl;
    cout << "   h            - show this help menu" << endl;

}
int main (int argc, char *argv[])
{
    string conf = string(MAPF_ROOT) + "/share/global_bus.conf";
    string endpoint, capture, frontend, backend;
    int opt;

    while ((opt = getopt(argc, argv,"c:f:b:d:h")) != EOF) {
        switch(opt) {
            case 'c':
                conf = optarg;
                cout << "conf path: " << conf << endl;
                break;
            case 'f':
                frontend = optarg;
                break;
            case 'b':
                backend = optarg;
                break;
            case 'd':
                capture = optarg;
                break;
            case 'h':
            default: 
                usage();
                return -1;
        }
    }

    Broker broker(conf);
    
    istringstream f_endpoints(frontend);
    while(std::getline(f_endpoints, endpoint, ','))
        broker.Bind(BrokerSocket::FRONTEND, endpoint);

    istringstream b_endpoints(backend);
    while(std::getline(b_endpoints, endpoint, ','))
        broker.Bind(BrokerSocket::BACKEND, endpoint);

    istringstream c_endpoints(capture);
    while(std::getline(c_endpoints, endpoint, ','))
        broker.Bind(BrokerSocket::CAPTURE, endpoint);

    broker.PrintConfig();
    broker.Run();

	return 0;
}
