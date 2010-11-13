#include "LensDistortPlugin.hpp"
#include "LensDistortProcess.hpp"
#include "lensDistortAlgorithm.hpp" // to compute RoI

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/ofxToGil.hpp>
#include <tuttle/plugin/coordinateSystem.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <ofxsParam.h>

namespace tuttle {
namespace plugin {
namespace lens {

OfxRectD LensDistortPlugin::_dstRoi     = { 0, 0, 0, 0 };
OfxRectD LensDistortPlugin::_srcRoi     = { 0, 0, 0, 0 };
OfxRectD LensDistortPlugin::_srcRealRoi = { 0, 0, 0, 0 };

const static std::string kLensDistortHelpString( "<p>Apply or correct a lens distortion on an image.</p>" );

LensDistortPlugin::LensDistortPlugin( OfxImageEffectHandle handle )
	: ImageEffect( handle )
{
	_clipDst    = fetchClip( kOfxImageEffectOutputClipName );
	_clipSrc    = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_srcRefClip = fetchClip( kClipOptionalSourceRef );

	_reverse              = fetchBooleanParam( kParamReverse );
	_displaySource        = fetchBooleanParam( kParamDisplaySource );
	_lensType             = fetchChoiceParam( kParamLensType );
	_coef1                = fetchDoubleParam( kParamCoef1 );
	_coef2                = fetchDoubleParam( kParamCoef2 );
	_squeeze              = fetchDoubleParam( kParamSqueeze );
	_asymmetric           = fetchDouble2DParam( kParamAsymmetric );
	_center               = fetchDouble2DParam( kParamCenter );
	_centerOverlay        = fetchBooleanParam( kParamCenterOverlay );
	_centerType           = fetchChoiceParam( kParamCenterType );
	_preScale             = fetchDoubleParam( kParamPreScale );
	_postScale            = fetchDoubleParam( kParamPostScale );
	_interpolation        = fetchChoiceParam( kParamInterpolation );
	_resizeRod            = fetchChoiceParam( kParamResizeRod );
	_resizeRodManualScale = fetchDoubleParam( kParamResizeRodManualScale );
	_groupDisplayParams   = fetchGroupParam( kParamDisplayOptions );
	_gridOverlay          = fetchBooleanParam( kParamGridOverlay );
	_gridCenter           = fetchDouble2DParam( kParamGridCenter );
	_gridCenterOverlay    = fetchBooleanParam( kParamGridCenterOverlay );
	_gridScale            = fetchDouble2DParam( kParamGridScale );
	_debugDisplayRoi      = fetchBooleanParam( kParamDebugDisplayRoi );
	_helpButton           = fetchPushButtonParam( kParamHelp );

	initParamsProps();
}

void LensDistortPlugin::initParamsProps()
{
	static const OFX::InstanceChangedArgs args;

	changedParam( args, kParamLensType );
	changedParam( args, kParamResizeRod );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void LensDistortPlugin::render( const OFX::RenderArguments& args )
{
	using namespace bgil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				LensDistortProcess<rgba8_view_t> effect( *this );
				effect.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				LensDistortProcess<rgba16_view_t> effect( *this );
				effect.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				LensDistortProcess<rgba32f_view_t> effect( *this );
				effect.setupAndProcess( args );
				break;
			}
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString( dstBitDepth ) << ") not recognized by the plugin." );
				break;
			}
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				LensDistortProcess<gray8_view_t> effect( *this );
				effect.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				LensDistortProcess<gray16_view_t> effect( *this );
				effect.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				LensDistortProcess<gray32f_view_t> effect( *this );
				effect.setupAndProcess( args );
				break;
			}
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString( dstBitDepth ) << ") not recognized by the plugin." );
				break;
			}
		}
	}
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString( dstComponents ) << ") not supported by the plugin." );
	}
}

void LensDistortPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamLensType )
	{
		switch( _lensType->getValue() )
		{
			case 0: // normal
				_coef2->setIsSecret( true );
				_coef2->setEnabled( false );
				_squeeze->setIsSecret( true );
				_squeeze->setEnabled( false );
				_asymmetric->setIsSecret( true );
				_asymmetric->setEnabled( false );
				break;
			case 1: // fish-eye
				_coef2->setIsSecret( false );
				_coef2->setEnabled( true );
				_squeeze->setIsSecret( true );
				_squeeze->setEnabled( false );
				_asymmetric->setIsSecret( true );
				_asymmetric->setEnabled( false );
				break;
			case 2: // advanced
				_coef2->setIsSecret( false );
				_coef2->setEnabled( true );
				_squeeze->setIsSecret( false );
				_squeeze->setEnabled( true );
				_asymmetric->setIsSecret( false );
				_asymmetric->setEnabled( true );
				break;
			default:
				COUT_ERROR( "Lens type value not recognize." );
				break;
		}
	}
	else if( paramName == kParamResizeRod )
	{
		if( _resizeRod->getValue() == eParamResizeRodManual )
		{
			_resizeRodManualScale->setIsSecret( false );
			_resizeRodManualScale->setEnabled( true );
		}
		else
		{
			//			_resizeRodManualScale->setIsSecret( true );
			_resizeRodManualScale->setEnabled( false );
		}
	}
	else if( paramName == kParamGridOverlay || paramName == kParamGridCenter || paramName == kParamGridScale )
	{
		redrawOverlays();
	}
	else if( paramName == kParamHelp )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // no XML override
		             kLensDistortHelpString
		             );
	}
}

bool LensDistortPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
	bool isIdentity = false;

	if( _displaySource->getValue() )
	{
		isIdentity = true;
	}
	else if( _coef1->getValue() == 0 /*_coef1->getDefault( )*/ &&
	         _preScale->getValue() == _preScale->getDefault() &&
	         _postScale->getValue() == _postScale->getDefault() &&
	         ( !_coef2->getIsEnable() || _coef2->getValue() == _coef2->getDefault() ) &&
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

			if( params._lensCenterSrc.x > srcRodInDstFrame.x1 && params._lensCenterSrc.x < srcRodInDstFrame.x2 )
			{
				Point2 t = transformValues( getLensType(), params, Point2( params._lensCenterSrc.x, srcRodInDstFrame.y1 ) );
				pMin.y = std::max( pMin.y, t.y );
				Point2 b = transformValues( getLensType(), params, Point2( params._lensCenterSrc.x, srcRodInDstFrame.y2 ) );
				pMax.y = std::min( pMax.y, b.y );
			}
			if( params._lensCenterSrc.y > srcRodInDstFrame.y1 && params._lensCenterSrc.y < srcRodInDstFrame.y2 )
			{
				Point2 l = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x1, params._lensCenterSrc.y ) );
				pMin.x = std::max( pMin.x, l.x );
				Point2 r = transformValues( getLensType(), params, Point2( srcRodInDstFrame.x2, params._lensCenterSrc.y ) );
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
	//    srcRoi.x1 += 2; // if we remove 2 pixels to the needed RoI the plugin crash, because it tries to access to this pixels
	//    srcRoi.y1 += 2; // so the calcul of the RoI has a precision of one pixel
	//    srcRoi.x2 -= 2;
	//    srcRoi.y2 -= 2;
	OfxRectD srcRealRoi = rectanglesIntersection( srcRoi, srcRod );
	srcRealRoi = srcRod;

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
	const bool useOptionalInputRod  = ( optionalInputRod.x1 != optionalInputRod.x2 ) && ( optionalInputRod.y1 != optionalInputRod.y2 );
	const OfxRectD& choosedInputRod = useOptionalInputRod ? optionalInputRod : inputRod;

	typedef bgil::point2<Scalar> Point2;
	LensDistortProcessParams<Scalar> params;

	params._coef1 = _coef1->getValue();
	if( params._coef1 >= 0 )
		params._distort = true; // distort
	else
		params._distort = false; // undistort
	params._coef1      = std::abs( params._coef1 );
	params._coef2      = _coef2->getValue();
	params._squeeze    = _squeeze->getValue();
	params._asymmetric = ofxToGil( _asymmetric->getValue() );
	double preScale = _preScale->getValue();
	params._preScale.x = ( 1.0 / preScale );
	params._preScale.y = 1.0 / preScale;
	double postScale = _postScale->getValue();
	params._postScale.x = ( 1.0 / postScale );
	params._postScale.y = 1.0 / postScale;
	Point2 imgShift = Point2( inputRod.x1 - outputRod.x1, inputRod.y1 - outputRod.y1 ); // translate output -> source
	params._imgSizeSrc      = Point2( choosedInputRod.x2 - choosedInputRod.x1, choosedInputRod.y2 - choosedInputRod.y1 );
	params._imgCenterSrc    = Point2( params._imgSizeSrc.x * 0.5, params._imgSizeSrc.y * 0.5 );
	params._imgCenterDst    = params._imgCenterSrc + imgShift;
	params._imgHalfDiagonal = std::sqrt( params._imgCenterSrc.x * params._imgCenterSrc.x * pixelAspectRatio * pixelAspectRatio + params._imgCenterSrc.y * params._imgCenterSrc.y );
	params._pixelRatio      = pixelAspectRatio;
	switch( getCenterType() )
	{
		case eParamCenterTypeSource:
		{
			params._lensCenterSrc = pointNormalizedXXcToCanonicalXY( ofxToGil( _center->getValue() ), params._imgSizeSrc );
			if( useOptionalInputRod )
			{
				Point2 imgShiftBetweenInputs = Point2( optionalInputRod.x1 - inputRod.x1, optionalInputRod.y1 - inputRod.y1 ); // translate inputRod -> optionalInputRod
				params._lensCenterSrc += imgShiftBetweenInputs;
			}
			params._lensCenterDst = params._lensCenterSrc + imgShift;
			break;
		}
		case eParamCenterTypeRoW:
		{
			// we can't place in RoW because we don't have the RoW size...
			break;
		}
	}
	if( _reverse->getValue() != reverse )
	{
		params._distort = !params._distort;
		Point2 swapPreScale = params._preScale;
		params._preScale  = 1.0 / params._postScale;
		params._postScale = 1.0 / swapPreScale;
	}
	return params;
}

}
}
}

