#include "ChannelShufflePluginFactory.hpp"
#include "ChannelShufflePlugin.hpp"
#include "ChannelShuffleDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace channelShuffle
{

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ChannelShufflePluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("ChannelShuffle", "ChannelShuffle", "ChannelShuffle");
    desc.setPluginGrouping("tuttle/image/process/channel");

    desc.setDescription("Shuffle image channels.");

    // add the supported contexts, only filter at the moment
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
void ChannelShufflePluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->setSupportsTiles(kSupportTiles);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::ChoiceParamDescriptor* red = desc.defineChoiceParam(kParamRed);
    red->setLabel("Red");
    red->appendOption(kParamRed);
    red->appendOption(kParamGreen);
    red->appendOption(kParamBlue);
    red->appendOption(kParamAlpha);
    red->setDefault(eChannelRed);

    OFX::ChoiceParamDescriptor* green = desc.defineChoiceParam(kParamGreen);
    green->setLabel("Green");
    green->appendOption(kParamRed);
    green->appendOption(kParamGreen);
    green->appendOption(kParamBlue);
    green->appendOption(kParamAlpha);
    green->setDefault(eChannelGreen);

    OFX::ChoiceParamDescriptor* blue = desc.defineChoiceParam(kParamBlue);
    blue->setLabel("Blue");
    blue->appendOption(kParamRed);
    blue->appendOption(kParamGreen);
    blue->appendOption(kParamBlue);
    blue->appendOption(kParamAlpha);
    blue->setDefault(eChannelBlue);

    OFX::ChoiceParamDescriptor* alpha = desc.defineChoiceParam(kParamAlpha);
    alpha->setLabel("Alpha");
    alpha->appendOption(kParamRed);
    alpha->appendOption(kParamGreen);
    alpha->appendOption(kParamBlue);
    alpha->appendOption(kParamAlpha);
    alpha->setDefault(eChannelAlpha);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ChannelShufflePluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ChannelShufflePlugin(handle);
}
}
}
}
