#include "OutputBufferPluginFactory.hpp"
#include "OutputBufferPlugin.hpp"
#include "OutputBufferDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace outputBuffer
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void OutputBufferPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleOutputBuffer", "OutputBuffer", "OutputBuffer");
    desc.setPluginGrouping("tuttle/image/io");

    desc.setDescription("This is a DANGEROUS plugin dedicated to developers.\n"
                        "\n"
                        "WARNING: If you put a wrong value, you will crash your application.\n"
                        "\n"
                        "It allows users to read an image buffer directly from the graph.");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextWriter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsTiles(false);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void OutputBufferPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    OFX::StringParamDescriptor* callbackPointer = desc.defineStringParam(kParamOutputCallbackPointer);
    callbackPointer->setLabel("Callback Pointer");
    callbackPointer->setHint("This parameter represents a pointer to a function.\n"
                             "WARNING:\n"
                             " - Your application will crash if you set an invalid value here.\n");
    callbackPointer->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    callbackPointer->setIsPersistant(false);
    callbackPointer->setAnimates(false);
    callbackPointer->setDefault("");

    OFX::StringParamDescriptor* customData = desc.defineStringParam(kParamOutputCustomData);
    customData->setLabel("Callback Custom Data Pointer");
    customData->setHint("This parameter represent a pointer to a custom data which is given to the callback function.\n"
                        "WARNING:\n"
                        " - Your application could crash if you set an invalid value here.\n");
    customData->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    customData->setIsPersistant(false);
    customData->setAnimates(false);
    customData->setDefault("");

    OFX::StringParamDescriptor* callbackDestroyCustomData = desc.defineStringParam(kParamOutputCallbackDestroyCustomData);
    callbackDestroyCustomData->setLabel("Callback Custom Data Pointer");
    callbackDestroyCustomData->setHint("This parameter represents a pointer to a function.\n"
                                       "WARNING:\n"
                                       " - Your application could crash if you set an invalid value here.\n");
    callbackDestroyCustomData->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    callbackDestroyCustomData->setIsPersistant(false);
    callbackDestroyCustomData->setAnimates(false);
    callbackDestroyCustomData->setDefault("");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* OutputBufferPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new OutputBufferPlugin(handle);
}
}
}
}
