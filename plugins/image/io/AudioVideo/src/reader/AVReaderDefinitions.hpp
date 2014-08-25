#ifndef _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_AV_READER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

static const std::string kParamFormatGroup               = "f_group";
static const std::string kParamFormatDetailledGroup      = "f_detailledGroup";
static const std::string kParamVideoGroup                = "v_group";
static const std::string kParamVideoDetailledGroup       = "v_detailledGroup";
static const std::string kParamMetaGroup                 = "m_group";
static const std::string kParamMetaDetailledGroup        = "m_detailledGroup";

static const std::string kParamUseCustomSAR = "v_useCustomSAR";
static const std::string kParamCustomSAR = "v_customSAR";

static const std::string kParamVideoStreamIndex      = "v_streamIndex";
static const std::string kParamVideoStreamIndexLabel = "Video stream index";

static const std::string kParamMetaDataWrapper = "m_wrapper";
static const std::string kParamMetaDataVideo = "m_video";
static const std::string kParamMetaDataAudio = "m_audio";
static const std::string kParamMetaDataData = "m_data";
static const std::string kParamMetaDataSubtitle = "m_subtitle";
static const std::string kParamMetaDataAttachement = "m_attachement";
static const std::string kParamMetaDataUnknown = "m_unknown";

}
}
}
}

#endif
