#ifndef _TUTTLE_HOST_OUTPUTBUFFERWRAPPER_HPP_
#define	_TUTTLE_HOST_OUTPUTBUFFERWRAPPER_HPP_

#include "INode.hpp"
#include <tuttle/host/attribute/Param.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/attribute/ClipImage.hpp>
#include <tuttle/host/ofx/attribute/OfxhClipImageDescriptor.hpp>
#include <tuttle/host/graph/ProcessVertexData.hpp>
#include <tuttle/host/graph/ProcessVertexAtTimeData.hpp>

namespace tuttle {
namespace host {

class OutputBufferWrapper
{
private:
	INode& _node;
	
public:

  enum EBitDepth
    {
      eBitDepthCustom = -1, ///< some non standard bit depth
      eBitDepthNone = 0, ///< bit depth that indicates no data is present
      eBitDepthUByte = 1,
      eBitDepthUShort = 2,
      eBitDepthFloat = 3
    };


  enum EPixelComponent
    {
      ePixelComponentNone,
      ePixelComponentRGBA,
      ePixelComponentRGB,
      ePixelComponentAlpha,
      ePixelComponentCustom ///< some non standard pixel type
    };

  enum EField
    {
      eFieldNone,   /**< @brief unfielded image */
      eFieldBoth,   /**< @brief fielded image with both fields present */
      eFieldLower,  /**< @brief only the spatially lower field is present */
      eFieldUpper   /**< @brief only the spatially upper field is present  */
    };

       typedef void (*OutCallbackPtr)(OfxTime time, void* outputCustomData, void* rawdata, int width, int height, int rowSizeBytes, EBitDepth bitDepth, EPixelComponent components, EField field);
       typedef void* CallbackCustomDataPtr;

	OutputBufferWrapper( INode& node )
	: _node(node)
	{}
	~OutputBufferWrapper(){}

	INode& getNode() { return _node; }
	
	void setCallback( OutCallbackPtr callback, CallbackCustomDataPtr customData = NULL );
};

}
}

#endif

