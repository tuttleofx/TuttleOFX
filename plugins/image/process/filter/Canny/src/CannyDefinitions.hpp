#ifndef _TUTTLE_PLUGIN_CANNY_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CANNY_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace canny {

const static std::string kParamBorder = "border";
const static std::string kParamBorderBlack = "Black";
const static std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
	eParamBorderBlack,
	eParamBorderPadded
};

//const static std::string kParamFillAllChannels = "fillAllChannels";

}
}
}

#endif
