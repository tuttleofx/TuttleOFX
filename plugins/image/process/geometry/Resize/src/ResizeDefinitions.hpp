#ifndef _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace resize {

static const std::string kParamType = "Type";
static const std::string kParamOutputFormat = "Output Format";
static const std::string kParamScale = "Scale";
enum EParamType
{
	eParamFilterToFormat = 0,
	eParamFilterScale
};

static const std::string kParamSplit = "Split";

static const std::string kParamFilter = "Filter";

static const std::string kParamFilterNearest  = "Nearest neighbor";
static const std::string kParamFilterBilinear = "Bilinear";
static const std::string kParamFilterBicubic  = "Bicubic";
static const std::string kParamFilterLanczos  = "Lanczos";
enum EParamFilter
{
	eParamFilterNearest = 0,
	eParamFilterBilinear,
	eParamFilterBicubic,
	eParamFilterLanczos
};

}
}
}

#endif
