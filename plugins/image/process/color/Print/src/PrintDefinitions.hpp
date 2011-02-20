#ifndef _TUTTLE_PLUGIN_PRINT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_PRINT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace print {

const static std::string kParamMode = "mode";
const static std::string kParamModeImage = "image";
const static std::string kParamModeRegion = "region";
const static std::string kParamModePixel = "pixel";
enum EParamMode
{
	eParamModeImage = 0,
	eParamModeRegion,
	eParamModePixel
};

const static std::string kParamPixel = "pixel";
const static std::string kParamRegionMin = "regionMin";
const static std::string kParamRegionMax = "regionMax";

const static std::string kParamOutput = "output";
const static std::string kParamOutputNumeric = "numeric";
const static std::string kParamOutputAscii = "ascii";
enum EParamOutput
{
	eParamOutputNumeric = 0,
	eParamOutputAscii
};
const static std::string kParamFlip = "flip";


}
}
}

#endif
