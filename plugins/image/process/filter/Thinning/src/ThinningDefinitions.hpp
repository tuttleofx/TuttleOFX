#ifndef _TUTTLE_PLUGIN_THINNING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_THINNING_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace thinning {

const static std::string kParamBorder = "border";
//const static std::string kParamBorderMirror = "Mirror";
//const static std::string kParamBorderConstant = "Constant";
const static std::string kParamBorderBlack = "Black";
const static std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
//	eParamBorderMirror = 0,
//	eParamBorderConstant,
	eParamBorderBlack = 0,
	eParamBorderPadded
};

}
}
}

#endif
