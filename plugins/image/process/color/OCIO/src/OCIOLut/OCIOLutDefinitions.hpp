#ifndef _TUTTLE_PLUGIN_OCIOLUTDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_OCIOLUTDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace ocio{
namespace lut {

/// @toto: remove help param and use description

/// @todo prefix with kParam
static const std::string kParamInputFilename      = "filename";
static const std::string kParamInputFilenameLabel = "Lut input filename (csp, 3dl, cube, hdl... see OpenColorIO docs to see all formats)";
static const std::string kParamInterpolationType   = "interpolation";
static const std::string kParamInterpolationTypeLabel   = "InterpolationType";

static const std::string kParamInterpolationNearest   = "nearest";
static const std::string kParamInterpolationLinear = "linear";
static const std::string kParamInterpolationTetrahedral = "tetrahedral";

static const std::string inputspace = "RawInput";
static const std::string outputspace = "ProcessedOutput";

enum EInterpolationType
{
	eInterpolationTypeNearest = 0,
	eInterpolationTypeLinear = 1,
	eInterpolationTypeTetrahedral = 2,

};
}
}
}
}

#endif
