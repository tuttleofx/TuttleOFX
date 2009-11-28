/**
 * @file MergeProcess.hpp
 * @brief
 * @author
 * @date    27/11/09 13:42
 *
 */
#ifndef MERGE_DEFINITIONS_HPP
#define MERGE_DEFINITIONS_HPP

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace merge {

// Descriptors name
const static std::string kMergeSourceA = "SourceA";
const static std::string kMergeSourceB = "SourceB";
const static std::string kMergeHelpButton = "Help";
const static std::string kMergeFunction = "MergeFunction";

// Plugin internal data
const static std::string kMergeHelpString = "<b>Clip merging</b> plugin is used to merge two clips A and B.  <br />";
enum EMergeFunction { eMergeFunctionATop, eMergeFunctionAverage,
                      eMergeFunctionColorBurn, eMergeFunctionColorDodge, 
                      eMergeFunctionConjointOver, eMergeFunctionCopy,
                      eMergeFunctionDifference, eMergeFunctionDisjointOver,
                      eMergeFunctionDivide, eMergeFunctionExclusion, 
                      eMergeFunctionFrom, eMergeFunctionGeometric,
                      eMergeFunctionHardLight, eMergeFunctionHypot,
                      eMergeFunctionIn, eMergeFunctionMask, 
                      eMergeFunctionMatte, eMergeFunctionMax, 
                      eMergeFunctionMin, eMergeFunctionMinus,
                      eMergeFunctionMultiply, eMergeFunctionOut,
                      eMergeFunctionOver, eMergeFunctionOverlay,
                      eMergeFunctionPlus, eMergeFunctionScreen,
                      eMergeFunctionStencil, eMergeFunctionUnder,
                      eMergeFunctionXOR };
}
}
}

#endif  // MERGE_DEFINITIONS_HPP
