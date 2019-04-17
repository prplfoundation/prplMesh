#include <mapf/transport/ieee1905_transport.h>

#include <unistd.h>
#include <net/if.h>

MAPF_INITIALIZE_LOGGER

using namespace mapf;

int main(int argc, char *argv[])
{
    mapf::Logger::Instance().LoggerInit("transport");
    Ieee1905Transport ieee1905_transport;

    MAPF_INFO("starting main loop...");
    ieee1905_transport.run();

    MAPF_INFO("done");

    return 0;
}
