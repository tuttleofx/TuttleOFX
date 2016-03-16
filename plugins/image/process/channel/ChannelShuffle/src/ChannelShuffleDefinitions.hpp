#ifndef _TUTTLE_PLUGIN_CHANNELSHUFFLE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CHANNELSHUFFLE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace channelShuffle
{

static const std::string kParamRed = "red";
static const std::string kParamGreen = "green";
static const std::string kParamBlue = "blue";
static const std::string kParamAlpha = "alpha";

enum EChannel
{
    eChannelRed = 0,
    eChannelGreen,
    eChannelBlue,
    eChannelAlpha
};
}
}
}

#endif
