/**
 * @file EXRReaderProcess.hpp
 * @brief
 * @author
 * @date    16/12/09 15:34
 *
 */
#ifndef EXRREADER_DEFINITIONS_HPP
#define EXRREADER_DEFINITIONS_HPP

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

enum ECompType { eGray, eRGB, eRGBA };
static const std::string kOutputComponents   = "Output components";
static const std::string kOutputRedIs        = "Red is";
static const std::string kOutputGreenIs      = "Green is";
static const std::string kOutputBlueIs       = "Blue is";
static const std::string kOutputAlphaIs      = "Alpha is";

static const std::string kInputFilename      = "Input filename";
static const std::string kSlice				 = "Channel";

const static std::string kEXRReaderHelpButton = "Help";

const static std::string kExrReaderHelpString = "<b>EXR File reader</b> plugin is used to read exr files.  <br />";


}
}
}
}

#endif  // EXRREADER_DEFINITIONS_HPP
