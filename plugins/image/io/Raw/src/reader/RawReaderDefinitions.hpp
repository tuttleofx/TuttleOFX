#ifndef _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_RAWREADERDEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/ioplugin/context/ReaderDefinition.hpp>

namespace tuttle
{
namespace plugin
{
namespace raw
{
namespace reader
{

static const std::string kParamMetadata = "metadata";
static const std::string kGroupMetadataLabel = "Metadata";

static const std::string kParamManufacturer = "manufacturer";
static const std::string kParamManufacturerLabel = "Camera Manufacturer";
static const std::string kParamManufacturerHint = "";

static const std::string kParamModel = "model";
static const std::string kParamModelLabel = "Camera Model";
static const std::string kParamModelHint = "";

static const std::string kParamIso = "iso";
static const std::string kParamIsoLabel = "ISO sensitivity";
static const std::string kParamIsoHint = "";

static const std::string kParamShutter = "shutter";
static const std::string kParamShutterLabel = "Shutter speed";
static const std::string kParamShutterHint = "In seconds";

static const std::string kParamAperture = "aperture";
static const std::string kParamApertureLabel = "Aperture";
static const std::string kParamApertureHint = "f/value";

static const std::string kParamFocal = "focal";
static const std::string kParamFocalLabel = "Focal Length";
static const std::string kParamFocalHint = "In millimeters";

static const std::string kParamWBR = "whiteBalanceR";
static const std::string kParamWBRLabel = "White Balance - R Coeff";
static const std::string kParamWBG = "whiteBalanceG";
static const std::string kParamWBGLabel = "White Balance - G Coeff";
static const std::string kParamWBB = "whiteBalanceB";
static const std::string kParamWBBLabel = "White Balance - B Coeff";
static const std::string kParamWBHint = "White balance coefficients (as shot). Either read from file or calculated.";

static const std::string kParamDateOfShooting = "timestamp";
static const std::string kParamDateOfShootingLabel = "Date of shooting";
static const std::string kParamDateOfShootingHint = "";

static const std::string kParamGPS = "gps";
static const std::string kParamGPSLabel = "GPS data";
static const std::string kParamGPSHint = "";

static const std::string kParamDesc = "desc";
static const std::string kParamDescLabel = "Image description";
static const std::string kParamDescHint = "";

static const std::string kParamArtist = "artist";
static const std::string kParamArtistLabel = "Author of image";
static const std::string kParamArtistHint = "";

static const std::string kParamGreyboxPoint = "greyBoxPoint";
static const std::string kParamGreyboxPointLabel = "GreyBox Point";
static const std::string kParamGreyboxPointHint = "Coordinates of the rectangle that is used to calculate the white "
                                                  "balance. X and Y are coordinates of the left-top rectangle corner.";

static const std::string kParamGreyboxSize = "greyBoxSize";
static const std::string kParamGreyboxSizeLabel = "GreyBox Size";
static const std::string kParamGreyboxSizeHint = "Coordinates of the rectangle that is used to calculate the white balance. "
                                                 "w and h are the rectangle's width and height, respectively.";

static const std::string kParamRedAbber = "redAbberation";
static const std::string kParamRedAbberLabel = "Red Abberation Chromatic";
static const std::string kParamRedAbberHint = "Correction of chromatic aberrations.";

static const std::string kParamGreenAbber = "greenAbberation";
static const std::string kParamGreenAbberLabel = "Green Abberation Chromatic";
static const std::string kParamGreenAbberHint = "Correction of chromatic aberrations.";

static const std::string kParamGammaPower = "gammaPower";
static const std::string kParamGammaPowerLabel = "Gamma Power";
static const std::string kParamGammaPowerHint = "Inverted gamma value";

static const std::string kParamGammaToe = "gammaToe";
static const std::string kParamGammaToeLabel = "Gamma Toe";
static const std::string kParamGammaToeHint =
    "Slope for linear part (so called toe slope). Set to zero for simple power curve.";

static const std::string kParamBright = "bright";
static const std::string kParamBrightLabel = "Bright";
static const std::string kParamBrightHint = "";

static const std::string kParamAutoBright = "autoBright";
static const std::string kParamAutoBrightLabel = "Auto bright";
static const std::string kParamAutoBrightHint = "Use automatic increase of brightness by histogram.";

static const std::string kParamFBDDNoiseRd = "fbddNoiseReduction";
static const std::string kParamFBDDNoiseRdLabel = "FBDD Noise Reduction";
static const std::string kParamFBDDNoiseRdHint =
    "Controls FBDD (Fragment Based Drug Discovery) noise reduction before demosaic.";
static const std::string kParamFBDDNoiseRdDisable = "disable";
static const std::string kParamFBDDNoiseRdLight = "light";
static const std::string kParamFBDDNoiseRdFull = "full";

enum EFBDDNoiseRd
{
    eFBDDNoiseRdDisable,
    eFBDDNoiseRdLight,
    eFBDDNoiseRdFull
};

static const std::string kParamThreshold = "threshold";
static const std::string kParamThresholdLabel = "Threshold";
static const std::string kParamThresholdHint = "Parameter for noise reduction through wavelet denoising.";

static const std::string kParamFourColorRgb = "fourColorRGB";
static const std::string kParamFourColorRgbLabel = "4 color RGB";
static const std::string kParamFourColorRgbHint = "Switches on separate interpolations for two green components.";

static const std::string kParamHighlight = "highlight";
static const std::string kParamHighlightLabel = "Highlight";
static const std::string kParamHighlightHint =
    "Highlight mode\t rebuild : Low  numbers favor whites; high numbers favor colors. 5 as a compromise.";
static const std::string kParamHighlightClip = "clip";
static const std::string kParamHighlightUnclip = "unclip";
static const std::string kParamHighlightBlend = "blend";
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

static const std::string kParamInterpolation = "interpolation";
static const std::string kParamInterpolationLabel = "Interpolation";
static const std::string kParamInterpolationHint = "Interpolation quality";
static const std::string kParamInterpolationLinear = "linear";
static const std::string kParamInterpolationVng = "VNG Variable Number of Gradients";
static const std::string kParamInterpolationPpg = "PPG Patterned Pixel Grouping";
static const std::string kParamInterpolationAhd = "AHD Adaptive Homogeneity-Directed";
static const std::string kParamInterpolationDcb = "DCB";
static const std::string kParamInterpolationModifiedAhd = "modifiedAHD";
static const std::string kParamInterpolationAfd = "AFD";
static const std::string kParamInterpolationVcd = "VCD";
static const std::string kParamInterpolationMixed = "MixedVCDModifiedAHD";
static const std::string kParamInterpolationLmmse = "LMMSE";
static const std::string kParamInterpolationAmaze = "AMaZE";
static const std::string kParamInterpolationDisable = "disable";

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

static const std::string kParamExposure = "exposure";
static const std::string kParamExposureLabel = "Exposure correction";
static const std::string kParamExposureHint = "Exposure shift in linear scale. Usable range from 0.25 (2-stop darken) to "
                                              "8.0 (3-stop lighter). Default: 1.0 (no exposure shift).";

static const std::string kParamExposurePreserve = "exposurePreserve";
static const std::string kParamExposurePreserveLabel = "Exposure preserve highlights";
static const std::string kParamExposurePreserveHint = "Preserve highlights when lighten the image. Usable range from 0.0 "
                                                      "(no preservation) to 1.0 (full preservation). 0.0 is the default "
                                                      "value.";

static const std::string kParamWhiteBalance = "whiteBalance";
static const std::string kParamWhiteBalanceLabel = "White Balance";
static const std::string kParamWhiteBalanceHint = "Select white balance mode.";
static const std::string kParamWhiteBalanceAutoWb = "auto";
static const std::string kParamWhiteBalanceCameraWb = "camera";
static const std::string kParamWhiteBalanceManualWb = "manual";
enum EWhiteBalance
{
    eAutoWb = 0,
    eCameraWb,
    eManualWb,
};

static const std::string kParamManualWBKelvin = "manualWhiteBalance";
static const std::string kParamManualWBKelvinLabel = "Manual White Balance in Kelvin";
static const std::string kParamManualWBKelvinHint = "Use your own white balance: \n\
1,700 K	Match flame, low pressure sodium lamps (LPS/SOX) \n\
1,850 K	Candle flame, sunset/sunrise \n\
2,700–3,300 K	Incandescent lamps \n\
3,000 K	Soft (or Warm) White compact fluorescent lamps \n\
3,200 K	Studio lamps, photofloods, etc. \n\
3,350 K	Studio 'CP' light \n\
4,100–4,150 K	Moonlight[2] \n\
5,000 K	Horizon daylight \n\
5,000 K	Tubular fluorescent lamps or cool white/daylight compact fluorescent lamps (CFL) \n\
5,500–6,000 K	Vertical daylight, electronic flash \n\
6,200 K	Xenon short-arc lamp[3] \n\
6,500 K	Daylight, overcast \n\
6,500–10,500 K	LCD or CRT screen \n\
15,000–27,000 K	Clear blue poleward sky";

static const std::string kParamFiltering = "filtering";
static const std::string kParamFilteringAuto = "Auto";
static const std::string kParamFilteringNone = "None";

enum EFiltering
{
    eFilteringAuto = 0,
    eFilteringNone
};

static const std::string kParamOutputColor = "outputColor";
static const std::string kParamOutputColorLabel = "Output Colorspace";
static const std::string kParamOutputColorHint = "Output colorspace.";
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
