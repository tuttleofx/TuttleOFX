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
	size_t pixelDepth = 8;// waiting for getMaxBitPerChannel() in avTranscoder
	oPixel.setBitsPerPixel( pixelDepth * pixelComponents );
	oPixel.setComponents( pixelComponents );
	oPixel.setColorComponents( avtranscoder::eComponentRgb );
	oPixel.setSubsampling( avtranscoder::eSubsamplingNone );
	oPixel.setAlpha( false );
	oPixel.setPlanar( false );
	
	// get image to encode
	avtranscoder::ImageDesc imageToEncodeDesc( sourceImageDesc );
	imageToEncodeDesc.setPixel( oPixel.findPixel() ); // seg fault with bitDepth: 32
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
	else
	{
		_plugin._colorTransform.convert( sourceImage, *(_imageToEncode) );
	}
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
	size_t bitDepth = 8;// waiting for getMaxBitPerChannel() in avTranscoder
	
	switch( bitDepth )
	{
		case 8:
			switch( components )
			{
				case 3:
					readImage<bits8, rgb_layout_t, rgb8c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				case 4:
					readImage<bits8, rgba_layout_t, rgba8c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				default:
					readImage<bits8, gray_layout_t, gray8c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
			}
			break;
		case 16:
			switch( components )
			{
				case 3:
					readImage<bits16, rgb_layout_t, rgb16c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				case 4:
					readImage<bits16, rgba_layout_t, rgba16c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				default:
					readImage<bits16, gray_layout_t, gray16c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
			}
			break;
		case 32:
			switch( components )
			{
				case 3:
					readImage<bits32, rgb_layout_t, rgb32c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				case 4:
					readImage<bits32, rgba_layout_t, rgba32c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
				default:
					readImage<bits32, gray_layout_t, gray32c_view_t>( this->_dstView, *(_imageToEncode) );
					break;
			}
			break;
		default:
			// bitDepth: 16, color componenent: Gray
			readImage<bits16, gray_layout_t, gray16c_view_t>( this->_dstView, *(_imageToEncode) );
			break;
			
	}
}

template<class View>
template<typename bitDepth, typename layout, typename fileView>
View& AVReaderProcess<View>::readImage( View& dst, avtranscoder::Image& image )
{	
	typedef boost::gil::pixel<bitDepth, layout> pixel_t;
	typedef boost::gil::image<pixel_t, false>   image_t;
	
	size_t width = image.desc().getWidth();
	size_t height = image.desc().getHeight();
		
	image_t tmpImg( width, height );
	fileView tmpView = view( tmpImg );
	
	const int outRowSizeBytes = tmpView.pixels().row_size();
	
	fileView avSrcView = interleaved_view
		( 
		width, height,
		(const pixel_t*)( image.getPtr() ),
		outRowSizeBytes );
		
	copy_and_convert_pixels( avSrcView, dst );
		
	return dst;
}

}
}
}
}
