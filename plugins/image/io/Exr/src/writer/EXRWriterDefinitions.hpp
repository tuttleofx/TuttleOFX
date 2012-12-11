#ifndef EXRWRITER_DEFINITIONS_HPP
#define EXRWRITER_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

#include <tuttle/plugin/context/WriterDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

enum ETuttlePluginBitDepth
{
	eTuttlePluginBitDepth16f,
	eTuttlePluginBitDepth32,
	eTuttlePluginBitDepth32f
};

enum ETuttlePluginComponents
{
	eTuttlePluginComponentsAuto = 0,
	eTuttlePluginComponentsGray,
	eTuttlePluginComponentsRGB,
	eTuttlePluginComponentsRGBA,
	eTuttlePluginComponentsABGR
};

static const std::string kParamStorageType     = "storage";
static const std::string kParamStorageScanLine = "scanLine";
static const std::string kParamStorageTiles    = "tiles";

enum EParamStorage
{
	eParamStorageScanLine = 0,
	eParamStorageTiles
};

}
}
}
}

#endif

