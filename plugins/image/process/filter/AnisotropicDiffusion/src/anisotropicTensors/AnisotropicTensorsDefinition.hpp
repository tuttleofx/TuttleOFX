#ifndef _TUTTLE_PLUGIN_ANISOTROPICTENSORSDEFINITION_HPP_
#define _TUTTLE_PLUGIN_ANISOTROPICTENSORSDEFINITION_HPP_

#include <string>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

static const std::string kParamDisplayStructureTensors("displayStructureTensors");
static const std::string kParamDisplayEffectMargin("displayEffectMargin");

static const std::string kParamGroupStructureTensors("groupStructureTensors");
static const std::string kParamAlpha("alpha");
static const std::string kParamSigma("sigma");
static const std::string kParamGeometryFactor("geometryFactor");
static const std::string kParamThresholdingQuantization("thresholdingQuantization");

static const std::string kParamGroupDiffuseTensors("groupDiffuseTensors");

static const std::string kParamEdgeDetectAlgo("edgeDetectAlgorithm");
static const std::string kParamSharpness("sharpness");
static const std::string kParamAnisotropy("anisotropy");
}
}
}
}

#endif
