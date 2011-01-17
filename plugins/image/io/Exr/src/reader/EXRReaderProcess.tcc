#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/plugin/image/gil/globals.hpp>
#include <tuttle/plugin/image/gil/basic_colors.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>
#include "../half/gilHalf.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <ImfChannelList.h>
#include <ImfArray.h>
#include <ImathVec.h>

#include <boost/gil/gil_all.hpp>
#include <boost/gil/packed_pixel.hpp>

#include <boost/integer.hpp>  // for boost::uint_t
#include <boost/cstdint.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/assert.hpp>
#include <boost/filesystem/fstream.hpp>

namespace tuttle {
namespace plugin {
namespace exr {
namespace reader {

namespace bfs = boost::filesystem;

template<class View>
EXRReaderProcess<View>::EXRReaderProcess( EXRReaderPlugin& instance )
	: ImageGilProcessor<View>( instance )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void EXRReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.time );

	_exrImage.reset( new Imf::InputFile( _params._filepath.c_str() ) );
	const Imf::Header& h = _exrImage->header();
	typename Imath::V2i imageDims = h.dataWindow().size();
	imageDims.x++;
	imageDims.y++;
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void EXRReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	try
	{
		typedef pixel<typename channel_type< typename View::value_type >::type, rgb_layout_t > rgb_pixel_t;
		typedef pixel<typename channel_type< typename View::value_type >::type, rgba_layout_t > rgba_pixel_t;
		typedef pixel<typename channel_type< typename View::value_type >::type, gray_layout_t > gray_pixel_t;
		typedef image<rgb_pixel_t, false> rgb_image_t;
		typedef image<rgba_pixel_t, false> rgba_image_t;
		typedef image<gray_pixel_t, false> gray_image_t;

		View dst = this->_dstView;
		if( _params._flip )
		{
			dst = flipped_up_down_view( this->_dstView );
		}

		switch( (ECompType)_params._outComponents )
		{
			case eGray:
			{
				gray_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename gray_image_t::view_t dv( view( img ) );
				readImage( color_converted_view<gray_pixel_t>( dst ), _params._filepath );
				copy_and_convert_pixels( dv, this->_dstView );
				break;
			}
			case eRGB:
			{
				rgb_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgb_image_t::view_t dv( view( img ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				fill_alpha_max( dst );
				break;
			}
			case eRGBA:
			{
				rgba_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgba_image_t::view_t dv( view( img ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				break;
			}
		}
	}
	catch( boost::exception& e )
	{
		e << exception::filename( _params._filepath );
		TUTTLE_COUT_ERROR( boost::diagnostic_information( e ) );
		//		throw;
	}
	catch(... )
	{
		//		BOOST_THROW_EXCEPTION( exception::Unknown()
		//			<< exception::user( "Unable to write image")
		//			<< exception::filename(params._filepath) );
		TUTTLE_COUT_ERROR( boost::current_exception_diagnostic_information() );
	}
}

/**
 */
template<class View>
template<class DView>
void EXRReaderProcess<View>::readImage( DView dst, const std::string& filepath )
{
	using namespace boost;
	using namespace mpl;
	using namespace boost::gil;
	using namespace Imf;

	EXRReaderProcessParams params = _plugin.getProcessParams( this->_renderArgs.time );

	Imf::InputFile in( filepath.c_str() );
	Imf::FrameBuffer frameBuffer;
	const Imf::Header& header = in.header();
	const Imath::Box2i& dw    = header.dataWindow();
	typename Imath::V2i imageDims = dw.size();
	imageDims.x++;  // Width
	imageDims.y++;  // Height

	// Get number of output components
	switch( (ECompType)params._outComponents )
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
	using namespace boost::gil;
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
			if( !slice )
			{
				BOOST_THROW_EXCEPTION( exception::Value()
				    << exception::user( std::string( "Slice " ) + _plugin.channelNames()[_plugin.channelChoice()[s]->getValue()] + " not found!" ) );
			}
			sliceCopy( slice, dst, w, h, s );
		}
	}
}

template<class View>
template<class DView>
void EXRReaderProcess<View>::sliceCopy( const Imf::Slice* slice, DView& dst, int w, int h, int n )
{
	using namespace boost::gil;
	switch( slice->type )
	{
		case Imf::HALF:
		{
			gray16h_view_t vw( interleaved_view( w, h, (gray16h_view_t::value_type*)slice->base, w * sizeof( half ) ) );
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
