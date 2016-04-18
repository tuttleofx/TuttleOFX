#ifndef _TUTTLE_PLUGIN_CTL_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CTL_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/Definition.hpp>

namespace tuttle
{
namespace plugin
{
namespace ctl
{

static const std::string kParamChooseInput("input");
static const std::string kParamChooseInputCode("code");
static const std::string kParamChooseInputFile("file");

static const std::string kParamChooseInputCodeUpdate("update");

enum EParamChooseInput
{
    eParamChooseInputCode = 0,
    eParamChooseInputFile,
};

static const std::string kParamCTLCode("code");
}
}
}

#endif
