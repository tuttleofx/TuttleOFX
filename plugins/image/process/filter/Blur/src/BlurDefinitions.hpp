#ifndef _TUTTLE_PLUGIN_BLUR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_BLUR_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace blur {

const static std::string kParamSize = "size";
const static std::string kParamBorder = "border";
const static std::string kParamBorderExtendMirror = "Extend mirror";
const static std::string kParamBorderExtendConstant = "Extend constant";
const static std::string kParamBorderExtendBlack = "Extend black";
const static std::string kParamBorderOutputBlack = "Output black";

enum EBorder
{
	eBorderExtendMirror = 0,
	eBorderExtendConstant,
	eBorderExtendBlack,
	eBorderOutputBlack
};


}
}
}

#endif
