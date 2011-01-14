#ifndef _EXRREADER_DEFINITIONS_HPP_
#define _EXRREADER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

enum ECompType { eGray, eRGB, eRGBA };

static const std::string kParamOutputComponents   = "outComponents";
static const std::string kParamOutputRedIs        = "redIs";
static const std::string kParamOutputGreenIs      = "greenIs";
static const std::string kParamOutputBlueIs       = "blueIs";
static const std::string kParamOutputAlphaIs      = "alphaIs";
static const std::string kParamExplicitConversion = "explicitConversion";

}
}
}
}

#endif
