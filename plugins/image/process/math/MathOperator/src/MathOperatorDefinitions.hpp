#ifndef _TUTTLE_PLUGIN_MATHOPERATOR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_MATHOPERATOR_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace mathOperator
{

const static std::string kMathOperatorOperator = "operation";

const static std::string kMathOperatorPlus = "plus";
const static std::string kMathOperatorMultiply = "multiply";
const static std::string kMathOperatorPow = "pow";
const static std::string kMathOperatorSqrt = "sqrt";
const static std::string kMathOperatorLog = "log";
const static std::string kMathOperatorLn = "ln";

const static std::string kMathOperatorType = "type";

const static std::string kMathOperatorRgba = "rgba";
const static std::string kMathOperatorRgb = "rgb";
const static std::string kMathOperatorChannels = "split";

const static std::string kMasterValue = "master";
const static std::string kRedValue = "red";
const static std::string kGreenValue = "green";
const static std::string kBlueValue = "blue";
const static std::string kAlphaValue = "alpha";

const static std::string kRedChannel = "red_channel";
const static std::string kGreenChannel = "green_channel";
const static std::string kBlueChannel = "blue_channel";
const static std::string kAlphaChannel = "alpha_channel";

enum EMathOperatorMathOperator
{
    eMathOperatorOperatorPlus = 0,
    eMathOperatorOperatorMultiply,
    eMathOperatorOperatorPow,
    eMathOperatorOperatorSqrt,
    eMathOperatorOperatorLog,
    eMathOperatorOperatorLn
};

enum EMathOperatorType
{
    eMathOperatorTypeRgba = 0,
    eMathOperatorTypeRgb = 1,
    eMathOperatorTypeChannels = 2
};
}
}
}

#endif
