#ifndef EXRREADER_DEFINITIONS_HPP
#define EXRREADER_DEFINITIONS_HPP

#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

enum ECompType { eGray, eRGB, eRGBA };
static const std::string kOutputComponents      = "outComponents";
static const std::string kOutputComponentsLabel = "Output components";
static const std::string kOutputRedIs           = "redIs";
static const std::string kOutputRedIsLabel      = "Red is";
static const std::string kOutputGreenIs         = "greenIs";
static const std::string kOutputGreenIsLabel    = "Green is";
static const std::string kOutputBlueIs          = "blueIs";
static const std::string kOutputBlueIsLabel     = "Blue is";
static const std::string kOutputAlphaIs         = "alphaIs";
static const std::string kOutputAlphaIsLabel    = "Alpha is";

static const std::string kInputFilename = "filename";
static const std::string kInputFilenameLabel = "Input filename";

const static std::string kEXRReaderHelpButton = "Help";

const static std::string kExrReaderHelpString = "<b>EXR File reader</b> plugin is used to read exr files.  <br />";

}
}
}
}

#endif
