#ifndef _TUTTLE_PLUGIN_CROP_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_CROP_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

static const bool kSupportTiles              = false;
static const bool kSupportTemporalClipAccess = false;


static const std::string kParamMode = "mode";
static const std::string kParamModeCrop = "crop";
static const std::string kParamModeFillColor = "fill";
//static const std::string kParamModeResize = "resize";
enum EParamMode
{
	eParamModeCrop = 0,
	eParamModeFillColor
//	eParamModeResize
};
static const std::string kParamFillColor = "color";
static const std::string kParamAxis = "axis";
static const std::string kParamAxisXY = "XY";
static const std::string kParamAxisX = "X";
static const std::string kParamAxisY = "Y";
enum EParamAxis
{
	eParamAxisXY = 0,
	eParamAxisX,
	eParamAxisY
};
static const std::string kParamSymmetric = "symmetric";
static const std::string kParamSymmetricNone = "None";
static const std::string kParamSymmetricXY = "XY";
static const std::string kParamSymmetricX = "X";
static const std::string kParamSymmetricY = "Y";
enum EParamSymmetric
{
	eParamSymmetricNone = 0,
	eParamSymmetricXY,
	eParamSymmetricX,
	eParamSymmetricY
};
static const std::string kParamFixedRatio = "fixedRatio";
static const std::string kParamRatio = "ratio";
static const std::string kParamPreset = "preset";
static const std::string kParamPreset_custom = "custom";
static const std::string kParamPreset_1_33 = "1.33 (4/3)";
static const std::string kParamPreset_1_77 = "1.77 (16/9)";
static const std::string kParamPreset_1_85 = "1.85";
static const std::string kParamPreset_2_35 = "2.35 (cinemascope)";
static const std::string kParamPreset_2_40 = "2.40";
enum EParamPreset
{
	eParamPreset_custom = 0,
	eParamPreset_1_33,
	eParamPreset_1_77,
	eParamPreset_1_85,
	eParamPreset_2_35,
	eParamPreset_2_40
};
static const std::string kParamOverlay = "overlay";
static const std::string kParamGroupCropRegion = "cropRegion";
static const std::string kParamXMin = "x1";
static const std::string kParamYMin = "y1";
static const std::string kParamXMax = "x2";
static const std::string kParamYMax = "y2";


}
}
}

#endif
