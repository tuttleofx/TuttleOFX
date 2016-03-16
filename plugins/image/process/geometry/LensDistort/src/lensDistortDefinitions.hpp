#ifndef _LENSDISTORTDEFINITIONS_HPP
#define _LENSDISTORTDEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>
#include <tuttle/plugin/context/SamplerDefinition.hpp>

#include <ofxCore.h>
#include <string>

namespace tuttle
{
namespace plugin
{
namespace lens
{

static const std::string kClipOptionalSourceRef("SourceRef");

static const std::string kParamReverse("reverse");
static const std::string kParamDisplaySource("displaySource");
static const std::string kParamLensType("lensType");
static const std::string kParamLensTypeBrown1("brown1");
static const std::string kParamLensTypeBrown3("brown3");
static const std::string kParamLensTypePTLens("ptlens");
static const std::string kParamLensTypeFishEye1("fisheye1");
static const std::string kParamLensTypeFishEye4("fisheye4");
static const std::string kParamCoef1("coef1");
static const std::string kParamCoef2("coef2");
static const std::string kParamCoef3("coef3");
static const std::string kParamCoef4("coef4");
static const std::string kParamSqueeze("squeeze");
static const std::string kParamAsymmetric("asymmetric");
static const std::string kParamCenter("center");
static const std::string kParamCenterUnit("centerUnit");
static const std::string kParamCenterUnitCenteredPixel("pixel centered");
static const std::string kParamCenterUnitPixel("pixel");
static const std::string kParamCenterUnitCenteredNormWidth("normalized width centered");
static const std::string kParamCenterUnitNormWidth("normWidth");
enum EParamCenterUnit
{
    eParamCenterUnitCenteredPixel = 0,
    eParamCenterUnitPixel,
    eParamCenterUnitCenteredNormWidth,
    eParamCenterUnitNormWidth
};

static const std::string kParamCenterOverlay("lensCenterOverlay");
static const std::string kParamCenterType("centerType");
static const std::string kParamCenterTypeSource("source");
static const std::string kParamCenterTypeRoW("RoW");
static const std::string kParamPreScale("preScale");
static const std::string kParamPostScale("postScale");
static const std::string kParamPreOffset("preOffset");
static const std::string kParamPostOffset("postOffset");

static const std::string kParamNormalization("normalization");
static const std::string kParamNormalizationWidth("width");
static const std::string kParamNormalizationHeight("height");
static const std::string kParamNormalizationMinSize("minSize");
static const std::string kParamNormalizationMaxSize("maxSize");
static const std::string kParamNormalizationDiagonal("diagonal");
static const std::string kParamNormalizationHalfDiagonal("halfDiagonal");
static const std::string kParamNormalizationFocal("focal");
enum EParamNormalization
{
    eParamNormalizationWidth = 0,
    eParamNormalizationHeight,
    eParamNormalizationMinSize,
    eParamNormalizationMaxSize,
    eParamNormalizationDiagonal,
    eParamNormalizationHalfDiagonal,
    eParamNormalizationFocal
};

static const std::string kParamFocal("focal");

static const std::string kParamResizeRod("resizeRod");
static const std::string kParamResizeRodNo("no");
static const std::string kParamResizeRodSourceRef("sourceRef");
static const std::string kParamResizeRodMin("min");
static const std::string kParamResizeRodMax("max");
static const std::string kParamResizeRodManual("manual");
enum EParamResizeRod
{
    eParamResizeRodNo = 0,
    eParamResizeRodSourceRef,
    eParamResizeRodMin,
    eParamResizeRodMax,
    eParamResizeRodManual,
};

static const std::string kParamResizeRodManualScale("scaleRod");
static const std::string kParamDisplayOptions("displayOptions");
static const std::string kParamGridOverlay("gridOverlay");
static const std::string kParamGridCenter("gridCenter");
static const std::string kParamGridCenterOverlay("gridCenterOverlay");
static const std::string kParamGridScale("gridScale");
static const std::string kParamDebugOptions("debugOptions");
static const std::string kParamDebugDisplayRoi("debugDisplayRoi");
static const std::string kParamHelp("help");

enum EParamLensType
{
    eParamLensTypeBrown1 = 0,
    eParamLensTypeBrown3,
    eParamLensTypePTLens,
    eParamLensTypeFisheye,
    eParamLensTypeFisheye4,
};

enum EParamCenterType
{
    eParamCenterTypeSource = 0,
    eParamCenterTypeRoW,
};
}
}
}

#endif
