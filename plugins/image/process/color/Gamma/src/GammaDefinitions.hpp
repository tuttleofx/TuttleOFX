#ifndef _TUTTLE_PLUGIN_GAMMA_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_GAMMA_DEFINITIONS_HPP_

#include <tuttle/common/utils/global.hpp>


namespace tuttle {
namespace plugin {
namespace gamma {
	const static std::string kMasterValue = "Master";
	const static std::string kRedValue = "Red";
	const static std::string kGreenValue = "Green";
	const static std::string kBlueValue = "Blue";
	const static std::string kAlphaValue = "Alpha";
	const static std::string kGammaType = "GammaType";
	const static std::string kInvert = "Invert";

	const static std::string kGammaGlobal = "Global";
	const static std::string kGammaChannels = "RGBA";
	enum EGammaType {
		eGammaTypeGlobal = 0,
		eGammaTypeChannels = 1,
	};
}
}
}

#endif
