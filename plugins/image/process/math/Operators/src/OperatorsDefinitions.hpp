#ifndef _TUTTLE_PLUGIN_OPERATORS_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_OPERATORS_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace operators {



const static std::string kOperatorsOperator  = "operation";

const static std::string kOperatorsPlus      = "plus";
const static std::string kOperatorsMultiply  = "multiply";
const static std::string kOperatorsPow       = "pow";
const static std::string kOperatorsSqrt      = "sqrt";
const static std::string kOperatorsLog       = "log";
const static std::string kOperatorsLn        = "ln";

const static std::string kOperatorsType      = "type";

const static std::string kOperatorsRgba      = "rgba";
const static std::string kOperatorsRgb       = "rgb";
const static std::string kOperatorsChannels  = "split";

const static std::string kMasterValue        = "master";
const static std::string kRedValue           = "red";
const static std::string kGreenValue         = "green";
const static std::string kBlueValue          = "blue";
const static std::string kAlphaValue         = "alpha";

const static std::string kRedChannel         = "red_channel";
const static std::string kGreenChannel       = "green_channel";
const static std::string kBlueChannel        = "blue_channel";
const static std::string kAlphaChannel       = "alpha_channel";

enum EOperatorsOperators
{
	eOperatorsOperatorPlus     = 0,
	eOperatorsOperatorMultiply,
	eOperatorsOperatorPow,
	eOperatorsOperatorSqrt,
	eOperatorsOperatorLog,
	eOperatorsOperatorLn
};

enum EOperatorsType
{
	eOperatorsTypeRgba     = 0,
	eOperatorsTypeRgb      = 1,
	eOperatorsTypeChannels = 2
};


}
}
}

#endif
