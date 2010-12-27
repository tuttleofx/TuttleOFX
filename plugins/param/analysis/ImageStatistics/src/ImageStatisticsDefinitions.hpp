#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace imageStatistics {

const static std::string kParamCoordinateSystem           = "coordinateSystem";
const static std::string kParamCoordinateSystemNormalized = "coordinateSystemNormalized";
const static std::string kParamCoordinateSystemCanonical  = "coordinateSystemCanonical";

enum EParamCoordinateSystem
{
	eParamCoordinateSystemNormalized = 0,
	eParamCoordinateSystemCanonical
};

const static std::string kParamRectCenter  = "rectCenter";
const static std::string kParamRectSize    = "rectSize";
const static std::string kParamOutputGroup = "outputGroup";

const static std::string kParamOutputGroupRGBA     = "outputGroupRGBA";
const static std::string kParamOutputAverage       = "outputAverage";
const static std::string kParamOutputChannelMin    = "outputChannelMin";
const static std::string kParamOutputChannelMax    = "outputChannelMax";
const static std::string kParamOutputLuminosityMin = "outputLuminosityMin";
const static std::string kParamOutputLuminosityMax = "outputLuminosityMax";
const static std::string kParamOutputKurtosis      = "outputKurtosis";
const static std::string kParamOutputSkewness      = "outputSkewness";

const static std::string kParamOutputGroupHSL         = "outputGroupHSL";
const static std::string kParamOutputAverageHSL       = "outputAverageHSL";
const static std::string kParamOutputChannelMinHSL    = "outputChannelMinHSL";
const static std::string kParamOutputChannelMaxHSL    = "outputChannelMaxHSL";
const static std::string kParamOutputLuminosityMinHSL = "outputLuminosityMinHSL";
const static std::string kParamOutputLuminosityMaxHSL = "outputLuminosityMaxHSL";
const static std::string kParamOutputKurtosisHSL      = "outputKurtosisHSL";
const static std::string kParamOutputSkewnessHSL      = "outputSkewnessHSL";

const static std::string kParamChooseOutput              = "chooseOutput";
const static std::string kParamChooseOutputSource        = "source";
const static std::string kParamChooseOutputAverage       = "average";
const static std::string kParamChooseOutputChannelMin    = "min values";
const static std::string kParamChooseOutputChannelMax    = "max values";
const static std::string kParamChooseOutputLuminosityMin = "min luminosity";
const static std::string kParamChooseOutputLuminosityMax = "max luminosity";

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
