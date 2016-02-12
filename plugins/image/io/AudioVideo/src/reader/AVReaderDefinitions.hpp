#ifndef _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/ioplugin/context/ReaderPlugin.hpp>

#include <common/util.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

static const std::string kParamFormatGroup               = common::kPrefixFormat + "group";
static const std::string kParamFormatDetailedGroup       = common::kPrefixFormat + "detailedGroup";
static const std::string kParamVideoGroup                = common::kPrefixVideo + "group";
static const std::string kParamVideoDetailedGroup        = common::kPrefixVideo + "detailedGroup";
static const std::string kParamMetaGroup                 = common::kPrefixMetaData + "group";
static const std::string kParamMetaDetailedGroup         = common::kPrefixMetaData + "detailedGroup";

static const std::string kParamUseCustomSAR = common::kPrefixVideo + "useCustomSAR";
static const std::string kParamCustomSAR = common::kPrefixVideo + "customSAR";

static const std::string kParamVideoStreamIndex      = common::kPrefixVideo + "streamIndex";
static const std::string kParamVideoStreamIndexLabel = "Video stream index";

static const std::string kParamMetaDataWrapper = common::kPrefixMetaData + "wrapper";
static const std::string kParamMetaDataWrapperLabel = "Wrapper";
static const std::string kParamMetaDataVideo = common::kPrefixMetaData + "video";
static const std::string kParamMetaDataVideoLabel = "Video";
static const std::string kParamMetaDataAudio = common::kPrefixMetaData + "audio";
static const std::string kParamMetaDataAudioLabel = "Audio";
static const std::string kParamMetaDataData = common::kPrefixMetaData + "data";
static const std::string kParamMetaDataDataLabel = "Data";
static const std::string kParamMetaDataSubtitle = common::kPrefixMetaData + "subtitle";
static const std::string kParamMetaDataSubtitleLabel = "Subtitle";
static const std::string kParamMetaDataAttachement = common::kPrefixMetaData + "attachement";
static const std::string kParamMetaDataAttachementLabel = "Attachement";
static const std::string kParamMetaDataUnknown = common::kPrefixMetaData + "unknown";
static const std::string kParamMetaDataUnknownLabel = "Unknown";

static const std::string kParamVerbose = "verbose";

}
}
}
}

#endif
