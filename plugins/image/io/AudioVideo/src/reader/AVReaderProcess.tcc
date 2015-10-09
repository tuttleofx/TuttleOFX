#include "AVReaderProcess.hpp"

#include <boost/gil/gil_all.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

template<class View>
AVReaderProcess<View>::AVReaderProcess( AVReaderPlugin& instance )
	: ImageGilProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
{
	this->setNoMultiThreading();
}

template<class View>
void AVReaderProcess<View>::setup( const OFX::RenderArguments& args )
{
	ImageGilProcessor<View>::setup( args );

	// if need to support interlace, use args.fieldToRender
	
	// optimization for seek
	if( ( _plugin._lastFrame + 1 ) != args.time )
	{
		_plugin._inputFile->seekAtFrame( args.time );
		_plugin._inputDecoder->flushDecoder();
	}
	
	_plugin._lastFrame = args.time;
	
	// Fetch output image
	if( ! _plugin._inputDecoder->decodeNextFrame( *_plugin._sourceImage ) )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "Can't open the frame at time " + args.time
		    << exception::filename( _plugin._paramFilepath->getValue() ) );
	}
	
	_plugin._colorTransform.convert( *_plugin._sourceImage, *_plugin._imageToDecode );
}

/**
 * @brief Function called by rendering thread each time a process must be done.
 * @param[in] procWindowRoW  Processing window in RoW
 */
template<class View>
void AVReaderProcess<View>::multiThreadProcessImages( const OfxRectI& procWindowRoW )
{	
	using namespace boost::gil;
	BOOST_ASSERT( procWindowRoW == this->_dstPixelRod );

	avtranscoder::PixelProperties pixel( _plugin._inputFile->getProperties().getVideoProperties().at( _plugin._paramVideoStreamIndex->getValue() ).getPixelProperties() );
	size_t components = pixel.getNbComponents();
	size_t bitDepth = 8; // @todo: waiting for getMaxBitPerChannel() in avTranscoder
	
	switch( bitDepth )
	{
		case 8:
			switch( components )
			{
				case 3:
					readImage<rgb8c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
				case 4:
					readImage<rgba8c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
				default:
					readImage<gray8c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
			}
			break;
		case 16:
			switch( components )
			{
				case 3:
					readImage<rgb16c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
				case 4:
					readImage<rgba16c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
				default:
					readImage<gray16c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
			}
			break;
		case 32:
			switch( components )
			{
				case 3:
					readImage<rgb32c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
				case 4:
					readImage<rgba32c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
				default:
					readImage<gray32c_view_t>( this->_dstView, *_plugin._imageToDecode );
					break;
			}
			break;
		default:
			readImage<gray16c_view_t>( this->_dstView, *_plugin._imageToDecode );
			break;
			
	}
}

template<class View>
template<typename FileView>
View& AVReaderProcess<View>::readImage( View& dst, avtranscoder::VideoFrame& image )
{
	typedef typename FileView::value_type Pixel;

	size_t width = image.desc().getWidth();
	size_t height = image.desc().getHeight();
	avtranscoder::PixelProperties pixel = _plugin._inputFile->getProperties().getVideoProperties().at( _plugin._paramVideoStreamIndex->getValue() ).getPixelProperties();
	size_t rowSizeInBytes = pixel.getNbComponents() * width;

	FileView avSrcView = interleaved_view( 
		width, height,
		(const Pixel*)( image.getData() ),
		rowSizeInBytes );
	
	copy_and_convert_pixels( avSrcView, dst );
	
	return dst;
}

}
}
}
}
