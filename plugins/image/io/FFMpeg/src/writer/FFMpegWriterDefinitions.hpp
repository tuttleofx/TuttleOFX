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
static const std::string kParamGroupFormatParams         = "formatParams";
static const std::string kParamCodec                     = "codec";
static const std::string kParamGroupCodecParams          = "codecParams";
static const std::string kParamGroupVideoCodecPreset     = "videoCodecPreset";
static const std::string kParamBitrate                   = "bitrate";

}
}
}
}

#endif
