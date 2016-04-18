#ifndef _TUTTLE_PLUGIN_SOBEL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_SOBEL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace sobel
{

static const std::string kParamSize = "size";
static const std::string kParamGroupAdvanced = "advanced";
static const std::string kParamUnidimensional = "unidimensional";
static const std::string kParamReverseKernel = "reverseKernel";
static const std::string kParamNormalizedKernel = "normalizedKernel";
static const std::string kParamKernelEpsilon = "kernelEpsilon";
static const std::string kParamPass = "pass";
static const std::string kParamPassFull = "full";
static const std::string kParamPass1 = "1";
static const std::string kParamPass2 = "2";

enum EParamPass
{
    eParamPassFull = 0,
    eParamPass1,
    eParamPass2
};

static const std::string kParamBorder = "border";
static const std::string kParamBorderMirror = "Mirror";
static const std::string kParamBorderConstant = "Constant";
static const std::string kParamBorderBlack = "Black";
static const std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
    eParamBorderMirror = 0,
    eParamBorderConstant,
    eParamBorderBlack,
    eParamBorderPadded
};

static const std::string kParamComputeGradientNorm = "computeGradientNorm";
static const std::string kParamGradientNormManhattan = "gradientNormManhattan";
static const std::string kParamComputeGradientDirection = "computeGradientDirection";
static const std::string kParamGradientDirectionAbs = "gradientDirectionAbs";

static const std::string kParamOutputComponent = "outputComponent";
static const std::string kParamOutputComponentRGBA = "RGBA";
static const std::string kParamOutputComponentRGB = "RGB";

enum EParamOutputComponent
{
    eParamOutputComponentRGBA = 0,
    eParamOutputComponentRGB,
};

static const std::string kParamInfos = "infos";
}
}
}

#endif
