#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/ioplugin/context/ReaderPlugin.hpp>

#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <terry/algorithm/transform_pixels.hpp>
#include <terry/numeric/assign.hpp>

#include <terry/globals.hpp>
#include <terry/draw/fill.hpp>
#include <terry/numeric/init.hpp>
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

	// TODO: Exr can contain a background color
	terry::draw::fill_pixels(this->_dstView, terry::numeric::pixel_zeros<Pixel>());

	try
	{
		readImage( _params._filepath );
	}
	catch( boost::exception& e )
	{
		e << exception::filename( _params._filepath );
		TUTTLE_LOG_ERROR( boost::diagnostic_information( e ) );
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

template<class View>
void EXRReaderProcess<View>::readImage( const std::string& filepath )
{
	using namespace boost;
	using namespace mpl;
	using namespace boost::gil;
	using namespace Imf;

	EXRReaderProcessParams params = _plugin.getProcessParams( this->_renderArgs.time );
	Imf::InputFile in( filepath.c_str() );
	
	int nbChannels = std::min(_params._fileNbChannels, int(num_channels<View>::type::value));
	nbChannels = std::min(nbChannels, _params._userNbComponents);

	if( nbChannels == 0 )
	{
		BOOST_THROW_EXCEPTION( exception::FileNotExist()
							   << exception::user() + "EXR: doesn't support " + _params._fileNbChannels + " channels." );
	}

	channelCopy( in, params, this->_dstView, nbChannels );
}

template<class View>
template< typename PixelType >
void EXRReaderProcess<View>::initExrChannel( DataVector& data, Imf::Slice& slice, Imf::FrameBuffer& frameBuffer, Imf::PixelType pixelType, std::string channelID, const Imath::Box2i& dw )
{
	Imath::V2i s = dw.size();
	s.x += 1;
	s.y += 1;
	
	const std::size_t allocSize = sizeof( PixelType ) * s.x * s.y;
	data.resize( allocSize );
	
	slice.type = pixelType;
	slice.base = (char*) (&data[0] - sizeof( PixelType ) * ( dw.min.x + dw.min.y * s.x ) );
	slice.xStride   = sizeof( PixelType );
	slice.yStride   = sizeof( PixelType ) * s.x;
	slice.xSampling = 1;
	slice.ySampling = 1;
	slice.fillValue = 1.0;
	
	frameBuffer.insert( channelID.c_str(), slice );
}

template<class View>
void EXRReaderProcess<View>::channelCopy( Imf::InputFile& input, const EXRReaderProcessParams& params, View& dst, const std::size_t nbChannels )
{
	using namespace boost::gil;

	const Imf::Header& header = input.header();
	const Imath::Box2i& dataWindow = header.dataWindow();

	Imf::FrameBuffer frameBuffer;
	std::vector<DataVector> data(nbChannels);
	std::vector<Imf::Slice> slices(nbChannels);

	for( size_t channelIndex = 0; channelIndex < nbChannels; ++channelIndex )
	{
		const Imf::ChannelList& cl( header.channels() );
		const Imf::Channel& ch = cl[ getChannelName( channelIndex ).c_str() ];
		switch( ch.type )
		{
			case Imf::HALF:
			{
				initExrChannel<half>( data[channelIndex], slices[channelIndex], frameBuffer, ch.type, getChannelName( channelIndex ), dataWindow );
				break;
			}
			case Imf::FLOAT:
			{
				initExrChannel<float>( data[channelIndex], slices[channelIndex], frameBuffer, ch.type, getChannelName( channelIndex ), dataWindow );
				break;
			}
			case Imf::UINT:
			{
				initExrChannel<boost::uint32_t>( data[channelIndex], slices[channelIndex], frameBuffer, ch.type, getChannelName( channelIndex ), dataWindow );
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
	input.readPixels( dataWindow.min.y, dataWindow.max.y );

	for( size_t channelIndex = 0; channelIndex < nbChannels; ++channelIndex )
	{
		switch( slices[channelIndex].type )
		{
			case Imf::HALF:
			{
				sliceCopy<gray16h_view_t>( input, &slices[channelIndex], dst, params, channelIndex );
				break;
			}
			case Imf::FLOAT:
			{
				sliceCopy<gray32f_view_t>( input, &slices[channelIndex], dst, params, channelIndex );
				break;
			}
			case Imf::UINT:
			{
				sliceCopy<gray32_view_t>( input, &slices[channelIndex], dst, params, channelIndex );
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
}

Imath::Box2i boxIntersection( const Imath::Box2i& a, const Imath::Box2i& b )
{
	Imath::Box2i res;
	
	res.min.x = std::max( a.min.x, b.min.x );
	res.min.y = std::max( a.min.y, b.min.y );
	
	res.max.x = std::min( a.max.x, b.max.x );
	res.max.y = std::min( a.max.y, b.max.y );
	
	return res;
}

template<class View>
template<typename workingView>
void EXRReaderProcess<View>::sliceCopy( Imf::InputFile& input, const Imf::Slice* slice, View& dst, const EXRReaderProcessParams& params, const std::size_t channelIndex )
{
	using namespace terry;
	const Imath::Box2i dataWindow = input.header().dataWindow();
	Imath::V2i dataWindowSize = dataWindow.size();
	dataWindowSize.x += 1;
	dataWindowSize.y += 1;

	if( dataWindowSize.x <= 0 || dataWindowSize.y <= 0 )
		return;

	workingView dataView( interleaved_view( dataWindowSize.x, dataWindowSize.y, ( typename workingView::value_type*)slice->base, dataWindowSize.x * sizeof( typename workingView::value_type ) ) );

	if( params._displayWindow )
	{
		const Imath::Box2i displayWindow = input.header().displayWindow();
		const Imath::Box2i croppedDisplayWindow = boxIntersection( displayWindow, dataWindow );
		Imath::V2i croppedDisplayWindowSize = croppedDisplayWindow.size();
		croppedDisplayWindowSize.x += 1;
		croppedDisplayWindowSize.y += 1;
		
		if( croppedDisplayWindowSize.x <= 0 || croppedDisplayWindowSize.y <= 0 )
			return;
		
		workingView dataSubView = subimage_view( dataView,
								 croppedDisplayWindow.min.x,
								 croppedDisplayWindow.min.y,
								 croppedDisplayWindowSize.x,
								 croppedDisplayWindowSize.y
								 );

		View dstSubView = subimage_view( dst,
								 croppedDisplayWindow.min.x - displayWindow.min.x,
								 croppedDisplayWindow.min.y - displayWindow.min.y,
								 croppedDisplayWindowSize.x,
								 croppedDisplayWindowSize.y
								 );
		
		copy_and_convert_pixels( dataSubView, nth_channel_view( dstSubView, channelIndex ) );
	}
	else
	{
		workingView dataSubView = subimage_view( dataView, dataWindow.min.x, dataWindow.min.y, dataWindowSize.x, dataWindowSize.y );
		copy_and_convert_pixels( dataSubView, nth_channel_view( dst, channelIndex ) );
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
