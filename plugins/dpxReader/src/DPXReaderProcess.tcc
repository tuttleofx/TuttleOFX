#include "DPXReaderPlugin.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include <tuttle/common/image/gilViewTypes.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/Progress.hpp>
#include <tuttle/plugin/PluginException.hpp>

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <iostream>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/gil/extension/io/png_io.hpp>
#include <boost/gil/extension/io/png_dynamic_io.hpp>
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

using namespace boost::gil;
namespace bfs = boost::filesystem;

namespace tuttle {
namespace plugin {
namespace dpx {

template<class View>
DPXReaderProcess<View>::DPXReaderProcess( DPXReaderPlugin& instance )
	: tuttle::plugin::ImageGilProcessor<View>( instance ),
	tuttle::plugin::Progress( instance ),
	_plugin( instance )
{
	_filepath = instance.fetchStringParam( "Input filename" );
	assert( _filepath != NULL );
}

template<class View>
void DPXReaderProcess<View>::setupAndProcess( const OFX::RenderArguments& args )
{
	try
	{
		std::string sFilepath;
		// Fetch output image
		_filepath->getValue( sFilepath );
		if( bfs::exists( sFilepath ) )
		{
			_plugin.getDpxImg().read( sFilepath );

			point2<ptrdiff_t> imageDims( _plugin.getDpxImg().width(),
			                             _plugin.getDpxImg().height() );

			double par       = _plugin.getDstClip()->getPixelAspectRatio();
			OfxRectD reqRect = { 0, 0, imageDims.x * par, imageDims.y };
			boost::scoped_ptr<OFX::Image> dst( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
			OfxRectI bounds = dst->getBounds();
			if( !dst.get() )
				throw( tuttle::plugin::ImageNotReadyException() );
			// Build destination view
			this->_dstView = interleaved_view( std::abs( bounds.x2 - bounds.x1 ), std::abs( bounds.y2 - bounds.y1 ),
			                                   static_cast<value_t*>( dst->getPixelData() ),
			                                   dst->getRowBytes() );

			// Set the render window
			this->setRenderWindow( args.renderWindow );
			// Call the base class process member
			this->process();
		}
		else
			throw( tuttle::plugin::PluginException( "Unable to open : %s", sFilepath.c_str() ) );
	}
	catch( tuttle::plugin::PluginException e )
	{
		COUT_EXCEPTION( e );
	}
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void DPXReaderProcess<View>::multiThreadProcessImages( OfxRectI procWindow )
{
	try
	{
		std::string filepath;
		this->_filepath->getValue( filepath );
		readImage( this->_dstView, filepath );
	}
	catch( tuttle::plugin::PluginException err )
	{
		COUT_EXCEPTION( err );
	}
}

/**
 * @brief Function called to apply an anisotropic blur
 *
 * @param[out]  dst     Destination image view
 * @param[in]   amplitude     Amplitude of the anisotropic blur
 * @param dl    spatial discretization.
 * @param da    angular discretization.
 * @param gauss_prec    precision of the gaussian function
 * @param fast_approx   Tell to use the fast approximation or not.
 *
 * @return Result view of the blurring process
 */
template<class View>
View& DPXReaderProcess<View>::readImage( View& dst, std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace std;
	using namespace boost;
	using namespace mpl;
	using namespace boost::gil;
	switch( _plugin.getDpxImg().componentsType() )
	{
		case tuttle::io::DpxImage::eCompTypeR8G8B8:
		{
			rgb8c_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
			                                     (const rgb8_pixel_t*)( _plugin.getDpxImg().data() ),
			                                     _plugin.getDpxImg().width() * 3 );
			copy_and_convert_pixels( flipped_up_down_view( src ), dst );
			break;
		}
		case tuttle::io::DpxImage::eCompTypeR8G8B8A8:
		{
			rgba8c_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
			                                      (const rgba8_pixel_t*)( _plugin.getDpxImg().data() ),
			                                      _plugin.getDpxImg().width() * 4 );

			copy_and_convert_pixels( flipped_up_down_view( src ), dst );
			break;
		}
		case tuttle::io::DpxImage::eCompTypeA8B8G8R8:
		{
			abgr8c_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
			                                      (const abgr8_pixel_t*)( _plugin.getDpxImg().data() ),
			                                      _plugin.getDpxImg().width() * 4 );

			copy_and_convert_pixels( clamp<rgb8_pixel_t>( flipped_up_down_view( src ) ), dst );
			break;
		}
		case tuttle::io::DpxImage::eCompTypeR10G10B10:
		{
			rgb10c_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
			                                      (const rgb10_pixel_t*)( _plugin.getDpxImg().data() ),
			                                      _plugin.getDpxImg().width() * sizeof( uint32_t ) );
			copy_and_convert_pixels( flipped_up_down_view( src ), dst );
			break;
		}
		case tuttle::io::DpxImage::eCompTypeR10G10B10A10:
		{
			std::cout << "RGBA10: " << std::endl;
			boost::uint8_t* pData = _plugin.getDpxImg().data();
			// Interpret pixels according to its bit packing
			switch( _plugin.getDpxImg().packing() )
			{
				// bit stream
				case 0: {
					typedef unsigned char byte_t;
					int width             = _plugin.getDpxImg().width();
					int height            = _plugin.getDpxImg().height();
					int num_channels      = 4;       // RGBA
					int channel_size      = 10; // in bits
					int scanline_in_bits  = width * num_channels * channel_size;
					int scanline_in_bytes = scanline_in_bits / 8;
					scanline_in_bytes += ( scanline_in_bits % 8 != 0 ) ? 1 : 0;
					rgba10101010_ptr_t p( pData, 0 );
					rgba16_image_t img( dst.width(), img.height() );
					rgba16_view_t vw( view( img ) );
					for( typename rgba16_view_t::y_coord_t y = 0; y < height; ++y )
					{
						typename rgba16_view_t::x_iterator it = vw.row_begin( y );

						for( typename rgba16_view_t::x_coord_t x = 0; x < width; ++x )
						{
							color_convert( *p, *it );
							++p;
							++it;
						}
					}
					copy_and_convert_pixels( vw, dst );
				}
				case 1:
				case 2:
					rgba10101010_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
					                                            ( rgba10101010_pixel_t* )( pData ),
					                                            _plugin.getDpxImg().width() * 5 );
					copy_and_convert_pixels( flipped_up_down_view( src ), color_converted_view<rgba16_pixel_t>( dst ) );
					break;
			}
			break;
		}
		case tuttle::io::DpxImage::eCompTypeA10B10G10R10:
		{
			abgr16_view_t src = interleaved_view( _plugin.getDpxImg().width(), _plugin.getDpxImg().height(),
			                                      ( abgr16_pixel_t* )( _plugin.getDpxImg().data() ),
			                                      _plugin.getDpxImg().width() * 4 * sizeof( uint16_t ) );

			copy_and_convert_pixels( flipped_up_down_view( src ), dst );
			break;
		}
		default:
			break;
	}

	return dst;
}

}
}
}
