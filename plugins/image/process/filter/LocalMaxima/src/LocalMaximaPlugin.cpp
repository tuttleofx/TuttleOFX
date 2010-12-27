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


bool LocalMaximaPlugin::isIdentity( const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime )
{
//	LocalMaximaProcessParams<Scalar> params = getProcessParams();
//	if( params._in == params._out )
//	{
//		identityClip = _clipSrc;
//		identityTime = args.time;
//		return true;
//	}
	return false;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void LocalMaximaPlugin::render( const OFX::RenderArguments &args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth = _clipDst->getPixelDepth( );
	OFX::EPixelComponent components = _clipDst->getPixelComponents( );

    switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<LocalMaximaProcess, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender<LocalMaximaProcess, boost::gil::rgb_layout_t>( *this, args, bitDepth );
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
}


}
}
}
