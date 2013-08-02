#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/globals.hpp>
#include <terry/basic_colors.hpp>
#include <terry/openexr/half.hpp>

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
	: ImageGilProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void EXRReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	_params = _plugin.getProcessParams( args.time );

	try
	{
		_exrImage.reset( new Imf::InputFile( _params._filepath.c_str() ) );
		const Imf::Header& h = _exrImage->header();
		typename Imath::V2i imageDims = h.dataWindow().size();
		imageDims.x++;
		imageDims.y++;
	}
	catch( ... )
	{
		BOOST_THROW_EXCEPTION( exception::File()
		<< exception::user( "EXR: Error when reading header." )
		<< exception::filename( _params._filepath.c_str() ) );
	}
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void EXRReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{
	using namespace boost::gil;
	using namespace terry;
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );
	try
	{
		View dst = this->_dstView;
		//TUTTLE_LOG_VAR( TUTTLE_INFO, _params._fileComponents );
		switch( _params._fileComponents )
		{
			case 1:
			{
				gray32f_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename gray32f_image_t::view_t dv( view( img ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				break;
			}
			case 3:
			{
				rgb32f_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgb32f_image_t::view_t dv( view( img ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				//fill_alpha_min( dst );
				break;
			}
			case 4:
			{
				rgba32f_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgba32f_image_t::view_t dv( view( img ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				break;
			}
			default:
			{
				BOOST_THROW_EXCEPTION( exception::Unsupported()
				    << exception::user( "ExrWriter: file channel not supported" ) );
			}
		}
	}
	catch( boost::exception& e )
	{
		e << exception::filename( _params._filepath );
		TUTTLE_TLOG( TUTTLE_ERROR, boost::diagnostic_information( e ) );
		throw;
	}
	catch( ... )
	{
		BOOST_THROW_EXCEPTION( exception::Unknown()
			<< exception::user( "Unable to read image")
			<< exception::dev( boost::current_exception_diagnostic_information() )
			<< exception::filename( _params._filepath ) );
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
	switch( (EParamReaderChannel)params._outComponents )
	{
		case eParamReaderChannelGray:
		{
			// Copy 1 channel seletected by alpha channel ( index 3 )
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 1 );
			break;
		}
		case eParamReaderChannelRGB:
		{
			// Copy 3 channels starting by the first channel (0, 1, 2)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 3 );
			break;
		}
		case eParamReaderChannelRGBA:
		{
			// Copy 4 channels starting by the first channel (0, 1, 2, 3)
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, 4 );
			break;
		}
		case eParamReaderChannelAuto:
		{
			if( ! ( _params._fileComponents == 1 || _params._fileComponents == 3 || _params._fileComponents == 4 ) )
			{
				std::string msg = "EXR: not support ";
				msg += _params._fileComponents;
				msg += " channels.";
				BOOST_THROW_EXCEPTION( exception::FileNotExist()
									   << exception::user( msg ) );
			}
			
			channelCopy( in, frameBuffer, dst, imageDims.x, imageDims.y, _params._fileComponents );
		}
	}
}

template< typename PixelType >
void initExrChannel( char*& data, Imf::Slice& slice, Imf::FrameBuffer& frameBuffer, Imf::PixelType pixelType, std::string channelID, const Imath::Box2i& dw, int w, int h )
{
	data = new char[ sizeof( PixelType ) * w * h ];
	
	slice.type = pixelType;
	slice.base = (char*) (data - sizeof( PixelType ) * ( dw.min.x + dw.min.y * w ) );
	slice.xStride   = sizeof( PixelType ) * 1;
	slice.yStride   = sizeof( PixelType ) * w;
	slice.xSampling = 1;
	slice.ySampling = 1;
	slice.fillValue = 1.0;
	
	frameBuffer.insert( channelID.c_str(), slice );
}

template<class View>
template<class DView>
void EXRReaderProcess<View>::channelCopy( Imf::InputFile& input, Imf::FrameBuffer& frameBuffer,
										  DView& dst, int w, int h, size_t nc )
{
	using namespace boost::gil;
	const Imf::Header& header = input.header();
	const Imath::Box2i& dw    = header.dataWindow();

	char*      data   [nc];
	Imf::Slice slices [nc];
	
	for( size_t layer = 0; layer < nc; ++layer )
	{
		const Imf::ChannelList& cl( header.channels() );
		const Imf::Channel& ch = cl[ getChannelName( layer ).c_str() ];
		switch( ch.type )
		{
			case Imf::HALF:
			{
				initExrChannel<half>( data[layer], slices[layer], frameBuffer, ch.type, getChannelName( layer ), dw, w, h );
				break;
			}
			case Imf::FLOAT:
			{
				initExrChannel<float>( data[layer], slices[layer], frameBuffer, ch.type, getChannelName( layer ), dw, w, h );
				break;
			}
			case Imf::UINT:
			{
				initExrChannel<boost::uint32_t>( data[layer], slices[layer], frameBuffer, ch.type, getChannelName( layer ), dw, w, h );
				break;
			}
			case Imf::NUM_PIXELTYPES:
			{
				BOOST_THROW_EXCEPTION( exception::Value()
				    << exception::user( "Pixel type not supported." ) );
				break;
			}
		}
	}
	
	input.setFrameBuffer( frameBuffer );
	input.readPixels( dw.min.y, dw.max.y );

	for( size_t layer = 0; layer < nc; ++layer )
	{
		sliceCopy( &slices[layer], dst, w, h, layer );
	}
	
	for( size_t layer = 0; layer < nc; ++layer )
	{
		delete[] data[layer];
	}
}

template<class View>
template<class DView>
void EXRReaderProcess<View>::sliceCopy( const Imf::Slice* slice, DView& dst, int w, int h, int n )
{
	using namespace boost::gil;
	using namespace terry;
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

template<class View>
std::string EXRReaderProcess<View>::getChannelName( size_t index )
{
	return _plugin.channelNames()[ _plugin.channelChoice()[index]->getValue() ];
}

}
}
}
}
