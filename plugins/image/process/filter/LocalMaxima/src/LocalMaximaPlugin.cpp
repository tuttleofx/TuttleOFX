#include "LocalMaximaPlugin.hpp"
#include "LocalMaximaProcess.hpp"
#include "LocalMaximaDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace localmaxima {


LocalMaximaPlugin::LocalMaximaPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );

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
	using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );

    // do the rendering
    if( dstComponents == OFX::ePixelComponentRGBA )
    {
        switch( dstBitDepth )
        {
            case OFX::eBitDepthUByte :
            {
                LocalMaximaProcess<rgba8_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthUShort :
            {
                LocalMaximaProcess<rgba16_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
            case OFX::eBitDepthFloat :
            {
                LocalMaximaProcess<rgba32f_view_t> p( *this );
                p.setupAndProcess( args );
                break;
            }
			default:
			{
				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
				break;
			}
        }
    }
//    else if( dstComponents == OFX::ePixelComponentAlpha )
//    {
//        switch( dstBitDepth )
//        {
//            case OFX::eBitDepthUByte :
//            {
//                LocalMaximaProcess<gray8_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthUShort :
//            {
//                LocalMaximaProcess<gray16_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//            case OFX::eBitDepthFloat :
//            {
//                LocalMaximaProcess<gray32f_view_t> p( *this );
//                p.setupAndProcess( args );
//                break;
//            }
//			default:
//			{
//				COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin." );
//				break;
//			}
//        }
//    }
	else
	{
		COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(dstComponents) << ") not supported by the plugin." );
	}
}


}
}
}
