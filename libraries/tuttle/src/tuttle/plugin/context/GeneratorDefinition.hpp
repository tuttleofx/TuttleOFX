#ifndef _TUTTLE_PLUGIN_CONTEXT_GENERATORDEFINITION_HPP_
#define _TUTTLE_PLUGIN_CONTEXT_GENERATORDEFINITION_HPP_

#include "Definition.hpp"
#include "ResolutionDefinition.hpp"

namespace tuttle {
namespace plugin {

static const std::string kParamGeneratorExplicitConversion = "explicitConversion";
enum EParamGeneratorExplicitConversion
{
	eParamGeneratorExplicitConversionAuto = 0,
	eParamGeneratorExplicitConversionByte,
	eParamGeneratorExplicitConversionShort,
	eParamGeneratorExplicitConversionFloat
};

enum EParamGeneratorComponents
{
	eParamGeneratorComponentsAlpha = 0,
	eParamGeneratorComponentsRGB,
	eParamGeneratorComponentsRGBA
};


static const std::string kParamMode              = "mode";
static const std::string kParamModeFormat        = "format";
static const std::string kParamModeSize          = "size";
enum EParamMode
{
	eParamModeFormat = 0,
	eParamModeSize
};

static const std::string kParamFormat            = "format";

static const std::string kParamSize              = "size";
static const std::string kParamSizeWidth         = "width";
static const std::string kParamSizeHeight        = "height";
static const std::string kParamSizeSpecificRatio = "specificRatio";
static const std::string kParamSizeRatioValue    = "ratio";
static const std::string kParamSizeOrientation   = "orientation";
static const std::string kParamSizeOrientationX  = kParamSizeWidth;
static const std::string kParamSizeOrientationY  = kParamSizeHeight;

enum EParamSizeOrientation
{
	eParamSizeOrientationX = 0,
	eParamSizeOrientationY
};

}
}

#endif

