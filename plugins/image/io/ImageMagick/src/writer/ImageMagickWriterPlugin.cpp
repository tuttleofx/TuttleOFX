#include "ImageMagickWriterDefinitions.hpp"
#include "ImageMagickWriterPlugin.hpp"
#include "ImageMagickWriterProcess.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace writer {

using namespace boost::gil;

ImageMagickWriterPlugin::ImageMagickWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_premult = fetchBooleanParam( kParamPremult );
	_quality = fetchIntParam( kParamQuality );
}

ImageMagickWriterProcessParams ImageMagickWriterPlugin::getProcessParams(const OfxTime time)
{
	ImageMagickWriterProcessParams params;
	params._filepath = getFilenameAt(time);
	params._quality = this->_quality->getValue();
	params._premult = this->_premult->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageMagickWriterPlugin::render( const OFX::RenderArguments& args )
{
	if( _paramRenderAlways->getValue() || OFX::getImageEffectHostDescription()->hostIsBackground )
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
					ImageMagickWriterProcess<rgba8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					ImageMagickWriterProcess<rgba16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					ImageMagickWriterProcess<rgba32f_view_t> fred( *this );
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
					ImageMagickWriterProcess<gray8_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthUShort:
				{
					ImageMagickWriterProcess<gray16_view_t> fred( *this );
					fred.setupAndProcess( args );
					break;
				}
				case OFX::eBitDepthFloat:
				{
					ImageMagickWriterProcess<gray32f_view_t> fred( *this );
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
