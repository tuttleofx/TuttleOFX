#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/common/image/gilGlobals.hpp>
#include "EXRReaderProcess.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
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
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

using namespace boost::gil;
namespace bfs = boost::filesystem;

template<class View>
EXRReaderProcess<View>::EXRReaderProcess( EXRReaderPlugin& instance )
	: ImageGilProcessor<View>( instance ),
	_plugin( instance )
{
	this->setNoMultiThreading();
	_filepath      = instance.fetchStringParam( kInputFilename );
	_outComponents = instance.fetchChoiceParam( kOutputComponents );
}

template<class View>
void EXRReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	std::string sFilepath;
	// Fetch output image
	_filepath->getValue( sFilepath );
	if( ! bfs::exists( sFilepath ) )
	{
		throw( PluginException( "Unable to open : " + sFilepath ) );
	}
	_exrImage.reset( new Imf::InputFile( sFilepath.c_str() ) );
	const Imf::Header& h = _exrImage->header();
	typename Imath::V2i imageDims = h.dataWindow().size();
	imageDims.x++;
	imageDims.y++;

	double par                = _plugin.getDstClip()->getPixelAspectRatio();
	OfxRectD reqRect          = { 0, 0, imageDims.x * par, imageDims.y };

	// Fetch output image
	this->_dst.reset( _plugin.getDstClip()->fetchImage( args.time, reqRect ) );
	if( !this->_dst.get( ) )
	    throw( ImageNotReadyException( ) );
	if( this->_dst->getRowBytes( ) <= 0 )
		throw( WrongRowBytesException( ) );
	// Create destination view
	this->_dstView = this->getView( this->_dst.get(), _plugin.getDstClip()->getPixelRod(args.time) );
}

/**
 * @brief Function called by rendering thread each time
 *        a process must be done.
 *
 * @param[in] procWindow  Processing window
 */
template<class View>
void EXRReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindow )
{
	try
	{
		std::string filepath;
		this->_filepath->getValue( filepath );
		typedef pixel<typename channel_type< typename View::value_type >::type, rgb_layout_t > rgb_pixel_t;
		typedef pixel<typename channel_type< typename View::value_type >::type, rgba_layout_t > rgba_pixel_t;
		typedef pixel<typename channel_type< typename View::value_type >::type, gray_layout_t > gray_pixel_t;
		typedef image<rgb_pixel_t, false> rgb_image_t;
		typedef image<rgba_pixel_t, false> rgba_image_t;
		typedef image<gray_pixel_t, false> gray_image_t;

		switch( (ECompType)_outComponents->getValue() )
		{
			case eGray:
			{
				gray_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename gray_image_t::view_t dv( view( img ) );
				readImage( flipped_up_down_view( color_converted_view<gray_pixel_t>( this->_dstView ) ), filepath );
				copy_and_convert_pixels( dv, this->_dstView );
				break;
			}
			case eRGB:
			{
				rgb_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgb_image_t::view_t dv( view( img ) );
				readImage( flipped_up_down_view( dv ), filepath );
				copy_and_convert_pixels( dv, this->_dstView );
				break;
			}
			case eRGBA:
			{
				rgba_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgba_image_t::view_t dv( view( img ) );
				readImage( flipped_up_down_view( dv ), filepath );
				copy_and_convert_pixels( dv, this->_dstView );
				break;
			}
		}
	}
	catch( tuttle::plugin::PluginException& e )
	{
		COUT_EXCEPTION( e );
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
template<class DView>
void EXRReaderProcess<View>::readImage( DView dst, std::string& filepath ) throw( tuttle::plugin::PluginException )
{
	using namespace std;
	using namespace boost;
	using namespace mpl;
	using namespace boost::gil;
	using namespace Imf;

	Imf::InputFile in( filepath.c_str() );
	Imf::FrameBuffer frameBuffer;
	const Imf::Header& header = in.header();
	const Imath::Box2i& dw    = header.dataWindow();
	typename Imath::V2i imageDims = dw.size();
	imageDims.x++;  // Width
	imageDims.y++;  // Height

	// Get number of output components
	switch( (ECompType)_outComponents->getValue() )
	{
		case eGray:
		{
			// Copy 1 channel starting by the first channel (0)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 0, 1, 1 );
			break;
		}
		case eRGB:
		{
			// Copy 3 channels starting by the first channel (0)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 0, 3, 3 );
			break;
		}
		case eRGBA:
		{
			// Copy 4 channels starting by the first channel (0)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 0, 4, 4 );
			break;
		}
	}
}

template<class View>
template<class DView>
void EXRReaderProcess<View>::channelCopy( Imf::InputFile& input,
                                          Imf::FrameBuffer& frameBuffer,
                                          DView& dst, int w, int h,
                                          int n, int left, int nc )
{
	const Imf::Header& header = input.header();
	const Imath::Box2i& dw    = header.dataWindow();

	if( left )
	{
		// If channel left, prepare them
		const Imf::ChannelList& cl( header.channels() );
		const Imf::Channel& ch = cl[ _plugin.channelNames()[ _plugin.channelChoice()[n]->getValue() ].c_str() ];
		switch( ch.type )
		{
			case Imf::HALF:
			{
				std::vector<half> array( w * h );
				//@todo: check: this may bug: swap w and h
				frameBuffer.insert( _plugin.channelNames()[_plugin.channelChoice()[n]->getValue()].c_str(),
				                    Imf::Slice( ch.type,
				                                (char*)&array[0],
				                                sizeof( half ) * 1,        // xStride
				                                sizeof( half ) * w,        // yStride
				                                1, 1,                     // x/y sampling
				                                1.0 )
				                    ); // fillValue
				channelCopy( input, frameBuffer, dst, w, h, ++n, --left, nc );
				break;
			}
			case Imf::FLOAT:
			{
				std::vector<float> array( w * h );
				frameBuffer.insert( _plugin.channelNames()[_plugin.channelChoice()[n]->getValue()].c_str(),
				                    Imf::Slice( ch.type,
				                                (char*)&array[0],
				                                sizeof( float ) * 1,       // xStride
				                                sizeof( float ) * w,       // yStride
				                                1, 1,                     // x/y sampling
				                                std::numeric_limits<float>::max() )
				                    ); // fillValue
				channelCopy( input, frameBuffer, dst, w, h, ++n, --left, nc );
				break;
			}
			default:
			{
				std::vector<boost::uint32_t> array( w * h );
				frameBuffer.insert( _plugin.channelNames()[_plugin.channelChoice()[n]->getValue()].c_str(),
				                    Imf::Slice( ch.type,
				                                (char*)&array[0],
				                                sizeof( boost::uint32_t ) * 1,         // xStride
				                                sizeof( boost::uint32_t ) * w,         // yStride
				                                1, 1, // x/y sampling
				                                std::numeric_limits<boost::uint32_t>::max() )
				                    ); // fillValue
				channelCopy( input, frameBuffer, dst, w, h, ++n, --left, nc );
				break;
			}
		}
	}
	else
	{
		// Read prepared channels and copy pixels
		input.setFrameBuffer( frameBuffer );
		input.readPixels( dw.min.y, dw.max.y );
		for( int s = 0; s < nc; ++s )
		{
			const Imf::Slice* slice =
			    frameBuffer.findSlice(
			        _plugin.channelNames()[_plugin.channelChoice()[s]->getValue()].c_str() );
			sliceCopy( slice, dst, w, h, s );
		}
	}
}

template<class View>
template<class DView>
void EXRReaderProcess<View>::sliceCopy( const Imf::Slice* slice, DView& dst, int w, int h, int n )
{
	switch( slice->type )
	{
		case Imf::HALF:
		{
			gray16h_view_t vw( interleaved_view( w, h, (gray16h_view_t::value_type*)slice->base, w * sizeof( boost::uint16_t ) ) );
			copy_and_convert_pixels( vw, nth_channel_view( dst, n ) );
			break;
		}
		case Imf::FLOAT:
		{
			gray32f_view_t vw( interleaved_view( w, h, (gray32f_view_t::value_type*)slice->base, w * sizeof( float ) ) );
			copy_and_convert_pixels( vw, nth_channel_view( dst, n ) );
			break;
		}
		default:
		{
			gray32_view_t vw( interleaved_view( w, h, (gray32_view_t::value_type*)slice->base, w * sizeof( boost::uint32_t ) ) );
			copy_and_convert_pixels( vw, nth_channel_view( dst, n ) );
			break;
		}
	}
}

}
}
}
}
