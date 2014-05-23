#include <AvTranscoder/DatasStructures/Pixel.hpp>

#include "AVReaderPlugin.hpp"

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
	avtranscoder::ImageDesc sourceImageDesc = _plugin._inputFile->getStream( _plugin._indexVideoStream ).getVideoDesc().getImageDesc();
	avtranscoder::Image sourceImage( sourceImageDesc );
	
	// set pixel data of image to encode
	avtranscoder::Pixel oPixel;
	size_t pixelComponents;
	switch( _plugin._clipDst->getPixelComponents() )
	{
		case OFX::ePixelComponentRGB:
			pixelComponents = 3;
			oPixel.setAlpha( false );
			break;
		case OFX::ePixelComponentRGBA:
			pixelComponents = 4;
			oPixel.setAlpha(true );
			break;
		default:
			pixelComponents = 1;
			oPixel.setAlpha( false );
			break;	
	}
	size_t pixelDepth;
	switch( _plugin._clipDst->getPixelDepth() )
	{
		case OFX::eBitDepthUByte:
			pixelDepth = 8;
			break;
		default:
			pixelDepth = 16;
			break;
	}
	oPixel.setBitsPerPixel( pixelComponents * pixelDepth );
	oPixel.setComponents( pixelComponents );
	oPixel.setColorComponents( avtranscoder::eComponentRgb );
	oPixel.setSubsampling( avtranscoder::eSubsamplingNone );
	oPixel.setPlanar( false );
	
	// get image to encode
	avtranscoder::ImageDesc encodeImageDesc( sourceImageDesc );
	encodeImageDesc.setPixel( oPixel.findPixel() );
	_imageToEncode.reset( new avtranscoder::Image( encodeImageDesc ) );

	// Fetch output image
	if( !_plugin._inputStreamVideo->readNextFrame( sourceImage ) )
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

	size_t width = _plugin._inputFile->getProperties().videoStreams.at( _plugin._indexVideoStream ).width;
	size_t height = _plugin._inputFile->getProperties().videoStreams.at( _plugin._indexVideoStream ).height;
	
	rgb8c_view_t avSrcView =
	    interleaved_view( width, height,
	                      (const rgb8c_pixel_t*)( _imageToEncode->getPtr() ),
	                      width * 3 );
	
	copy_and_convert_pixels( avSrcView, this->_dstView );
}

}
}
}
}
