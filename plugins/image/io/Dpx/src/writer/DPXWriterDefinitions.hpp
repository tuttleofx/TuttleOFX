#ifndef DPXWRITER_DEFINITIONS_HPP
#define DPXWRITER_DEFINITIONS_HPP

#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

static const std::string kOutputFilename           = "filename";
static const std::string kOutputFilenameLabel      = "Output filename";
static const std::string kRender                   = "Render";
static const std::string kRenderLabel              = "Render";
static const std::string kParamBitDepth            = "bitDepth";
static const std::string kParamBitDepthLabel       = "Bit depth";
static const std::string kParamComponentsType      = "compType";
static const std::string kParamComponentsTypeLabel = "Components type";
static const std::string kParamCompressed          = "Compressed";
static const std::string kParamCompressedLabel     = "Compressed";
static const std::string kParamRenderAlways        = "renderAlways";

const static std::string kDPXWriterHelpButton = "Help";

const static std::string kDPXWriterHelpString = "<b>DPX File writer</b> plugin is used to output dpx files.  <br />";

}
}
}
}

#endif
