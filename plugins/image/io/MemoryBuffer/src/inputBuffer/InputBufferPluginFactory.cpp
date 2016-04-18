#include "InputBufferPluginFactory.hpp"
#include "InputBufferPlugin.hpp"
#include "InputBufferDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace inputBuffer
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void InputBufferPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleInputBuffer", "InputBuffer", "InputBuffer");
    desc.setPluginGrouping("tuttle/image/io");

    desc.setDescription("This is a DANGEROUS plugin dedicated to developers.\n"
                        "\n"
                        "WARNING: If you put a wrong value, you will crash your application.\n"
                        "\n"
                        "It allows to introduce an image buffer inside the graph.");

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
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsTiles(false);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void InputBufferPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::ChoiceParamDescriptor* inputMode = desc.defineChoiceParam(kParamInputMode);
    inputMode->setLabel("Mode");
    inputMode->appendOption(kParamInputModeBufferPointer);
    inputMode->appendOption(kParamInputModeCallbackPointer);
    inputMode->setDefault(0);
    inputMode->setAnimates(false);

    OFX::StringParamDescriptor* inputBuffer = desc.defineStringParam(kParamInputBufferPointer);
    inputBuffer->setLabel("Input Buffer Pointer");
    inputBuffer->setHint(
        "This parameter represent a pointer to a memory allocated image buffer.\n"
        "WARNING:\n"
        " - Your application will crash if you set an invalid value here.\n"
        " - If you want to animate this parameter to put different image buffer, be careful to disable interpolation.\n");
    inputBuffer->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    // This is a pointer to a dynamic memory allocation, so there no sense to store this value!
    // Need to be setted at each load.
    inputBuffer->setIsPersistant(false);
    inputBuffer->setDefault("");

    OFX::StringParamDescriptor* callbackPointer = desc.defineStringParam(kParamInputCallbackPointer);
    callbackPointer->setLabel("Callback Pointer");
    callbackPointer->setHint(
        "This parameter represent a pointer to a function of type \"void*(*)(int time, void* inputCustomData)\" .\n"
        "WARNING:\n"
        " - Your application will crash if you set an invalid value here.\n");
    callbackPointer->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    callbackPointer->setIsPersistant(false);
    callbackPointer->setAnimates(false);
    callbackPointer->setDefault("");

    OFX::StringParamDescriptor* customData = desc.defineStringParam(kParamInputCustomData);
    customData->setLabel("Callback Custom Data Pointer");
    customData->setHint("This parameter represent a pointer to a custom data which is given to the callback function.\n"
                        "WARNING:\n"
                        " - Your application could crash if you set an invalid value here.\n");
    customData->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    customData->setIsPersistant(false);
    customData->setAnimates(false);
    customData->setDefault("");

    OFX::Int2DParamDescriptor* imgSize = desc.defineInt2DParam(kParamSize);
    imgSize->setLabel("Image Size");
    imgSize->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    imgSize->setDefault(0, 0);

    OFX::IntParamDescriptor* rowBytesSize = desc.defineIntParam(kParamRowBytesSize);
    rowBytesSize->setLabel("Row Bytes Size");
    rowBytesSize->setHint("Put 0 if there is no padding between lines.");
    rowBytesSize->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    rowBytesSize->setDefault(0);

    OFX::DoubleParamDescriptor* par = desc.defineDoubleParam(kParamPixelAspectRatio);
    par->setLabel("Pixel Aspect Ratio");
    par->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    par->setAnimates(false);
    par->setDefault(1.0);

    OFX::DoubleParamDescriptor* framerate = desc.defineDoubleParam(kParamFramerate);
    framerate->setLabel("Framerate");
    framerate->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    framerate->setAnimates(false);
    framerate->setDefault(25.0);

    //	OFX::ImageEffectHostDescription* hostDesc = OFX::getImageEffectHostDescription();

    OFX::ChoiceParamDescriptor* components = desc.defineChoiceParam(kParamPixelComponents);
    components->setLabel("Pixel Components");
    components->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    //	if( hostDesc->supportsPixelComponent( OFX::ePixelComponentAlpha ) )
    components->appendOption(kParamPixelComponentsAlpha);
    //	if( hostDesc->supportsPixelComponent( OFX::ePixelComponentRGB ) )
    components->appendOption(kParamPixelComponentsRGB);
    //	if( hostDesc->supportsPixelComponent( OFX::ePixelComponentRGBA ) )
    components->appendOption(kParamPixelComponentsRGBA);
    components->setDefault(eParamPixelComponentRGBA);

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam(kParamBitDepth);
    bitDepth->setLabel("Bit Depth");
    bitDepth->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    //	if( hostDesc->supportsBitDepth( OFX::eBitDepthUByte ) )
    bitDepth->appendOption(kParamBitDepthUByte);
    //	if( hostDesc->supportsBitDepth( OFX::eBitDepthUShort ) )
    bitDepth->appendOption(kParamBitDepthUShort);
    //	if( hostDesc->supportsBitDepth( OFX::eBitDepthFloat ) )
    bitDepth->appendOption(kParamBitDepthFloat);
    bitDepth->setDefault(eParamBitDepthFloat);

    OFX::ChoiceParamDescriptor* field = desc.defineChoiceParam(kParamField);
    field->setLabel("Field");
    field->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    field->appendOption(kParamFieldNone);
    field->appendOption(kParamFieldBoth);
    field->appendOption(kParamFieldLower);
    field->appendOption(kParamFieldUpper);
    field->setDefault(eParamFieldNone);

    OFX::ChoiceParamDescriptor* orientation = desc.defineChoiceParam(kParamOrientation);
    orientation->setLabel("Orientation");
    orientation->setHint("Orientation in memory of the image buffer.");
    orientation->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    orientation->appendOption(kParamOrientationFromBottomToTop);
    orientation->appendOption(kParamOrientationFromTopToBottom);
    orientation->setDefault(eParamOrientationFromBottomToTop);

    OFX::Double2DParamDescriptor* timeDomain = desc.defineDouble2DParam(kParamTimeDomain);
    timeDomain->setLabel("Time Domain");
    timeDomain->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    timeDomain->setDefault(kOfxFlagInfiniteMin, kOfxFlagInfiniteMax);
    timeDomain->setAnimates(false);

    OFX::StringParamDescriptor* callbackDestroyCustomData = desc.defineStringParam(kParamInputCallbackDestroyCustomData);
    callbackDestroyCustomData->setLabel("Callback Destroy Custom Data");
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
OFX::ImageEffect* InputBufferPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new InputBufferPlugin(handle);
}
}
}
}
