#include "ImageMagickReaderDefinitions.hpp"
#include "ImageMagickReaderProcess.hpp"

#include <terry/globals.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <magick/MagickCore.h>

#include <boost/gil/gil_all.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assert.hpp>


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

// Gray
typedef const packed_channel_reference<boost::uint64_t, sizeof( Quantum )*0*8, 8, true> gray8_quantum_packed_channel0_t;
typedef mpl::vector<rgb8_quantum_packed_channel0_t> gray8_quantum_packed_channels_t;
typedef packed_pixel<uint64_t, gray8_quantum_packed_channels_t, bgr_layout_t> gray8_quantum_packed_pixel_t;
typedef view_type_from_pixel<gray8_quantum_packed_pixel_t>::type gray8_quantum_packed_view_t;
typedef image<gray8_quantum_packed_pixel_t, false> gray8_quantum_packed_image_t;

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
	: ImageGilProcessor<View>( instance, eImageOrientationFromTopToBottom )
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
void copy_and_convert_from_buffer( Image* image, DView& dst )
{
	ExceptionInfo exception;
	PixelPacket* buffer =  GetAuthenticPixels( image, 0, 0, dst.width(), dst.height(), &exception );

	SView bufferView = interleaved_view( dst.width(), dst.height(),
					  ( typename SView::value_type* )( buffer ),
					  dst.width() * sizeof( typename SView::value_type ) );

	boost::gil::copy_and_convert_pixels( bufferView, dst );
}

/**
 */
template<class View>
View& ImageMagickReaderProcess<View>::readImage( View& dst, const std::string& filepath )
{
	BOOST_STATIC_ASSERT( sizeof( Quantum ) == 2 ); // imagemagick compiled in 16 bits not 8 !

	//TUTTLE_COUT_VAR( filepath );

	ImageInfo* imageInfo = AcquireImageInfo();
	GetImageInfo( imageInfo );
	strcpy( imageInfo->filename, filepath.c_str() );
	ExceptionInfo* exceptionsInfo = AcquireExceptionInfo();
	GetExceptionInfo( exceptionsInfo );

	Image* image = ReadImage( imageInfo, exceptionsInfo );

	CatchException( exceptionsInfo );

	if( image->colorspace != RGBColorspace &&
	   // image->colorspace != GRAYColorspace &&
	    image->colorspace != TransparentColorspace )
	{
		TUTTLE_COUT( " Change colorspace to RGB." );
		SetImageColorspace( image, RGBColorspace );
	}


	QuantumType colorType = GetQuantumType( image, exceptionsInfo );

	switch( colorType )
	{
		case RGBQuantum:
		{
			rgb16_image_t tmpImg ( dst.width(), dst.height() );
			rgb16_view_t tmpVw = view( tmpImg );

			copy_and_convert_from_buffer<bgr8_quantum_packed_view_t, rgb16_view_t>( image, tmpVw );
			copy_and_convert_pixels( tmpVw, dst );
			break;
		}
		case RGBAQuantum:
		{

			copy_and_convert_from_buffer<bgra16_view_t, View>( image, dst );
			break;
		}
		case AlphaQuantum:
		case GrayQuantum:
		{
			rgba16_image_t tmpImg ( dst.width(), dst.height() );
			rgba16_view_t tmpVw = view( tmpImg );

			copy_and_convert_from_buffer<bgra8_quantum_packed_view_t, rgba16_view_t>( image, tmpVw );

			copy_and_convert_pixels( boost::gil::nth_channel_view ( tmpVw, 4), dst );
			//copy_and_convert_from_buffer<gray32_view_t, View>( image, dst );
			break;
		}
		default:
		{
			BOOST_THROW_EXCEPTION( exception::Unsupported()
			    << exception::user( "Unknown color type (" + boost::lexical_cast<std::string>( colorType )+")" ) );
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
