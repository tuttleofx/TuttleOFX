#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderProcess.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <magick/MagickCore.h>

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

#if ( MAGICKCORE_QUANTUM_DEPTH == 8 )
 #warning "MAGICKCORE_QUANTUM_DEPTH is 8"
#elif ( MAGICKCORE_QUANTUM_DEPTH == 16 )
 #warning "MAGICKCORE_QUANTUM_DEPTH is 16"
#elif ( MAGICKCORE_QUANTUM_DEPTH == 32 )
 #warning "MAGICKCORE_QUANTUM_DEPTH is 32"
#endif

namespace boost {
namespace gil  {

// RGBA
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*0*8, 8, true> rgba8_quantum_packed_channel0_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*1*8, 8, true> rgba8_quantum_packed_channel1_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*2*8, 8, true> rgba8_quantum_packed_channel2_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*3*8, 8, true> rgba8_quantum_packed_channel3_t;
typedef mpl::vector4<rgba8_quantum_packed_channel0_t, rgba8_quantum_packed_channel1_t,
                     rgba8_quantum_packed_channel2_t, rgba8_quantum_packed_channel3_t> rgba8_quantum_packed_channels_t;
typedef packed_pixel<uint64_t, rgba8_quantum_packed_channels_t, bgra_layout_t> bgra8_quantum_packed_pixel_t;
typedef view_type_from_pixel<bgra8_quantum_packed_pixel_t>::type bgra8_quantum_packed_view_t;
typedef image<bgra8_quantum_packed_pixel_t, false> bgra8_quantum_packed_image_t;

// RGB
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*0*8, 8, true> rgb8_quantum_packed_channel0_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*1*8, 8, true> rgb8_quantum_packed_channel1_t;
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*2*8, 8, true> rgb8_quantum_packed_channel2_t;
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
	: ImageGilProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void ImageMagickReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.time );
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
	readImage( this->_dstView, _plugin.getProcessParams( this->_renderArgs.time )._filepath );
}

template<class SView, class DView>
void copy_and_convert_from_buffer( Image* image, DView& dst, const bool flip )
{
	TUTTLE_COUT_VAR( sizeof( typename SView::value_type ) );
	//	boost::scoped_ptr<PixelPacket> buffer( GetImagePixels( image, 0, 0, dst.width(), dst.height() ) );
	PixelPacket* buffer = GetImagePixels( image, 0, 0, dst.width(), dst.height() );

	SView bufferView = interleaved_view( dst.width(), dst.height(),
	                                  ( typename SView::value_type* )( buffer ),
	                                  dst.width() * sizeof( typename SView::value_type ) ); //* sizeof(typename channel_type<SView>::type) );
	SView srcView;
	if( flip )
	{
		srcView = flipped_up_down_view( bufferView );
	}
	else
	{
		srcView = bufferView;
	}
	boost::gil::copy_and_convert_pixels( srcView, dst );
}

/**
 */
template<class View>
View& ImageMagickReaderProcess<View>::readImage( View& dst, const std::string& filepath )
{
	BOOST_STATIC_ASSERT( sizeof( Quantum ) == 2 ); // imagemagick compiled in 16 bits not 8 !

	TUTTLE_COUT_VAR( filepath );

	ImageInfo* imageInfo = AcquireImageInfo();
	GetImageInfo( imageInfo );
	strcpy( imageInfo->filename, filepath.c_str() );
	ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
	GetExceptionInfo( exceptionsInfo );

	Image* image = ReadImage( imageInfo, exceptionsInfo );

	CatchException( exceptionsInfo );

	unsigned long bitDepth = GetImageDepth( image, exceptionsInfo );
	
	switch( image->colorspace )
	{
		case RGBColorspace:
			TUTTLE_COUT( "RGBColorspace" );
			break;
		case GRAYColorspace:
			TUTTLE_COUT( "GRAYColorspace" );
			break;
		case TransparentColorspace:
			TUTTLE_COUT( "TransparentColorspace" );
			break;
		default:
			TUTTLE_COUT( "Particular colorspace: needs a conversion to RGB." );
	}

	if( image->colorspace != RGBColorspace &&
	    image->colorspace != GRAYColorspace &&
	    image->colorspace != TransparentColorspace )
	{
		TUTTLE_COUT( "Change colorspace to RGB." );
		SetImageColorspace( image, RGBColorspace );
	}

	QuantumType colorType = GetQuantumType( image, exceptionsInfo );

	switch( colorType )
	{
		case RGBQuantum:
		{
			switch( bitDepth )
			{
				case 8:
				{
					copy_and_convert_from_buffer<bgra8_quantum_packed_view_t, View>( image, dst, _params._flip );
					break;
				}
				case 16:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				case 32:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				default:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					TUTTLE_COUT( "Unknown combination of color type and bit depth (RGB, " + boost::lexical_cast<std::string>( bitDepth ) );
					//					BOOST_THROW_EXCEPTION( exception::Unsupported()
					//						<< exception::user( "Unknown combination of color type and bit depth (RGB, " + boost::lexical_cast<std::string>(bitDepth) ) );
					break;
				}
			}
			break;
		}
		case RGBAQuantum:
		{
			switch( bitDepth )
			{
				case 8:
				{
					copy_and_convert_from_buffer<bgra8_quantum_packed_view_t, View>( image, dst, _params._flip );
					break;
				}
				case 16:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				case 32:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				default:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					TUTTLE_COUT( "Unknown combination of color type and bit depth (RGBA, " + boost::lexical_cast<std::string>( bitDepth ) );
					//					BOOST_THROW_EXCEPTION( exception::Unsupported()
					//						<< exception::user( "Unknown combination of color type and bit depth (RGBA, " + boost::lexical_cast<std::string>(bitDepth) ) );
					break;
				}
			}
			break;
		}
		case AlphaQuantum:
		case GrayQuantum:
		{
			switch( bitDepth )
			{
				case 8:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				case 16:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				case 32:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					break;
				}
				default:
				{
					copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst, _params._flip );
					TUTTLE_COUT( "Unknown combination of color type and bit depth (Gray, " + boost::lexical_cast<std::string>( bitDepth ) );
					//					BOOST_THROW_EXCEPTION( exception::Unsupported()
					//						<< exception::user( "Unknown combination of color type and bit depth (Gray, " + boost::lexical_cast<std::string>(bitDepth) ) );
					break;
				}
			}
			break;
		}
		default:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
			    << exception::user( "Unknown color type (" + boost::lexical_cast<std::string>( colorType ) ) );
			break;
		}
	}

	image          = DestroyImage( image );
	imageInfo      = DestroyImageInfo( imageInfo );
	exceptionsInfo = DestroyExceptionInfo( exceptionsInfo );
	return dst;
}

}
}
}
}
