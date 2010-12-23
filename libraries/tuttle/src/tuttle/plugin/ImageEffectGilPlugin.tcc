#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

ImageEffectGilPlugin::ImageEffectGilPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );
}

ImageEffectGilPlugin::~ImageEffectGilPlugin()
{}

template<class Plugin, template<class> class Process, class Layout, class Bits>
OfxStatus ImageEffectGilPlugin::doGilRender( const OFX::RenderArguments &args )
{
	typedef boost::gil::pixel<Bits, Layout> Pixel;
	typedef boost::gil::image<Pixel, false> Image;
	typedef typename Image::view_t View;

	Process<View> procObj( dynamic_cast<Plugin&>(*this) );
	return procObj.setupAndProcess( args );
}

template<class Plugin, template<class> class Process, class Layout>
OfxStatus ImageEffectGilPlugin::doGilRender( const OFX::EBitDepth bitDepth, const OFX::RenderArguments &args )
{
    switch( bitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			return doGilRender<Layout, boost::gil::bits8>( args );
		}
		case OFX::eBitDepthUShort:
		{
			return doGilRender<Layout, boost::gil::bits16>( args );
		}
		case OFX::eBitDepthFloat:
		{
			return doGilRender<Layout, boost::gil::bits32f>( args );
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
		{
			COUT_ERROR( "Bit depth (" << mapBitDepthEnumToString(bitDepth) << ") not recognized by the plugin." );
			return kOfxStatErrUnsupported;
		}
	}
	return kOfxStatErrUnknown;
}

template<class Plugin, template<class> class Process>
OfxStatus ImageEffectGilPlugin::doGilRender( const OFX::EPixelComponent components, const OFX::EBitDepth bitDepth, const OFX::RenderArguments &args )
{
    switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			return doGilRender<boost::gil::rgba_layout_t>( bitDepth, args );
		}
		case OFX::ePixelComponentRGB:
		{
			return doGilRender<boost::gil::rgb_layout_t>( bitDepth, args );
		}
		case OFX::ePixelComponentAlpha:
		{
			return doGilRender<boost::gil::gray_layout_t>( bitDepth, args );
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			COUT_ERROR( "Pixel components (" << mapPixelComponentEnumToString(components) << ") not supported by the plugin." );
			return kOfxStatErrUnsupported;
		}
	}
	return kOfxStatErrUnknown;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
template<class Plugin, template<class> class Process>
OfxStatus ImageEffectGilPlugin::doGilRender( const OFX::RenderArguments &args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	return doGilRender( _clipDst->getPixelComponents(), _clipDst->getPixelDepth(), args );
}

}
}
