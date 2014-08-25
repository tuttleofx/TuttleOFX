#ifndef _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <common/util.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

static const std::string kParamFormatGroup               = common::kPrefixFormat + "group";
static const std::string kParamFormatDetailledGroup      = common::kPrefixFormat + "detailledGroup";
static const std::string kParamVideoGroup                = common::kPrefixVideo + "group";
static const std::string kParamVideoDetailledGroup       = common::kPrefixVideo + "detailledGroup";
static const std::string kParamMetaGroup                 = common::kPrefixMetaData + "group";
static const std::string kParamMetaDetailledGroup        = common::kPrefixMetaData + "detailledGroup";

static const std::string kParamUseCustomSAR = common::kPrefixVideo + "useCustomSAR";
static const std::string kParamCustomSAR = common::kPrefixVideo + "customSAR";

static const std::string kParamVideoStreamIndex      = common::kPrefixVideo + "streamIndex";
static const std::string kParamVideoStreamIndexLabel = "Video stream index";

static const std::string kParamMetaDataWrapper = common::kPrefixMetaData + "wrapper";
static const std::string kParamMetaDataVideo = common::kPrefixMetaData + "video";
static const std::string kParamMetaDataAudio = common::kPrefixMetaData + "audio";
static const std::string kParamMetaDataData = common::kPrefixMetaData + "data";
static const std::string kParamMetaDataSubtitle = common::kPrefixMetaData + "subtitle";
static const std::string kParamMetaDataAttachement = common::kPrefixMetaData + "attachement";
static const std::string kParamMetaDataUnknown = common::kPrefixMetaData + "unknown";

}
}
}
}

#endif
