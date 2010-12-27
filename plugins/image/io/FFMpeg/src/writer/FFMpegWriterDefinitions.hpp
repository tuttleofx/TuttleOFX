#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

const static std::string kParamFFMpegHelpButton = "Help";

static const std::string kParamFilename     = "filename";
static const std::string kParamFormatLong   = "formatLong";
static const std::string kParamFormat       = "format";
static const std::string kParamCodecLong    = "codecLong";
static const std::string kParamCodec        = "codec";
static const std::string kParamBitrate      = "bitrate";
static const std::string kParamRenderAlways = "renderAlways";

const static std::string kFFMpegHelpString = "<b>ffmpeg video io</b> plugin is used to output video file.  <br />";

}
}
}
}

#endif
