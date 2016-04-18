#ifndef _TUTTLE_PLUGIN_PINNING_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_PINNING_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/SamplerDefinition.hpp>

namespace tuttle
{
namespace plugin
{
namespace pinning
{

static const std::string kParamMethod = "method";
static const std::string kParamMethodAffine = "affine";
static const std::string kParamMethodPerspective = "perspective";
static const std::string kParamMethodBilinear = "bilinear";
enum EParamMethod
{
    eParamMethodAffine = 0,
    eParamMethodPerspective,
    eParamMethodBilinear
};

static const std::string kParamSetToCornersIn = "setToCornersIn";
static const std::string kParamSetToCornersOut = "setToCornersOut";
static const std::string kParamOverlay = "overlay";
static const std::string kParamInverse = "inverse";

static const std::string kParamGroupCentre = "groupCentre";
static const std::string kParamPointCentre = "pCentre";
static const std::string kParamOverlayCentre = "overlayCentre";
static const std::string kParamOverlayCentreColor = "overlayCentreColor";

static const std::string kParamGroupIn = "groupIn";
static const std::string kParamPointIn = "pIn";
static const std::string kParamOverlayIn = "overlayIn";
static const std::string kParamOverlayInColor = "overlayInColor";

static const std::string kParamGroupOut = "groupOut";
static const std::string kParamPointOut = "pOut";
static const std::string kParamOverlayOut = "overlayOut";
static const std::string kParamOverlayOutColor = "overlayOutColor";

static const std::string kParamGroupPerspMatrix = "groupPerspMatrix";
static const std::string kParamPerspMatrixRow = "perpMatrix";
static const std::string kParamGroupBilinearMatrix = "groupBilinearMatrix";
static const std::string kParamBilinearMatrixRow = "bilinearMatrix";

static const std::string kParamManipulatorMode = "manipulatorMode";
static const std::string kParamManipulatorModeTranslate = "translate";
static const std::string kParamManipulatorModeRotate = "rotate";
static const std::string kParamManipulatorModeScale = "scale";
enum EParamManipulatorMode
{
    eParamManipulatorModeTranslate = 0,
    eParamManipulatorModeRotate,
    eParamManipulatorModeScale,
};
}
}
}

#endif
