#ifndef PNGWRITER_DEFINITIONS_HPP
#define PNGWRITER_DEFINITIONS_HPP

#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

static const std::string kOutputFilename      = "filename";
static const std::string kOutputFilenameLabel = "Output filename";
static const std::string kRender              = "render";
static const std::string kRenderLabel         = "Render";
static const std::string kParamBitDepth       = "bitDepth";
static const std::string kParamBitDepthLabel  = "Bit depth";

const static std::string kPNGWriterHelpButton = "Help";
const static std::string kPNGWriterHelpString = "<b>PNG File writer</b> plugin is used to output png files.  <br />";

}
}
}
}

#endif
