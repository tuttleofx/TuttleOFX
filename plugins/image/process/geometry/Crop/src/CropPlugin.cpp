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

	changedParam( _changedArgs, kParamMode );
	changedParam( _changedArgs, kParamAxis );
	changedParam( _changedArgs, kParamFixedRatio );
	changedParam( _changedArgs, kParamRatio );
	changedParam( _changedArgs, kParamPreset );
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
	if( paramName == kParamMode )
	{
		EParamMode mode = static_cast<EParamMode>( _paramMode->getValue() );
		switch( mode )
		{
			case eParamModeCrop:
			{
				_paramFillColor->setIsSecretAndDisabled( true );
				break;
			}
			case eParamModeColor:
			{
				_paramFillColor->setIsSecretAndDisabled( false );
				break;
			}
		}
	}
	else if( paramName == kParamAxis ||
			 paramName == kParamSymmetric ||
			 paramName == kParamFixedRatio )
	{
		EParamAxis axis = static_cast<EParamAxis>( _paramAxis->getValue() );
		EParamSymmetric symmetric  = static_cast<EParamSymmetric>( _paramSymmetric->getValue() );
		bool xMinSecret = false;
		bool yMinSecret = false;
		bool xMaxSecret = false;
		bool yMaxSecret = false;
		switch( axis )
		{
			case eParamAxisXY:
			{
				xMinSecret = false;
				yMinSecret = false;

				xMaxSecret = false;
				yMaxSecret = false;
				break;
			}
			case eParamAxisX:
			{
				xMinSecret = false;
				xMaxSecret = false;

				yMinSecret = true;
				yMaxSecret = true;
				break;
			}
			case eParamAxisY:
			{
				xMinSecret = true;
				xMaxSecret = true;

				yMinSecret = false;
				yMaxSecret = false;
				break;
			}
		}
		if( symmetric == eParamSymmetricX ||
			symmetric == eParamSymmetricXY )
		{
			xMaxSecret = true;
		}
		if( symmetric == eParamSymmetricY ||
			symmetric == eParamSymmetricXY )
		{
			yMaxSecret = true;
		}
		bool fixedRatio = _paramFixedRatio->getValue();
		if( fixedRatio )
		{
			yMinSecret = true;
			yMaxSecret = true;
		}
		_paramXMin->setIsSecretAndDisabled( xMinSecret );
		_paramYMin->setIsSecretAndDisabled( yMinSecret );
		_paramXMax->setIsSecretAndDisabled( xMaxSecret );
		_paramYMax->setIsSecretAndDisabled( yMaxSecret );
	}
	else if( paramName == kParamFixedRatio )
	{
//		if( _paramFixedRatio->getValue() )
//		{
//			_paramRatio->setIsSecretAndDisabled( false );
//			_paramPreset->setIsSecretAndDisabled( false );
//		}
//		else
//		{
//			_paramRatio->setIsSecretAndDisabled( true );
//			_paramPreset->setIsSecretAndDisabled( true );
//		}
	}
	else if( paramName == kParamRatio )
	{
//		if( ??? )
//		{
			_paramPreset->setValue( eParamPreset_custom );
//		}
		if( _clipSrc->isConnected() )
		{
			CropProcessParams<rgba32f_pixel_t> params = getProcessParams<rgba32f_pixel_t>( args.time );
			_paramYMin->setValue( params._cropRegion.y1 );
			_paramYMax->setValue( params._cropRegion.y2 );
		}
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
	else if( args.reason != OFX::eChangePluginEdit &&
			 ( paramName == kParamXMin ||
	           paramName == kParamYMin ||
	           paramName == kParamXMax ||
	           paramName == kParamYMax ) )
	{
		bool fixedRatio = _paramFixedRatio->getValue();
		if( fixedRatio )
		{
			CropProcessParams<rgba32f_pixel_t> params = getProcessParams<rgba32f_pixel_t>( args.time );
			_paramXMin->setValue( params._cropRegion.x1 );
			_paramYMin->setValue( params._cropRegion.y1 );
			_paramXMax->setValue( params._cropRegion.x2 );
			_paramYMax->setValue( params._cropRegion.y2 );
		}
		else
		{
			const double x = _paramXMax->getValue() - _paramXMin->getValue();
			const double y = _paramYMax->getValue() - _paramYMin->getValue();
			_paramRatio->setValue( (y!=0) ? (x/y) : 0 );
		}
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
		case eParamModeColor:
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
