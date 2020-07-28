
#include <bcl/beerocks_defines.h>

#include <tlvf/ieee_1905_1/eMediaType.h>

#include <string>

namespace beerocks {

class MediaType {

public:
    /**
     * @brief Gets media type from given frequency band and max bandwidth values.
     *
     * Media type value is obtained by looking up into table_6_12_media_type_802_11 table.
     * Returns UNKNOWN_MEDIA if frequency band and max bandwidth are not found in table.
     *
     * @param frequency_band Frequency band.
     * @param max_bandwidth Maximum bandwidth.
     * @return Media type value as per table_6_12_media_type_802_11 table.
     */
    static ieee1905_1::eMediaType get_802_11_media_type(eFreqType frequency_band,
                                                        eWiFiBandwidth max_bandwidth);
    /**
     * @brief Gets media type for given interface.
     *
     * The mechanism to use to obtain media type depends on the media type group:
     * Ethernet, WiFi, MoCA, etc.
     *
     * @param[in] interface_name Name of the local interface.
     * @param[in] media_type_group The media type group of the connecting interface.
     * @param[in, out] media_type The underlying network technology of the connecting interface.
     *
     * @return True on success and false otherwise.
     */
    static bool get_media_type(const std::string &interface_name,
                               ieee1905_1::eMediaTypeGroup media_type_group,
                               ieee1905_1::eMediaType &media_type);
};

} // namespace beerocks
