#ifndef _LENSDISTORTDEFINITIONS_HPP
#define _LENSDISTORTDEFINITIONS_HPP

#include <tuttle/plugin/global.hpp>

#include <ofxCore.h>
#include <string>

namespace tuttle {
namespace plugin {
namespace lens {

const std::string kClipOptionalSourceRef( "SourceRef" );

const std::string kParamReverse( "reverse" );
const std::string kParamDisplaySource( "displaySource" );
const std::string kParamLensType( "lensType" );
const std::string kParamLensTypeStandard( "standard" );
const std::string kParamLensTypeFishEye( "fish-eye" );
const std::string kParamLensTypeAdvanced( "advanced" );
const std::string kParamCoef1( "coef1" );
const std::string kParamCoef2( "coef2" );
const std::string kParamSqueeze( "squeeze" );
const std::string kParamAsymmetric( "asymmetric" );
const std::string kParamCenter( "center" );
const std::string kParamCenterOverlay( "lensCenterOverlay" );
const std::string kParamCenterType( "centerType" );
const std::string kParamCenterTypeSource( "source" );
const std::string kParamCenterTypeRoW( "RoW" );
const std::string kParamPreScale( "preScale" );
const std::string kParamPostScale( "postScale" );
const std::string kParamInterpolation( "interpolation" );
const std::string kParamInterpolationNearest( "nearest neighbor" );
const std::string kParamInterpolationBilinear( "bilinear" );
const std::string kParamResizeRod( "resizeRod" );
const std::string kParamResizeRodNo( "no" );
const std::string kParamResizeRodSourceRef( "sourceRef" );
const std::string kParamResizeRodMin( "min" );
const std::string kParamResizeRodMax( "max" );
const std::string kParamResizeRodManual( "manual" );
enum EParamResizeRod
{
	eParamResizeRodNo = 0,
	eParamResizeRodSourceRef,
	eParamResizeRodMin,
	eParamResizeRodMax,
	eParamResizeRodManual,
};

const std::string kParamResizeRodManualScale( "scaleRod" );
const std::string kParamDisplayOptions( "displayOptions" );
const std::string kParamGridOverlay( "gridOverlay" );
const std::string kParamGridCenter( "gridCenter" );
const std::string kParamGridCenterOverlay( "gridCenterOverlay" );
const std::string kParamGridScale( "gridScale" );
const std::string kParamDebugOptions( "debugOptions" );
const std::string kParamDebugDisplayRoi( "debugDisplayRoi" );
const std::string kParamHelp( "help" );

enum EParamLensType
{
	eParamLensTypeStandard = 0,
	eParamLensTypeFisheye,
	eParamLensTypeAdvanced,
};
enum EParamInterpolation
{
	eParamInterpolationNearest = 0,
	eParamInterpolationBilinear,
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

