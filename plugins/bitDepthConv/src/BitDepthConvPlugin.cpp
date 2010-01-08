/**
 * @file BitDepthConvPlugin.cpp
 * @brief
 * @author
 * @date    08/01/10 17:46
 *
 */

#include "BitDepthConvPlugin.hpp"
#include "BitDepthConvProcess.hpp"
#include "BitDepthConvDefinitions.hpp"

#include <tuttle/common/utils/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepthConvert {

using namespace boost;
using namespace boost::gil;

BitDepthConvPlugin::BitDepthConvPlugin( OfxImageEffectHandle handle ) :
ImageEffect( handle )
{
    _srcClip = fetchClip( kOfxImageEffectSimpleSourceClipName );
    _dstClip = fetchClip( kOfxImageEffectOutputClipName );
	_outBitDepth = fetchChoiceParam( kOutputBitDepth );
}

OFX::Clip * BitDepthConvPlugin::getSrcClip( ) const
{
    return _srcClip;
}

OFX::Clip * BitDepthConvPlugin::getDstClip( ) const
{
    return _dstClip;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void BitDepthConvPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    const OFX::BitDepthEnum srcBitDepth = _srcClip->getPixelDepth( );
    const OFX::PixelComponentEnum srcComponents = _srcClip->getPixelComponents( );

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
void BitDepthConvPlugin::setupDestView( const OFX::RenderArguments &args )
{
	const OFX::BitDepthEnum dstBitDepth = _dstClip->getPixelDepth( );
    const OFX::PixelComponentEnum dstComponents = _dstClip->getPixelComponents( );
	if (dstComponents == OFX::ePixelComponentRGBA)
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				BitDepthConvProcess<sview_t, rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthUShort:
			{
				BitDepthConvProcess<sview_t, rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthFloat:
			{
				BitDepthConvProcess<sview_t, rgba32f_view_t> fred( *this );
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
				BitDepthConvProcess<sview_t, gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthUShort:
			{
				BitDepthConvProcess<sview_t, gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				return;
			}
			case OFX::eBitDepthFloat:
			{
				BitDepthConvProcess<sview_t, gray32f_view_t> fred( *this );
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

void BitDepthConvPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kBitDepthConvHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kBitDepthConvHelpString );
    }
}

void BitDepthConvPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	clipPreferences.setClipComponents( *_dstClip, OFX::ePixelComponentRGBA );
	clipPreferences.setClipBitDepth( *_dstClip, (OFX::BitDepthEnum)(_outBitDepth->getValue() + 1) );
}

}
}
}
