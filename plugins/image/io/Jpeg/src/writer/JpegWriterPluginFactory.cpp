#include "JpegWriterPluginFactory.hpp"
#include "JpegWriterDefinitions.hpp"
#include "JpegWriterPlugin.hpp"

#include <tuttle/ioplugin/context/WriterPluginFactory.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>

#include <string>
#include <vector>

namespace tuttle
{
namespace plugin
{
namespace jpeg
{
namespace writer
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void JpegWriterPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleJpegWriter", "JpegWriter", "Jpeg file writer");
    desc.setPluginGrouping("tuttle/image/io");

    using namespace boost::assign;
    std::vector<std::string> supportedExtensions;
    supportedExtensions += "jpeg", "jpg", "jpe", "jfif", "jfi";

    desc.setDescription("JPEG File writer\n"
                        "Plugin is used to write jpeg files.\n\n"
                        "supported extensions: \n" +
                        boost::algorithm::join(supportedExtensions, ", "));

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextWriter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // add supported extensions
    desc.addSupportedExtensions(supportedExtensions);
    desc.setPluginEvaluation(50);

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
void JpegWriterPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    // Controls
    describeWriterParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* bitDepth =
        static_cast<OFX::ChoiceParamDescriptor*>(desc.getParamDescriptor(kTuttlePluginBitDepth));
    bitDepth->resetOptions();
    bitDepth->appendOption(kTuttlePluginBitDepth8);
    bitDepth->setDefault(eTuttlePluginBitDepth8);
    bitDepth->setEnabled(false);

    OFX::ChoiceParamDescriptor* channel =
        static_cast<OFX::ChoiceParamDescriptor*>(desc.getParamDescriptor(kTuttlePluginChannel));
    channel->resetOptions();
    channel->appendOption(kTuttlePluginChannelRGB);
    channel->setDefault(0);
    channel->setEnabled(false);

    OFX::BooleanParamDescriptor* premult =
        static_cast<OFX::BooleanParamDescriptor*>(desc.getParamDescriptor(kParamPremultiplied));
    premult->setDefault(true);

    OFX::IntParamDescriptor* quality = desc.defineIntParam(kParamQuality);
    quality->setLabel("Quality");
    quality->setRange(0, 100);
    quality->setDisplayRange(0, 100);
    quality->setDefault(80);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* JpegWriterPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new JpegWriterPlugin(handle);
}
}
}
}
}
