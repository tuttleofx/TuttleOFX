#ifndef CROP_DEFINITIONS_HPP
#define CROP_DEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

static const bool kSupportTiles              = false;
static const bool kSupportTemporalClipAccess = false;

static const std::string kParamFillModeLabel    = "Fill bands with black";
static const std::string kParamFillMode         = "fillMode";
static const std::string kParamUpLabel          = "Up";
static const std::string kParamUp               = "upBand";
static const std::string kParamDownLabel        = "Down";
static const std::string kParamDown             = "downBand";
static const std::string kParamLeftLabel        = "Left";
static const std::string kParamLeft             = "leftBand";
static const std::string kParamRightLabel       = "Right";
static const std::string kParamRight            = "rightBand";
static const std::string kParamPresetsLabel     = "Pre-defined formats";
static const std::string kParamPresets          = "presets";
static const std::string kParamDisplayRectLabel = "Display overlay rectangle";
static const std::string kParamDisplayRect      = "displayOverlay";
static const std::string kParamAnamorphicLabel  = "Anamorphic";
static const std::string kParamAnamorphic       = "anamorphic";

static const int k1_1_33 = 0;
static const int k1_1_77 = 1;
static const int k1_1_85 = 2;
static const int k1_2_35 = 3;
static const int k1_2_40 = 4;

static const std::string kCropHelpButton = "Help";
static const std::string kCropHelpString = "<b>Crop</b> plugin is used to crop an image.  <br />";

}
}
}

#endif
