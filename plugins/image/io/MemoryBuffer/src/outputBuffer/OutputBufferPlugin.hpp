#ifndef _TUTTLE_PLUGIN_OUTPUTBUFFER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_OUTPUTBUFFER_PLUGIN_HPP_

#include "OutputBufferDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle {
namespace plugin {
namespace outputBuffer {

struct OutputBufferProcessParams
{
	CallbackOutputImagePtr _callbackPtr;
	CustomDataPtr _customDataPtr;
	CallbackDestroyCustomDataPtr _callbackDestroyPtr;
};

/**
 * @brief OutputBuffer plugin
 */
class OutputBufferPlugin : public OFX::ImageEffect
{
public:
	OutputBufferPlugin( OfxImageEffectHandle handle );
	~OutputBufferPlugin();

	void changedParam( const OFX::InstanceChangedArgs& args, const std::string& paramName );

	OutputBufferProcessParams getProcessParams() const;
	bool getRegionOfDefinition( const OFX::RegionOfDefinitionArguments& args, OfxRectD& rod );
	void getRegionsOfInterest( const OFX::RegionsOfInterestArguments& args, OFX::RegionOfInterestSetter& rois );
	void render( const OFX::RenderArguments& args );

public:
	/// @group Attributes
	/// @{
	OFX::Clip* _clipSrc;       ///< Input image clip

	OFX::StringParam* _paramCallbackOutputPointer;
	OFX::StringParam* _paramCustomData;
	OFX::StringParam* _paramCallbackDestroyCustomData;
        OFX::Int2DParam*  _paramMaxImageSize;
	/// @}
	
	CustomDataPtr _tempStoreCustomDataPtr; //< keep track of the previous value
};

}
}
}

#endif
