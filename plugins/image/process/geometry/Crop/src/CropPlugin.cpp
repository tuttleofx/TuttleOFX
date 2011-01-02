#include "CropDefinitions.hpp"
#include "CropPlugin.hpp"
#include "CropProcess.hpp"

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
	_paramFormats = fetchChoiceParam( kParamPresets );
	_paramOverlayRect    = fetchBooleanParam( kParamDisplayRect );
}


bool CropPlugin::displayRect()
{
	return _paramOverlayRect->getValue();
}

void CropPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kCropHelpButton )
	{
		sendMessage( OFX::Message::eMessageMessage,
		             "", // No XML resources
		             kCropHelpString );
	}
	else if( paramName == kParamPresets )
	{
		// Compute bands sizes in pixels
		int f, bandSize;
		double ratio;
		_paramFormats->getValue( f );
		OFX::IntParam* upBand    = fetchIntParam( kParamUp );
		OFX::IntParam* downBand  = fetchIntParam( kParamDown );
		OFX::IntParam* leftBand  = fetchIntParam( kParamLeft );
		OFX::IntParam* rightBand = fetchIntParam( kParamRight );
		OfxRectD rod             = _clipSrc->getCanonicalRod( timeLineGetTime() );
		double par               = _clipSrc->getPixelAspectRatio();
		int w                    = (int)std::abs( rod.x2 - rod.x1 );
		int h                    = (int)std::abs( rod.y2 - rod.y1 );

		switch( f )
		{
			// 4/3
			case k1_1_33:
				ratio = 4.0 / 3.0;
				break;
			// 16 / 9
			case k1_1_77:
				ratio = 16.0 / 9.0;
				break;
			// 1:1.85
			case k1_1_85:
				ratio = 1.85;
				break;
			// Cinemascope
			case k1_2_35:
				ratio = 2.35;
				break;
			case k1_2_40:
				ratio = 2.40;
				break;
			default:
				ratio = 0;
				break;
		}

		// If image ratio is lesser than the specified ratio, we need to add left and right bands
		if( ( (double)( w ) / h ) > ratio )
		{
			bandSize = (int)round( ( w - ( h / ( 1.0 / ratio ) ) ) / 2.0 );
			upBand->setValue( 0 );
			downBand->setValue( 0 );
			leftBand->setValue( (int)round( bandSize / par ) );
			rightBand->setValue( (int)round( bandSize / par ) );
		}
		else if( ( (double)( w )  / h ) < ratio )
		{
			// Add top and bottom bands
			bandSize = (int)round( ( h - ( ( w ) / ratio ) ) / 2.0 );
			upBand->setValue( bandSize );
			downBand->setValue( bandSize );
			leftBand->setValue( 0 );
			rightBand->setValue( 0 );
		}
		else
		{
			upBand->setValue( 0 );
			downBand->setValue( 0 );
			leftBand->setValue( 0 );
			rightBand->setValue( 0 );
		}
	}
}

OfxRectD CropPlugin::getCropRect( const OfxRectD& rod, const double par )
{
	OfxRectD rect;
	OFX::IntParam* upBand    = fetchIntParam( kParamUp );
	OFX::IntParam* downBand  = fetchIntParam( kParamDown );
	OFX::IntParam* leftBand  = fetchIntParam( kParamLeft );
	OFX::IntParam* rightBand = fetchIntParam( kParamRight );

	rect.x1 = par * leftBand->getValue();
	rect.x2 = rod.x2 - par* rightBand->getValue();
	rect.y1 = downBand->getValue();
	rect.y2 = rod.y2 - upBand->getValue();
	return rect;
}

OfxRectD CropPlugin::getCropRect( const OfxTime time )
{
	const OfxRectD srcRod = _clipSrc->getCanonicalRod( time );
	const double par       = _clipSrc->getPixelAspectRatio();
	return getCropRect( srcRod, par );
}

bool CropPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	OFX::BooleanParam* bop = fetchBooleanParam( kParamFillMode );

	if( bop->getValue() == false )
	{
		rod = getCropRect( args.time );
		return true;
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
