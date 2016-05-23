#ifndef _TUTTLE_PLUGIN_FADE_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_FADE_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace fade
{

static const std::string kParamColor = "color";

static const std::string kParamMode = "mode";
static const std::string kParamModeFromColor = "fromColor";
static const std::string kParamModeToColor = "toColor";
enum EParamMode
{
    eParamModeFromColor = 0,
    eParamModeToColor
};

static const std::string kParamRod = "rod";
static const std::string kParamRodIntersect = "intersection";
static const std::string kParamRodUnion = "union";
static const std::string kParamRodA = "A";
static const std::string kParamRodB = "B";

enum EParamRod
{
    eParamRodIntersect = 0,
    eParamRodUnion,
    eParamRodA,
    eParamRodB
};
}
}
}

#endif
