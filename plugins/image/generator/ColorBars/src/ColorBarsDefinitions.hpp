#ifndef _TUTTLE_PLUGIN_COLORBARS_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORBARS_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorBars
{

static const std::string kColorBarsLevels = "levels";
static const std::string kColorBarsLevels100 = "100%";
static const std::string kColorBarsLevels75 = "75%";

enum EColorBarsLevels
{
    eColorBarsLevels100 = 0,
    eColorBarsLevels75
};
}
}
}

#endif
