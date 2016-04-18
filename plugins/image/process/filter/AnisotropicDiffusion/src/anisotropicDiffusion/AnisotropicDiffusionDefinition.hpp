#ifndef _TUTTLE_PLUGIN_ANISOTROPICDIFFUSIONDEFINITION_HPP_
#define _TUTTLE_PLUGIN_ANISOTROPICDIFFUSIONDEFINITION_HPP_

#include <string>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace diffusion
{

static const std::string kClipInputTensors("inputTensors");

static const std::string kParamGroupPDEAlgorithm("groupPDEAlgorithm");

static const std::string kParamFastApproximation("fastApproximation");
static const std::string kParamAmplitude("amplitude");
}
}
}
}

#endif
