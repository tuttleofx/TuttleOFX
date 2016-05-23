#ifndef _TUTTLE_PLUGIN_COLORWHEEL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORWHEEL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorWheel
{

static const std::string kColorWheelMode = "type";

static const std::string kColorWheelModeWhite = "white";
static const std::string kColorWheelModeBlack = "black";

static const std::string kColorWheelModeRainbow = "rainbow";

enum EColorWheelMode
{
    eColorWheelModeWhite = 0,
    eColorWheelModeBlack,
    eColorWheelModeRainbow
};
}
}
}

#endif
