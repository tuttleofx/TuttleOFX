#include "FadePluginFactory.hpp"
#include "FadePlugin.hpp"
#include "FadeDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace fade
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void FadePluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleFade", "Fade", "Fade");
    desc.setPluginGrouping("tuttle/image/process/transition");

    desc.setDescription("Plugin under early development.");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextTransition);
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
void FadePluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClipFrom = desc.defineClip(kOfxImageEffectTransitionSourceFromClipName);
    srcClipFrom->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClipFrom->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClipFrom->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClipFrom->setSupportsTiles(kSupportTiles);
    srcClipFrom->setOptional(true);

    OFX::ClipDescriptor* srcClipTo = desc.defineClip(kOfxImageEffectTransitionSourceToClipName);
    srcClipTo->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClipTo->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClipTo->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClipTo->setSupportsTiles(kSupportTiles);
    srcClipTo->setOptional(true);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::DoubleParamDescriptor* transition = desc.defineDoubleParam(kOfxImageEffectTransitionParamName);
    transition->setLabel("Transition");
    transition->setDefault(0.5);
    transition->setRange(0.0, 1.0);
    transition->setDisplayRange(0.0, 1.0);

    OFX::ChoiceParamDescriptor* mode = desc.defineChoiceParam(kParamMode);
    mode->setLabel("Mode");
    mode->appendOption(kParamModeFromColor);
    mode->appendOption(kParamModeToColor);

    OFX::RGBAParamDescriptor* color = desc.defineRGBAParam(kParamColor);
    color->setLabel("Color");
    color->setDefault(0.0, 0.0, 0.0, 0.0);

    OFX::ChoiceParamDescriptor* rod = desc.defineChoiceParam(kParamRod);
    rod->appendOption(kParamRodIntersect);
    rod->appendOption(kParamRodUnion);
    rod->appendOption(kParamRodA);
    rod->appendOption(kParamRodB);
    rod->setDefault(eParamRodIntersect);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* FadePluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new FadePlugin(handle);
}
}
}
}
