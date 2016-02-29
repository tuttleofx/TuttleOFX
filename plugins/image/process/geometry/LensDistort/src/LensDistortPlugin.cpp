#include "LensDistortPlugin.hpp"
#include "LensDistortProcess.hpp"
#include "lensDistortAlgorithm.hpp" // to compute RoI

#include <tuttle/plugin/ofxToGil/point.hpp>
#include <tuttle/plugin/numeric/coordinateSystem.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace lens {

OfxRectD LensDistortPlugin::_dstRoi     = { 0, 0, 0, 0 };
OfxRectD LensDistortPlugin::_srcRoi     = { 0, 0, 0, 0 };
OfxRectD LensDistortPlugin::_srcRealRoi = { 0, 0, 0, 0 };

LensDistortPlugin::LensDistortPlugin( OfxImageEffectHandle handle )
	: SamplerPlugin( handle )
{
	_srcRefClip = fetchClip( kClipOptionalSourceRef );

	_reverse              = fetchBooleanParam       ( kParamReverse );
	_displaySource        = fetchBooleanParam       ( kParamDisplaySource );
	_lensType             = fetchChoiceParam        ( kParamLensType );
	_normalization        = fetchChoiceParam        ( kParamNormalization );
	_focal                = fetchDoubleParam        ( kParamFocal );
	_coef1                = fetchDoubleParam        ( kParamCoef1 );
	_coef2                = fetchDoubleParam        ( kParamCoef2 );
	_coef3                = fetchDoubleParam        ( kParamCoef3 );
	_coef4                = fetchDoubleParam        ( kParamCoef4 );
	_squeeze              = fetchDoubleParam        ( kParamSqueeze );
	_asymmetric           = fetchDouble2DParam      ( kParamAsymmetric );
	_center               = fetchDouble2DParam      ( kParamCenter );
	_centerUnit           = fetchChoiceParam        ( kParamCenterUnit );
	_centerOverlay        = fetchBooleanParam       ( kParamCenterOverlay );
	_centerType           = fetchChoiceParam        ( kParamCenterType );
	_preScale             = fetchDoubleParam        ( kParamPreScale );
	_postScale            = fetchDoubleParam        ( kParamPostScale );
	_preOffset            = fetchDouble2DParam      ( kParamPreOffset );
	_postOffset           = fetchDouble2DParam      ( kParamPostOffset );
	_resizeRod            = fetchChoiceParam        ( kParamResizeRod );
	_resizeRodManualScale = fetchDoubleParam        ( kParamResizeRodManualScale );
	_groupDisplayParams   = fetchGroupParam         ( kParamDisplayOptions );
	_gridOverlay          = fetchBooleanParam       ( kParamGridOverlay );
	_gridCenter           = fetchDouble2DParam      ( kParamGridCenter );
	_gridCenterOverlay    = fetchBooleanParam       ( kParamGridCenterOverlay );
	_gridScale            = fetchDouble2DParam      ( kParamGridScale );
	_debugDisplayRoi      = fetchBooleanParam       ( kParamDebugDisplayRoi );

	initParamsProps();
}

void LensDistortPlugin::initParamsProps()
{
	static const OFX::InstanceChangedArgs args;

	changedParam( args, kParamFilter );
	changedParam( args, kParamLensType );
	changedParam( args, kParamResizeRod );
	changedParam( args, kParamNormalization );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void LensDistortPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<LensDistortProcess>( *this, args );
}

void LensDistortPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	SamplerPlugin::changedParam( args, paramName );

	if( paramName == kParamLensType )
	{
		switch( (EParamLensType)_lensType->getValue() )
		{
			case eParamLensTypeBrown1:
				_coef2     -> setIsSecretAndDisabled( true );
				_coef3     -> setIsSecretAndDisabled( true );
				_coef4     ->setIsSecretAndDisabled( true );
				_squeeze    -> setIsSecretAndDisabled( true );
				_asymmetric -> setIsSecretAndDisabled( true );
				break;
			case eParamLensTypeBrown3:
				_coef2     -> setIsSecretAndDisabled( false );
				_coef3     -> setIsSecretAndDisabled( false );
				_coef4     ->setIsSecretAndDisabled( true );
				_squeeze    -> setIsSecretAndDisabled( true );
				_asymmetric -> setIsSecretAndDisabled( true );
				break;
			case eParamLensTypePTLens:
				_coef2     -> setIsSecretAndDisabled( false );
				_coef3     -> setIsSecretAndDisabled( false );
				_coef4     ->setIsSecretAndDisabled( true );
				_squeeze    -> setIsSecretAndDisabled( true );
				_asymmetric -> setIsSecretAndDisabled( true );
				break;
			case eParamLensTypeFisheye:
				_coef2     ->setIsSecretAndDisabled( true );
				_coef3     ->setIsSecretAndDisabled( true );
				_coef4     ->setIsSecretAndDisabled( true );
				_squeeze    -> setIsSecretAndDisabled( true );
				_asymmetric -> setIsSecretAndDisabled( true );
				break;
			case eParamLensTypeFisheye4:
				_coef2     ->setIsSecretAndDisabled( false );
				_coef3     ->setIsSecretAndDisabled( false );
				_coef4     ->setIsSecretAndDisabled( false );
				_squeeze    -> setIsSecretAndDisabled( true );
				_asymmetric -> setIsSecretAndDisabled( true );
				break;
			default:
				BOOST_THROW_EXCEPTION( exception::Bug()
					<< exception::user() + "Lens type value not recognize." );
				break;
		}
	}
	else if( paramName == kParamNormalization )
	{
		_focal->setIsSecretAndDisabled( _normalization->getValue() != eParamNormalizationFocal );
	}
	else if( paramName == kParamResizeRod )
	{
		if( _resizeRod->getValue() == eParamResizeRodManual )
		{
			_resizeRodManualScale -> setIsSecret( false );
			_resizeRodManualScale -> setEnabled( true );
		}
		else
		{
			//			_resizeRodManualScale->setIsSecret( true );
			_resizeRodManualScale -> setEnabled( false );
		}
	}
	else if( paramName == kParamGridOverlay || paramName == kParamGridCenter || paramName == kParamGridScale )
	{
		redrawOverlays();
	}
}

bool LensDistortPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	bool isIdentity = false;

	if( _displaySource->getValue() )
	{
		isIdentity = true;
	}
	else if( _coef1->getValue() == 0 &&
	         _preScale->getValue() == _preScale->getDefault() &&
	         _postScale->getValue() == _postScale->getDefault() &&
	         _preOffset->getValue() == _preOffset->getDefault() &&
	         _postOffset->getValue() == _postOffset->getDefault() &&
	         ( !_coef2->getIsEnable() || _coef2->getValue() == 0 ) &&
	         ( !_coef3->getIsEnable() || _coef3->getValue() == 0 ) &&
	         ( !_squeeze->getIsEnable() || _squeeze->getValue() == _squeeze->getDefault() ) &&
	         ( !_asymmetric->getIsEnable() || _asymmetric->getValue() == _asymmetric->getDefault() ) )
	{
		isIdentity = true;
	}
	if( isIdentity )
	{
		identityClip = _clipSrc;
		identityTime = args.time;
		return true;
	}
	return false;
}

bool LensDistortPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	using namespace bgil;
	const OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	const Point2 srcRodCorner( srcRod.x1, srcRod.y1 );
	const Point2 srcRodSize( srcRod.x2 - srcRod.x1, srcRod.y2 - srcRod.y1 );
	const OfxRectD srcRodInDstFrame = { 0, 0, srcRodSize.x, srcRodSize.y };

	bool modified = false;

	LensDistortProcessParams<Scalar> params( getProcessParams( srcRod, srcRod, _clipDst->getPixelAspectRatio(), true ) );
	switch( static_cast<EParamResizeRod>( _resizeRod->getValue() ) )
	{
		case eParamResizeRodNo:
		{
			rod = srcRod;
			return false;
		}
		case eParamResizeRodSourceRef:
		{
			if( !_srcRefClip->isConnected() )
				return false;
			rod = _srcRefClip->getCanonicalRod( args.time );
			return true;
		}
		case eParamResizeRodMin:
		{
			Point2 tl = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x1, srcRodInDstFrame.y1 ) ); // top left
			Point2 tr = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x2, srcRodInDstFrame.y1 ) ); // top right
			Point2 bl = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x1, srcRodInDstFrame.y2 ) ); // bottom left
			Point2 br = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x2, srcRodInDstFrame.y2 ) ); // bottom right
			point2<double> pMin; // top left corner
			point2<double> pMax; // down right corner

			pMin.x = std::max( tl.x, bl.x );
			pMin.y = std::max( tl.y, tr.y );
			pMax.x = std::min( tr.x, br.x );
			pMax.y = std::min( bl.y, br.y );

			if( params.lensCenterSrc.x > srcRodInDstFrame.x1 && params.lensCenterSrc.x < srcRodInDstFrame.x2 )
			{
				Point2 t = transformValues( getLensType(), params, Point2( params.lensCenterSrc.x, srcRodInDstFrame.y1 ) );
				pMin.y = std::max( pMin.y, t.y );
				Point2 b = transformValues( getLensType(), params, Point2( params.lensCenterSrc.x, srcRodInDstFrame.y2 ) );
				pMax.y = std::min( pMax.y, b.y );
			}
			if( params.lensCenterSrc.y > srcRodInDstFrame.y1 && params.lensCenterSrc.y < srcRodInDstFrame.y2 )
			{
				Point2 l = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x1, params.lensCenterSrc.y ) );
				pMin.x = std::max( pMin.x, l.x );
				Point2 r = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x2, params.lensCenterSrc.y ) );
				pMax.x = std::min( pMax.x, r.x );
			}
			rod.x1   = pMin.x;
			rod.y1   = pMin.y;
			rod.x2   = pMax.x;
			rod.y2   = pMax.y;
			modified = true;
			break;
		}
		case eParamResizeRodMax:
		{
			rod      = transformValues( getLensType(), params, srcRodInDstFrame );
			modified = true;
			break;
		}
		case eParamResizeRodManual:
		{
			double scale = _resizeRodManualScale->getValue();
			if( scale == 1.0 )
				return false;

			point2<double> pMin( srcRodInDstFrame.x1, srcRodInDstFrame.y1 ); // top left corner
			point2<double> pMax( srcRodInDstFrame.x2, srcRodInDstFrame.y2 ); // down right corner
			point2<double> center( srcRodSize * 0.5 );
			pMin     = ( ( pMin - center ) * scale ) + center;
			pMax     = ( ( pMax - center ) * scale ) + center;
			rod.x1   = pMin.x;
			rod.y1   = pMin.y;
			rod.x2   = pMax.x;
			rod.y2   = pMax.y;
			modified = true;
		}
	}
	rod.x1 += srcRodCorner.x;
	rod.y1 += srcRodCorner.y;
	rod.x2 += srcRodCorner.x;
	rod.y2 += srcRodCorner.y;
	return modified;
}

void LensDistortPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );
	OfxRectD dstRod = _clipDst->getCanonicalRod( args.time );

	LensDistortProcessParams<Scalar> params;
	terry::sampler::EParamFilter interpolation = getInterpolation();
	if( _srcRefClip->isConnected() )
	{
		OfxRectD srcRefRod = _srcRefClip->getCanonicalRod( args.time );
		params = getProcessParams( srcRod, dstRod, srcRefRod, _clipDst->getPixelAspectRatio() );
	}
	else
	{
		params = getProcessParams( srcRod, dstRod, _clipDst->getPixelAspectRatio() );
	}

	OfxRectD outputRoi = args.regionOfInterest;
	outputRoi.x1 -= dstRod.x1; // to dest rod coordinates
	outputRoi.y1 -= dstRod.y1;
	outputRoi.x2 -= dstRod.x1;
	outputRoi.y2 -= dstRod.y1;
	OfxRectD srcRoi = transformValues( getLensType(), params, outputRoi );
	srcRoi.x1    += srcRod.x1; // to RoW coordinates
	srcRoi.y1    += srcRod.y1;
	srcRoi.x2    += srcRod.x1;
	srcRoi.y2    += srcRod.y1;
	outputRoi.x1 += dstRod.x1; // to RoW coordinates
	outputRoi.y1 += dstRod.y1;
	outputRoi.x2 += dstRod.x1;
	outputRoi.y2 += dstRod.y1;

	// add margin around depending on interpolation region sizes
	double margin = 0.0;
	switch( interpolation )
	{
		case terry::sampler::eParamFilterNearest:
		case terry::sampler::eParamFilterBilinear:
		{
			margin = 1.0; // one pixel margin
			break;
		}
		case terry::sampler::eParamFilterBicubic:
		case terry::sampler::eParamFilterBC:
		case terry::sampler::eParamFilterCatrom:
		case terry::sampler::eParamFilterMitchell:
		case terry::sampler::eParamFilterParzen:
		case terry::sampler::eParamFilterKeys:
		case terry::sampler::eParamFilterSimon:
		case terry::sampler::eParamFilterRifman:
		{
			margin = 2.0; // two pixels margin
			break;
		}
		case terry::sampler::eParamFilterLanczos:
		case terry::sampler::eParamFilterLanczos3:
		case terry::sampler::eParamFilterLanczos4:
		case terry::sampler::eParamFilterLanczos6:
		case terry::sampler::eParamFilterLanczos12:
		case terry::sampler::eParamFilterGaussian:
		{
			margin = 6.0;
			break;
		}
	}
	srcRoi.x1 -= margin;
	srcRoi.y1 -= margin;
	srcRoi.x2 += margin;
	srcRoi.y2 += margin;

	//    srcRoi.x1 += 2; // if we remove 2 pixels to the needed RoI the plugin crash, because it tries to access to this pixels
	//    srcRoi.y1 += 2; // so the calculation of the RoI has a precision of one pixel
	//    srcRoi.x2 -= 2;
	//    srcRoi.y2 -= 2;
	OfxRectD srcRealRoi = rectanglesIntersection( srcRoi, srcRod );

	rois.setRegionOfInterest( *_clipSrc, srcRealRoi );

	if( _debugDisplayRoi->getValue() )
	{
		_srcRealRoi = srcRealRoi;
		_srcRoi     = srcRoi;
		_dstRoi     = outputRoi;
	}
}

LensDistortProcessParams<LensDistortPlugin::Scalar> LensDistortPlugin::getProcessParams( const OfxRectD& inputRod, const OfxRectD& outputRod, const OfxRectD& optionalInputRod, const double pixelAspectRatio, const bool reverse ) const
{
	const bool      useOptionalInputRod  = ( optionalInputRod.x1 != optionalInputRod.x2 ) && ( optionalInputRod.y1 != optionalInputRod.y2 );
	const OfxRectD& choosedInputRod      = useOptionalInputRod ? optionalInputRod : inputRod;

	typedef     bgil::point2<Scalar> Point2;
	LensDistortProcessParams<Scalar> params;

	double preScale         = _preScale->getValue();
	double postScale        = _postScale->getValue();

	params.distort          = _reverse->getValue();

	params.coef1           = _coef1->getValue();
	params.coef2           = _coef2->getValue();
	params.coef3           = _coef3->getValue();
	params.coef4           = _coef4->getValue();
	params.squeeze         = _squeeze->getValue();
	params.asymmetric      = ofxToGil( _asymmetric->getValue() );

	params.preScale.x = params.preScale.y = 1.0 / preScale;
	params.postScale.x = params.postScale.y = 1.0 / postScale;

	Point2 preOffset = ofxToGil(_preOffset->getValue());
	Point2 postOffset = ofxToGil(_postOffset->getValue());

	if( reverse )
	{
		params.distort   = !params.distort;

		// swap pre/post scale
		Point2 swapPoint = params.preScale;
		params.preScale  = 1.0 / params.postScale;
		params.postScale = 1.0 / swapPoint;

		// swap pre/post offset
		swapPoint = preOffset;
		preOffset = postOffset;
		postOffset = swapPoint;
	}

	Point2  imgShift       = Point2( inputRod.x1 - outputRod.x1, inputRod.y1 - outputRod.y1 ); // translate output -> source
	params.imgSizeSrc      = Point2( choosedInputRod.x2 - choosedInputRod.x1, choosedInputRod.y2 - choosedInputRod.y1 );
	params.imgCenterSrc    = Point2( params.imgSizeSrc.x * 0.5, params.imgSizeSrc.y * 0.5 );
	params.imgCenterDst    = params.imgCenterSrc + imgShift;
	switch( (EParamNormalization)_normalization->getValue() )
	{
		case eParamNormalizationWidth:
			params.normalizeCoef = params.imgSizeSrc.x;
			break;
		case eParamNormalizationHeight:
			params.normalizeCoef = params.imgSizeSrc.y;
			break;
		case eParamNormalizationMinSize:
			params.normalizeCoef = std::min(params.imgSizeSrc.x, params.imgSizeSrc.y);
			break;
		case eParamNormalizationMaxSize:
			params.normalizeCoef = std::max(params.imgSizeSrc.x, params.imgSizeSrc.y);
			break;
		case eParamNormalizationDiagonal:
			params.normalizeCoef = std::sqrt( params.imgSizeSrc.x * params.imgSizeSrc.x + params.imgSizeSrc.y * params.imgSizeSrc.y );
			break;
		case eParamNormalizationHalfDiagonal:
			params.normalizeCoef = std::sqrt( params.imgCenterSrc.x * params.imgCenterSrc.x + params.imgCenterSrc.y * params.imgCenterSrc.y );
			break;
		case eParamNormalizationFocal:
			params.normalizeCoef = _focal->getValue();
			break;
	}
	params.pixelRatio = pixelAspectRatio;

	switch( getCenterType() )
	{
		case eParamCenterTypeSource:
		{
			switch( (EParamCenterUnit)_centerUnit->getValue() )
			{
				case eParamCenterUnitCenteredPixel:
					params.lensCenterSrc = ofxToGil( _center->getValue() ) + params.imgCenterSrc;
					break;
				case eParamCenterUnitPixel:
					params.lensCenterSrc = ofxToGil( _center->getValue() );
					break;
				case eParamCenterUnitCenteredNormWidth:
					params.lensCenterSrc = pointNormalizedXXcToCanonicalXY( ofxToGil( _center->getValue() ), params.imgSizeSrc );
					break;
				case eParamCenterUnitNormWidth:
					params.lensCenterSrc = pointNormalizedXXToCanonicalXY( ofxToGil( _center->getValue() ), params.imgSizeSrc );
					break;
			}
			if( useOptionalInputRod )
			{
				Point2 imgShiftBetweenInputs = Point2( optionalInputRod.x1 - inputRod.x1, optionalInputRod.y1 - inputRod.y1 ); // translate inputRod -> optionalInputRod
				params.lensCenterSrc += imgShiftBetweenInputs;
			}
			params.lensCenterDst = params.lensCenterSrc + imgShift;
			break;
		}
		case eParamCenterTypeRoW:
		{
			// we can't place in RoW because we don't have the RoW size...
			break;
		}
	}
	params.lensCenterSrc -= preOffset;
	params.lensCenterDst += postOffset;

	return params;
}

LensDistortParams LensDistortPlugin::getProcessParams(  ) const
{
	LensDistortParams lensDistortParams;
	SamplerPlugin::fillProcessParams( lensDistortParams._samplerProcessParams );

	lensDistortParams._lensType          = (tuttle::plugin::lens::EParamLensType)   _lensType        -> getValue();
	lensDistortParams._centerType        = (tuttle::plugin::lens::EParamCenterType) _centerType      -> getValue();

	return lensDistortParams;
}

}
}
}

