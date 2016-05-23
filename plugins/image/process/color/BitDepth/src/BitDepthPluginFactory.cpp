#include "BitDepthPluginFactory.hpp"
#include "BitDepthPlugin.hpp"
#include "BitDepthDefinitions.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle
{
namespace plugin
{
namespace bitDepth
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void BitDepthPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleBitDepth", "BitDepth", "Bit depth convertor");
    desc.setPluginGrouping("tuttle/image/process/color");

    desc.setDescription("Convert channels bit depth."
                        "\n"
                        "\n"
                        "In computer graphics, color depth or bit depth is the number of bits used to "
                        "represent the color of a single pixel in a bitmapped image or video frame "
                        "buffer. This concept is also known as bits per pixel (bpp), particularly when "
                        "specified along with the number of bits used. Higher color depth gives a "
                        "broader range of distinct colors."
                        "\n"
                        "\n"
                        "http://en.wikipedia.org/wiki/Color_depth");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
    desc.setSupportsMultipleClipDepths(true);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void BitDepthPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    OFX::ChoiceParamDescriptor* outBitDepth = desc.defineChoiceParam(kParamOutputBitDepth);
    outBitDepth->setLabel("Output bit depth");
    outBitDepth->appendOption("auto");
    outBitDepth->appendOption("byte (8 bits)");
    outBitDepth->appendOption("short (16 bits)");
    outBitDepth->appendOption("float (32 bits)");
    outBitDepth->setDefault(3);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* BitDepthPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new BitDepthPlugin(handle);
}
}
}
}
