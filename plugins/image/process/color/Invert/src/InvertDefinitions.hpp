#ifndef _TUTTLE_PLUGIN_INVERT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_INVERT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace invert
{

static const std::string kParamProcessGroup = "channelGroup";
static const std::string kParamProcessGroupLabel = "Channels";

static const std::string kParamProcessR = "r";
static const std::string kParamProcessRLabel = "R";
static const std::string kParamProcessRHint = "Invert red component";
static const std::string kParamProcessG = "g";
static const std::string kParamProcessGLabel = "G";
static const std::string kParamProcessGHint = "Invert green component";
static const std::string kParamProcessB = "b";
static const std::string kParamProcessBLabel = "B";
static const std::string kParamProcessBHint = "Invert blue component";
static const std::string kParamProcessA = "a";
static const std::string kParamProcessALabel = "A";
static const std::string kParamProcessAHint = "Invert alpha component";

static const std::string kParamProcessGray = "gray";
static const std::string kParamProcessGrayLabel = "Gray";
static const std::string kParamProcessGrayHint = "Invert gray image";
}
}
}

#endif
