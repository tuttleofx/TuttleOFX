#ifndef _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RESIZE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ResolutionDefinition.hpp>
#include <tuttle/plugin/context/SamplerDefinition.hpp>


namespace tuttle {
namespace plugin {
namespace resize {

static const std::string kParamOptions        = "options";
static const std::string kParamFormat         = "toFormat";
static const std::string kParamBox            = "toBox";
static const std::string kParamScale          = "toScale";
enum EParamOptions
{
	eParamFormat = 0,
	eParamBox,
	eParamScale
};


static const std::string kParamOutputFormat    = "outputFormat";
static const std::string kParamOutputDimension = "outputDimension";

static const std::string kParamDirection       = "direction";
static const std::string kParamSizeX           = "width";
static const std::string kParamSizeY           = "height";

enum EParamSize
{
	eParamSizeX = 0,
	eParamSizeY
};

static const std::string kParamScaleDim        = "scale";

static const std::string kParamScaleX          = "scaleX";
static const std::string kParamScaleY          = "scaleY";

static const std::string kParamSplit           = "split";

static const std::string kParamCenter          = "center";
static const std::string kParamCenterPoint     = "centerPoint";

}
}
}

#endif
