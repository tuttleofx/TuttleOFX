#ifndef _TUTTLE_PLUGIN_AV_COMMON_LIBAV_FEATURES_AVAILABLE_HPP
#define _TUTTLE_PLUGIN_AV_COMMON_LIBAV_FEATURES_AVAILABLE_HPP

#include <AvTranscoder/util.hpp>

#include <string>
#include <utility>
#include <vector>

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace common
{

/**
 * @brief List of codecs and formats available within libav.
 */
struct LibAVFeaturesAvailable
{
    typedef std::vector<std::pair<std::string, std::string> > NamesArray;

    /**
     * Short/long names of formats and codecs, as a vector.
     */
    NamesArray _availableFormats;
    NamesArray _availableVideoCodecs;
    NamesArray _availableAudioCodecs;

    /**
     * List of libav options per formats and codecs.
     */
    const avtranscoder::OptionArrayMap _optionsPerOutputFormat;
    const avtranscoder::OptionArrayMap _optionsPerVideoCodec;
    const avtranscoder::OptionArrayMap _optionsPerAudioCodec;

    /**
     * Name of supported pixel /sample formats.
     */
    const std::vector<std::string> _allSupportedPixelFormats;
    const std::vector<std::string> _allSupportedSampleFormats;

    LibAVFeaturesAvailable();
};
}
}
}
}

#endif
