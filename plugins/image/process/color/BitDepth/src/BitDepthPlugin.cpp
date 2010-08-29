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
	_paramOutBitDepth = fetchChoiceParam( kParamOutputBitDepth );
}

void BitDepthPlugin::changedParam( const OFX::InstanceChangedArgs &args, const std::string &paramName )
{
    if( paramName == kParamHelpButton )
    {
        sendMessage( OFX::Message::eMessageMessage,
                     "", // No XML resources
                     kBitDepthHelpString );
    }
}

void BitDepthPlugin::getClipPreferences( OFX::ClipPreferencesSetter& clipPreferences )
{
	if( _paramOutBitDepth->getValue() != 0 )
	{
		clipPreferences.setClipBitDepth( *_clipDst, static_cast<OFX::EBitDepth>(_paramOutBitDepth->getValue()) );
	}
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void BitDepthPlugin::render( const OFX::RenderArguments &args )
{
    // instantiate the render code based on the pixel depth of the dst clip
    const OFX::EBitDepth srcBitDepth = _clipSrc->getPixelDepth( );
    const OFX::EPixelComponent srcComponents = _clipSrc->getPixelComponents( );

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
	const OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth( );
    const OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents( );
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
		}
	}
}

}
}
}
