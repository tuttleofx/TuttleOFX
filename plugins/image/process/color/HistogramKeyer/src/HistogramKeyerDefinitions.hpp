#ifndef _TUTTLE_PLUGIN_HISTOGRAMKEYER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAMKEYER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>


namespace tuttle {
namespace plugin {
namespace histogramKeyer {

typedef long Number;
const static std::size_t nbCurves = 6;
const static std::size_t nbCurvesRGB = 3;
const static std::size_t nbCurvesHSL = 3;
static std::size_t nbStep = 255;

//Curves params
const static std::string kParamRGBColorSelection = "colorRGBSelection";
const static std::string kParamHSLColorSelection = "colorHSLSelection";
const static std::string kParamColorSelectionRed = "red";
const static std::string kParamColorSelectionGreen = "green";
const static std::string kParamColorSelectionBlue = "blue";
const static std::string kParamColorSelectionHue = "hue";
const static std::string kParamColorSelectionSaturation = "saturation";
const static std::string kParamColorSelectionLightness = "lightness";

//Cuvres groups
const static std::string kGroupRGB = "RGB channels";
const static std::string kGroupRGBLabel = "Red Green Blue";
const static std::string kGroupHSL = "HSL channels";
const static std::string kGroupHSLLabel = "Hue Saturation Lightness";
const static std::string kGroupAdvanced = "Adv";
const static std::string kGroupAdvancedLabel = "Advanced";

//Histogram display settings
const static std::string kHistoDisplayListParamLabel = "Histogram display settings ";
const static std::string kHistoDisplayListParamOpt1 = "adapt to the current channel";
const static std::string kHistoDisplayListParamOpt2 = "adapt with others channels";

//Histogram check box
const static std::string kBoolRed = "Red";
const static std::string kBoolGreen = "Green";
const static std::string kBoolBlue = "Blue";
const static std::string kBoolHue = "Hue";
const static std::string kBoolSaturation = "Saturation";
const static std::string kBoolLightness = "Lightness";

//Clear buttons
const static std::string kButtonCleanRGB = "CleanRGB";
const static std::string kButtonCleanHSL = "CleanHSL";
const static std::string kButtonCleanAll = "CleanAll";

const static std::string kButtonCleanRGBLabel = "Clean";
const static std::string kButtonCleanHSLLabel = "Clean";
const static std::string kButtonCleanAllLabel = "Clean all";

//Refresh overlay button
const static std::string kButtonRefreshOverlay = "RefreshOverlay";
const static std::string kButtonRefreshOverlayLabel = "Refresh overlay";

//nbStep range
const static std::string knbStepRange = "numberOfStep";
const static std::string knbStepRangeLabel = "Number of steps ";

//Hue indicator
const static int kPrecisionHueIndicator = 360;
}
}
}

#endif
