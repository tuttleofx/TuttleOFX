#ifndef _DPXWRITER_DEFINITIONS_HPP_
#define _DPXWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8 = 0,
	eTuttlePluginBitDepth10,
	eTuttlePluginBitDepth12,
	eTuttlePluginBitDepth16
};

enum ETuttlePluginComponents
{
	eTuttlePluginComponentsRGB = 0,
	eTuttlePluginComponentsRGBA,
	eTuttlePluginComponentsABGR
};

static const std::string kParamPacket      = "packed";
static const std::string kParamPacketLabel = "Packed";

}
}
}
}

#endif
