#ifndef _MERGE_DEFINITIONS_HPP_
#define _MERGE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace merge {

// Descriptors name
const static std::string kParamSourceA       = "A";
const static std::string kParamSourceB       = "B";
const static std::string kParamFunction      = "mergingFunction";
const static std::string kParamFunctionLabel = "Merging function";

const static std::string kParamOffsetA       = "offsetA";
const static std::string kParamOffsetB       = "offsetB";
const static std::string kParamRod           = "rod";
const static std::string kParamRodIntersect  = "intersection";
const static std::string kParamRodUnion      = "union";
const static std::string kParamRodA          = "A";
const static std::string kParamRodB          = "B";

enum EParamRod
{
	eParamRodIntersect = 0,
	eParamRodUnion,
	eParamRodA,
	eParamRodB
};

// Plugin internal data

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
