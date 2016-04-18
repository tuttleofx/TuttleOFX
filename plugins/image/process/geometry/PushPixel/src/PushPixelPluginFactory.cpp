#include "PushPixelPluginFactory.hpp"
#include "PushPixelPlugin.hpp"
#include "PushPixelDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace pushPixel
{

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void PushPixelPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttlePushPixel", "PushPixel", "Distort an image based on mask gradient.");
    desc.setPluginGrouping("tuttle/image/process/geometry");

    desc.setDescription("Distort an image based on mask gradient.");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void PushPixelPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    // no tiles on src clip, because it depends on the mask content so we can't
    // define the maximal bounding box needed...
    srcClip->setSupportsTiles(false);

    OFX::ClipDescriptor* maskClip = desc.defineClip(kClipMask);
    maskClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    maskClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    maskClip->setIsMask(true);
    maskClip->setOptional(true);
    maskClip->setSupportsTiles(true);

    OFX::ChoiceParamDescriptor* output = desc.defineChoiceParam(kParamOutput);
    output->setLabel("Output");
    output->appendOption(kParamOutputMotionVectors);
    output->appendOption(kParamOutputPushPixel);
    output->setDefault(1);

    OFX::DoubleParamDescriptor* size = desc.defineDoubleParam(kParamSize);
    size->setLabel("Size");
    size->setHint("Size of the gradient window.");
    size->setRange(0.0, std::numeric_limits<double>::max());
    size->setDisplayRange(1.0, 10.0);
    size->setDefault(2.0);

    OFX::BooleanParamDescriptor* normalizedKernel = desc.defineBooleanParam(kParamNormalizedKernel);
    normalizedKernel->setLabel("Normalized kernel");
    normalizedKernel->setHint("Use a normalized kernel to compute the gradient.");
    normalizedKernel->setDefault(true);
    normalizedKernel->setIsSecret(true);

    OFX::DoubleParamDescriptor* intensity = desc.defineDoubleParam(kParamIntensity);
    intensity->setLabel("Intensity");
    intensity->setHint("Scale motion vectors.");
    intensity->setDisplayRange(0.0, 2.0);
    intensity->setDefault(0.75);

    OFX::DoubleParamDescriptor* angle = desc.defineDoubleParam(kParamAngle);
    angle->setLabel("Angle");
    angle->setHint("Rotation on the gradient.");
    angle->setDisplayRange(-180, 180);
    angle->setDoubleType(OFX::eDoubleTypeAngle);
    angle->setDefault(0.0);

    OFX::ChoiceParamDescriptor* interpolation = desc.defineChoiceParam(kParamInterpolation);
    interpolation->setLabel("Interpolation");
    interpolation->setHint("Interpolation method.");
    interpolation->appendOption(kParamInterpolationNearest);
    interpolation->appendOption(kParamInterpolationBilinear);
    interpolation->setDefault(1);

    OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam(kParamBorder);
    border->setLabel("Gradient border");
    border->setHint("Border method for gradient computation.");
    border->appendOption(kParamBorderMirror);
    border->appendOption(kParamBorderConstant);
    border->appendOption(kParamBorderBlack);
    border->appendOption(kParamBorderPadded);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* PushPixelPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new PushPixelPlugin(handle);
}
}
}
}
