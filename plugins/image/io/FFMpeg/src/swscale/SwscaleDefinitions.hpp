#ifndef _TUTTLE_PLUGIN_SWSCALE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_SWSCALE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ResolutionDefinition.hpp>
#include <tuttle/plugin/context/SamplerDefinition.hpp>


namespace tuttle {
namespace plugin {
namespace swscale {

// RESIZE options

static const std::string kParamMode             = "mode";
static const std::string kParamModeFormat       = "format";
static const std::string kParamModeSize         = "size";
static const std::string kParamModeScale        = "scale";
enum EParamMode
{
        eParamModeFormat = 0,
        eParamModeSize,
        eParamModeScale
};

static const std::string kParamFormat           = "format";

static const std::string kParamSize             = "size";
static const std::string kParamSizeWidth        = "width";
static const std::string kParamSizeHeight       = "height";
static const std::string kParamSizeKeepRatio    = "keepRatio";
static const std::string kParamSizeOrientation  = "orientation";
static const std::string kParamSizeOrientationX = kParamSizeWidth;
static const std::string kParamSizeOrientationY = kParamSizeHeight;
enum EParamSizeOrientation
{
        eParamSizeOrientationX = 0,
        eParamSizeOrientationY
};

static const std::string kParamScale            = "scale";



// FILTER
static const std::string kParamFilter                = "filter";


static const std::string kParamFilterFastBilinear = "fastBilinear";
static const std::string kParamFilterBilinear     = "bilinear";
static const std::string kParamFilterBicubic      = "bicubic";
static const std::string kParamFilterX            = "x";
static const std::string kParamFilterPoint        = "point";
static const std::string kParamFilterArea         = "area";
static const std::string kParamFilterBicublin     = "bicublin";
static const std::string kParamFilterGauss        = "gauss";
static const std::string kParamFilterSinc         = "sinc";
static const std::string kParamFilterLanczos      = "lanczos";
static const std::string kParamFilterSpline       = "spline";

enum EParamFilter
{
	eParamFilterFastBilinear = 0,
	eParamFilterBilinear,
	eParamFilterBicubic,
	eParamFilterX,
	eParamFilterPoint,
	eParamFilterArea,
	eParamFilterBicublin,
	eParamFilterGauss,
	eParamFilterSinc,
	eParamFilterLanczos,
	eParamFilterSpline
};

}
}
}

#endif
