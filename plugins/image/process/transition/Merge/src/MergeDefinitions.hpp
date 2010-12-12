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
