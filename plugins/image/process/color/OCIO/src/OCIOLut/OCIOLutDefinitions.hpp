#ifndef _TUTTLE_PLUGIN_OCIOLUTDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_OCIOLUTDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace ocio{
namespace lut {

/// @toto: remove help param and use description
static const std::string kHelp      = "help";

/// @todo prefix with kParam
static const std::string kInputFilename      = "filename";
static const std::string kInputFilenameLabel = "OpenColorIO compatible Lut input filename";
static const std::string kInterpolationType   = "interpolation";
static const std::string kInterpolationTypeLabel   = "InterpolationType : ";

static const std::string kInterpolationNearest   = "Nearest";
static const std::string kInterpolationLinear = "Linear";
static const std::string kInterpolationTetrahedral = "Tetrahedral";

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
