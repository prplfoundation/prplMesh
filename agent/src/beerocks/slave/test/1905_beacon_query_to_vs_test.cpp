
#include "../1905_beacon_query_to_vs.h"

//bool load( std::shared_ptr<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST> lhs, const ieee1905_1::CmduMessageRx &rhs ); 

int main()
{
    constexpr const uint16_t BUF_SIZE_BEEROCKS=10000;
    uint8_t buff_beerocks[BUF_SIZE_BEEROCKS];
    ClassList cl(buff_beerocks,BUF_SIZE_BEEROCKS);

    cl.addClass<beerocks_message::cACTION_MONITOR_CLIENT_BEACON_11K_REQUEST>();

    constexpr const uint16_t BUF_SIZE_1905=10000;
    uint8_t buff_1905[BUF_SIZE_1905];
    
    ieee1905_1::CmduMessageRx rx(buff_1905,BUF_SIZE_1905);


    return 0;
}

