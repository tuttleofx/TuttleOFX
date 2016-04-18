#include "NLMDenoiserPluginFactory.hpp"
#include "NLMDenoiserDefinitions.hpp"
#include "NLMDenoiserPlugin.hpp"

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>
#include <iostream>
#include <cstdio>
#include <cmath>

namespace tuttle
{
namespace plugin
{
namespace nlmDenoiser
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void NLMDenoiserPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    // basic labels
    desc.setLabels("TuttleNLMDenoiser", "NLMDenoiser", "Non Local-Means Denoiser");
    desc.setPluginGrouping("tuttle/image/process/filter");

    desc.setDescription(
        "Optimized non Local-Means denoiser\n"
        "\n"
        "The Non Local Means denoiser is the state of the art in matter of high quality image denoising. "
        "But it's also the most expensive algorithm. "
        "\n"
        "\n"
        "Be careful to:\n"
        "- put the input image in the right colorspace. This colorspace is the original colorspace in which the noise has "
        "been generated (the space of your camera, eg. Cineon for pellicle).\n"
        "- the algorithm complexity: it takes a lot of time. We recommend you to tune the parameters using a small RoI."
        "\n"
        "Advices: \n"
        "- You can denoise a video using multiple frames via the 'depth' parameter. This doesn't introduce flick artefacts "
        "and clearly enhance the result. So with video, it's interesting to use a 'depth' parameter greater than region "
        "radius.\n"
        "- If the animation is very fast, select a larger region radius.\n"
        "- A good patch radius is about 2 or 3.\n"
        "- The higher 'radius' parameters are, the slower the algorithm is.");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextGeneral);
    desc.addSupportedContext(OFX::eContextFilter);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // set a few flags
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setHostFrameThreading(false); // The plugin is able to manage per frame threading
    desc.setSupportsMultiResolution(true);
    desc.setSupportsTiles(false);
    desc.setTemporalClipAccess(true);
    desc.setRenderTwiceAlways(false);
    desc.setSupportsMultipleClipPARs(false);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void NLMDenoiserPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Source clip only in the filter context
    // create the mandated source clip
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setTemporalClipAccess(true);
    srcClip->setSupportsTiles(true);
    srcClip->setIsMask(false);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(true);

    // Controls
    // Define NLMeans Based algorithm controls.
    OFX::GroupParamDescriptor* groupChGrSize = desc.defineGroupParam("Per channel grain size");
    OFX::DoubleParamDescriptor* redGrainSize = desc.defineDoubleParam(kParamRedGrainSize);
    redGrainSize->setLabels(kParamRedGrainSizeLabel, kParamRedGrainSizeLabel, kParamRedGrainSizeLabel);
    redGrainSize->setParent(*groupChGrSize);
    redGrainSize->setDefault(kParamDefaultBandwidthValueR);
    redGrainSize->setRange(-1.0, 100000.0);
    redGrainSize->setDisplayRange(-1.0, 500.0);
    redGrainSize->setHint("Red grain size (strength of the smoothing, -1 = automatic)");

    OFX::DoubleParamDescriptor* greenGrainSize = desc.defineDoubleParam(kParamGreenGrainSize);
    greenGrainSize->setLabels(kParamGreenGrainSizeLabel, kParamGreenGrainSizeLabel, kParamGreenGrainSizeLabel);
    greenGrainSize->setParent(*groupChGrSize);
    greenGrainSize->setDefault(kParamDefaultBandwidthValueG);
    greenGrainSize->setRange(-1.0, 100000.0);
    greenGrainSize->setDisplayRange(-1.0, 500.0);
    greenGrainSize->setHint("Green grain size (strength of the smoothing, -1 = automatic)");

    OFX::DoubleParamDescriptor* blueGrainSize = desc.defineDoubleParam(kParamBlueGrainSize);
    blueGrainSize->setLabels(kParamBlueGrainSizeLabel, kParamBlueGrainSizeLabel, kParamBlueGrainSizeLabel);
    blueGrainSize->setParent(*groupChGrSize);
    blueGrainSize->setDefault(kParamDefaultBandwidthValueB);
    blueGrainSize->setRange(-1.0, 100000.0);
    blueGrainSize->setDisplayRange(-1.0, 500.0);
    blueGrainSize->setHint("Blue grain size (strength of the smoothing, -1 = automatic)");

    OFX::GroupParamDescriptor* groupChStrength = desc.defineGroupParam("Per channel strength");
    OFX::DoubleParamDescriptor* redStrength = desc.defineDoubleParam(kParamRedStrength);
    redStrength->setParent(*groupChStrength);
    redStrength->setLabels(kParamRedStrengthLabel, kParamRedStrengthLabel, kParamRedStrengthLabel);
    redStrength->setDefault(1.0);
    redStrength->setRange(0.0, 1.0);
    redStrength->setDisplayRange(0.0, 1.0);
    redStrength->setHint("Red effect strength (in percent)");

    OFX::DoubleParamDescriptor* greenStrength = desc.defineDoubleParam(kParamGreenStrength);
    greenStrength->setLabels(kParamGreenStrengthLabel, kParamGreenStrengthLabel, kParamGreenStrengthLabel);
    greenStrength->setParent(*groupChStrength);
    greenStrength->setDefault(1.0);
    greenStrength->setRange(0.0, 1.0);
    greenStrength->setDisplayRange(0.0, 1.0);
    greenStrength->setHint("Green effect strength (in percent)");

    OFX::DoubleParamDescriptor* blueStrength = desc.defineDoubleParam(kParamBlueStrength);
    blueStrength->setLabels(kParamBlueStrengthLabel, kParamBlueStrengthLabel, kParamBlueStrengthLabel);
    blueStrength->setParent(*groupChStrength);
    blueStrength->setDefault(1.0);
    blueStrength->setRange(0.0, 1.0);
    blueStrength->setDisplayRange(0.0, 1.0);
    blueStrength->setHint("Blue effect strength (in percent)");

    OFX::GroupParamDescriptor* groupParams = desc.defineGroupParam("NL-Means common parameters");
    OFX::BooleanParamDescriptor* optimized = desc.defineBooleanParam(kParamOptimization);
    optimized->setLabels(kParamOptimizationLabel, kParamOptimizationLabel, kParamOptimizationLabel);
    optimized->setParent(*groupParams);
    optimized->setDefault(true);
    optimized->setIsSecret(true);

    OFX::DoubleParamDescriptor* preBlurring = desc.defineDoubleParam(kParamPreBlurring);
    preBlurring->setLabels(kParamPreBlurringLabel, kParamPreBlurringLabel, kParamPreBlurringLabel);
    preBlurring->setParent(*groupParams);
    preBlurring->setRange(0.0, 10.0);
    preBlurring->setDisplayRange(0.0, 1.0);
    preBlurring->setHint("Pre-blurring will make noise to have lesser influence on similar patch research.");
    preBlurring->setDefault(0.0);
    preBlurring->setIsSecret(true);

    OFX::IntParamDescriptor* patchRadius = desc.defineIntParam(kParamPatchRadius);
    patchRadius->setLabels(kParamPatchRadiusLabel, kParamPatchRadiusLabel, kParamPatchRadiusLabel);
    patchRadius->setParent(*groupParams);
    patchRadius->setDefault(kParamDefaultPatchSizeValue);
    patchRadius->setRange(1, 8);
    patchRadius->setDisplayRange(1, 8);
    patchRadius->setHint("Patch radius for nl-means algorithm");

    OFX::IntParamDescriptor* regionRadius = desc.defineIntParam(kParamRegionRadius);
    regionRadius->setLabels(kParamRegionRadiusLabel, kParamRegionRadiusLabel, kParamRegionRadiusLabel);
    regionRadius->setParent(*groupParams);
    regionRadius->setDefault(kParamDefaultRegionValue);
    regionRadius->setRange(1, 150);
    regionRadius->setDisplayRange(2, 15);
    regionRadius->setHint("Radius of the region where similar patchs are searched");

    OFX::IntParamDescriptor* depth = desc.defineIntParam(kParamDepth);
    depth->setLabels(kParamDepthLabel, kParamDepthLabel, kParamDepthLabel);
    depth->setParent(*groupParams);
    depth->setDefault(kParamDefaultDepth);
    depth->setRange(0, 20);
    depth->setDisplayRange(0, 4);
    depth->setHint("Searching depth (3D version) for the nl-means algorithm");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in]   handle  effect handle
 * @param[in]   context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* NLMDenoiserPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new NLMDenoiserPlugin(handle);
}
}
}
}
