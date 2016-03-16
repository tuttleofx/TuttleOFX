#ifndef _TUTTLE_PLUGIN_THINNING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_THINNING_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace thinning
{

static const std::string kParamBorder = "border";
// static const std::string kParamBorderMirror = "Mirror";
// static const std::string kParamBorderConstant = "Constant";
static const std::string kParamBorderBlack = "Black";
static const std::string kParamBorderPadded = "Padded";

enum EParamBorder
{
    //	eParamBorderMirror = 0,
    //	eParamBorderConstant,
    eParamBorderBlack = 0,
    eParamBorderPadded
};
}
}
}

#endif
