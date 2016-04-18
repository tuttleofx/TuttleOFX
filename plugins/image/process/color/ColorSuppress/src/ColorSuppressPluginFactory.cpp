#include "ColorSuppressPluginFactory.hpp"
#include "ColorSuppressPlugin.hpp"
#include "ColorSuppressDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorSuppress
{

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorSuppressPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleColorSuppress", "ColorSuppress", "Color Suppress");
    desc.setPluginGrouping("tuttle/image/process/color");

    desc.setDescription("ColorSuppress\n"
                        "\n"
                        "To remove a color / tint from an image.\n"
                        "\n"
                        "The node can modify the color and/or extract the amount of color "
                        "selected as a mask. "
                        "So you can use it to extract a mask and/or fix the despill of your "
                        "keying.\n");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextGeneral);
    desc.addSupportedContext(OFX::eContextFilter);

    // add supported pixel depths
    //	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    //	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorSuppressPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->setSupportsTiles(kSupportTiles);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::DoubleParamDescriptor* redSuppress = desc.defineDoubleParam(kParamRedSuppressRate);
    redSuppress->setLabel("Red");
    redSuppress->setHint("Suppressing X percent of red.");
    redSuppress->setDefault(0.0);
    redSuppress->setDisplayRange(0.0, 1.0);

    OFX::DoubleParamDescriptor* greenSuppress = desc.defineDoubleParam(kParamGreenSuppressRate);
    greenSuppress->setLabel("Green");
    greenSuppress->setHint("Suppressing X percent of green.");
    greenSuppress->setDefault(0.0);
    greenSuppress->setDisplayRange(0.0, 1.0);

    OFX::DoubleParamDescriptor* blueSuppress = desc.defineDoubleParam(kParamBlueSuppressRate);
    blueSuppress->setLabel("Blue");
    blueSuppress->setHint("Suppressing X percent of the blue.");
    blueSuppress->setDefault(0.0);
    blueSuppress->setDisplayRange(0.0, 1.0);

    OFX::DoubleParamDescriptor* cyanSuppress = desc.defineDoubleParam(kParamCyanSuppressRate);
    cyanSuppress->setLabel("Cyan");
    cyanSuppress->setHint("Suppressing X percent of the cyan.");
    cyanSuppress->setDefault(0.0);
    cyanSuppress->setDisplayRange(0.0, 1.0);

    OFX::DoubleParamDescriptor* magentaSuppress = desc.defineDoubleParam(kParamMagentaSuppressRate);
    magentaSuppress->setLabel("Magenta");
    magentaSuppress->setHint("Suppressing X percent of the magenta.");
    magentaSuppress->setDefault(0.0);
    magentaSuppress->setDisplayRange(0.0, 1.0);

    OFX::DoubleParamDescriptor* yellowSuppress = desc.defineDoubleParam(kParamYellowSuppressRate);
    yellowSuppress->setLabel("Yellow");
    yellowSuppress->setHint("Suppressing X percent of the yellow.");
    yellowSuppress->setDefault(0.0);
    yellowSuppress->setDisplayRange(0.0, 1.0);

    OFX::ChoiceParamDescriptor* outputTo = desc.defineChoiceParam(kParamApplyOn);
    outputTo->setLabel("Output");
    outputTo->setHint("Suppress mode.");
    outputTo->appendOption(kParamApplyOnImage);
    outputTo->appendOption(kParamApplyOnAlpha);
    outputTo->appendOption(kParamApplyOnImageAndAlpha);

    OFX::BooleanParamDescriptor* preserveLuma = desc.defineBooleanParam(kParamPreserveLuma);
    preserveLuma->setLabel("Preserve Luminance");
    preserveLuma->setHint("Preserve image luminosity.");
    preserveLuma->setDefault(false);

    OFX::BooleanParamDescriptor* obeyAlpha = desc.defineBooleanParam(kParamObeyAlpha);
    obeyAlpha->setLabel("Obey Alpha");
    obeyAlpha->setHint("Apply through the alpha channel.");
    obeyAlpha->setDefault(false);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorSuppressPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ColorSuppressPlugin(handle);
}
}
}
}
