#include "FlipPluginFactory.hpp"
#include "FlipDefinitions.hpp"
#include "FlipPlugin.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsParam.h>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace flip
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void FlipPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleFlip", "Flip", "Flip-flop image");
    desc.setPluginGrouping("tuttle/image/process/geometry");

    desc.setDescription("Flip\n"
                        "Plugin is used to flip and or flop an image.");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void FlipPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(kSupportTiles);

    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::BooleanParamDescriptor* flip = desc.defineBooleanParam(kParamFlip);
    flip->setLabel(kParamFlipLabel);
    flip->setHint(kParamFlipHint);
    flip->setDefault(false);

    OFX::BooleanParamDescriptor* flop = desc.defineBooleanParam(kParamFlop);
    flop->setLabel(kParamFlopLabel);
    flop->setHint(kParamFlopHint);
    flop->setDefault(false);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FlipPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new FlipPlugin(handle);
}
}
}
}
