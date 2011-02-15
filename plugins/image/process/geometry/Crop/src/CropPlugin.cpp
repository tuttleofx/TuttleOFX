#include "CropDefinitions.hpp"
#include "CropPlugin.hpp"
#include "CropProcess.hpp"

#include <tuttle/plugin/image/ofxToGil.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/math/special_functions/round.hpp>

namespace tuttle {
namespace plugin {
namespace crop {

using namespace boost::math;
using namespace boost::gil;

CropPlugin::CropPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramMode = fetchChoiceParam( kParamMode );
	_paramFillColor = fetchRGBAParam( kParamFillColor );
	_paramAxis = fetchChoiceParam( kParamAxis );
	_paramSymmetric = fetchChoiceParam( kParamSymmetric );
	_paramFixedRatio = fetchBooleanParam( kParamFixedRatio );
	_paramRatio = fetchDoubleParam( kParamRatio );
	_paramPreset = fetchChoiceParam( kParamPreset );
	_paramOverlay = fetchBooleanParam( kParamOverlay );
	_paramCropRegion = fetchGroupParam( kParamGroupCropRegion );
	_paramXMin = fetchIntParam( kParamXMin );
	_paramYMin = fetchIntParam( kParamYMin );
	_paramXMax = fetchIntParam( kParamXMax );
	_paramYMax = fetchIntParam( kParamYMax );

//	changedParam( _changedArgs, kParamMode );
	changedParam( _changedArgs, kParamAxis );
	changedParam( _changedArgs, kParamFixedRatio );
	changedParam( _changedArgs, kParamRatio );
	changedParam( _changedArgs, kParamPreset );
}

OfxRectI CropPlugin::getCropRegionValue() const
{
	OfxRectI cropRegion;

	cropRegion.x1 = _paramXMin->getValue();
	cropRegion.y1 = _paramYMin->getValue();
	cropRegion.x2 = _paramXMax->getValue();
	cropRegion.y2 = _paramYMax->getValue();
	
	return cropRegion;
}

OfxRectI CropPlugin::computeCropRegion( const OfxTime time, const bool fromRatio ) const
{
	OfxRectI cropRegion = getCropRegionValue();

	if( ! _clipSrc->isConnected() )
	{
		return cropRegion;
	}
	const OfxRectI sRod = _clipSrc->getPixelRod( time );
	const OfxPointI sRodSize = _clipSrc->getPixelRodSize( time );

	const EParamAxis axis = static_cast<EParamAxis>( _paramAxis->getValue() );
	const EParamSymmetric symmetric = static_cast<EParamSymmetric>( _paramSymmetric->getValue() );
	switch( axis )
	{
		case eParamAxisXY:
		{
			if( symmetric == eParamSymmetricNone )
					break;

			if( symmetric == eParamSymmetricX ||
			    symmetric == eParamSymmetricXY )
			{
					cropRegion.x1 %= int( sRodSize.x * 0.5 );
					cropRegion.x2 = sRodSize.x - cropRegion.x1;
			}
			if( symmetric == eParamSymmetricY ||
			    symmetric == eParamSymmetricXY )
			{
					cropRegion.y1 %= int( sRodSize.y * 0.5 );
					cropRegion.y2 = sRodSize.y - cropRegion.y1;
			}
			break;
		}
		case eParamAxisX:
		{
			// don't modify Y axis
			cropRegion.y1 = sRod.y1;
			cropRegion.y2 = sRod.y2;
			if( symmetric == eParamSymmetricX ||
			    symmetric == eParamSymmetricXY )
			{
				cropRegion.x1 %= int( sRodSize.x * 0.5 );
				cropRegion.x2 = sRodSize.x - cropRegion.x1;
			}
			break;
		}
		case eParamAxisY:
		{
			// don't modify X axis
			cropRegion.x1 = sRod.x1;
			cropRegion.x2 = sRod.x2;
			if( symmetric == eParamSymmetricY ||
			    symmetric == eParamSymmetricXY )
			{
				cropRegion.y1 %= int( sRodSize.y * 0.5 );
				cropRegion.y2 = sRodSize.y - cropRegion.y1;
			}
			break;
		}
	}
	bool fixedRatio = _paramFixedRatio->getValue();
	if( fromRatio || fixedRatio )
	{
		double ratio = _paramRatio->getValue();
		if( ratio == 0.0 )
			ratio = 1.0;

		switch( axis )
		{
			case eParamAxisXY:
			{
				const double xD2 = (cropRegion.x2 - cropRegion.x1) * 0.5;
				const double yD2 = (cropRegion.y2 - cropRegion.y1) * 0.5;
				const double yCenter = cropRegion.y1 + yD2;
				const double nyD2 = xD2 / ratio;
				cropRegion.y1 = int( yCenter-nyD2 );
				cropRegion.y2 = int( yCenter+nyD2 );
				break;
			}
			case eParamAxisY:
			{
				const double xD2 = (sRod.x2 - sRod.x1) * 0.5;
				const double yD2 = (sRod.y2 - sRod.y1) * 0.5;
				const double yCenter = sRod.y1 + yD2;
				const double nyD2 = xD2 / ratio;
				cropRegion.y1 = int( yCenter-nyD2 );
				cropRegion.y2 = int( yCenter+nyD2 );
				break;
			}
			case eParamAxisX:
			{
				const double xD2 = (sRod.x2 - sRod.x1) * 0.5;
				const double yD2 = (sRod.y2 - sRod.y1) * 0.5;
				const double xCenter = sRod.x1 + xD2;
				const double nxD2 = yD2 * ratio;
				cropRegion.x1 = int( xCenter-nxD2 );
				cropRegion.x2 = int( xCenter+nxD2 );
				break;
			}
		}
	}

	return cropRegion;
}

void CropPlugin::changedClip( const OFX::InstanceChangedArgs& args, const std::string& clipName )
{
//	TUTTLE_TCOUT( "changedClip:" << clipName );
	if( clipName == kOfxImageEffectSimpleSourceClipName )
	{
		changedParam( _changedArgs, kParamRatio );
	}
}

void CropPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
//	TUTTLE_TCOUT( "changedParam:" << paramName );
//	if( paramName == kParamMode )
//	{
//		EParamMode mode = static_cast<EParamMode>( _paramMode->getValue() );
//		switch( mode )
//		{
//			case eParamModeCrop:
//			{
//				_paramFillColor->setIsSecretAndDisabled( true );
//				break;
//			}
//			case eParamModeColor:
//			{
//				_paramFillColor->setIsSecretAndDisabled( false );
//				break;
//			}
//		}
//	}
//	else
	if( paramName == kParamAxis ||
			 paramName == kParamSymmetric ||
			 paramName == kParamFixedRatio )
	{
		EParamAxis axis = static_cast<EParamAxis>( _paramAxis->getValue() );
		EParamSymmetric symmetric  = static_cast<EParamSymmetric>( _paramSymmetric->getValue() );
		bool xMinEnabled = false;
		bool yMinEnabled = false;
		bool xMaxEnabled = false;
		bool yMaxEnabled = false;
		switch( axis )
		{
			case eParamAxisXY:
			{
				xMinEnabled = true;
				yMinEnabled = true;

				xMaxEnabled = true;
				yMaxEnabled = true;
				break;
			}
			case eParamAxisX:
			{
				xMinEnabled = true;
				xMaxEnabled = true;

				yMinEnabled = false;
				yMaxEnabled = false;
				break;
			}
			case eParamAxisY:
			{
				xMinEnabled = false;
				xMaxEnabled = false;

				yMinEnabled = true;
				yMaxEnabled = true;
				break;
			}
		}
		if( symmetric == eParamSymmetricX ||
			symmetric == eParamSymmetricXY )
		{
			xMaxEnabled = false;
		}
		if( symmetric == eParamSymmetricY ||
			symmetric == eParamSymmetricXY )
		{
			yMaxEnabled = false;
		}
		bool fixedRatio = _paramFixedRatio->getValue();
		if( fixedRatio )
		{
			yMinEnabled = false;
			yMaxEnabled = false;
		}
		_paramXMin->setEnabled( xMinEnabled );
		_paramYMin->setEnabled( yMinEnabled );
		_paramXMax->setEnabled( xMaxEnabled );
		_paramYMax->setEnabled( yMaxEnabled );

		if( paramName == kParamFixedRatio )
		{
			if( _paramFixedRatio->getValue() )
			{
				_paramRatio->setEnabled( true );
				_paramPreset->setIsSecretAndDisabled( false );
			}
			else
			{
				_paramRatio->setEnabled( false );
				_paramPreset->setIsSecretAndDisabled( true );
			}
		}
	}
	else if( paramName == kParamRatio )
	{
//		if( ??? )
//		{
			_paramPreset->setValue( eParamPreset_custom );
//		}
//		bool fixedRatio = _paramFixedRatio->getValue();
//
//		if( fixedRatio &&
//			_clipSrc->isConnected() )
//		{
//			OfxRectI cropRegion = computeCropRegion( args.time, true );
//			_paramXMin->setValue( cropRegion.x1 );
//			_paramXMax->setValue( cropRegion.x2 );
//			_paramYMin->setValue( cropRegion.y1 );
//			_paramYMax->setValue( cropRegion.y2 );
//		}
	}
	else if( paramName == kParamPreset )
	{
		double ratio = 0.0;
		EParamPreset preset = static_cast<EParamPreset>(_paramPreset->getValue());

		switch( preset )
		{
			case eParamPreset_custom:
				break;
			// 4/3
			case eParamPreset_1_33:
				ratio = 4.0 / 3.0;
				break;
			// 16 / 9
			case eParamPreset_1_77:
				ratio = 16.0 / 9.0;
				break;
			// 1:1.85
			case eParamPreset_1_85:
				ratio = 1.85;
				break;
			// Cinemascope
			case eParamPreset_2_35:
				ratio = 2.35;
				break;
			case eParamPreset_2_40:
				ratio = 2.40;
				break;
		}
		if( preset != eParamPreset_custom )
		{
			_paramRatio->setValue( ratio );
		}
	}

	// if a param which have an impact on crop region
	// we need to recompute this region
	if( paramName == kParamXMin ||
	    paramName == kParamYMin ||
	    paramName == kParamXMax ||
	    paramName == kParamYMax ||
	    paramName == kParamAxis ||
	    paramName == kParamSymmetric ||
	    paramName == kParamRatio
	   )
	{
//		bool fixedRatio = _paramFixedRatio->getValue();
		const OfxRectI cropRegionParams = getCropRegionValue();
		const OfxRectI cropRegion = computeCropRegion( args.time );
//		if( fixedRatio )
//		{
		if( cropRegionParams.x1 != cropRegion.x1 )
			_paramXMin->setValue( cropRegion.x1 );
		if( cropRegionParams.y1 != cropRegion.y1 )
			_paramYMin->setValue( cropRegion.y1 );
		if( cropRegionParams.x2 != cropRegion.x2 )
			_paramXMax->setValue( cropRegion.x2 );
		if( cropRegionParams.y2 != cropRegion.y2 )
			_paramYMax->setValue( cropRegion.y2 );
//		}
//		else
//		{
			const double x = cropRegion.x2 - cropRegion.x1;
			const double y = cropRegion.y2 - cropRegion.y1;
			const double nRatio = (y!=0) ? (x/y) : 0;
			if( nRatio != _paramRatio->getValue() )
				_paramRatio->setValue( nRatio );
//		}
	}
}

bool CropPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	CropProcessParams<rgba32f_pixel_t> params = getProcessParams<rgba32f_pixel_t>( args.time, args.renderScale );

	switch( _paramMode->getValue() )
	{
		case eParamModeCrop:
		{
			rod.x1 = params._cropRegion.x1;
			rod.y1 = params._cropRegion.y1;
			rod.x2 = params._cropRegion.x2;
			rod.y2 = params._cropRegion.y2;
			return true;
		}
		case eParamModeFillColor:
		{
			break;
		}
	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void CropPlugin::render( const OFX::RenderArguments& args )
{
	doGilRender<CropProcess>( *this, args );
}

}
}
}
