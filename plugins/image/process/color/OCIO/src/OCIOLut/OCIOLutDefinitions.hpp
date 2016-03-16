#ifndef _TUTTLE_PLUGIN_OCIOLUTDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_OCIOLUTDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/Definition.hpp>

namespace tuttle
{
namespace plugin
{
namespace ocio
{
namespace lut
{

static const std::string kParamInterpolationType = "interpolation";

static const std::string kTuttlePluginFilenameHint =
    "open file with one of these extensions: 3dl, ccc, cc, csp, cub, cube, hdl, mga, m3d, spi1d, spi3d, spimtx, vf";

static const std::string kParamInterpolationNearest = "nearest";
static const std::string kParamInterpolationLinear = "linear";
static const std::string kParamInterpolationTetrahedral = "tetrahedral";

enum EInterpolationType
{
    eInterpolationTypeNearest = 0,
    eInterpolationTypeLinear = 1,
    eInterpolationTypeTetrahedral = 2,

};

static const std::string kOCIOInputspace = "RawInput";
static const std::string kOCIOOutputspace = "ProcessedOutput";
}
}
}
}

#endif
