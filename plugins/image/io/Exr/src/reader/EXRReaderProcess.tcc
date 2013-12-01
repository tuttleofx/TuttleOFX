#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/plugin/context/ReaderPlugin.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/algorithm/transform_pixels.hpp>
#include <terry/numeric/assign.hpp>

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

#include <algorithm>

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
				terry::algorithm::transform_pixels( dv, terry::numeric::pixel_assigns_color_t<gray32f_pixel_t>( gray32f_pixel_t( 0.0 ) ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				break;
			}
			case 3:
			{
				rgb32f_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgb32f_image_t::view_t dv( view( img ) );
				terry::algorithm::transform_pixels( dv, terry::numeric::pixel_assigns_color_t<rgb32f_pixel_t>( rgb32f_pixel_t( 0.0, 0.0, 0.0 ) ) );
				readImage( dv, _params._filepath );
				copy_and_convert_pixels( dv, dst );
				break;
			}
			case 4:
			{
				rgba32f_image_t img( this->_dstView.width(), this->_dstView.height() );
				typename rgba32f_image_t::view_t dv( view( img ) );
				terry::algorithm::transform_pixels( dv, terry::numeric::pixel_assigns_color_t<rgba32f_pixel_t>( rgba32f_pixel_t( 0.0, 0.0, 0.0, 0.0 ) ) );
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
			channelCopy( in, frameBuffer, params, dst, imageDims.x, imageDims.y, 1 );
			break;
		}
		case eParamReaderChannelRGB:
		{
			// Copy 3 channels starting by the first channel (0, 1, 2)
			channelCopy( in, frameBuffer, params, dst, imageDims.x, imageDims.y, 3 );
			break;
		}
		case eParamReaderChannelRGBA:
		{
			// Copy 4 channels starting by the first channel (0, 1, 2, 3)
			channelCopy( in, frameBuffer, params, dst, imageDims.x, imageDims.y, 4 );
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
			
			channelCopy( in, frameBuffer, params, dst, imageDims.x, imageDims.y, _params._fileComponents );
		}
	}
}

template<class View>
template< typename PixelType >
void EXRReaderProcess<View>::initExrChannel( DataVector& data, Imf::Slice& slice, Imf::FrameBuffer& frameBuffer, Imf::PixelType pixelType, std::string channelID, const Imath::Box2i& dw, int w, int h )
{
	data.resize( sizeof( PixelType ) * w * h );
	
	slice.type = pixelType;
	slice.base = (char*) (&data[0] - sizeof( PixelType ) * ( dw.min.x + dw.min.y * w ) );
	slice.xStride   = sizeof( PixelType ) * 1;
	slice.yStride   = sizeof( PixelType ) * w;
	slice.xSampling = 1;
	slice.ySampling = 1;
	slice.fillValue = 1.0;
	
	frameBuffer.insert( channelID.c_str(), slice );
}

template<class View>
template<class DView>
void EXRReaderProcess<View>::channelCopy( Imf::InputFile& input, Imf::FrameBuffer& frameBuffer, const EXRReaderProcessParams& params,
										  DView& dst, int w, int h, size_t nc )
{
	using namespace boost::gil;
	const Imf::Header& header = input.header();
	const Imath::Box2i& dw    = header.dataWindow();

	DataVector *data = new DataVector[nc];
	Imf::Slice *slices = new  Imf::Slice[nc];

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
		switch( slices[layer].type )
		{
			case Imf::HALF:
			{
				sliceCopy<DView, gray16h_view_t>( input, &slices[layer], dst, params, w, h, layer );
				break;
			}
			case Imf::FLOAT:
			{
				sliceCopy<DView, gray32f_view_t>( input, &slices[layer], dst, params, w, h, layer );
				break;
			}
			case Imf::UINT:
			{
				sliceCopy<DView, gray32_view_t>( input, &slices[layer], dst, params, w, h, layer );
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
	delete []data;
	delete []slices;
}

template<class View>
template<class DView, typename workingView>
void EXRReaderProcess<View>::sliceCopy( Imf::InputFile& input, const Imf::Slice* slice, DView& dst, const EXRReaderProcessParams& params, int w, int h, int n )
{
	using namespace terry;
	const Imath::Box2i& dataw    = input.header().dataWindow();
	const Imath::Box2i& dispw    = input.header().displayWindow();
	
	workingView vw( interleaved_view( w, h, ( typename workingView::value_type*)slice->base, w * sizeof( typename workingView::value_type ) ) );
	workingView subView;
	
	if( params._displayWindow )
	{
		size_t xoffsetData = dataw.min.x + std::max( 0, dispw.min.x );
		size_t yoffsetData = dataw.min.y + std::max( 0, dispw.min.y );
		size_t xoffsetDisp = dataw.min.x - std::min( 0, dispw.min.x );
		size_t yoffsetDisp = dataw.min.y - std::min( 0, dispw.min.y );
		
		size_t wView = std::min( dataw.max.x, dispw.max.x ) - std::max( dataw.min.x, dispw.min.x ) + 1;
		size_t hView = std::min( dataw.max.y, dispw.max.y ) - std::max( dataw.min.y, dispw.min.y ) + 1;

		TUTTLE_TLOG_VAR4( TUTTLE_WARNING, dataw.min.x, dataw.min.y, dataw.max.x, dataw.max.y );
		TUTTLE_TLOG_VAR4( TUTTLE_WARNING, dispw.min.x, dispw.min.y, dispw.max.x, dispw.max.y );
		TUTTLE_TLOG_VAR4( TUTTLE_WARNING, xoffsetData, yoffsetData, xoffsetDisp, yoffsetDisp );
		TUTTLE_TLOG_VAR2( TUTTLE_WARNING, wView, hView );
		
		subView = subimage_view( vw,
								 xoffsetData,
								 yoffsetData,
								 wView,
								 hView
								 );

		DView dstView = subimage_view( dst,
								 xoffsetDisp,
								 yoffsetDisp,
								 wView,
								 hView
								 );
		
		if( wView > 0 && hView > 0 )
		{
			copy_and_convert_pixels( subView, nth_channel_view( dstView, n ) );
		}
	}
	else
	{
		subView = subimage_view( vw, dataw.min.x, dataw.min.y, w, h );
		copy_and_convert_pixels( subView, nth_channel_view( dst, n ) );
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
