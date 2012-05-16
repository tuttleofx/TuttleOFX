#ifndef _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

static const std::string kParamGreyboxPoint      = "greyboxpoint";
static const std::string kParamGreyboxPointLabel = "GreyBox Point";
static const std::string kParamGreyboxPointHint  = "Coordinates of the rectangle that is used to calculate the white balance. X and Y are coordinates of the left-top rectangle corner.";

static const std::string kParamGreyboxSize      = "greyboxsize";
static const std::string kParamGreyboxSizeLabel = "GreyBox Size";
static const std::string kParamGreyboxSizeHint  = "Coordinates of the rectangle that is used to calculate the white balance. w and h are the rectangle's width and height, respectively.";

static const std::string kParamRedAbber      = "redabberation";
static const std::string kParamRedAbberLabel = "Red Abberation Chromatic";
static const std::string kParamRedAbberHint  = "Correction of chromatic aberrations.";

static const std::string kParamGreenAbber      = "greenabberation";
static const std::string kParamGreenAbberLabel = "Green Abberation Chromatic";
static const std::string kParamGreenAbberHint  = "Correction of chromatic aberrations.";

static const std::string kParamGammaPower      = "gammaPower";
static const std::string kParamGammaPowerLabel = "Gamma Power";
static const std::string kParamGammaPowerHint  = "Inverted gamma value";

static const std::string kParamGammaToe      = "gammatoe";
static const std::string kParamGammaToeLabel = "Gamma Toe";
static const std::string kParamGammaToeHint  = "Slope for linear part (so called toe slope). Set to zero for simple power curve.";




static const std::string kParamBright      = "bright";
static const std::string kParamBrightLabel = "Bright";
static const std::string kParamBrightHint  = "";

static const std::string kParamInterpolation      = "interpolation";
static const std::string kParamInterpolationLabel = "Interpolation";
static const std::string kParamInterpolationHint  = "Interpolation quality";

static const std::string kParamInterpolationLinear      = "linear";
static const std::string kParamInterpolationVng         = "VNG";
static const std::string kParamInterpolationPpg         = "PPG";
static const std::string kParamInterpolationAhd         = "AHD";
static const std::string kParamInterpolationDcb         = "DCB";
static const std::string kParamInterpolationModifiedAhd = "modifiedAHD";
static const std::string kParamInterpolationAfd         = "AFD";
static const std::string kParamInterpolationVcd         = "VCD";
static const std::string kParamInterpolationMixed       = "MixedVCDModifiedAHD";
static const std::string kParamInterpolationLmmse       = "LMMSE";
static const std::string kParamInterpolationAmaze       = "AMaZE";

enum EInterpolation
{
	eInterpolationLinear = 0,
	eInterpolationVng,
	eInterpolationPpg,
	eInterpolationAhd,
	eInterpolationDcb,
	eInterpolationModifiedAhd,
	eInterpolationAfd,
	eInterpolationVcd,
	eInterpolationMixed,
	eInterpolationLmmse,
	eInterpolationAmaze
};

static const std::string kParamFiltering     = "filtering";
static const std::string kParamFilteringAuto = "Auto";
static const std::string kParamFilteringNone = "None";

enum EFiltering
{
	eFilteringAuto = 0,
	eFilteringNone
};

}
}
}
}

#endif
