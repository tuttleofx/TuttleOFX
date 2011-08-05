#ifndef _TUTTLE_PLUGIN_MULTIPLY_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_MULTIPLY_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace multiply {

const static std::string kMultiplyType   = "type";

const static std::string kMultiplyGlobal   = "global";
const static std::string kMultiplyChannels = "split";

const static std::string kMasterValue = "master";
const static std::string kRedValue    = "red";
const static std::string kGreenValue  = "green";
const static std::string kBlueValue   = "blue";
const static std::string kAlphaValue  = "alpha";


enum EMultiplyType
{
	eMultiplyTypeGlobal   = 0,
	eMultiplyTypeChannels = 1,
};


}
}
}

#endif
