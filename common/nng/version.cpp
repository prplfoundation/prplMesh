//  Report 0MQ version

#include <mapf/common/config.h>
#include <mapf/common/logger.h>
#include <nng/nng.h>

MAPF_INITIALIZE_LOGGER

int main (void)
{
    printf("Current nng version: %s\n", nng_version());
	printf("Current MAPF version is %s\n", MAPF_VERSION);
    return 0;
}
