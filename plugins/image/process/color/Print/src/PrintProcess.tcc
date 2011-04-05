#include "PrintAlgorithm.hpp"
#include "PrintPlugin.hpp"

#include <boost/gil/extension/numeric/pixel_by_channel.hpp>

#include <caca.h>

#include <cstdio>

namespace tuttle {
namespace plugin {
namespace print {

struct CacaImage
{
    char *pixels;
    unsigned int w, h;
    caca_dither_t *dither;
};

template<class SView>
CacaImage load_cacaimage_from_view( const SView sView )
{
	typedef typename boost::gil::channel_mapping_type<SView>::type Channel;
	typedef typename boost::gil::channel_base_type<Channel>::type ChannelBaseType;

	CacaImage im;
	unsigned int bpp, rmask, gmask, bmask, amask;

	im.pixels = (char*)boost::gil::interleaved_view_get_raw_data( sView );
	im.w = sView.width();
	im.h = sView.height();
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
	bpp = boost::gil::num_channels<SView>::value * sizeof( ChannelBaseType ) * 8;

	// Create the libcaca dither
	im.dither = caca_create_dither( bpp, im.w, im.h, sView.pixels().row_size(),
									 rmask, gmask, bmask, amask);
	if( ! im.dither )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown()
				<< exception::dev( "Unable to load buffer." ) );
	}
	return im;
}

template<typename Channel>
struct channel_cout_t : public std::unary_function<Channel,Channel> {
	GIL_FORCEINLINE
    Channel operator()(typename boost::gil::channel_traits<Channel>::const_reference ch) const
	{
		std::cout << ch << " ";
        return ch;
    }
};

template<template<class> class Func>
struct call_pixel_by_channel_t
{
	template<typename Pixel>
	GIL_FORCEINLINE
	Pixel operator()( const Pixel& v ) const
	{
		static_for_each(
			v,
			Func<typename boost::gil::channel_type<Pixel>::type>()
			);
		return v;
	}
};

template<class View>
PrintProcess<View>::PrintProcess( PrintPlugin &effect )
: ImageGilFilterProcessor<View>( effect )
, _plugin( effect )
{
	this->setNoMultiThreading();
}

template<class View>
void PrintProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.renderScale );

}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window
 */
template<class View>
void PrintProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	OfxRectI procWindowOutput = this->translateRoWToOutputClipCoordinates( procWindowRoW );

	const OfxRectI procWindowSrc = translateRegion( procWindowRoW, this->_srcPixelRod );
//	OfxPointI procWindowSize = { procWindowRoW.x2 - procWindowRoW.x1,
//								 procWindowRoW.y2 - procWindowRoW.y1 };
//	View src = subimage_view( this->_srcView, procWindowSrc.x1, procWindowSrc.y1,
//											  procWindowSize.x, procWindowSize.y );
//	View dst = subimage_view( this->_dstView, procWindowOutput.x1, procWindowOutput.y1,
//											  procWindowSize.x, procWindowSize.y );
	View src = this->_srcView;
	View dst = this->_dstView;

	OfxRectI region = procWindowSrc;

	copy_pixels( src, dst );

	switch( _params._mode )
	{
		case eParamModePixel:
		{
			call_pixel_by_channel_t<channel_cout_t>()( src( _params._pixel ) );
			std::cout << std::endl;
			break;
		}
		case eParamModeRegion:
		{
			src = subimage_view( this->_srcView, _params._region.x1, _params._region.y1,
											     _params._region.x2 - _params._region.x1,
											     _params._region.y2 - _params._region.y1 );
			region = _params._region;
			// continue with #src as a part of the image...
		}
		case eParamModeImage:
		{
			switch( _params._output )
			{
				case eParamOutputNumeric:
				{
					std::cout << std::fixed;
					std::cout << std::setprecision( 2 );
					call_pixel_by_channel_t<channel_cout_t> proc;
					for( int y = region.y1;
							 y < region.y2;
							 ++y )
					{
						std::cout << "| ";
						typename View::x_iterator src_it = this->_srcView.x_at( region.x1, y );
						for( int x = region.x1;
							 x < region.x2;
							 ++x, ++src_it )
						{
							proc( *src_it );
							std::cout << " | ";
						}
						std::cout << "\n";
						if( this->progressForward() )
							return;
					}
					break;
				}
				case eParamOutputAscii:
				{
					// temporary gray buffer to compute the char values.
					gray8_image_t gImg( src.dimensions() );
					gray8_view_t gView( view(gImg) );
					if( _params._flip )
					{
						src = flipped_up_down_view( src );
					}
					copy_and_convert_pixels( src, gView );

					// libcaca context
					caca_canvas_t *cv = NULL;

					try
					{
						void *output;
						size_t len;
						struct CacaImage cacaImg;
						unsigned int cols = 0, lines = 0, font_width = 6, font_height = 10;
						char *format = NULL;
						char *dither = NULL;
						float gamma = -1, brightness = -1, contrast = -1;

						cv = caca_create_canvas(0, 0);
						if( !cv )
						{
							TUTTLE_CERR( "Unable to initialise libcaca." );
							return;
						}

						cacaImg = load_cacaimage_from_view( gView );
						/*
						 *  - \c "mono": use light gray on a black background.
						 *  - \c "gray": use white and two shades of gray on a black background.
						 *  - \c "8": use the 8 ANSI colours on a black background.
						 *  - \c "16": use the 16 ANSI colours on a black background.
						 *  - \c "fullgray": use black, white and two shades of gray for both the
						 *    characters and the background.
						 *  - \c "full8": use the 8 ANSI colours for both the characters and the
						 *    background.
						 *  - \c "full16" or \c "default": use the 16 ANSI colours for both the
						 *    characters and the background. This is the default value.
						 */
						caca_set_dither_color( cacaImg.dither, "8" );
						//    i = load_image(argv[argc-1]);

						if( !cols && !lines )
						{
							cols = 60;
							lines = cols * cacaImg.h * font_width / cacaImg.w / font_height;
						}
						else if( cols && !lines )
						{
							lines = cols * cacaImg.h * font_width / cacaImg.w / font_height;
						}
						else if( !cols && lines )
						{
							cols = lines * cacaImg.w * font_height / cacaImg.h / font_width;
						}

						caca_set_canvas_size( cv, cols, lines );
				//	    caca_set_color_ansi( cv, CACA_DEFAULT, CACA_TRANSPARENT );
				//		caca_set_color_ansi( cv, CACA_WHITE, CACA_BLACK );
						caca_clear_canvas( cv );
						if( caca_set_dither_algorithm( cacaImg.dither, dither?dither:"fstein" ) )
						{
							BOOST_THROW_EXCEPTION( exception::Unknown()
									<< exception::dev() + "Can't dither image with algorithm " + dither );
						}

						if( brightness != -1 )
							caca_set_dither_brightness( cacaImg.dither, brightness );
						if( contrast != -1 )
							caca_set_dither_contrast( cacaImg.dither, contrast );
						if( gamma != -1 )
							caca_set_dither_gamma( cacaImg.dither, gamma );

						caca_dither_bitmap( cv, 0, 0, cols, lines, cacaImg.dither, cacaImg.pixels );

					//	unload_image(i);
						caca_free_dither( cacaImg.dither );

						output = caca_export_canvas_to_memory( cv, format ? format:"ansi", &len );
						if( !output )
						{
							BOOST_THROW_EXCEPTION( exception::Unknown()
									<< exception::dev() + "Can't export to format " + format );
						}

						fwrite( output, len, 1, stdout );
						free( output );

						caca_free_canvas( cv );
					}
					catch(...)
					{
						TUTTLE_COUT_CURRENT_EXCEPTION;
						caca_free_canvas( cv );
					}
					break;
				}
			}
			break;
		}
	}
}


}
}
}
