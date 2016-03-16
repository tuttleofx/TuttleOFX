#include "IdKeyerPluginFactory.hpp"
#include "IdKeyerPlugin.hpp"
#include "IdKeyerDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <algorithm>

namespace tuttle
{
namespace plugin
{
namespace idKeyer
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void IdKeyerPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleIdKeyer", "IdKeyer", "Id keyer");
    desc.setPluginGrouping("tuttle/image/process/color");

    desc.setDescription("Simplest Keyer: ID selectioner Keyer");

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
void IdKeyerPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);

    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(kSupportTiles);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::IntParamDescriptor* nbPoint = desc.defineIntParam(kParamNbPoints);
    nbPoint->setRange(1, kMaxNbPoints);
    nbPoint->setDisplayRange(1, kMaxNbPoints);
    nbPoint->setDefault(1);

    for(size_t i = 0; i < kMaxNbPoints; ++i)
    {
        OFX::RGBAParamDescriptor* color = desc.defineRGBAParam(getColorParamName(i));
        color->setLabel(getColorParamName(i));
    }

    OFX::BooleanParamDescriptor* useAlpha = desc.defineBooleanParam(kParamUseAlpha);
    useAlpha->setLabel("Use alpha");
    useAlpha->setDefault(false);

    OFX::DoubleParamDescriptor* tolerance = desc.defineDoubleParam(kParamTolerance);
    tolerance->setLabel("Tolerance");
    tolerance->setDefault(0.00001);
    tolerance->setRange(0.0, 1000);
    tolerance->setDisplayRange(0.0, 1.0);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* IdKeyerPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new IdKeyerPlugin(handle);
}
}
}
}
