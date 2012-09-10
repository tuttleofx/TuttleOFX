#ifndef _TUTTLE_PLUGIN_OUTPUTBUFFER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_OUTPUTBUFFER_PLUGIN_HPP_

#include "OutputBufferDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

extern "C" {
  typedef void (*OutCallbackPtr)(OfxTime time, void* outputCustomData, void *rawdata, int width, int height, int rowSizeBytes, OFX::EBitDepth bitDepth, OFX::EPixelComponent components, OFX::EField field);
  typedef void* CallbackCustomDataPtr;
}

namespace tuttle {
namespace plugin {
namespace outputBuffer {

struct OutputBufferProcessParams
{
  OutCallbackPtr _callbackPtr;
  CallbackCustomDataPtr _callbackCustomDataPtr;
};

/**
 * @brief OutputBuffer plugin
 */
class OutputBufferPlugin : public OFX::ImageEffect
{
public:
        OutputBufferPlugin( OfxImageEffectHandle handle );

        OutputBufferProcessParams getProcessParams( const OfxTime time ) const;

	void render( const OFX::RenderArguments& args );

public:
	/// @group Attributes
	/// @{
	OFX::Clip* _clipSrc;       ///< Input image clip
  	OFX::Clip* _clipDst;       ///< Ouput image clip

        OFX::StringParam* _paramOutputCallbackPointer;
	OFX::StringParam* _paramOutputCallbackCustomData;
	/// @}
};

}
}
}

#endif
