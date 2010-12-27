#ifndef _TUTTLE_PLUGIN_BLUR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_BLUR_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace blur {

const static std::string kParamSize           = "size";
const static std::string kParamBorder         = "border";
const static std::string kParamBorderMirror   = "Mirror";
const static std::string kParamBorderConstant = "Constant";
const static std::string kParamBorderBlack    = "Black";
const static std::string kParamBorderPadded   = "Padded";

enum EParamBorder
{
	eParamBorderMirror = 0,
	eParamBorderConstant,
	eParamBorderBlack,
	eParamBorderPadded
};

const static std::string kParamGroupAdvanced = "advanced";
const static std::string kParamNormalizedKernel = "normalizedKernel";
const static std::string kParamKernelEpsilon = "kernelEpsilon";

}
}
}

#endif
