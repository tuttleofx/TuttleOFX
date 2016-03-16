#ifndef _TUTTLE_PLUGIN_MERGE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_MERGE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace merge
{

// Descriptors name
static const std::string kParamSourceA = "A";
static const std::string kParamSourceB = "B";
static const std::string kParamFunction = "mergingFunction";
static const std::string kParamFunctionLabel = "Merging function";

static const std::string kParamOffsetA = "offsetA";
static const std::string kParamOffsetB = "offsetB";

static const std::string kParamRod = "rod";
static const std::string kParamRodIntersect = "intersection";
static const std::string kParamRodUnion = "union";
static const std::string kParamRodA = "A";
static const std::string kParamRodB = "B";

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
    eParamMergeATop,
    eParamMergeAverage,
    eParamMergeColor,
    eParamMergeColorBurn,
    eParamMergeColorDodge,
    eParamMergeConjointOver,
    eParamMergeCopy,
    eParamMergeDifference,
    eParamMergeDisjointOver,
    eParamMergeDivide,
    eParamMergeExclusion,
    eParamMergeFreeze,
    eParamMergeFrom,
    eParamMergeGeometric,
    eParamMergeHardLight,
    eParamMergeHypot,
    eParamMergeIn,
    eParamMergeInterpolated,
    eParamMergeMask,
    eParamMergeMatte,
    eParamMergeLighten,
    eParamMergeDarken,
    eParamMergeMinus,
    eParamMergeMultiply,
    eParamMergeOut,
    eParamMergeOver,
    eParamMergeOverlay,
    eParamMergePinLight,
    eParamMergePlus,
    eParamMergeReflect,
    eParamMergeScreen,
    eParamMergeStencil,
    eParamMergeUnder,
    eParamMergeXOR
};
}
}
}

#endif
