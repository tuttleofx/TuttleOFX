#ifndef _TUTTLE_PLUGIN_HISTOGRAM_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_HISTOGRAM_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <ofxPixels.h>

#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace histogram
{

const static std::size_t nbCurves = 6;
const static std::size_t nbCurvesRGB = 3;
const static std::size_t nbCurvesHSL = 3;
const static std::size_t curveFromSelection = 40;

// Curves params
const static std::string kParamColorSelectionRed = "red";
const static std::string kParamColorSelectionGreen = "green";
const static std::string kParamColorSelectionBlue = "blue";
const static std::string kParamColorSelectionHue = "hue";
const static std::string kParamColorSelectionSaturation = "saturation";
const static std::string kParamColorSelectionLightness = "lightness";

// Groups
const static std::string kGroupRGB = "RGB channels";
const static std::string kGroupRGBLabel = "Red Green Blue";
const static std::string kGroupHSL = "HSL channels";
const static std::string kGroupHSLLabel = "Hue Saturation Lightness";
const static std::string kGroupAdvanced = "Adv";
const static std::string kGroupSelection = "Selec";
const static std::string kGroupSelectionLabel = "Selection settings";
const static std::string kGroupHistogramOverlay = "HOverlay";
const static std::string kGroupHistogramOverlayLabel = "Histogram overlay";

// Histogram display settings
const static std::string kHistoDisplayListParamLabel = "Histogram adjustment ";
const static std::string kHistoDisplayListParamOpt1 = "by channel";
const static std::string kHistoDisplayListParamOpt2 = "global";

// Histogram check box
const static std::string kBoolRed = "Red";
const static std::string kBoolGreen = "Green";
const static std::string kBoolBlue = "Blue";
const static std::string kBoolHue = "Hue";
const static std::string kBoolSaturation = "Saturation";
const static std::string kBoolLightness = "Lightness";

// Display selection check box
const static std::string kBoolSelection = "Display selection";

// Refresh overlay button
const static std::string kButtonResetSelection = "Reset Selection";
const static std::string kButtonResetSelectionLabel = "Reset selection";

// Refresh overlay button
const static std::string kButtonRefreshOverlay = "RefreshOverlay";
const static std::string kButtonRefreshOverlayLabel = "Refresh overlay";

// nbStep range
const static std::string knbStepRange = "numberOfStep";
const static std::string knbStepRangeLabel = "Number of steps ";

// selection multiplier
const static std::string kselectionMultiplier = "SelectionMultiplier";
const static std::string kselectionMultiplierLabel = "Selection multiplier ";

// Hue indicator
const static int kPrecisionHueIndicator = 360;

// Selection mode
const static std::string kSelectionModeListParamLabel = "Selection mode";
const static std::string kSelectionModeListParamOpt1 = "unique";
const static std::string kSelectionModeListParamOpt2 = "additive";
const static std::string kSelectionModeListParamOpt3 = "subtractive";

// Global display check box
const static std::string kGlobalDisplay = "global display";

typedef OfxRGBColourF Color;

// Histograms color (struct)
struct HistogramColor
{
    OfxRGBColourF _colorBorder; // border color
    OfxRGBColourF _colorFill;   // filling color
};

// Histograms colors
static const OfxRGBColourF redFilling = {1.0f, 0.2f, 0.2f};
static const OfxRGBColourF greenFilling = {0.2f, 1.0f, 0.2f};
static const OfxRGBColourF blueFilling = {0.2f, 0.2f, 1.0f};

static const OfxRGBColourF redBorder = {0.8f, 0.0f, 0.0f};
static const OfxRGBColourF greenBorder = {0.0f, 0.8f, 0.0f};
static const OfxRGBColourF blueBorder = {0.0f, 0.0f, 0.8f};

static const HistogramColor redHisto = {redBorder, redFilling};
static const HistogramColor greenHisto = {greenBorder, greenFilling};
static const HistogramColor blueHisto = {blueBorder, blueFilling};

// Average bar color
static const OfxRGBColourF averageFilling = {0.2f, 0.2f, 1.0f};
static const OfxRGBColourF averageBorder = {1.0f, 0.84f, 0.0f};
static const HistogramColor averageHisto = {averageBorder, averageFilling};

static const std::string kMultiplierLabel = "Multiplier";
// Channel multiplier (RGB)
static const std::string kMultiplierRed = "Red multiplier";
static const std::string kMultiplierGreen = "Green multiplier";
static const std::string kMultiplierBlue = "Blue multiplier";
// Channel mutiliplier(HSL)
static const std::string kMultiplierHue = "Hue multiplier";
static const std::string kMultiplierSaturation = "Saturation multiplier";
static const std::string kMultiplierLightness = "Lightness multiplier";

///@todo: remove when Nuke overlay option works
// Translation (separate HSL from RGB)
const static float kTranslationHSL = 200.0f;
// Translation (separate HSL from RGB)
const static float kTranslationRGB = 200.0f;
}
}
}

#endif
