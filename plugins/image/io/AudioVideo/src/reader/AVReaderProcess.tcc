#include <AvTranscoder/DatasStructures/Pixel.hpp>

#include "AVReaderPlugin.hpp"

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>

namespace tuttle {
namespace plugin {
namespace av {
namespace reader {

template<class View>
AVReaderProcess<View>::AVReaderProcess( AVReaderPlugin& instance )
	: ImageGilProcessor<View>( instance, eImageOrientationFromTopToBottom )
	, _plugin( instance )
	, _imageToEncode( NULL )
{
	this->setNoMultiThreading();
}

template<class View>
void AVReaderProcess<View>::setup( const OFX::RenderArguments& args )
{	
	ImageGilProcessor<View>::setup( args );
	
	// get source image
	avtranscoder::ImageDesc sourceImageDesc = _plugin._inputFile->getStream( _plugin._idVideoStream ).getVideoDesc().getImageDesc();
	avtranscoder::Image sourceImage( sourceImageDesc );
	
	// set pixel data of image to encode
	avtranscoder::Pixel oPixel;
	size_t pixelComponents = sourceImageDesc.getPixelDesc().getComponents();
	size_t pixelDepth = 8; // waiting for getMaxBitPerChannel() in avTranscoder
	oPixel.setBitsPerPixel( pixelDepth * pixelComponents );
	oPixel.setComponents( pixelComponents );
	oPixel.setColorComponents( avtranscoder::eComponentRgb );
	oPixel.setSubsampling( avtranscoder::eSubsamplingNone );
	oPixel.setAlpha( false );
	oPixel.setPlanar( false );
	
	// get image to encode
	avtranscoder::ImageDesc imageToEncodeDesc( sourceImageDesc );
	imageToEncodeDesc.setPixel( oPixel.findPixel() );
	_imageToEncode.reset( new avtranscoder::Image( imageToEncodeDesc ) );

	TUTTLE_LOG_ERROR( "Read at frame : " << _plugin._lastFrame << " / " << args.time );
	// optimization for seek
	if( ( _plugin._lastFrame + 1 ) != args.time )
	{
		_plugin._inputFile->seekAtFrame( args.time );
		_plugin._inputStreamVideo->flushDecoder();
		
		TUTTLE_LOG_ERROR( "Seek : " << args.time );
	}
	
	_plugin._lastFrame = args.time;
	
	// Fetch output image	
	if( ! _plugin._inputStreamVideo->readNextFrame( sourceImage ) )
	{
		BOOST_THROW_EXCEPTION( exception::Failed()
		    << exception::user() + "Can't open the frame at time " + args.time
		    << exception::filename( _plugin._paramFilepath->getValue() ) );
	}
	
	_plugin._colorTransform.convert( sourceImage, *(_imageToEncode) );
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
		
	size_t components = _imageToEncode->desc().getPixelDesc().getComponents();
	size_t bitDepth = 8; // waiting for getMaxBitPerChannel() in avTranscoder
	
	switch( bitDepth )
	{
		case 8:
			switch( components )
			{
				case 3:
					readImage<rgb8c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				case 4:
					readImage<rgba8c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				default:
					readImage<gray8c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
			}
			break;
		case 16:
			switch( components )
			{
				case 3:
					readImage<rgb16c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				case 4:
					readImage<rgba16c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				default:
					readImage<gray16c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
			}
			break;
		case 32:
			switch( components )
			{
				case 3:
					readImage<rgb32c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				case 4:
					readImage<rgba32c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				default:
					readImage<gray32c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
			}
			break;
		default:
			readImage<gray16c_view_t>( this->_dstView, *(_imageToEncode) );
			break;
			
	}
}

template<class View>
template<typename FileView>
View& AVReaderProcess<View>::readImage( View& dst, avtranscoder::Image& image )
{
	typedef typename FileView::value_type Pixel;
	
	std::size_t width = image.desc().getWidth();
	std::size_t height = image.desc().getHeight();
	std::size_t rowSize = image.desc().getPixelDesc().getComponents() * 1 * image.desc().getWidth(); //in bytes
	
	FileView avSrcView = interleaved_view( 
		width, height,
		(const Pixel*)( image.getPtr() ),
		rowSize );
	
	copy_and_convert_pixels( avSrcView, dst );
	
	return dst;
}

}
}
}
}
