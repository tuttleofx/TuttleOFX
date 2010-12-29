#include "ImageMagickWriterDefinitions.hpp"
#include "ImageMagickWriterPlugin.hpp"
#include "ImageMagickWriterProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace writer {

ImageMagickWriterPlugin::ImageMagickWriterPlugin( OfxImageEffectHandle handle )
	: WriterPlugin( handle )
{
	_premult = fetchBooleanParam( kParamPremult );
	_quality = fetchIntParam( kParamQuality );
}

ImageMagickWriterProcessParams ImageMagickWriterPlugin::getProcessParams( const OfxTime time )
{
	ImageMagickWriterProcessParams params;

	params._filepath = getAbsoluteFilenameAt( time );
	params._quality  = this->_quality->getValue();
	params._premult  = this->_premult->getValue();
	return params;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void ImageMagickWriterPlugin::render( const OFX::RenderArguments& args )
{
	WriterPlugin::render( args );
	
	// instantiate the render code based on the pixel depth of the dst clip
	OFX::EBitDepth bitDepth         = _clipSrc->getPixelDepth();
	OFX::EPixelComponent components = _clipSrc->getPixelComponents();

	switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<ImageMagickWriterProcess, false, boost::gil::rgba_layout_t>( *this, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
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
}
