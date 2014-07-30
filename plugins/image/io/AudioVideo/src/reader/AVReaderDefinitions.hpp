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

static const std::string kParamMetaDataInputFile = "metaDataInputFile";

}
}
}
}

#endif
