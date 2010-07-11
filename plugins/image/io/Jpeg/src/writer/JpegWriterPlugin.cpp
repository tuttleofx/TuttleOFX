#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"
#include "JpegWriterProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace jpeg {
namespace writer {

using namespace boost::gil;

JpegWriterPlugin::JpegWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_premult = fetchBooleanParam( kParamPremult );
	_quality = fetchIntParam( kParamQuality );
}

JpegWriterProcessParams JpegWriterPlugin::getProcessParams(const OfxTime time)
{
	JpegWriterProcessParams params;
	params._filepath = this->_filePattern.getFilenameAt(time);
	params._quality = this->_quality->getValue();
	params._premult = this->_premult->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void JpegWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _renderAlways->getValue() || OFX::getImageEffectHostDescription()->hostIsBackground )
	{
		// instantiate the render code based on the pixel depth of the dst clip
		OFX::BitDepthEnum dstBitDepth         = _clipSrc->getPixelDepth();
		OFX::PixelComponentEnum dstComponents = _clipSrc->getPixelComponents();

		// do the rendering
		if( dstComponents == OFX::ePixelComponentRGBA )
		{
			switch( dstBitDepth )
			{
				case OFX::eBitDepthUByte:
				{
					JpegWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					JpegWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					JpegWriterProcess<rgba32f_view_t> fred( *this );
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
					JpegWriterProcess<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					JpegWriterProcess<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					JpegWriterProcess<gray32f_view_t> fred( *this );
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
			COUT_FATALERROR( "Pixel component unrecognize ! (" << mapPixelComponentEnumToStr( dstComponents ) << ")" );
		}
	}
}

}
}
}
}
