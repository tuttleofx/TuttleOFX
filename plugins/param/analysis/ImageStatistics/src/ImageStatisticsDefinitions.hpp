#ifndef _TUTTLE_PLUGIN_IMAGESTATISTICS_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_IMAGESTATISTICS_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace imageStatistics {

const static std::string kCornerA = "cornerA";
const static std::string kCornerB = "cornerB";
const static std::string kOutputGroup = "outputGroup";
const static std::string kOutputAverage = "outputAverage";
const static std::string kOutputChannelMin = "outputChannelMin";
const static std::string kOutputChannelMax = "outputChannelMax";
const static std::string kOutputLuminosityMin = "outputLuminosityMin";
const static std::string kOutputLuminosityMax = "outputLuminosityMax";

const static std::string kChooseOutput = "chooseOutput";
const static std::string kChooseOutputSource = "source";
const static std::string kChooseOutputAverage = "average";
const static std::string kChooseOutputChannelMin = "min values";
const static std::string kChooseOutputChannelMax = "max values";
const static std::string kChooseOutputLuminosityMin = "min luminosity";
const static std::string kChooseOutputLuminosityMax = "max luminosity";

enum EChooseOutput
{
	eChooseOutputSource = 0,
	eChooseOutputAverage,
	eChooseOutputChannelMin,
	eChooseOutputChannelMax,
	eChooseOutputLuminosityMin,
	eChooseOutputLuminosityMax
};

}
}
}

#endif
