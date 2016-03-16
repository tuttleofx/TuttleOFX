#ifndef _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_
#define _TUTTLE_PLUGIN_WARP_DEFINITIONS_HPP_

#include <tuttle/plugin/global.hpp>

namespace tuttle
{
namespace plugin
{
namespace warp
{

static const std::size_t kMaxNbPoints = 20;
// static const std::size_t kPasBezier = 0.2;
static const float lineWidth = 0.5;
static const float pointWidth = 3.0;
static const int seuil = 15;

static const float positionOrigine = -200.0;

// static const int nbCoeffBezier = 50;

// static const std::string kClipSourceA = "A";
static const std::string kClipSourceB = "B";

static const std::string kParamMethod = "method";
static const std::string kParamMethodCreation = "creation";
static const std::string kParamMethodDelete = "delete";
static const std::string kParamMethodMove = "move";
enum EParamMethod
{
    eParamMethodCreation = 0,
    eParamMethodDelete,
    eParamMethodMove,

};

static const std::string kParamOverlay = "overlay";
static const std::string kParamInverse = "inverse";
static const std::string kParamNbPoints = "nbPoints";
static const std::string kParamTransition = "transition";
static const std::string kParamReset = "reset";
static const std::string kParamNextCurve = "nextCurve";
static const std::string kParamSetKey = "setKey";

static const std::string kParamGroupSettings = "settings";
static const std::string kParamNbPointsBezier = "Points Bezier";
static const std::string kParamRigiditeTPS = "Rigidite TPS";

static const std::string kParamGroupIn = "groupIn";
static const std::string kParamPointIn = "pIn";
static const std::string kParamOverlayIn = "overlayIn";
static const std::string kParamOverlayInColor = "overlayInColor";

static const std::string kParamGroupOut = "groupOut";
static const std::string kParamPointOut = "pOut";
static const std::string kParamOverlayOut = "overlayOut";
static const std::string kParamOverlayOutColor = "overlayOutColor";

static const std::string kParamGroupTgtIn = "groupTgtIn";
static const std::string kParamPointTgtIn = "pTangenteIn";
static const std::string kParamOverlayTgtIn = "overlayTgtIn";
static const std::string kParamOverlayTgtInColor = "overlayTangentInColor";

static const std::string kParamGroupTgtOut = "groupTgtOut";
static const std::string kParamPointTgtOut = "pTangenteOut";
static const std::string kParamOverlayTgtOut = "overlayTgtOut";
static const std::string kParamOverlayTgtOutColor = "overlayTangentOutColor";

static const std::string kParamGroupCurveBegin = "groupCurveBegin";
static const std::string kParamCurveBegin = "pCurveBegin";

static const std::string kParamGroupOverlay = "groupOverlay";
}
}
}

#endif
