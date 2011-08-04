#ifndef _TUTTLE_PLUGIN_ADD_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_ADD_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace add {

const static std::string kAddType   = "type";

const static std::string kAddGlobal   = "global";
const static std::string kAddChannels = "split";

const static std::string kMasterValue = "master";
const static std::string kRedValue    = "red";
const static std::string kGreenValue  = "green";
const static std::string kBlueValue   = "blue";
const static std::string kAlphaValue  = "alpha";


enum EAddType
{
	eAddTypeGlobal   = 0,
	eAddTypeChannels = 1,
};


}
}
}

#endif
