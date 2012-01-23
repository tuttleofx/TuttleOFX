#ifndef EXRWRITER_DEFINITIONS_HPP
#define EXRWRITER_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace writer {

enum EParamBitDepth
{
	eParamBitDepth16f = 0,
	eParamBitDepth32f,
	eParamBitDepth32
};

static const std::string kParamComponentsType = "componentType";
static const std::string kParamComponentsGray = "gray";
static const std::string kParamComponentsRgb  = "rgb";
static const std::string kParamComponentsRgba = "rgba";

enum EParamComponents
{
    eGray,
    eRGB,
    eRGBA
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

