#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderProcess.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <Magick++.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>

/*
namespace boost {
namespace gil {

struct red2_t {};
struct green2_t {};
struct blue2_t {};
struct alpha2_t {};

/// \ingroup ColorSpaceModel
typedef boost::mpl::vector8<red_t,red2_t,green_t,green2_t,blue_t,blue2_t,alpha_t,alpha2_t> RrGgBbAa_t;
//typedef RrGgBbAa_t RrGgBbaa_t;
/// \ingroup LayoutModel
typedef layout<RrGgBbAa_t> RrGgBbAa_layout_t;
//typedef layout<RrGgBbaa_t> RrGgBbaa_layout_t;

GIL_DEFINE_ALL_TYPEDEFS(8, RrGgBbAa)
//GIL_DEFINE_ALL_TYPEDEFS(8, RrGgBbaa)


template<>
struct default_color_converter_impl<RrGgBbAa_t,rgb_t> {
    template <typename P1, typename P2>
    void operator()(const P1& src, P2& dst) const {
        typedef typename channel_type<P2>::type T2;
        pixel<T2,rgb_layout_t> tmp;
        get_color(dst,red_t())  =get_color(tmp,red_t());
        get_color(dst,green_t())=get_color(tmp,green_t());
        get_color(dst,blue_t()) =get_color(tmp,blue_t());
    }
};

template<>
struct default_color_converter_impl<RrGgBbAa_t,rgba_t> {
    template <typename P1, typename P2>
    void operator()(const P1& src, P2& dst) const {
        typedef typename channel_type<P2>::type T2;
        pixel<T2,rgb_layout_t> tmp;
        get_color(dst,red_t())  =get_color(tmp,red_t());
        get_color(dst,green_t())=get_color(tmp,green_t());
        get_color(dst,blue_t()) =get_color(tmp,blue_t());
        get_color(dst,alpha_t())=channel_convert<T2>(alpha_or_max(src));
    }
};

template<>
struct default_color_converter_impl<RrGgBbAa_t,RrGgBbAa_t> {
    template <typename P1, typename P2>
    void operator()(const P1& src, P2& dst) const {
        typedef typename channel_type<P2>::type T2;
        pixel<T2,rgb_layout_t> tmp;
        get_color(dst,red_t())  =get_color(tmp,red_t());
        get_color(dst,red2_t())  =get_color(tmp,red2_t());
        get_color(dst,green_t())=get_color(tmp,green_t());
        get_color(dst,green2_t())=get_color(tmp,green2_t());
        get_color(dst,blue_t()) =get_color(tmp,blue_t());
        get_color(dst,blue2_t()) =get_color(tmp,blue2_t());
    }
};

}
}
*/

namespace boost {
namespace gil  {

// RGBA
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*0*8, 8, true> rgba8_quantum_packed_channel0_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*1*8, 8, true> rgba8_quantum_packed_channel1_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*2*8, 8, true> rgba8_quantum_packed_channel2_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*3*8, 8, true> rgba8_quantum_packed_channel3_t;
typedef mpl::vector4<rgba8_quantum_packed_channel0_t, rgba8_quantum_packed_channel1_t,
                     rgba8_quantum_packed_channel2_t, rgba8_quantum_packed_channel3_t> rgba8_quantum_packed_channels_t;
typedef packed_pixel<uint64_t, rgba8_quantum_packed_channels_t, bgra_layout_t> bgra8_quantum_packed_pixel_t;
typedef view_type_from_pixel<bgra8_quantum_packed_pixel_t>::type bgra8_quantum_packed_view_t;
typedef image<bgra8_quantum_packed_pixel_t, false> bgra8_quantum_packed_image_t;

// RGB
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*0*8, 8, true> rgb8_quantum_packed_channel0_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*1*8, 8, true> rgb8_quantum_packed_channel1_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof(Magick::Quantum)*2*8, 8, true> rgb8_quantum_packed_channel2_t;
typedef mpl::vector3<rgb8_quantum_packed_channel0_t, rgb8_quantum_packed_channel1_t,
                     rgb8_quantum_packed_channel2_t> rgb8_quantum_packed_channels_t;
typedef packed_pixel<uint64_t, rgb8_quantum_packed_channels_t, bgr_layout_t> bgr8_quantum_packed_pixel_t;
typedef view_type_from_pixel<bgr8_quantum_packed_pixel_t>::type bgr8_quantum_packed_view_t;
typedef image<bgr8_quantum_packed_pixel_t, false> bgr8_quantum_packed_image_t;

}
}

namespace tuttle {
namespace plugin {
namespace imagemagick {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;

template<class View>
ImageMagickReaderProcess<View>::ImageMagickReaderProcess( ImageMagickReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void ImageMagickReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageMagickReaderProcessParams params = _plugin.getProcessParams(args.time);
	if( ! bfs::exists( params._filepath ) )
	{
		BOOST_THROW_EXCEPTION( OFX::Exception::Suite(kOfxStatFailed, std::string("Unable to open : ") + params._filepath ) );
	}

	ImageGilProcessor<View>::setup( args );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void ImageMagickReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	readImage( this->_dstView, _plugin.getProcessParams(this->_renderArgs.time)._filepath );
}



template<class SView, class DView>
void copy_and_convert_from_buffer( Magick::Image& image, DView& dst )
{
	COUT_VAR( sizeof(typename SView::value_type) );
	Magick::PixelPacket* buffer = image.getPixels( 0, 0, dst.width(), dst.height() );
	SView imgView = interleaved_view( dst.width(), dst.height(),
									 (typename SView::value_type*)( buffer ),
									 dst.width() * sizeof(typename SView::value_type) ); //* sizeof(typename channel_type<SView>::type) );
	boost::gil::copy_and_convert_pixels( imgView, dst );
	
////	Magick::ExceptionInfo *exception;
////	MagickCore::CacheView* image_view = AcquireCacheView(image);
//	const Magick::PixelPacket* src_pix = image.getPixels( 0, 0, dst.width(), dst.height() );
//	for( int y = 0;
//			 y < dst.height();
//			 ++y )
//	{
//		typename DView::x_iterator dst_it = dst.x_at( 0, y );
////		const Magick::PixelPacket* src_pix = GetCacheViewVirtualPixels(image_view,0,y,image->columns,1 /*,exception*/);
//		for( int x = 0;
//			 x < dst.width();
//			 ++x, ++src_pix, ++dst_it )
//		{
//			get_color( (*dst_it), red_t() ) = src_pix->red;
//			get_color( (*dst_it), green_t() ) = src_pix->green;
//			get_color( (*dst_it), blue_t() ) = src_pix->blue;
////			get_color( (*dst_it), alpha_t() ) = src_pix->alpha;
//		}
//	}
////	image_view = DestroyCacheView(image_view);

}


/**
 */
template<class View>
View& ImageMagickReaderProcess<View>::readImage( View& dst, const std::string& filepath ) throw( PluginException )
{
	BOOST_STATIC_ASSERT( sizeof(Magick::Quantum) == 2 ); // imagemagick compiled in 16 bits not 8 !
//	BOOST_ASSERT( dst.is_1d_traversable() );
	try
	{
//		Magick::Blob bl( &((*dst.begin())[0]), dst.height() * dst.pixels().row_size() );
//		COUT_VAR( dst.size() );
//		COUT_VAR( dst.width() );
//		COUT_VAR( dst.height() );
//		Magick::Blob bl( &((*dst.begin())[0]), dst.size() );
//		Magick::Image image( bl, Magick::Geometry(dst.width(),dst.height()) );

		// Read a file into image object
//		image.read( filepath );

		Magick::Image image;
		image.magick("RGBA");
		image.depth( sizeof(typename channel_type<View>::type) * 8 );
		image.read( filepath );

//		Magick::ImageType image_type = image.type(); // is BilevelType
		Magick::ColorspaceType colorType = image.colorSpace(); // is RGBColorspace
		COUT_VAR( sizeof(typename channel_type<View>::type) );
		COUT_VAR( num_channels<View>::value );
		unsigned int bitDepth = image.depth();
		COUT_VAR( bitDepth );
		
		COUT_VAR( sizeof(Magick::Quantum) );
		COUT_VAR( sizeof(Magick::PixelPacket) );

//		Magick::PixelPacket* buffer = image.getPixels( 0, 0, dst.width(), dst.height() );

//		if( colorType != Magick::RGBColorspace &&
//		    colorType != Magick::GRAYColorspace &&
//		    colorType != Magick::TransparentColorspace
//			)
//		{
//			TransformImageColorspace( &image, Magick::RGBColorspace );
//			colorType = image.colorSpace();
//		}

		switch( colorType )
		{
			case Magick::RGBColorspace:
			{
				if( image.matte() == Magick::MagickFalse )
				{
					switch( bitDepth )
					{
						case 8:
						{
							copy_and_convert_from_buffer<bgra8_quantum_packed_view_t, View>( image, dst );
							break;
						}
						case 16:
						{
							copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
							break;
						}
						case 32:
//						{
							copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
							break;
//						}
						default:
						{
							BOOST_THROW_EXCEPTION( PluginException( "Unknown combination of color type and bit depth (RGB, " + boost::lexical_cast<std::string>(bitDepth) ) );
							break;
						}
					}
				}
				else
				{
					switch( bitDepth )
					{
						case 8:
						{
							copy_and_convert_from_buffer<bgra8_quantum_packed_view_t, View>( image, dst );
							break;
						}
						case 16:
						{
							copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
							break;
						}
						case 32:
//						{
							copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
							break;
//						}
						default:
						{
							BOOST_THROW_EXCEPTION( PluginException( "Unknown combination of color type and bit depth (RGBA, " + boost::lexical_cast<std::string>(bitDepth) ) );
							break;
						}
					}
				}
			}
			case Magick::TransparentColorspace:
			case Magick::GRAYColorspace:
			{
				switch( bitDepth )
				{
					case 8:
					{
						copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
						break;
					}
					case 16:
					{
						copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
						break;
					}
					case 32:
					{
						copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
						break;
					}
					default:
					{
						BOOST_THROW_EXCEPTION( PluginException( "Unknown combination of color type and bit depth (RGBA, " + boost::lexical_cast<std::string>(bitDepth) ) );
						break;
					}
				}
				break;
			}
			default:
			{
				BOOST_THROW_EXCEPTION( PluginException( "Unknown color type (" + boost::lexical_cast<std::string>(colorType) ) );
				break;
			}
        }
//		copy_and_convert_pixels( subimage_view( flipped_up_down_view( view( anyImg ) ), 0, 0, dst.width(), dst.height() ), dst );
	}
	catch( Magick::WarningCoder& warning )
	{
		// Process coder warning while loading file (e.g. TIFF warning)
		// Maybe the user will be interested in these warnings (or not).
		// If a warning is produced while loading an image, the image
		// can normally still be used (but not if the warning was about
		// something important!)
		COUT( "ImageMagick Coder Warning: " << warning.what() );
	}
	catch( Magick::Warning& warning )
	{
		// Handle any other Magick++ warning.
		COUT( "ImageMagick Warning: " << warning.what() );
	}
	catch( Magick::ErrorBlob& e )
	{
		// Process Magick++ file open error
		BOOST_THROW_EXCEPTION( PluginException( /*kOfxStatErrFatal,*/ std::string("ImageMagick Error blob: ") + e.what() ) );
	}
	catch( Magick::Exception& e )
	{
		// Process Magick++ file open error
		BOOST_THROW_EXCEPTION( PluginException( /*kOfxStatErrFatal,*/ std::string("ImageMagick: ") + e.what() ) );
	}
	catch( std::exception& e )
	{
		// Process Magick++ file open error
		BOOST_THROW_EXCEPTION( PluginException( /*kOfxStatErrFatal,*/ std::string("std: ") + e.what() ) );
	}
	catch(...)
	{
		BOOST_THROW_EXCEPTION( PluginException( "kOfxStatErrUnknown" ) );
	}
	return dst;
}

}
}
}
}
