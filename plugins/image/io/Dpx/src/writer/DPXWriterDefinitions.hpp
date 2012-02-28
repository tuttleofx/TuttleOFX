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
#ifndef TUTTLE_PRODUCTION
	eTuttlePluginBitDepth10,
	eTuttlePluginBitDepth12,
#endif
	eTuttlePluginBitDepth16
};

enum ETuttlePluginComponents
{
	eTuttlePluginComponentsRGB = 0,
	eTuttlePluginComponentsRGBA,
	eTuttlePluginComponentsABGR
};

static const std::string kParamCompressed     = "compressed";

}
}
}
}

#endif
