#ifndef _MERGE_DEFINITIONS_HPP_
#define _MERGE_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

// Descriptors name
const static std::string kMergeSourceA       = "A";
const static std::string kMergeSourceB       = "B";
const static std::string kMergeHelpButton    = "Help";
const static std::string kMergeFunction      = "mergingFunction";
const static std::string kMergeFunctionLabel = "Merging function";

// Plugin internal data
const static std::string kMergeHelpString = "<b>Clip merging</b> plugin is used to merge two clips A and B.  <br />";

enum EMergeFunction
{
	eMergeFunctionATop, eMergeFunctionAverage,
	eMergeFunctionColor,
	eMergeFunctionColorBurn, eMergeFunctionColorDodge,
	eMergeFunctionConjointOver, eMergeFunctionCopy,
	eMergeFunctionDifference, eMergeFunctionDisjointOver,
	eMergeFunctionDivide, eMergeFunctionExclusion,
	eMergeFunctionFreeze, eMergeFunctionFrom,
	eMergeFunctionGeometric, eMergeFunctionHardLight,
	eMergeFunctionHypot, eMergeFunctionIn,
	eMergeFunctionInterpolated, eMergeFunctionMask,
	eMergeFunctionMatte, eMergeFunctionLighten,
	eMergeFunctionDarken, eMergeFunctionMinus,
	eMergeFunctionMultiply, eMergeFunctionOut,
	eMergeFunctionOver, eMergeFunctionOverlay,
	eMergeFunctionPinLight, eMergeFunctionPlus,
	eMergeFunctionReflect, eMergeFunctionScreen,
	eMergeFunctionStencil, eMergeFunctionUnder,
	eMergeFunctionXOR
};

}
}
}

#endif
