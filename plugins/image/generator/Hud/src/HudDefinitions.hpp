#ifndef _TUTTLE_PLUGIN_HUD_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_HUD_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace hud {

static const std::string kConstantColor = "color";

//TODO factorize this (duplicate from merge plugin)

static const std::string kParamFunction      = "mergingFunction";
static const std::string kParamFunctionLabel = "Merging function";

enum EParamMerge
{
	eParamMergeATop, eParamMergeAverage,
	eParamMergeColor,
	eParamMergeColorBurn, eParamMergeColorDodge,
	eParamMergeConjointOver, eParamMergeCopy,
	eParamMergeDifference, eParamMergeDisjointOver,
	eParamMergeDivide, eParamMergeExclusion,
	eParamMergeFreeze, eParamMergeFrom,
	eParamMergeGeometric, eParamMergeHardLight,
	eParamMergeHypot, eParamMergeIn,
	eParamMergeInterpolated, eParamMergeMask,
	eParamMergeMatte, eParamMergeLighten,
	eParamMergeDarken, eParamMergeMinus,
	eParamMergeMultiply, eParamMergeOut,
	eParamMergeOver, eParamMergeOverlay,
	eParamMergePinLight, eParamMergePlus,
	eParamMergeReflect, eParamMergeScreen,
	eParamMergeStencil, eParamMergeUnder,
	eParamMergeXOR
};

}
}
}

#endif
