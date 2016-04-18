#ifndef _TUTTLE_PLUGIN_COMPONENT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COMPONENT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace component
{

static const std::string kParamTo = "to";
static const std::string kParamToLabel = "To";

static const std::string kConvertToGray = "gray";
static const std::string kConvertToRGB = "rgb";
static const std::string kConvertToRGBA = "rgba";

enum EConvertTo
{
    eConvertToGray = 0,
    eConvertToRGB,
    eConvertToRGBA
};

static const std::string kParamToGray = "toGray";
static const std::string kParamToGrayLabel = "To Gray";

static const std::string kConvertToGrayMean = "arithmeticMean (1/3, 1/3, 1/3)";
static const std::string kConvertToGrayRec601 = "rec601-SD";
static const std::string kConvertToGrayRec709 = "rec709-HD";
static const std::string kConvertToGraySelectRed = "red";
static const std::string kConvertToGraySelectGreen = "green";
static const std::string kConvertToGraySelectBlue = "blue";
static const std::string kConvertToGraySelectAlpha = "alpha";

static const std::string kParamPremutliplied = "premultiplied";
static const std::string kParamPremutlipliedLabel = "Premultiplied";
}
}
}

#endif
