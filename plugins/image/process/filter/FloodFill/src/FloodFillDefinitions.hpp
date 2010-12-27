#ifndef _TUTTLE_PLUGIN_FLOODFILL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FLOODFILL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


//#define DEBUG_FLOODFILL

namespace tuttle {
namespace plugin {
namespace floodFill {

const static std::string kParamUpperThres = "upperThres";
const static std::string kParamLowerThres = "lowerThres";
const static std::string kParamMinMaxRelative = "minMaxRelative";

const static std::string kParamMethod = "method";
const static std::string kParamMethod4Connections = "4 connections";
const static std::string kParamMethod8Connections = "8 connections";
const static std::string kParamMethodBruteForce = "bruteForce";

enum EParamMethod
{
	eParamMethod4 = 0,
	eParamMethod8,
	eParamMethodBruteForce
};



}
}
}

#endif
