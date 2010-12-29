#include "LocalMaximaPlugin.hpp"
#include "LocalMaximaProcess.hpp"
#include "LocalMaximaDefinitions.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace localmaxima {


LocalMaximaPlugin::LocalMaximaPlugin( OfxImageEffectHandle handle )
: ImageEffectGilPlugin( handle )
{
	_paramBorder = fetchChoiceParam( kParamBorder );
	_paramOutputComponent = fetchChoiceParam( kParamOutputComponent );
}

LocalMaximaProcessParams<LocalMaximaPlugin::Scalar> LocalMaximaPlugin::getProcessParams( const OfxPointD& renderScale ) const
{
	LocalMaximaProcessParams<Scalar> params;

	params._border     = static_cast<EParamBorder>( _paramBorder->getValue() );

	return params;
}

void LocalMaximaPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
//    if( paramName == kParamHelpButton )
//    {
//        sendMessage( OFX::Message::eMessageMessage,
//                     "", // No XML resources
//                     kParamHelpString );
//    }
}

void LocalMaximaPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	EParamOutputComponent comp = static_cast<EParamOutputComponent>(_paramOutputComponent->getValue());
	switch( comp )
	{
		case eParamOutputComponentRGBA:
		{
			clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
			break;
		}
		case eParamOutputComponentRGB:
		{
			clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGB );
			break;
		}
		case eParamOutputComponentAlpha:
		{
			clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentAlpha );
			break;
		}
	}
}

bool LocalMaximaPlugin::getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod )
{
	LocalMaximaProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	switch( params._border )
	{
		case eParamBorderPadded:
			rod.x1 = srcRod.x1 + 1;
			rod.y1 = srcRod.y1 + 1;
			rod.x2 = srcRod.x2 - 1;
			rod.y2 = srcRod.y2 - 1;
			return true;
		default:
			break;
	}
	return false;
}

void LocalMaximaPlugin::getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois )
{
	LocalMaximaProcessParams<Scalar> params = getProcessParams();
	OfxRectD srcRod = _clipSrc->getCanonicalRod( args.time );

	OfxRectD srcRoi;
	srcRoi.x1 = srcRod.x1 - 1;
	srcRoi.y1 = srcRod.y1 - 1;
	srcRoi.x2 = srcRod.x2 + 1;
	srcRoi.y2 = srcRod.y2 + 1;
	rois.setRegionOfInterest( *_clipSrc, srcRoi );
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void LocalMaximaPlugin::render( const OFX::RenderArguments &args )
{
	doGilRender2<LocalMaximaProcess>( *this, args, *_clipSrc, *_clipDst );
	
//	OFX::EBitDepth sBitDepth = _clipSrc->getPixelDepth( );
//	OFX::EPixelComponent sComponent = _clipSrc->getPixelComponents( );
//
//	OFX::EBitDepth dBitDepth = _clipDst->getPixelDepth( );
//	OFX::EPixelComponent dComponent = _clipDst->getPixelComponents( );
//
//	switch( sComponent )
//	{
//		case OFX::ePixelComponentRGBA:
//		{
//			doGilRender2<LocalMaximaProcess, false, boost::gil::rgba_layout_t>( *this, args, sBitDepth, false, dComponent, dBitDepth );
//			return;
//		}
//		case OFX::ePixelComponentRGB:
//		{
////			doGilRender2<LocalMaximaProcess, false, boost::gil::rgb_layout_t>( *this, args, sBitDepth, false, dComponent, dBitDepth );
//			return;
//		}
//		case OFX::ePixelComponentAlpha:
//		case OFX::ePixelComponentCustom:
//		case OFX::ePixelComponentNone:
//		{
//			BOOST_THROW_EXCEPTION( exception::Unsupported()
//				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(sComponent) + ") not supported by the plugin." );
//		}
//	}
//	BOOST_THROW_EXCEPTION( exception::Unknown() );

	/*
    switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<LocalMaximaProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender<LocalMaximaProcess, false, boost::gil::rgb_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
*/
}


}
}
}
