#include "PngWriterPluginFactory.hpp"
#include "PngWriterDefinitions.hpp"
#include "PngWriterPlugin.hpp"

#include <tuttle/ioplugin/context/WriterPluginFactory.hpp>

namespace tuttle
{
namespace plugin
{
namespace png
{
namespace writer
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void PngWriterPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttlePngWriter", "PngWriter", "Png file writer");
    desc.setPluginGrouping("tuttle/image/io");
    desc.setDescription("PNG File writer\n"
                        "Plugin is used to read png files.\n\n"
                        "supported extensions:\n"
                        "png");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextWriter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // add supported extensions
    desc.addSupportedExtension("png");
    desc.setPluginEvaluation(90);

    // plugin flags
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void PngWriterPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    describeWriterParamsInContext(desc, context);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* PngWriterPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new PngWriterPlugin(handle);
}
}
}
}
}
