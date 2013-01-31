
#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {

template< template<class> class Process,
          bool planar, class Layout, class Bits,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args )
{
	typedef boost::gil::pixel<Bits, Layout> Pixel;
	typedef boost::gil::image<Pixel, planar> Image;
	typedef typename Image::view_t View;

	Process<View> procObj( plugin );

	procObj.setupAndProcess( args );
}

template< template<class> class Process,
          bool planar, class Layout,
          class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth bitDepth )
{
    switch( bitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			doGilRender<Process, planar, Layout, boost::gil::bits8>( plugin, args );
			return;
		}
		case OFX::eBitDepthUShort:
		{
			doGilRender<Process, planar, Layout, boost::gil::bits16>( plugin, args );
			return;
		}
		case OFX::eBitDepthFloat:
		{
			doGilRender<Process, planar, Layout, boost::gil::bits32f>( plugin, args );
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

template< template<class> class Process,
          bool planar,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent component, const OFX::EBitDepth bitDepth )
{
    switch( component )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender<Process, planar, boost::gil::rgba_layout_t>( plugin, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender<Process, planar, boost::gil::rgb_layout_t>( plugin, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		{
			doGilRender<Process, planar, boost::gil::gray_layout_t>( plugin, args, bitDepth );
			return;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel component (" + mapPixelComponentEnumToString(component) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class> class Process,
          class Plugin >
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args, const OFX::Clip& clip )
{
//	if( ! clip.isPlanar() )
	doGilRender<Process, false, Plugin>( plugin, args, clip.getPixelComponents(), clip.getPixelDepth() );
}

/**
 * @brief This render function, instanciate a Process class templated with the image type (layout and bit depth).
 * @param[in]   args     Rendering parameters
 */
template<template<class> class Process, class Plugin>
void doGilRender( Plugin& plugin, const OFX::RenderArguments& args )
{
	// instantiate the render code based on the pixel depth of the dst clip
	doGilRender<Process, Plugin>( plugin, args, *plugin._clipDst );
}


template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
		  bool dPlanar, class DLayout, class DBits,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args )
{
	typedef boost::gil::pixel<SBits, SLayout> SPixel;
	typedef boost::gil::image<SPixel, sPlanar> SImage;
	typedef typename SImage::view_t SView;

	typedef boost::gil::pixel<DBits, DLayout> DPixel;
	typedef boost::gil::image<DPixel, dPlanar> DImage;
	typedef typename DImage::view_t DView;

	Process<SView, DView> procObj( plugin );

	procObj.setupAndProcess( args );
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
          bool dPlanar, class DLayout,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth dBitDepth )
{
    switch( dBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			doGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, DLayout, boost::gil::bits8>( plugin, args );
			return;
		}
		case OFX::eBitDepthUShort:
		{
			doGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, DLayout, boost::gil::bits16>( plugin, args );
			return;
		}
		case OFX::eBitDepthFloat:
		{
			doGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, DLayout, boost::gil::bits32f>( plugin, args );
			return;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(dBitDepth) + ") not recognized by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
          bool dPlanar,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
    switch( dComponent )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, boost::gil::rgba_layout_t>( plugin, args, dBitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, boost::gil::rgb_layout_t>( plugin, args, dBitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		{
			doGilRender2<Process, sPlanar, SLayout, SBits, dPlanar, boost::gil::gray_layout_t>( plugin, args, dBitDepth );
			return;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel component (" + mapPixelComponentEnumToString(dComponent) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout, class SBits,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
	if( dPlanar )
	{
//		doGilRender2<Process, sPlanar, SLayout, SBits, true>( plugin, args, dComponent, dBitDepth );
	}
	else
	{
		doGilRender2<Process, sPlanar, SLayout, SBits, false>( plugin, args, dComponent, dBitDepth );
	}
}

template< template<class,class> class Process,
          bool sPlanar, class SLayout,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EBitDepth sBitDepth, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
    switch( sBitDepth )
	{
		case OFX::eBitDepthUByte:
		{
			doGilRender2<Process, sPlanar, SLayout, boost::gil::bits8>( plugin, args, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::eBitDepthUShort:
		{
			doGilRender2<Process, sPlanar, SLayout, boost::gil::bits16>( plugin, args, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::eBitDepthFloat:
		{
			doGilRender2<Process, sPlanar, SLayout, boost::gil::bits32f>( plugin, args, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::eBitDepthCustom:
		case OFX::eBitDepthNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Bit depth (" + mapBitDepthEnumToString(sBitDepth) + ") not recognized by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}

template< template<class,class> class Process,
          bool sPlanar,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::EPixelComponent sComponent, const OFX::EBitDepth sBitDepth, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
    switch( sComponent )
	{
		case OFX::ePixelComponentRGBA:
		{
			doGilRender2<Process, sPlanar, boost::gil::rgba_layout_t>( plugin, args, sBitDepth, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::ePixelComponentRGB:
		{
			doGilRender2<Process, sPlanar, boost::gil::rgb_layout_t>( plugin, args, sBitDepth, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::ePixelComponentAlpha:
		{
			doGilRender2<Process, sPlanar, boost::gil::gray_layout_t>( plugin, args, sBitDepth, dPlanar, dComponent, dBitDepth );
			return;
		}
		case OFX::ePixelComponentCustom:
		case OFX::ePixelComponentNone:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
				<< exception::user() + "Pixel component (" + mapPixelComponentEnumToString(sComponent) + ") not supported by the plugin." );
		}
	}
	BOOST_THROW_EXCEPTION( exception::Unknown() );
}


template< template<class,class> class Process,
          class Plugin >
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const bool sPlanar, const OFX::EPixelComponent sComponent, const OFX::EBitDepth sBitDepth, const bool dPlanar, const OFX::EPixelComponent dComponent, const OFX::EBitDepth dBitDepth )
{
	if( sPlanar )
	{
//		doGilRender2<Process, true>( plugin, args, sComponent, sBitDepth, dPlanar, dComponent, dBitDepth );
	}
	else
	{
		doGilRender2<Process, false>( plugin, args, sComponent, sBitDepth, dPlanar, dComponent, dBitDepth );
	}
}

template< template<class,class> class Process,
          class Plugin>
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args, const OFX::Clip& sClip, const OFX::Clip& dClip )
{
	doGilRender2<Process, Plugin>( plugin, args, false, sClip.getPixelComponents(), sClip.getPixelDepth(), false, dClip.getPixelComponents(), dClip.getPixelDepth() );
}

/**
 * @brief This render function, instanciate a Process class templated with the image type (layout and bit depth).
 * @param[in]   args     Rendering parameters
 */
template<template<class,class> class Process, class Plugin>
void doGilRender2( Plugin& plugin, const OFX::RenderArguments& args )
{
	doGilRender2<Process, Plugin>( plugin, args, *plugin._clipSrc, *plugin._clipDst );
}

template< template<class,class,class> class Process,
          bool s1planar, class S1Layout, class S1Bits,
          bool s2planar, class S2Layout, class S2Bits,
		  bool dPlanar, class DLayout, class DBits,
          class Plugin>
void doGilRender3( Plugin& plugin, const OFX::RenderArguments& args )
{
	typedef boost::gil::pixel<S1Bits, S1Layout> S1Pixel;
	typedef boost::gil::image<S1Pixel, s1planar> S1Image;
	typedef typename S1Image::view_t S1View;

	typedef boost::gil::pixel<S2Bits, S2Layout> S2Pixel;
	typedef boost::gil::image<S2Pixel, s2planar> S2Image;
	typedef typename S2Image::view_t S2View;

	typedef boost::gil::pixel<DBits, DLayout> DPixel;
	typedef boost::gil::image<DPixel, dPlanar> DImage;
	typedef typename DImage::view_t DView;

	Process<S1View, S2View, DView> procObj( plugin );

	procObj.setupAndProcess( args );
}


}
}


