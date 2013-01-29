#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/context/WriterDefinition.hpp>
#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8 = 0
};


static const std::string kParamFormat                    = "format";
static const std::string kParamVideoCodec                = "videoCodec";
static const std::string kParamAudioCodec                = "audioCodec";

static const std::string kParamFormatGroup               = "formatGroup";
static const std::string kParamFormatDetailledGroup      = "formatDetailledGroup";
static const std::string kParamVideoGroup                = "videoGroup";
static const std::string kParamVideoDetailledGroup       = "videoDetailledGroup";
static const std::string kParamAudioGroup                = "audioGroup";
static const std::string kParamAudioDetailledGroup       = "audioDetailledGroup";
static const std::string kParamMetaGroup                 = "metaGroup";
static const std::string kParamMetaDetailledGroup        = "metaDetailledGroup";

static const std::string kParamMainPreset                = "mainPreset";

static const std::string kParamFormatPreset              = "formatPreset";
static const std::string kParamVideoPreset               = "videoPreset";
static const std::string kParamAudioPreset               = "audioPreset";

static const std::string kParamVideoCodecPixelFmt        = "videoPixelFormat";

}
}
}
}

#endif
