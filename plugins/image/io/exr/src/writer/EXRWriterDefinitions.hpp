#ifndef EXRWRITER_DEFINITIONS_HPP
#define EXRWRITER_DEFINITIONS_HPP

#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

enum ECompType { eGray, eRGB, eRGBA };
enum EBitDepth { eHalfFloat, eFloat, eInt32 };

static const std::string kOutputFilename      = "filename";
static const std::string kOutputFilenameLabel = "Output filename";
static const std::string kRender              = "render";
static const std::string kRenderLabel         = "Render";
static const std::string kParamBitDepth       = "bitDepth";
static const std::string kParamBitDepthLabel  = "Bit depth";
static const std::string kParamComponentsType = "compType";
static const std::string kParamComponentsTypeLabel = "Components type";
static const std::string kParamCompressed     = "Compressed";

const static std::string kEXRWriterHelpButton = "Help";

const static std::string kEXRWriterHelpString = "<b>EXR File writer</b> plugin is used to write exr files.<br />";

}
}
}
}

#endif

