#ifndef _TUTTLE_PLUGIN_LOCALMAXIMA_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_LOCALMAXIMA_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace localmaxima {

const static std::string kParamBorder = "border";
const static std::string kParamBorderBlack = "Black";
const static std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
	eParamBorderBlack,
	eParamBorderPadded
};

const static std::string kParamOutputComponent = "outputComponent";
const static std::string kParamOutputComponentRGBA = "RGBA";
const static std::string kParamOutputComponentRGB = "RGB";
const static std::string kParamOutputComponentAlpha = "Alpha";

enum EParamOutputComponent
{
	eParamOutputComponentRGBA = 0,
	eParamOutputComponentRGB,
	eParamOutputComponentAlpha
};

//const static std::string kParamFillAllChannels = "fillAllChannels";

}
}
}

#endif
