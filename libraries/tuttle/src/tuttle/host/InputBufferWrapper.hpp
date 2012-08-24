#ifndef _TUTTLE_HOST_INPUTBUFFERNODE_HPP_
#define	_TUTTLE_HOST_INPUTBUFFERNODE_HPP_

#include "INode.hpp"
#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

namespace tuttle {
namespace host {

class InputBufferWrapper
{
public:
	typedef void* (*CallbackPtr)(OfxTime time, void* inputCustomData);
	typedef void* CallbackCustomDataPtr;
	
	enum EMode
	{
		eModeBuffer,
		eModeCallback
	};
	enum EPixelComponent
	{
		ePixelComponentRGBA,
		ePixelComponentRGB,
		ePixelComponentAlpha
	};
	enum EBitDepth
	{
		eBitDepthUByte,
		eBitDepthUShort,
		eBitDepthFloat
	};
	enum EImageOrientation
	{
		eImageOrientationFromTopToBottom,
		eImageOrientationFromBottomToTop
	};
	

private:
	INode& _node;
	
public:
	InputBufferWrapper( INode& node )
	: _node(node)
	{}
	~InputBufferWrapper(){}

	INode& getNode() { return _node; }

	void setMode( const EMode mode );
	void setBuffer( void* rawBuffer );
	void setArrayBuffer( void* rawBuffer, const int width, const int height );
	void setArrayBuffer( void* rawBuffer, const int width, const int height, const int nbComponents );
	void setSize( const int width, const int height );
	void setComponents( const EPixelComponent components );
	void setBitDepth( const EBitDepth bitDepth );
	void setRowDistanceSize( const int rowDistanceBytes );
	void setOrientation( const EImageOrientation orientation );
	
	void setRawImageBuffer(
			void* rawBuffer,
			const int width, const int height,
			const EPixelComponent components,
			const EBitDepth bitDepth,
			const int rowDistanceBytes = 0,
			const EImageOrientation orientation = eImageOrientationFromBottomToTop );
	
	void setRawImageBuffer(
			unsigned char* rawBuffer,
			const int width, const int height,
			const EPixelComponent components,
			const int rowDistanceBytes = 0,
			const EImageOrientation orientation = eImageOrientationFromBottomToTop )
	{
		setRawImageBuffer( reinterpret_cast<void*>(rawBuffer),
			width, height,
			components,
			eBitDepthUByte,
			rowDistanceBytes,
			orientation );
	}
	
	void setRawImageBuffer(
			unsigned short* rawBuffer,
			const int width, const int height,
			const EPixelComponent components,
			const int rowDistanceBytes = 0,
			const EImageOrientation orientation = eImageOrientationFromBottomToTop )
	{
		setRawImageBuffer( reinterpret_cast<void*>(rawBuffer),
			width, height,
			components,
			eBitDepthUShort,
			rowDistanceBytes,
			orientation );
	}
	
	void setRawImageBuffer(
			float* rawBuffer,
			const int width, const int height,
			const EPixelComponent components,
			const int rowDistanceBytes = 0,
			const EImageOrientation orientation = eImageOrientationFromBottomToTop )
	{
		setRawImageBuffer( reinterpret_cast<void*>(rawBuffer),
			width, height,
			components,
			eBitDepthFloat,
			rowDistanceBytes,
			orientation );
	}
	
	void setCallback( CallbackPtr callback, CallbackCustomDataPtr customData = NULL );
	
};

}
}

#endif

