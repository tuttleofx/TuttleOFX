#ifndef _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FFMPEG_WRITER_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace ffmpeg {
namespace writer {

const static std::string kFFMpegHelpButton = "Help";

static const std::string kFilename    = "filename";
static const std::string kFormatLong  = "formatLong";
static const std::string kFormat      = "format";
static const std::string kCodecLong   = "codecLong";
static const std::string kCodec       = "codec";
static const std::string kBitrate     = "bitrate";

const static std::string kFFMpegHelpString = "<b>ffmpeg video io</b> plugin is used to output video file.  <br />";

}
}
}
}

#endif
