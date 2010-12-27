
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

template<template<class> class Process, class Layout, class Bits, class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args )
{
	typedef boost::gil::pixel<Bits, Layout> Pixel;
	typedef boost::gil::image<Pixel, false> Image;
	typedef typename Image::view_t View;

	Process<View> procObj( plugin );

	procObj.setupAndProcess( args );
}

template<template<class> class Process, class Layout, class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth bitDepth )
{
    switch( bitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			doGilRender<Process, Layout, boost::gil::bits8>( plugin, args );
			return;
		}
		case OFX::eBitDepthUShort:
		{
			doGilRender<Process, Layout, boost::gil::bits16>( plugin, args );
			return;
		}
		case OFX::eBitDepthFloat:
		{
			doGilRender<Process, Layout, boost::gil::bits32f>( plugin, args );
			return;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(bitDepth) + ") not recognized by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template<template<class> class Process, class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent components, const OFX::EBitDepth bitDepth )
{
    switch( components )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<Process, boost::gil::rgba_layout_t>( plugin, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender<Process, boost::gil::rgb_layout_t>( plugin, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		{
			doGilRender<Process, boost::gil::gray_layout_t>( plugin, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel components (" + mapPixelComponentEnumToString(components) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

/**
 * @brief This render function, instanciate a Process class templated with the image type (layout and bit depth).
 * @param[in]   args     Rendering parameters
 */
template<template<class> class Process, class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	doGilRender<Process,Plugin>( plugin, args, plugin._clipDst->getPixelComponents(), plugin._clipDst->getPixelDepth() );
}

}
}


