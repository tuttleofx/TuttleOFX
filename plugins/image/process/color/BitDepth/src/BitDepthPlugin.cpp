#include "BitDepthPlugin.hpp"
#include "BitDepthProcess.hpp"
#include "BitDepthDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepth {

using namespace boost;
using namespace boost::gil;

BitDepthPlugin::BitDepthPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _clipDst = fetchClip( kOfxImageEffectOutputClipName );
	_outBitDepth = fetchChoiceParam( kOutputBitDepth );
}

OFX::Clip * BitDepthPlugin::getSrcClip( ) const
{
    return _clipSrc;
}

OFX::Clip * BitDepthPlugin::getDstClip( ) const
{
    return _clipDst;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void BitDepthPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    const OFX::BitDepthEnum srcBitDepth = _clipSrc->getPixelDepth( );
    const OFX::PixelComponentEnum srcComponents = _clipSrc->getPixelComponents( );

	if (srcComponents == OFX::ePixelComponentRGBA)
	{
		switch( srcBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				setupDestView<rgba8_view_t>(args);
				return;
			}
			case OFX::eBitDepthUShort:
			{
				setupDestView<rgba16_view_t>(args);
				return;
			}
			case OFX::eBitDepthFloat:
			{
				setupDestView<rgba32f_view_t>(args);
				return;
			}
			case OFX::eBitDepthNone :
			{
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			}
			case OFX::eBitDepthCustom :
			{
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
			}
			default:
				break;
		}
	} else {
		switch( srcBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				setupDestView<gray8_view_t>(args);
				return;
			}
			case OFX::eBitDepthUShort:
			{
				setupDestView<gray16_view_t>(args);
				return;
			}
			case OFX::eBitDepthFloat:
			{
				setupDestView<gray32f_view_t>(args);
				return;
			}
			case OFX::eBitDepthNone :
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom :
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
			default:
				break;
		}
	}
}

/**
 * @brief Interpret source and then destination
 * @param[in]   args     Rendering parameters
 */
template<class sview_t>
void BitDepthPlugin::setupDestView( const OFX::RenderArguments &args )
{
	const OFX::BitDepthEnum dstBitDepth = _clipDst->getPixelDepth( );
    const OFX::PixelComponentEnum dstComponents = _clipDst->getPixelComponents( );
	if (dstComponents == OFX::ePixelComponentRGBA)
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				BitDepthProcess<sview_t, rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthUShort:
			{
				BitDepthProcess<sview_t, rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthFloat:
			{
				BitDepthProcess<sview_t, rgba32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthNone :
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom :
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
			default:
				break;
		}
	} else {
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				BitDepthProcess<sview_t, gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthUShort:
			{
				BitDepthProcess<sview_t, gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthFloat:
			{
				BitDepthProcess<sview_t, gray32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthNone :
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom :
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
			default:
				break;
		}
	}
}

void BitDepthPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kBitDepthHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kBitDepthHelpString );
    }
}

void BitDepthPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setClipComponents( *_clipDst, OFX::ePixelComponentRGBA );
	if( _outBitDepth->getValue() != 0 )
		clipPreferences.setClipBitDepth( *_clipDst, (OFX::BitDepthEnum)(_outBitDepth->getValue()) );
}

}
}
}
