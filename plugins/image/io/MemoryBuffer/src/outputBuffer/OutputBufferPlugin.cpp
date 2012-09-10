#include "OutputBufferPlugin.hpp"
#include "OutputBufferDefinitions.hpp"

#include <tuttle/common/ofx/imageEffect.hpp>
#include <tuttle/host/ofx/OfxhImage.hpp>

#include <boost/assert.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>

namespace tuttle {
namespace plugin {
namespace outputBuffer {

  /* @fixme duplicated in inputbuffer */
void* stringToPointer( const std::string& value )
{
	if( value.empty() )
		return NULL;
	std::ptrdiff_t p = boost::lexical_cast<std::ptrdiff_t>( value );
	return reinterpret_cast<void*>( p );
}

OutputBufferPlugin::OutputBufferPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramOutputCallbackPointer = fetchStringParam( kParamOutputCallbackPointer );
	_paramOutputCallbackCustomData = fetchStringParam( kParamOutputCallbackCustomData );
}

OutputBufferProcessParams OutputBufferPlugin::getProcessParams( const OfxTime time ) const
{
	OutputBufferProcessParams params;
	params._callbackPtr = reinterpret_cast<OutCallbackPtr>( stringToPointer( _paramOutputCallbackPointer->getValue() ) );
	params._callbackCustomDataPtr = static_cast<CallbackCustomDataPtr>( stringToPointer( _paramOutputCallbackCustomData->getValue() ) );
	return params;
}

void OutputBufferPlugin::render( const OFX::RenderArguments& args )
{
	TUTTLE_TCOUT( "        --> Output Buffer ");
	char* rawImage;
	void* dataSrcPtr;
	void* dataDstPtr;

	std::size_t imageDataBytes;
	std::size_t pixelSize = 0;

	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
	boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );

	// Get Image info
	const OfxRectI bounds = dst->getBounds();
	const OFX::EBitDepth depth = dst->getPixelDepth();
	const OFX::EPixelComponent components = dst->getPixelComponents();
	const OFX::EField field = dst->getField();

	// Find pixel size in bytes based on image info
	switch (depth) {
	case OFX::eBitDepthUByte:
	  pixelSize = 1;
	  break;
	case OFX::eBitDepthUShort:
	  pixelSize = 2;
	  break;
	case OFX::eBitDepthFloat:
	  pixelSize = 4;
	  break;
	default:
	  break;
	}
	switch (components) {
	case OFX::ePixelComponentRGBA:
	  pixelSize *= 4;
	  break;
	case OFX::ePixelComponentRGB:
	  pixelSize *= 3;
	  break;
	default:
	  break;
	}
	// User parameters
	OutputBufferProcessParams params = getProcessParams( args.time );



	if( src->isLinearBuffer() && dst->isLinearBuffer() )
	{
	  // Two linear buffers. No copy needed.
		imageDataBytes = dst->getBoundsImageDataBytes();
		if( imageDataBytes )
		{
		        rawImage = new char[imageDataBytes];
			TUTTLE_TCOUT( (long int)rawImage << std::endl);
			dataSrcPtr = src->getPixelAddress( bounds.x1, bounds.y1 );
			dataDstPtr = dst->getPixelAddress( bounds.x1, bounds.y1 );
			memcpy( dataDstPtr, dataSrcPtr, imageDataBytes );
			rawImage = (char *)dataDstPtr;
		}
	}
	else
	{
	  // Non-linear buffer. Copy to linear buffer
		const std::size_t rowBytesToCopy = dst->getBoundsRowDataBytes();
		imageDataBytes = rowBytesToCopy*(bounds.y2-bounds.y1);
		rawImage = new char[imageDataBytes];
		for( int y = bounds.y1; y < bounds.y2; ++y )
		{
			dataSrcPtr = src->getPixelAddress( bounds.x1, y );
			dataDstPtr = dst->getPixelAddress( bounds.x1, y );
			memcpy( dataDstPtr, dataSrcPtr, rowBytesToCopy );
			memcpy( rawImage + rowBytesToCopy*(y-bounds.y1),
				dataSrcPtr, rowBytesToCopy);
		}
	}

	if (params._callbackPtr != NULL) {
	  params._callbackPtr(args.time, params._callbackCustomDataPtr,rawImage,
			      bounds.x2-bounds.x1, bounds.y2-bounds.y1, pixelSize*(bounds.x2-bounds.x1),
			      depth, components, field);
	}
	if ((void *)rawImage != dataDstPtr)
	  delete rawImage;
}


}
}
}
