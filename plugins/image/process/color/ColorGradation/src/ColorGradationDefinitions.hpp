#ifndef _TUTTLE_PLUGIN_COLORGRADATION_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORGRADATION_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace colorGradation {

const std::string kParamIn  = "in";
const std::string kParamOut = "out";

const std::string kParamGradation_linear = "Linear";
const std::string kParamGradation_sRGB   = "sRGB";
const std::string kParamGradation_cineon = "Cineon";
const std::string kParamGradation_gamma = "Gamma";
const std::string kParamGradation_panalog = "Panalog";
const std::string kParamGradation_REDLog = "REDLog";
const std::string kParamGradation_ViperLog = "ViperLog";
const std::string kParamGradation_REDSpace = "REDSpace";
const std::string kParamGradation_AlexaLogC = "AlexaLogC";

//const std::string kParamGradation_rec709 = "rec709";
//const std::string kParamGradation_rec601 = "rec601";

enum EParamGradation
{
	eParamGradation_linear = 0,
	eParamGradation_sRGB,
	eParamGradation_cineon,
	eParamGradation_gamma,
	eParamGradation_panalog,
	eParamGradation_REDLog,
	eParamGradation_ViperLog,
	eParamGradation_REDSpace,
	eParamGradation_AlexaLogC
	//	eParamGradation_rec709,
	//	eParamGradation_rec601
};

const std::string kParamProcessAlpha = "processAlpha";
const std::string kParamInvert = "invert";

}
}
}

#endif
