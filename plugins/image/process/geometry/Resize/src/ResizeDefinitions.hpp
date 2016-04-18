#ifndef _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ResolutionDefinition.hpp>
#include <tuttle/plugin/context/SamplerDefinition.hpp>

namespace tuttle
{
namespace plugin
{
namespace resize
{

static const std::string kParamMode = "mode";
static const std::string kParamModeFormat = "format";
static const std::string kParamModeSize = "size";
static const std::string kParamModeScale = "scale";
enum EParamMode
{
    eParamModeFormat = 0,
    eParamModeSize,
    eParamModeScale
};

static const std::string kParamFormat = "format";

static const std::string kParamSize = "size";
static const std::string kParamSizeWidth = "width";
static const std::string kParamSizeHeight = "height";
static const std::string kParamSizeKeepRatio = "keepRatio";
static const std::string kParamSizeOrientation = "orientation";
static const std::string kParamSizeOrientationX = kParamSizeWidth;
static const std::string kParamSizeOrientationY = kParamSizeHeight;
enum EParamSizeOrientation
{
    eParamSizeOrientationX = 0,
    eParamSizeOrientationY
};

static const std::string kParamScale = "scale";

static const std::string kParamCenter = "center";
static const std::string kParamCenterPoint = "centerPoint";
}
}
}

#endif
