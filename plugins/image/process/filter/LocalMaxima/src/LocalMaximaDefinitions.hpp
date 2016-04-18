#ifndef _TUTTLE_PLUGIN_LOCALMAXIMA_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_LOCALMAXIMA_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace localmaxima
{

static const std::string kParamBorder = "border";
static const std::string kParamBorderBlack = "Black";
static const std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
    eParamBorderBlack,
    eParamBorderPadded
};

static const std::string kParamOutputComponent = "outputComponent";
static const std::string kParamOutputComponentRGBA = "RGBA";
static const std::string kParamOutputComponentRGB = "RGB";
static const std::string kParamOutputComponentAlpha = "Alpha";

enum EParamOutputComponent
{
    eParamOutputComponentRGBA = 0,
    eParamOutputComponentRGB,
    eParamOutputComponentAlpha
};

// static const std::string kParamFillAllChannels = "fillAllChannels";
}
}
}

#endif
