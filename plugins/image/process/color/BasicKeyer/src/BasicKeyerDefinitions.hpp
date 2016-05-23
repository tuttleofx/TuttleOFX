#ifndef _TUTTLE_PLUGIN_BASICKEYER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_BASICKEYER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <boost/lexical_cast.hpp>

namespace tuttle
{
namespace plugin
{
namespace basicKeyer
{

static const std::string kParamMode = "mode";
static const std::string kParamModePosition = "position";
static const std::string kParamModeColor = "color";
enum EParamMode
{
    eParamModePosition = 0,
    eParamModeColor
};

static const std::string kParamNbPoints = "nbPoints";
static const unsigned int kMaxNbPoints = 50;
static const std::string kParamPoint = "point";
static const std::string kParamColor = "color";
static const std::string kParamMargin = "margin";
static const std::string kParamSoftness = "softness";

inline std::string getPointParamName(const unsigned int i)
{
    return kParamPoint + boost::lexical_cast<std::string>(i);
}

inline std::string getColorParamName(const unsigned int i)
{
    return kParamColor + boost::lexical_cast<std::string>(i);
}
}
}
}

#endif
