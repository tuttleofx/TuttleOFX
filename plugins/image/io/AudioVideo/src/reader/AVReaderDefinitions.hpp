#ifndef _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

static const std::string kParamFormatGroup               = "formatGroup";
static const std::string kParamFormatDetailledGroup      = "formatDetailledGroup";
static const std::string kParamVideoGroup                = "videoGroup";
static const std::string kParamVideoDetailledGroup       = "videoDetailledGroup";
static const std::string kParamMetaGroup                 = "metaGroup";
static const std::string kParamMetaDetailledGroup        = "metaDetailledGroup";

static const std::string kParamUseCustomSAR = "useCustomSAR";
static const std::string kParamCustomSAR = "customSAR";

static const std::string kParamVideoStreamIndex      = "videoStreamIndex";
static const std::string kParamVideoStreamIndexLabel = "Video stream index";

static const std::string kParamWrapper = "wrapper";
static const std::string kParamFileName =  "fileName";
static const std::string kParamFormatName = "formatName";
static const std::string kParamFormatLongName = "formatLongName";
static const std::string kParamStartTime = "startTime";
static const std::string kParamDuration = "duration";
static const std::string kParamBitrate = "bitrate";
static const std::string kParamNbStream = "nbStream";
static const std::string kParamNbProgram = "nbProgram";
static const std::string kParamNbVideoStream = "nbVideoStream";
static const std::string kParamNbAudioStream = "nbAudioStream";
static const std::string kParamNbDataStream = "nbDataStream";
static const std::string kParamNbSubtitleStream = "nbSubTitleStream";
static const std::string kParamNbAttachementStream = "nbAttachementStream";
static const std::string kParamNbUnknownStream = "nbUnknownStream";

}
}
}
}

#endif
