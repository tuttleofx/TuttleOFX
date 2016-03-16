#ifndef _TUTTLE_PLUGIN_COLORSPACEKEYER_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_COLORSPACEKEYER_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

#include <GL/glew.h>
#include <tuttle/plugin/opengl/gl.h>
#include <tuttle/plugin/interact/interact.hpp>
#include <tuttle/plugin/interact/overlay.hpp>
#include <tuttle/plugin/ofxToGil/point.hpp>
#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/color/hsl.hpp>

#include <boost/scoped_ptr.hpp>

#include <vector>

namespace tuttle
{
namespace plugin
{
namespace colorSpaceKeyer
{

const static std::string kClipColorSelection = "color";
const static std::string kClipSpillSelection = "spill";

// Drawing constant
const static float kOpenGLSceneMaxSize = 200.0;

// Global display (check box)
const static std::string kPointCloudDisplay = "pointCloudDisplay";
const static std::string kPointCloudDisplayLabel = "Display data";

// Discretization display (check box)
const static std::string kBoolDiscretizationDisplay = "discretizationDisplayBool";
const static std::string kBoolDiscretizationDisplayLabel = "Discretize point cloud";

// Discretization display
const static std::string kIntDiscretizationDisplay = "discretizationDisplay";
const static std::string kIntDiscretizationDisplayLabel = "Discretization step";
// static std::size_t discretizationDisplayInt = 10;

// Number of divison (geodesic form)
const static std::string kIntNumberOfDivisonGeodesicForm = "numberOfDivisonGeodesicForm";
const static std::string kIntNumberOfDivisonGeodesicFormLabel = "Precision";

// Reset transformation button
const static std::string kPushButtonResetTransformationParameters = "ResetTransformation";
const static std::string kPushButtonResetTransformationParametersLabel = "Reset transformation";

// Display geodesic form (color)
const static std::string kBoolOnlySelection = "displayColorGeodesicForm";
const static std::string kBoolOnlySelectionLabel = "Display color form";

// Display geodesic form (spill)
const static std::string kBoolDisplaySpillGF = "displaySpillGeodesicForm";
const static std::string kBoolDisplaySpillGFLabel = "Display spill form";

// Group names
const static std::string kGroupDisplay = "groupDisplay";
const static std::string kGroupDisplayLabel = "Display";
const static std::string kGroupSettings = "groupSettings";
const static std::string kGroupSettingsLabel = "Settings";
const static std::string kGroupProcess = "groupProcess";
const static std::string kGroupProcessLabel = "Process";

// Color average selection
const static std::string kColorAverageSelection = "colorAverageSelection";
const static std::string kColorAverageSelectionLabel = "Average selection";

// Color average mode
const static std::string kColorAverageMode = "colorAverageMode";
const static std::string kColorAverageModeLabel = "Average mode";
const static std::string kColorAverageMode1 = "Automatic";
const static std::string kColorAverageMode2 = "Manual";

// Color average selection (computing)
const static std::string kColorAverageComputing = "colorAverageComputing";
const static std::string kColorAverageComputingLabel = "Compute";

// Color selection display
const static std::string kBoolColorSelectionDisplay = "displayColorSelectionDisplay";
const static std::string kBoolColorSelectionDisplayLabel = "Display color selection";

// Spill selection display
const static std::string kBoolSpillSelectionDisplay = "displaySpillSelectionDisplay";
const static std::string kBoolSpillSelectionDisplayLabel = "Display spill selection";

// Scale geodesic form
const static std::string kDoubleScaleGeodesicForm = "scaleGF";
const static std::string kDoubleScaleGeodesicFormLabel = "Scale geodesic form";

// Rotation constants
const static int KMaxDegres = 360; // 360° max for a rotation
const static int kRotationSpeed = 5; // mouse rotation scale
}
}
}

#endif
