#ifndef _EXRREADER_DEFINITIONS_HPP_
#define _EXRREADER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

static const std::string kParamOutputComponents     = "outComponents";
static const std::string kParamOutputComponentsGray = "gray";
static const std::string kParamOutputComponentsRGB  = "rgb";
static const std::string kParamOutputComponentsRGBA = "rgba";

enum EParamOutputComponents {
	eParamOutputComponentsGray = 0,
	eParamOutputComponentsRGB,
	eParamOutputComponentsRGBA
};

static const std::string kParamOutputRedIs        = "redIs";
static const std::string kParamOutputGreenIs      = "greenIs";
static const std::string kParamOutputBlueIs       = "blueIs";
static const std::string kParamOutputAlphaIs      = "alphaIs";

}
}
}
}

#endif
