#ifndef _TUTTLE_PLUGIN_BLUR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_BLUR_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace blur
{

static const std::string kParamSize = "size";
static const std::string kParamBorder = "border";
static const std::string kParamBorderNo = "No";
static const std::string kParamBorderMirror = "Mirror";
static const std::string kParamBorderConstant = "Constant";
static const std::string kParamBorderBlack = "Black";
static const std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
    eParamBorderNo = 0,
    eParamBorderMirror,
    eParamBorderConstant,
    eParamBorderBlack,
    eParamBorderPadded
};

static const std::string kParamGroupAdvanced = "advanced";
static const std::string kParamNormalizedKernel = "normalizedKernel";
static const std::string kParamKernelEpsilon = "kernelEpsilon";
}
}
}

#endif
