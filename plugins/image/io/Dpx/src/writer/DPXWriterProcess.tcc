#include "DPXWriterDefinitions.hpp"
#include "DPXWriterPlugin.hpp"

#include <terry/clamp.hpp>
#include <terry/typedefs.hpp>

#include <boost/exception/errinfo_file_name.hpp>
#include <boost/assert.hpp>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace dpx {
namespace writer {

using namespace boost::gil;

template<class View>
DPXWriterProcess<View>::DPXWriterProcess( DPXWriterPlugin& instance )
	: ImageGilFilterProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void DPXWriterProcess<View>::setup( const OFX::RenderArguments& args )
{
	using namespace boost::gil;
	ImageGilFilterProcessor<View>::setup( args );
	_params = _plugin.getProcessParams( args.time );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void DPXWriterProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	// no tiles and no multithreading supported
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	BOOST_ASSERT( this->_srcPixelRod == this->_dstPixelRod );

	View src = this->_srcView;

	try
	{
		switch( _params._bitDepth )
		{
			case eTuttlePluginBitDepth16:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsRGB:
					{
						writeImage<rgb16_image_t>( src, _params._filepath, 16, tuttle::io::DpxImage::eCompTypeR16G16B16, _params._packed );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						writeImage<rgba16_image_t>( src, _params._filepath, 16, tuttle::io::DpxImage::eCompTypeR16G16B16A16, _params._packed );
						break;
					}
					case eTuttlePluginComponentsABGR:
					{
						writeImage<abgr16_image_t>( src, _params._filepath, 16, tuttle::io::DpxImage::eCompTypeA16B16G16R16, _params._packed );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Dpx Writer: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth12:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsRGB:
					{
						//write10bitsImage<rgb12_stream_ptr_t>( src, _params._filepath, 12, tuttle::io::DpxImage::eCompTypeR12G12B12, _params._packed );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						//write10bitsImage<rgba12_stream_ptr_t>( src, _params._filepath, 12, tuttle::io::DpxImage::eCompTypeR12G12B12A12, _params._packed );
						break;
					}
					case eTuttlePluginComponentsABGR:
					{
						//write10bitsImage<abgr12_stream_ptr_t>( src, _params._filepath, 12, tuttle::io::DpxImage::eCompTypeA12B12G12R12, _params._packed );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Dpx Writer: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth10:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsRGB:
					{
						write10bitsImage<rgb10_stream_ptr_t>( src, _params._filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10, _params._packed );
						//writeImage<rgb10_image_t>( src, _params._filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10A10, _params._packed );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						//write10bitsImage<rgba10_stream_ptr_t>( src, _params._filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10A10, _params._packed );
						writeImage<rgba10_image_t>( src, _params._filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10A10, _params._packed );
						break;
					}
					case eTuttlePluginComponentsABGR:
					{
						//write10bitsImage<abgr10_stream_ptr_t>( src, _params._filepath, 10, tuttle::io::DpxImage::eCompTypeA10B10G10R10, _params._packed );
						writeImage<abgr10_image_t>( src, _params._filepath, 10, tuttle::io::DpxImage::eCompTypeR10G10B10A10, _params._packed );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Dpx Writer: components not supported" ) );
				}
				break;
			}
			case eTuttlePluginBitDepth8:
			{
				switch( _params._componentsType )
				{
					case eTuttlePluginComponentsRGB:
					{
						writeImage<rgb8_image_t>( src, _params._filepath, 8, tuttle::io::DpxImage::eCompTypeR8G8B8, _params._packed );
						break;
					}
					case eTuttlePluginComponentsRGBA:
					{
						writeImage<rgba8_image_t>( src, _params._filepath, 8, tuttle::io::DpxImage::eCompTypeR8G8B8A8, _params._packed );
						break;
					}
					case eTuttlePluginComponentsABGR:
					{
						writeImage<abgr8_image_t>( src, _params._filepath, 8, tuttle::io::DpxImage::eCompTypeA8B8G8R8, _params._packed );
						break;
					}
					default:
						BOOST_THROW_EXCEPTION( exception::Unsupported()
						    << exception::user( "Dpx Writer: components not supported" ) );
				}
				break;
			}
			default:
				BOOST_THROW_EXCEPTION( exception::Unsupported()
					    << exception::user( "DPX Writer: Unsupported bitdepth..." ) );
		}
	}
	catch( exception::Common& e )
	{
		e << exception::filename( _params._filepath );
		throw;
	}
	catch(... )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown()
			<< exception::user( "Unable to write image" )
			<< exception::dev( boost::current_exception_diagnostic_information() )
			<< exception::filename( _params._filepath ) );
	}
	copy_pixels( this->_srcView, this->_dstView );
}


template<class View>
template<class SRC_V, class T>
boost::uint8_t* DPXWriterProcess<View>::viewToBitStream( SRC_V& src, const int nChannels, const int channelSize )
{
	typedef unsigned char byte_t;
	int width             = src.width();
	int height            = src.height();
	int scanline_in_bits  = width * nChannels * channelSize;
	int scanline_in_bytes = scanline_in_bits / 8;
	scanline_in_bytes += ( scanline_in_bits % 8 != 0 ) ? 1 : 0;

	boost::uint8_t* pData = new boost::uint8_t[ scanline_in_bytes * height ];

	T p( pData, 0 );

	for( typename SRC_V::y_coord_t y = 0; y < height; ++y )
	{
		typename SRC_V::x_iterator it = src.row_begin( y );

		for( typename SRC_V::x_coord_t x = 0; x < width; ++x )
		{
			color_convert( *it , *p );
			++p;
			++it;
		}
	}
	return pData;
}

/*
template<class View>
template<class SRC_V, class T>
boost::uint8_t* DPXWriterProcess<View>::viewToBitStream( SRC_V& src, const int nChannels, const int channelSize )
{
	typedef unsigned char byte_t;
	int width             = src.width();
	int height            = src.height();
	int scanline_in_bits  = width * nChannels * channelSize;
	int scanline_in_bytes = scanline_in_bits / 8;
	scanline_in_bytes += ( scanline_in_bits % 8 != 0 ) ? 1 : 0;

	boost::uint8_t* pData = new boost::uint8_t[ scanline_in_bytes * height ];

	T p( pData, 0 );

	rgba10_image_t tmpView( src.dimensions() );
	copy_and_convert_pixels( src, view(tmpView));

	TUTTLE_COUT( tmpView.width() );

	for( typename rgba10_image_t::y_coord_t y = 0; y < height; ++y )
	{
	//	typename rgba10_image_t::x_iterator it = tmpView.row_begin( y );
	//	for( typename rgba10_image_t::x_coord_t x = 0; x < width; ++x )
	//	{
	//		//color_convert( *it , tmpPixel );
	//		++p;
	//		++it;
	//	}
	}
	return pData;
}
*/
/**
 */
template<class View>
template<class WImage>
void DPXWriterProcess<View>::write10bitsImage( View& src, const std::string& filepath, const int bitDepth, const tuttle::io::DpxImage::EDPX_CompType eCompType, const bool packing )
{
	using namespace terry;

	int nChannels = 0;

	switch( _dpxImg.compTypeToDescriptor( eCompType ) )
	{
		case 50: // rgb
			nChannels = 3;
			break;
		case 51: // rgba
		case 52: // abgr
			nChannels = 4;
			break;
		default:
			// unknown format (53)
			break;
	}

	boost::uint8_t* pData = viewToBitStream<View, WImage>( src, nChannels, bitDepth );

	// Little endian
	_dpxHeader.setBigEndian( false );
	// bitDepth
	_dpxHeader.setBitSize( bitDepth );
	// 1 rgb? element
	_dpxHeader.setElementNumber( 1 );
	_dpxHeader.setElementOffset( sizeof( tuttle::io::DpxHeader ), 0 );
	_dpxHeader.setWidth( src.width() );
	_dpxHeader.setHeight( src.height() );
	// Packed by default
	_dpxHeader.setPacking( ! packing );
	_dpxHeader.setDescriptor( _dpxImg.compTypeToDescriptor( eCompType ) );
	_dpxImg.setHeader( _dpxHeader );
	_dpxImg.setData( pData, false );
	_dpxImg.write( filepath );

	delete[] pData;
}

/**
 */
template<class View>
template<class WImage>
void DPXWriterProcess<View>::writeImage( View& src, const std::string& filepath, const int bitDepth, const tuttle::io::DpxImage::EDPX_CompType eCompType, const bool packing )
{
	using namespace terry;
	WImage img( src.width(), src.height() );

	switch( _dpxImg.compTypeToDescriptor( eCompType ) )
	{
		case 50: // rgb
			TUTTLE_COUT("RGB " << bitDepth);
			break;
		case 51: // rgba
		case 52: // abgr
			TUTTLE_COUT("RGBA " << bitDepth);
			break;
		default:
			// unknown format (53)
			break;
	}

	typename WImage::view_t vw( view( img ) );
	copy_and_convert_pixels( clamp_view( src ), vw );
	boost::uint8_t* pData = (boost::uint8_t*)boost::gil::interleaved_view_get_raw_data( vw );
	// Little endian
	_dpxHeader.setBigEndian( false );
	// bitDepth
	_dpxHeader.setBitSize( bitDepth );
	// 1 rgb? element
	_dpxHeader.setElementNumber( 1 );
	_dpxHeader.setElementOffset( sizeof( tuttle::io::DpxHeader ), 0 );
	_dpxHeader.setWidth( src.width() );
	_dpxHeader.setHeight( src.height() );
	// Packed by default
	_dpxHeader.setPacking( ! packing );
	_dpxHeader.setDescriptor( _dpxImg.compTypeToDescriptor( eCompType ) );
	_dpxImg.setHeader( _dpxHeader );
	_dpxImg.setData( pData, false );
	_dpxImg.write( filepath );
}

}
}
}
}
