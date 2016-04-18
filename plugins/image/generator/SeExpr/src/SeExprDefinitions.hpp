#ifndef _TUTTLE_PLUGIN_SEEXPR_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_SEEXPR_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/Definition.hpp>

namespace tuttle
{
namespace plugin
{
namespace seExpr
{

static const std::string kParamTextureOffset("textureOffset");
static const std::string kParamSeExprCode("code");

static const std::string kParamChooseInput("input");
static const std::string kParamChooseInputCode("code");
static const std::string kParamChooseInputFile("file");

enum EParamChooseInput
{
    eParamChooseInputCode = 0,
    eParamChooseInputFile
};
}
}
}

#endif
