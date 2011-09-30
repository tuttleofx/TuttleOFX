#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

static const std::string kParamCoordinateSystem           = "coordinateSystem";
static const std::string kParamCoordinateSystemNormalized = "coordinateSystemNormalized";
static const std::string kParamCoordinateSystemCanonical  = "coordinateSystemCanonical";

enum EParamCoordinateSystem
{
	eParamCoordinateSystemNormalized = 0,
	eParamCoordinateSystemCanonical
};

static const std::string kParamRectCenter  = "rectCenter";
static const std::string kParamRectSize    = "rectSize";
static const std::string kParamOutputGroup = "outputGroup";

static const std::string kParamOutputGroupRGBA     = "outputGroupRGBA";
static const std::string kParamOutputAverage       = "outputAverage";
static const std::string kParamOutputChannelMin    = "outputChannelMin";
static const std::string kParamOutputChannelMax    = "outputChannelMax";
static const std::string kParamOutputLuminosityMin = "outputLuminosityMin";
static const std::string kParamOutputLuminosityMax = "outputLuminosityMax";
static const std::string kParamOutputKurtosis      = "outputKurtosis";
static const std::string kParamOutputSkewness      = "outputSkewness";

static const std::string kParamOutputGroupHSL         = "outputGroupHSL";
static const std::string kParamOutputAverageHSL       = "outputAverageHSL";
static const std::string kParamOutputChannelMinHSL    = "outputChannelMinHSL";
static const std::string kParamOutputChannelMaxHSL    = "outputChannelMaxHSL";
static const std::string kParamOutputLuminosityMinHSL = "outputLuminosityMinHSL";
static const std::string kParamOutputLuminosityMaxHSL = "outputLuminosityMaxHSL";
static const std::string kParamOutputKurtosisHSL      = "outputKurtosisHSL";
static const std::string kParamOutputSkewnessHSL      = "outputSkewnessHSL";

static const std::string kParamChooseOutput              = "chooseOutput";
static const std::string kParamChooseOutputSource        = "source";
static const std::string kParamChooseOutputAverage       = "average";
static const std::string kParamChooseOutputChannelMin    = "min values";
static const std::string kParamChooseOutputChannelMax    = "max values";
static const std::string kParamChooseOutputLuminosityMin = "min luminosity";
static const std::string kParamChooseOutputLuminosityMax = "max luminosity";

enum EParamChooseOutput
{
	eParamChooseOutputSource = 0,
	eParamChooseOutputAverage,
	eParamChooseOutputChannelMin,
	eParamChooseOutputChannelMax,
	eParamChooseOutputLuminosityMin,
	eParamChooseOutputLuminosityMax
};

}
}
}

#endif
