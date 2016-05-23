#include "BlurPluginFactory.hpp"
#include "BlurPlugin.hpp"
#include "BlurDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace blur
{

static const bool kSupportTiles = true;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void BlurPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleBlur", "Blur", "Blur");
    desc.setPluginGrouping("tuttle/image/process/filter");

    desc.setDescription("A Gaussian blur is the result of blurring an image by a Gaussian function."
                        "\n"
                        "\n"
                        "It is a widely used effect in graphics software, typically to reduce image "
                        "noise and reduce detail. The visual effect of this blurring technique is a "
                        "smooth blur resembling that of viewing the image through a translucent "
                        "screen, distinctly different from the bokeh effect produced by an "
                        "out-of-focus lens or the shadow of an object under usual illumination. "
                        "Gaussian smoothing is also used as a pre-processing stage in computer vision "
                        "algorithms in order to enhance image structures at different scales—see "
                        "scale-space representation and scale-space implementation."
                        "\n"
                        "Mathematically, applying a Gaussian blur to an image is the same as "
                        "convolving the image with a Gaussian function; this is also known as a "
                        "two-dimensional Weierstrass transform. Applying a Gaussian blur has the "
                        "effect of reducing the image's high-frequency components; "
                        "a Gaussian blur is thus a low pass filter."
                        "\n"
                        "\n"
                        "http://en.wikipedia.org/wiki/Gaussian_blur");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextFilter);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    //	desc.addSupportedBitDepth( OFX::eBitDepthUByte );
    //	desc.addSupportedBitDepth( OFX::eBitDepthUShort );
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
void BlurPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    OFX::Double2DParamDescriptor* size = desc.defineDouble2DParam(kParamSize);
    size->setLabel("Size");
    size->setDefault(3, 3);
    size->setRange(0.0, 0.0, std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    size->setDisplayRange(0, 0, 10, 10);
    size->setDoubleType(OFX::eDoubleTypeScale);

    OFX::ChoiceParamDescriptor* border = desc.defineChoiceParam(kParamBorder);
    border->setLabel("Border");
    border->appendOption(kParamBorderNo);
    border->appendOption(kParamBorderMirror);
    border->appendOption(kParamBorderConstant);
    border->appendOption(kParamBorderBlack);
    border->appendOption(kParamBorderPadded);
    border->setDefault(eParamBorderMirror);

    OFX::GroupParamDescriptor* advanced = desc.defineGroupParam(kParamGroupAdvanced);
    advanced->setLabel("Advanced");
    advanced->setOpen(false);

    OFX::BooleanParamDescriptor* normalizedKernel = desc.defineBooleanParam(kParamNormalizedKernel);
    normalizedKernel->setLabel("Normalized kernel");
    normalizedKernel->setHint("Use a normalized kernel to compute the gradient.");
    normalizedKernel->setDefault(true);
    normalizedKernel->setParent(advanced);

    OFX::DoubleParamDescriptor* kernelEpsilon = desc.defineDoubleParam(kParamKernelEpsilon);
    kernelEpsilon->setLabel("Kernel espilon value");
    kernelEpsilon->setHint("Threshold at which we no longer consider the values of the function.");
    kernelEpsilon->setDefault(0.01);
    kernelEpsilon->setRange(std::numeric_limits<double>::epsilon(), 1.0);
    kernelEpsilon->setDisplayRange(0, 0.01);
    kernelEpsilon->setParent(advanced);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* BlurPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new BlurPlugin(handle);
}
}
}
}
