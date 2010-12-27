#include "BitDepthPlugin.hpp"
#include "BitDepthProcess.hpp"
#include "BitDepthDefinitions.hpp"

#include <tuttle/plugin/global.hpp>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace bitDepth {

using namespace boost;
using namespace boost::gil;

BitDepthPlugin::BitDepthPlugin( OfxImageEffectHandle handle )
	: ImageEffectGilPlugin( handle )
{
	_paramOutBitDepth = fetchChoiceParam( kParamOutputBitDepth );
}

void BitDepthPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
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
		clipPreferences.setClipBitDepth( *_clipDst, static_cast<OFX::EBitDepth>( _paramOutBitDepth->getValue() ) );
	}
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void BitDepthPlugin::render( const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	const OFX::EBitDepth srcBitDepth         = _clipSrc->getPixelDepth();
	const OFX::EPixelComponent srcComponents = _clipSrc->getPixelComponents();

    switch( srcComponents )
	{
		case OFX::ePixelComponentRGBA:
		{
			switch( srcBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					setupDestView<rgba8_view_t>( args );
					return;
				}
				case OFX::eBitDepthUShort:
				{
					setupDestView<rgba16_view_t>( args );
					return;
				}
				case OFX::eBitDepthFloat:
				{
					setupDestView<rgba32f_view_t>( args );
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatErrUnsupported, "Bit depth (" + mapBitDepthEnumToString(srcBitDepth) + ") not recognized by the plugin." ) );
				}
			}
			break;
		}
		case OFX::ePixelComponentRGB:
		{
			switch( srcBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					setupDestView<rgb8_view_t>( args );
					return;
				}
				case OFX::eBitDepthUShort:
				{
					setupDestView<rgb16_view_t>( args );
					return;
				}
				case OFX::eBitDepthFloat:
				{
					setupDestView<rgb32f_view_t>( args );
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(srcBitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentAlpha:
		{
			switch( srcBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					setupDestView<gray8_view_t>( args );
					return;
				}
				case OFX::eBitDepthUShort:
				{
					setupDestView<gray16_view_t>( args );
					return;
				}
				case OFX::eBitDepthFloat:
				{
					setupDestView<gray32f_view_t>( args );
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(srcBitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(srcComponents) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

/**
 * @brief Interpret source and then destination
 * @param[in]   args     Rendering parameters
 */
template<class sview_t>
void BitDepthPlugin::setupDestView( const OFX::RenderArguments& args )
{
	const OFX::EBitDepth dstBitDepth         = _clipDst->getPixelDepth();
	const OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

    switch( dstComponents )
	{
		case OFX::ePixelComponentRGBA:
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
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(dstBitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentRGB:
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					BitDepthProcess<sview_t, rgb8_view_t> fred( *this );
					fred.setupAndProcess( args );
					return;
				}
				case OFX::eBitDepthUShort:
				{
					BitDepthProcess<sview_t, rgb16_view_t> fred( *this );
					fred.setupAndProcess( args );
					return;
				}
				case OFX::eBitDepthFloat:
				{
					BitDepthProcess<sview_t, rgb32f_view_t> fred( *this );
					fred.setupAndProcess( args );
					return;
				}
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(dstBitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentAlpha:
		{
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
				case OFX::eBitDepthCustom:
				case OFX::eBitDepthNone:
				{
					BOOST_THROW_EXCEPTION( exception::Unsupported()
						<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(dstBitDepth) + ") not recognized by the plugin." );
				}
			}
			break;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(dstComponents) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

}
}
}
