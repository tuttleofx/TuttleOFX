#ifndef _TUTTLE_PLUGIN_GAMMA_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_GAMMA_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace gamma
{
static const std::string kMasterValue = "master";
static const std::string kRedValue = "red";
static const std::string kGreenValue = "green";
static const std::string kBlueValue = "blue";
static const std::string kAlphaValue = "alpha";
static const std::string kGammaType = "gammaType";
static const std::string kInvert = "invert";

static const std::string kGammaGlobal = "Global";
static const std::string kGammaChannels = "RGBA";
enum EGammaType
{
    eGammaTypeGlobal = 0,
    eGammaTypeChannels = 1,
};
}
}
}

#endif
