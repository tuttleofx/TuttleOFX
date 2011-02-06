#ifndef CROP_DEFINITIONS_HPP
#define CROP_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

static const bool kSupportTiles              = false;
static const bool kSupportTemporalClipAccess = false;

static const std::string kParamFillMode         = "fillMode";
static const std::string kParamUp               = "upBand";
static const std::string kParamDown             = "downBand";
static const std::string kParamLeft             = "leftBand";
static const std::string kParamRight            = "rightBand";
static const std::string kParamPresets          = "presets";
static const std::string kParamDisplayRect      = "displayOverlay";
static const std::string kParamAnamorphic       = "anamorphic";

enum EParamPreset
{
	eParamPreset_1_33 = 0,
	eParamPreset_1_77,
	eParamPreset_1_85,
	eParamPreset_2_35,
	eParamPreset_2_40
};

}
}
}

#endif
