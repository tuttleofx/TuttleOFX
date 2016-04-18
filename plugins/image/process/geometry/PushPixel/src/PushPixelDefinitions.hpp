#ifndef _TUTTLE_PLUGIN_PUSHPIXEL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_PUSHPIXEL_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace pushPixel
{

const static std::string kClipMask = "mask";
const static std::string kParamSize = "size";
const static std::string kParamNormalizedKernel = "normalizedKernel";
const static std::string kParamIntensity = "intensity";
const static std::string kParamAngle = "angle";

const std::string kParamInterpolation("interpolation");
const std::string kParamInterpolationNearest("nearest neighbor");
const std::string kParamInterpolationBilinear("bilinear");
enum EParamInterpolation
{
    eParamInterpolationNearest = 0,
    eParamInterpolationBilinear,
};

const static std::string kParamBorder = "border";
const static std::string kParamBorderMirror = "Mirror";
const static std::string kParamBorderConstant = "Constant";
const static std::string kParamBorderBlack = "Black";
const static std::string kParamBorderPadded = "Padded";
enum EParamBorder
{
    eParamBorderMirror = 0,
    eParamBorderConstant,
    eParamBorderBlack,
    eParamBorderPadded
};

const static std::string kParamOutput = "output";
const static std::string kParamOutputMotionVectors = "Motion vectors";
const static std::string kParamOutputPushPixel = "Push pixel";
enum EParamOutput
{
    eParamOutputMotionVectors = 0,
    eParamOutputPushPixel
};

const static std::string kHelpButton = "Help";
const static std::string kHelpString = "<b>PushPixel</b>\n"
                                       "Move pixels depending on the mask's gradient.";
}
}
}

#endif
