#ifndef _TUTTLE_PLUGIN_TEXT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_TEXT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace text
{

static const std::string kParamText = "text";

static const std::string kParamFont = "font";
static const std::string kParamFontPath = "fontFile";

static const std::string kParamSize = "textSize";
static const std::string kParamRatio = "ratio";
static const std::string kParamColor = "color";
static const std::string kParamBackgroundColor = "backgroundColor";
static const std::string kParamPosition = "position";
static const std::string kParamLetterSpacing = "letterSpacing";

static const std::string kParamVAlign = "vAlign";
static const std::string kParamVAlignTop = "top";
static const std::string kParamVAlignCenter = "center";
static const std::string kParamVAlignBottom = "bottom";
enum EParamVAlign
{
    eParamVAlignTop = 0,
    eParamVAlignCenter,
    eParamVAlignBottom
};
static const std::string kParamHAlign = "hAlign";
static const std::string kParamHAlignLeft = "left";
static const std::string kParamHAlignCenter = "center";
static const std::string kParamHAlignRight = "right";
enum EParamHAlign
{
    eParamHAlignLeft = 0,
    eParamHAlignCenter,
    eParamHAlignRight
};

static const std::string kParamVerticalFlip = "verticalFlip";
static const std::string kParamItalic = "italic";
static const std::string kParamBold = "bold";

static const std::string kParamFunction = "mergingFunction";
static const std::string kParamFunctionLabel = "Merging function";

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
