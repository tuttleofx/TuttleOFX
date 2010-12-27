#ifndef _TUTTLE_PLUGIN_GAMMA_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_GAMMA_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace gamma {
const static std::string kMasterValue = "master";
const static std::string kRedValue    = "red";
const static std::string kGreenValue  = "green";
const static std::string kBlueValue   = "blue";
const static std::string kAlphaValue  = "alpha";
const static std::string kGammaType   = "gammaType";
const static std::string kInvert      = "invert";

const static std::string kGammaGlobal   = "Global";
const static std::string kGammaChannels = "RGBA";
enum EGammaType
{
	eGammaTypeGlobal   = 0,
	eGammaTypeChannels = 1,
};
}
}
}

#endif
