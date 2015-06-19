#ifndef _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/ReaderDefinition.hpp>

namespace tuttle {
namespace plugin {
namespace raw {
namespace reader {

static const std::string kParamMetadata      = "metadata";
static const std::string kGroupMetadataLabel = "Metadata";

static const std::string kParamManufacturer      = "manufacturer";
static const std::string kParamManufacturerLabel = "Camera Manufacturer";
static const std::string kParamManufacturerHint  = "";

static const std::string kParamModel      = "model";
static const std::string kParamModelLabel = "Camera Model";
static const std::string kParamModelHint  = "";

static const std::string kParamIso      = "iso";
static const std::string kParamIsoLabel = "ISO sensitivity";
static const std::string kParamIsoHint  = "";

static const std::string kParamShutter      = "shutter";
static const std::string kParamShutterLabel = "Shutter speed";
static const std::string kParamShutterHint  = "";

static const std::string kParamAperture      = "aperture";
static const std::string kParamApertureLabel = "Aperture";
static const std::string kParamApertureHint  = "";

static const std::string kParamFocal      = "focal";
static const std::string kParamFocalLabel = "Focal Length";
static const std::string kParamFocalHint  = "";

static const std::string kParamDateOfShooting      = "timestamp";
static const std::string kParamDateOfShootingLabel = "Date of shooting";
static const std::string kParamDateOfShootingHint  = "";

static const std::string kParamGPS      = "gps";
static const std::string kParamGPSLabel = "GPS data";
static const std::string kParamGPSHint  = "";

static const std::string kParamDesc      = "desc";
static const std::string kParamDescLabel = "Image description";
static const std::string kParamDescHint  = "";

static const std::string kParamArtist      = "artist";
static const std::string kParamArtistLabel = "Author of image";
static const std::string kParamArtistHint  = "";



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

static const std::string kParamAutoBright      = "autobright";
static const std::string kParamAutoBrightLabel = "Auto bright";
static const std::string kParamAutoBrightHint  = "Use automatic increase of brightness by histogram.";

static const std::string kParamFBDDNoiseRd = "fbddNoiseReduction";
static const std::string kParamFBDDNoiseRdLabel = "FBDD Noise Reduction";
static const std::string kParamFBDDNoiseRdHint = "Controls FBDD (Fragment Based Drug Discovery) noise reduction before demosaic.";
static const std::string kParamFBDDNoiseRdDisable = "disable";
static const std::string kParamFBDDNoiseRdLight = "light";
static const std::string kParamFBDDNoiseRdFull = "full";

enum EFBDDNoiseRd
{
	eFBDDNoiseRdDisable,
	eFBDDNoiseRdLight,
	eFBDDNoiseRdFull 
};

static const std::string kParamThreshold      = "threshold";
static const std::string kParamThresholdLabel = "Threshold";
static const std::string kParamThresholdHint  = "Parameter for noise reduction through wavelet denoising.";

static const std::string kParamFourColorRgb      = "fourcolorrgb";
static const std::string kParamFourColorRgbLabel = "4 color RGB";
static const std::string kParamFourColorRgbHint  = "Switches on separate interpolations for two green components.";

static const std::string kParamHighlight         = "highlight";
static const std::string kParamHighlightLabel    = "Highlight";
static const std::string kParamHighlightHint     = "Highlight mode\t rebuild : Low  numbers favor whites; high numbers favor colors. 5 as a compromise.";
static const std::string kParamHighlightClip     = "clip";
static const std::string kParamHighlightUnclip   = "unclip";
static const std::string kParamHighlightBlend    = "blend";
static const std::string kParamHighlightRebuild1 = "rebuild1";
static const std::string kParamHighlightRebuild2 = "rebuild2";
static const std::string kParamHighlightRebuild3 = "rebuild3";
static const std::string kParamHighlightRebuild4 = "rebuild4";
static const std::string kParamHighlightRebuild5 = "rebuild5";
static const std::string kParamHighlightRebuild6 = "rebuild6";
static const std::string kParamHighlightRebuild7 = "rebuild7";

enum EHighlight
{
	eHighlightClip = 0,
	eHighlightUnclip,
	eHighlightBlend,
	eHighlightRebuild1,
	eHighlightRebuild2,
	eHighlightRebuild3,
	eHighlightRebuild4,
	eHighlightRebuild5,
	eHighlightRebuild6,
	eHighlightRebuild7
};

static const std::string kParamInterpolation      = "interpolation";
static const std::string kParamInterpolationLabel = "Interpolation";
static const std::string kParamInterpolationHint  = "Interpolation quality";
static const std::string kParamInterpolationLinear      = "linear";
static const std::string kParamInterpolationVng         = "VNG Variable Number of Gradients";
static const std::string kParamInterpolationPpg         = "PPG Patterned Pixel Grouping";
static const std::string kParamInterpolationAhd         = "AHD Adaptive Homogeneity-Directed";
static const std::string kParamInterpolationDcb         = "DCB";
static const std::string kParamInterpolationModifiedAhd = "modifiedAHD";
static const std::string kParamInterpolationAfd         = "AFD";
static const std::string kParamInterpolationVcd         = "VCD";
static const std::string kParamInterpolationMixed       = "MixedVCDModifiedAHD";
static const std::string kParamInterpolationLmmse       = "LMMSE";
static const std::string kParamInterpolationAmaze       = "AMaZE";
static const std::string kParamInterpolationDisable     = "disable";

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
	eInterpolationAmaze,
	eInterpolationDisable
};

static const std::string kParamExposure      = "exposure";
static const std::string kParamExposureLabel = "Exposure correction";
static const std::string kParamExposureHint  = "Exposure shift in linear scale. Usable range from 0.25 (2-stop darken) to 8.0 (3-stop lighter). Default: 1.0 (no exposure shift).";

static const std::string kParamExposurePreserve      = "exposurepreserve";
static const std::string kParamExposurePreserveLabel = "Exposure preserve highlights";
static const std::string kParamExposurePreserveHint  = "Preserve highlights when lighten the image. Usable range from 0.0 (no preservation) to 1.0 (full preservation). 0.0 is the default value.";

static const std::string kParamWhiteBalance      = "whitebalance";
static const std::string kParamWhiteBalanceLabel = "White Balance";
static const std::string kParamWhiteBalanceHint  = "Select white balance.";
static const std::string kParamWhiteBalanceAutoWb   = "auto";
static const std::string kParamWhiteBalanceCameraWb = "camera";
static const std::string kParamWhiteBalanceManualWb = "manual";
static const std::string kParamWhiteBalance2500 = "2500";
static const std::string kParamWhiteBalance2550 = "2550";
static const std::string kParamWhiteBalance2650 = "2650";
static const std::string kParamWhiteBalance2700 = "2700";
static const std::string kParamWhiteBalance2800 = "2800";
static const std::string kParamWhiteBalance2850 = "2850";
static const std::string kParamWhiteBalance2950 = "2950";
static const std::string kParamWhiteBalance3000 = "3000";
static const std::string kParamWhiteBalance3100 = "3100";
static const std::string kParamWhiteBalance3200 = "3200";
static const std::string kParamWhiteBalance3300 = "3300";
static const std::string kParamWhiteBalance3400 = "3400";
static const std::string kParamWhiteBalance3600 = "3600";
static const std::string kParamWhiteBalance3700 = "3700";
static const std::string kParamWhiteBalance3800 = "3800";
static const std::string kParamWhiteBalance4000 = "4000";
static const std::string kParamWhiteBalance4200 = "4200";
static const std::string kParamWhiteBalance4300 = "4300";
static const std::string kParamWhiteBalance4500 = "4500";
static const std::string kParamWhiteBalance4800 = "4800";
static const std::string kParamWhiteBalance5000 = "5000";
static const std::string kParamWhiteBalance5300 = "5300";
static const std::string kParamWhiteBalance5600 = "5600";
static const std::string kParamWhiteBalance5900 = "5900";
static const std::string kParamWhiteBalance6300 = "6300";
static const std::string kParamWhiteBalance6700 = "6700";
static const std::string kParamWhiteBalance7100 = "7100";
static const std::string kParamWhiteBalance7700 = "7700";
static const std::string kParamWhiteBalance8300 = "8300";
static const std::string kParamWhiteBalance9100 = "9100";
static const std::string kParamWhiteBalance10000 = "10000";

enum EWhiteBalance
{
	eAutoWb =0,
	eCameraWb,
	eManualWb,
	e2500,
	e2550,
	e2650,
	e2700,
	e2800,
	e2850,
	e2950,
	e3000,
	e3100,
	e3200,
	e3300,
	e3400,
	e3600,
	e3700,
	e3800,
	e4000,
	e4200,
	e4300,
	e4500,
	e4800,
	e5000,
	e5300,
	e5600,
	e5900,
	e6300,
	e6700,
	e7100,
	e7700,
	e8300,
	e9100,
	e10000
};

static const std::string kParamFiltering     = "filtering";
static const std::string kParamFilteringAuto = "Auto";
static const std::string kParamFilteringNone = "None";

enum EFiltering
{
	eFilteringAuto = 0,
	eFilteringNone
};

static const std::string kParamOutputColor = "outputcolor";
static const std::string kParamOutputColorLabel = "Output Colorspace";
static const std::string kParamOutputColorHint  = "Output colorspace.";
static const std::string kParamOutputColorRaw = "raw";
static const std::string kParamOutputColorsRGB = "sRGB";
static const std::string kParamOutputColorAbode = "Adobe";
static const std::string kParamOutputColorWide = "Wide";
static const std::string kParamOutputColorProPhoto = "ProPhoto";
static const std::string kParamOutputColorXYZ = "XYZ";

enum EOutputColor
{
    eOutputColorRaw,
    eOutputColorsRGB,
    eOutputColorAdobe,
    eOutputColorWide,
    eOutputColorProPhoto,
    eOutputColorXYZ
};

}
}
}
}

#endif
