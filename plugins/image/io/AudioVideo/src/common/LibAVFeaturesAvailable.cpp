#include "LibAVFeaturesAvailable.hpp"

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace common
{

LibAVFeaturesAvailable::LibAVFeaturesAvailable()
    : _availableFormats()
    , _availableVideoCodecs()
    , _availableAudioCodecs()
    , _optionsPerOutputFormat(avtranscoder::getAvailableOptionsPerOutputFormat())
    , _optionsPerVideoCodec(avtranscoder::getAvailableOptionsPerVideoCodec())
    , _optionsPerAudioCodec(avtranscoder::getAvailableOptionsPerAudioCodec())
    , _allSupportedPixelFormats(avtranscoder::getSupportedPixelFormats())
    , _allSupportedSampleFormats(avtranscoder::getSupportedSampleFormats())
{
    // get available formats as vector
    const avtranscoder::NamesMap formatsAsMap = avtranscoder::getAvailableVideoFormatsNames();
    for(avtranscoder::NamesMap::const_iterator itFormat = formatsAsMap.begin(); itFormat != formatsAsMap.end(); ++itFormat)
    {
        _availableFormats.push_back(std::make_pair(itFormat->first, itFormat->second));
    }

    // get available video codecs as vector
    const avtranscoder::NamesMap videoCodecsAsMap = avtranscoder::getAvailableVideoCodecsNames();
    for(avtranscoder::NamesMap::const_iterator itCodec = videoCodecsAsMap.begin(); itCodec != videoCodecsAsMap.end();
        ++itCodec)
    {
        _availableVideoCodecs.push_back(std::make_pair(itCodec->first, itCodec->second));
    }

    // get available audio codecs as vector
    const avtranscoder::NamesMap audioCodecsAsMap = avtranscoder::getAvailableAudioCodecsNames();
    for(avtranscoder::NamesMap::const_iterator itCodec = audioCodecsAsMap.begin(); itCodec != audioCodecsAsMap.end();
        ++itCodec)
    {
        _availableAudioCodecs.push_back(std::make_pair(itCodec->first, itCodec->second));
    }
}
}
}
}
}
