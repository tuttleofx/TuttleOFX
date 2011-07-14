#ifndef _TUTTLE_PLUGIN_PRINT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_PRINT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace print {

static const std::string kParamMode = "mode";
static const std::string kParamModeImage = "image";
static const std::string kParamModeRegion = "region";
static const std::string kParamModePixel = "pixel";
enum EParamMode
{
	eParamModeImage = 0,
	eParamModeRegion,
	eParamModePixel
};

static const std::string kParamPixel = "pixel";
static const std::string kParamRegionMin = "regionMin";
static const std::string kParamRegionMax = "regionMax";

static const std::string kParamOutput = "output";
static const std::string kParamOutputNumeric = "numeric";
static const std::string kParamOutputAscii = "ascii";
enum EParamOutput
{
	eParamOutputNumeric = 0,
	eParamOutputAscii
};
static const std::string kParamFlip = "flip";


}
}
}

#endif
