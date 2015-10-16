#include "OutputBufferPlugin.hpp"
#include "OutputBufferDefinitions.hpp"

#include <pointerParam.hpp>

#include <tuttle/plugin/memory/OfxAllocator.hpp>
#include <ofxImageEffect.h>

#include <boost/assert.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/cstdint.hpp>

namespace tuttle {
namespace plugin {
namespace outputBuffer {

using namespace memoryBuffer;

OutputBufferPlugin::OutputBufferPlugin( OfxImageEffectHandle handle )
: OFX::ImageEffect( handle )
, _tempStoreCustomDataPtr(NULL)
{
	_clipSrc = fetchClip( kOfxImageEffectSimpleSourceClipName );
	_clipDst = fetchClip( kOfxImageEffectOutputClipName );

	_paramCallbackOutputPointer = fetchStringParam( kParamOutputCallbackPointer );
	_paramCustomData = fetchStringParam( kParamOutputCustomData );
	_paramCallbackDestroyCustomData = fetchStringParam( kParamOutputCallbackDestroyCustomData );
}

OutputBufferPlugin::~OutputBufferPlugin()
{
	OutputBufferProcessParams params = getProcessParams();
	if( params._callbackDestroyPtr != NULL )
		params._callbackDestroyPtr( params._customDataPtr );
}

void OutputBufferPlugin::changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName )
{
	if( paramName == kParamOutputCustomData )
	{
		OutputBufferProcessParams params = getProcessParams();
		if( params._callbackDestroyPtr != NULL && _tempStoreCustomDataPtr != NULL )
			params._callbackDestroyPtr( _tempStoreCustomDataPtr );
		_tempStoreCustomDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramCustomData->getValue() ) );
	}
}

OutputBufferProcessParams OutputBufferPlugin::getProcessParams() const
{
	OutputBufferProcessParams params;
	params._callbackPtr = reinterpret_cast<CallbackOutputImagePtr>( stringToPointer( _paramCallbackOutputPointer->getValue() ) );
	params._customDataPtr = static_cast<CustomDataPtr>( stringToPointer( _paramCustomData->getValue() ) );
	params._callbackDestroyPtr = reinterpret_cast<CallbackDestroyCustomDataPtr>( stringToPointer( _paramCallbackDestroyCustomData->getValue() ) );
	return params;
}

void OutputBufferPlugin::render( const OFX::RenderArguments& args )
{
	TUTTLE_LOG_INFO( "        --> Output Buffer ");
	typedef std::vector<char, OfxAllocator<char> > DataVector;
	DataVector rawImage;
	char* rawImagePtrLink;

	boost::scoped_ptr<OFX::Image> src( _clipSrc->fetchImage( args.time ) );
	boost::scoped_ptr<OFX::Image> dst( _clipDst->fetchImage( args.time ) );

	// Get Image info
	const OfxRectI bounds = dst->getBounds();
	const OFX::EBitDepth depth = dst->getPixelDepth();
	const OFX::EPixelComponent components = dst->getPixelComponents();
	const OFX::EField field = dst->getField();

	// User parameters
	OutputBufferProcessParams params = getProcessParams();

	const std::size_t imageDataBytes = dst->getBoundsImageDataBytes();
	const std::size_t rowBytesToCopy = dst->getBoundsRowDataBytes();
	
	if( src->isLinearBuffer() && dst->isLinearBuffer() )
	{
		// Two linear buffers. No copy needed.
		if( imageDataBytes )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, bounds.y1 );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, bounds.y1 );
			memcpy( dataDstPtr, dataSrcPtr, imageDataBytes );
			
			// No image copy
			rawImagePtrLink = (char *)dataDstPtr;
		}
	}
	else
	{
		// Non-linear buffer. Line by line copy.
		for( int y = bounds.y1; y < bounds.y2; ++y )
		{
			void* dataSrcPtr = src->getPixelAddress( bounds.x1, y );
			void* dataDstPtr = dst->getPixelAddress( bounds.x1, y );
			memcpy( dataDstPtr, dataSrcPtr, rowBytesToCopy );
		}
		if( params._callbackPtr != NULL )
		{
			// need a temporary buffer copy to give a linear buffer to the callback
			rawImage.resize( imageDataBytes );
			rawImagePtrLink = &rawImage.front();
			for( int y = bounds.y1; y < bounds.y2; ++y )
			{
				void* dataSrcPtr = src->getPixelAddress( bounds.x1, y );
				void* dataDstPtr = rawImagePtrLink + rowBytesToCopy*(y-bounds.y1);
				memcpy( dataDstPtr, dataSrcPtr, rowBytesToCopy );
			}
		}
	}

	if( params._callbackPtr != NULL )
	{
		params._callbackPtr(
			args.time, params._customDataPtr, rawImagePtrLink,
			bounds.x2-bounds.x1, bounds.y2-bounds.y1, rowBytesToCopy,
			depth, components, field );
	}
}


}
}
}
