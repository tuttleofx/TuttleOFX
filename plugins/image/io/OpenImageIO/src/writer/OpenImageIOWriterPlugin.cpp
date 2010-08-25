#include "OpenImageIOWriterDefinitions.hpp"
#include "OpenImageIOWriterPlugin.hpp"
#include "OpenImageIOWriterProcess.hpp"

#include <OpenImageIO/imageio.h>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle {
namespace plugin {
namespace openImageIO {
namespace writer {


OpenImageIOWriterPlugin::OpenImageIOWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_components     = fetchChoiceParam( kParamOutputComponents );
}

OpenImageIOWriterProcessParams OpenImageIOWriterPlugin::getProcessParams(const OfxTime time)
{
	OpenImageIOWriterProcessParams params;
	params._filepath = getAbsoluteFilenameAt(time);
	params._components = static_cast<EParamComponents>( this->_components->getValue() );
	switch(static_cast<EParamBitDepth>(this->_paramBitDepth->getValue()))
	{
		case eParamBitDepth8:
			params._bitDepth = TypeDesc::UINT8;
			break;
		case eParamBitDepth16:
			params._bitDepth = TypeDesc::UINT16;
			break;
		case eParamBitDepth32:
			params._bitDepth = TypeDesc::FLOAT;
			break;
		default:
			BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatErrValue, "Incorrect bit depth.") );
			break;
	}
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void OpenImageIOWriterPlugin::render( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth dstBitDepth         = _clipSrc->getPixelDepth();
	OFX::EPixelComponent dstComponents = _clipSrc->getPixelComponents();

	// do the rendering
	if( dstComponents == OFX::ePixelComponentRGBA )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				OpenImageIOWriterProcess<rgba8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				OpenImageIOWriterProcess<rgba16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				OpenImageIOWriterProcess<rgba32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else if( dstComponents == OFX::ePixelComponentAlpha )
	{
		switch( dstBitDepth )
		{
			case OFX::eBitDepthUByte:
			{
				OpenImageIOWriterProcess<gray8_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthUShort:
			{
				OpenImageIOWriterProcess<gray16_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthFloat:
			{
				OpenImageIOWriterProcess<gray32f_view_t> fred( *this );
				fred.setupAndProcess( args );
				break;
			}
			case OFX::eBitDepthNone:
				COUT_FATALERROR( "BitDepthNone not recognize." );
				return;
			case OFX::eBitDepthCustom:
				COUT_FATALERROR( "BitDepthCustom not recognize." );
				return;
		}
	}
	else
	{
		COUT_FATALERROR( "Pixel component unrecognize ! (" << mapPixelComponentEnumToString( dstComponents ) << ")" );
	}
}

}
}
}
}
