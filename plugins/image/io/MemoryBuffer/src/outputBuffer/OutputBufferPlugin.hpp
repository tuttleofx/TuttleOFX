#ifndef _TUTTLE_PLUGIN_OUTPUTBUFFER_PLUGIN_HPP_
#define _TUTTLE_PLUGIN_OUTPUTBUFFER_PLUGIN_HPP_

#include "OutputBufferDefinitions.hpp"

#include <tuttle/plugin/ImageEffectGilPlugin.hpp>

namespace tuttle
{
namespace plugin
{
namespace outputBuffer
{

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
    OutputBufferPlugin(OfxImageEffectHandle handle);
    ~OutputBufferPlugin();

    void changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName);

    OutputBufferProcessParams getProcessParams() const;

    void render(const OFX::RenderArguments& args);

public:
    /// @group Attributes
    /// @{
    OFX::Clip* _clipSrc; ///< Input image clip
    OFX::Clip* _clipDst; ///< Ouput image clip

    OFX::StringParam* _paramCallbackOutputPointer;
    OFX::StringParam* _paramCustomData;
    OFX::StringParam* _paramCallbackDestroyCustomData;
    /// @}

    CustomDataPtr _tempStoreCustomDataPtr; //< keep track of the previous value
};
}
}
}

#endif
