#ifndef _TUTTLE_PLUGIN_COLORDISTRIBUTION_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORDISTRIBUTION_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace colorDistribution {

const std::string kParamIn  = "in";
const std::string kParamOut = "out";

const std::string kParamDistribution_linear = "Linear";
const std::string kParamDistribution_sRGB   = "sRGB";
const std::string kParamDistribution_cineon = "Cineon";
const std::string kParamDistribution_gamma = "Gamma";
const std::string kParamDistribution_panalog = "Panalog";
const std::string kParamDistribution_REDLog = "REDLog";
const std::string kParamDistribution_ViperLog = "ViperLog";
const std::string kParamDistribution_REDSpace = "REDSpace";
const std::string kParamDistribution_AlexaLogC = "AlexaLogC";

//const std::string kParamDistribution_rec709 = "rec709";
//const std::string kParamDistribution_rec601 = "rec601";

enum EParamDistribution
{
	eParamDistribution_linear = 0,
	eParamDistribution_sRGB,
	eParamDistribution_cineon,
	eParamDistribution_gamma,
	eParamDistribution_panalog,
	eParamDistribution_REDLog,
	eParamDistribution_ViperLog,
	eParamDistribution_REDSpace,
	eParamDistribution_AlexaLogC
	//	eParamDistribution_rec709,
	//	eParamDistribution_rec601
};

const std::string kParamProcessAlpha = "processAlpha";
const std::string kParamInvert = "invert";

}
}
}

#endif
