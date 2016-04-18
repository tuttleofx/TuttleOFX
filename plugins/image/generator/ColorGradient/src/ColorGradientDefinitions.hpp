#ifndef _TUTTLE_PLUGIN_COLORGRADIENT_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORGRADIENT_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/GeneratorDefinition.hpp>

#include <boost/lexical_cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorGradient
{

static const unsigned int kMaxNbPoints = 50;
static const std::string kPoint = "point";
static const std::string kColor = "color";

static const std::string kGradientType = "type";
static const std::string kGradientType1DLinear = "1dLinear";
static const std::string kGradientType1DRadial = "1dRadial";
static const std::string kGradientType2D = "2d";

enum EGradientType
{
    eGradientType1DLinear = 0,
    eGradientType1DRadial,
    eGradientType2D
};

static const std::string kNbPoints = "nbPoints";

inline std::string getPointParamName(const unsigned int i)
{
    return kPoint + boost::lexical_cast<std::string>(i);
}

inline std::string getColorParamName(const unsigned int i)
{
    return kColor + boost::lexical_cast<std::string>(i);
}
}
}
}

#endif
