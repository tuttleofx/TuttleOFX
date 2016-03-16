#ifndef _TUTTLE_PLUGIN_NLMDENOISERDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_NLMDENOISERDEFINITIONS_HPP_

#include <string>

namespace tuttle
{
namespace plugin
{
namespace nlmDenoiser
{

const std::string kParamRedStrengthLabel("Red strength");
const std::string kParamRedStrength("redStrength");
const std::string kParamGreenStrengthLabel("Green strength");
const std::string kParamGreenStrength("greenStrength");
const std::string kParamBlueStrengthLabel("Blue strength");
const std::string kParamBlueStrength("blueStrength");
const std::string kParamRedGrainSizeLabel("Red grain size");
const std::string kParamRedGrainSize("redGrainSize");
const std::string kParamGreenGrainSizeLabel("Green grain size");
const std::string kParamGreenGrainSize("greenGrainSize");
const std::string kParamBlueGrainSizeLabel("Blue grain size");
const std::string kParamBlueGrainSize("blueGrainSize");
const std::string kParamPatchRadiusLabel("Patch radius");
const std::string kParamPatchRadius("patchRadius");
const std::string kParamRegionRadiusLabel("Region radius");
const std::string kParamRegionRadius("regionRadius");
const std::string kParamDepthLabel("Depth");
const std::string kParamDepth("depth");
const std::string kParamOptimizationLabel("Optimization activated");
const std::string kParamOptimization("optimization");
const std::string kParamPreBlurring("preBlurring");
const std::string kParamPreBlurringLabel("Pre-blurring for patch research");

const int kParamDefaultPatchSizeValue = 2;
const int kParamDefaultBandwidthValueR = 3;
const int kParamDefaultBandwidthValueG = 4;
const int kParamDefaultBandwidthValueB = 10;
const int kParamDefaultRegionValue = 4;
const int kParamDefaultDepth = 1;
}
}
}

#endif // NLMDENOISER_HPP
