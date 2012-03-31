#ifndef _TUTTLE_PLUGIN_NLMDENOISERDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_NLMDENOISERDEFINITIONS_HPP_

#include <string>

namespace tuttle {
namespace plugin {
namespace nlmDenoiser {

const std::string kRedStrengthLabel( "Red strength" );
const std::string kRedStrength( "redStrength" );
const std::string kGreenStrengthLabel( "Green strength" );
const std::string kGreenStrength( "greenStrength" );
const std::string kBlueStrengthLabel( "Blue strength" );
const std::string kBlueStrength( "blueStrength" );
const std::string kRedGrainSizeLabel( "Red grain size" );
const std::string kRedGrainSize( "redGrainSize" );
const std::string kGreenGrainSizeLabel( "Green grain size" );
const std::string kGreenGrainSize( "greenGrainSize" );
const std::string kBlueGrainSizeLabel( "Blue grain size" );
const std::string kBlueGrainSize( "blueGrainSize" );
const std::string kPatchRadiusLabel( "Patch radius" );
const std::string kPatchRadius( "patchRadius" );
const std::string kRegionRadiusLabel( "Region radius" );
const std::string kRegionRadius( "regionRadius" );
const std::string kDepthLabel( "Depth" );
const std::string kDepth( "depth" );
const std::string kOptimizationLabel( "Optimization activated" );
const std::string kOptimization( "optimization" );
const std::string kPreBlurring( "preBlurring" );
const std::string kPreBlurringLabel( "Pre-blurring for patch research" );

const bool kSupportTiles = true;
const int kDefaultPatchSizeValue = 2;
const int kDefaultBandwidthValueR = 3;
const int kDefaultBandwidthValueG = 4;
const int kDefaultBandwidthValueB = 10;
const int kDefaultRegionValue = 15;
const int kDefaultDepth = 1;

}
}
}

#endif //NLMDENOISER_HPP
