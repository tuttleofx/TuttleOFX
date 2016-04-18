#ifndef _TUTTLE_PLUGIN_FLOODFILL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FLOODFILL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

//#define DEBUG_FLOODFILL

namespace tuttle
{
namespace plugin
{
namespace floodFill
{

static const std::string kParamUpperThres = "upperThres";
static const std::string kParamLowerThres = "lowerThres";
static const std::string kParamMinMaxRelative = "minMaxRelative";

static const std::string kParamMethod = "method";
static const std::string kParamMethod4Connections = "4 connections";
static const std::string kParamMethod8Connections = "8 connections";
static const std::string kParamMethodBruteForce = "bruteForce";

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
