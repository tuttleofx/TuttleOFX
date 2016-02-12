#ifndef _PNGWRITER_DEFINITIONS_HPP_
#define _PNGWRITER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/ioplugin/context/WriterDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace png {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth8 = 0,
	eTuttlePluginBitDepth16
};

enum ETuttlePluginComponents
{
	eTuttlePluginComponentsAuto = 0,
	eTuttlePluginComponentsGray,
	eTuttlePluginComponentsRGB,
	eTuttlePluginComponentsRGBA
};

}
}
}
}

#endif
