#include "AnisotropicDiffusionDefinition.hpp"
#include "AnisotropicDiffusionPluginFactory.hpp"
#include "AnisotropicDiffusionPlugin.hpp"

#include <tuttle/common/utils/global.hpp>
#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace diffusion
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */

void AnisotropicDiffusionPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    // basic labels
    desc.setLabels("TuttleAnisotropicDiffusion", "AnisotropicDiffusion", "Anisotropic diffusion");
    desc.setPluginGrouping("tuttle/image/process/filter");

    desc.setDescription("Anisotropic diffusion is the ability to blur along edges directions, thus it remove noise while "
                        "preserving important details in the image.\n"
                        "It's based on PDE (Partial Derivated Equations) and creates liquify effects.\n"
                        "\n"
                        "Inputs:\n"
                        "- Source: Source image to denoise.\n"
                        "- Input tensors: Needed to get edges directions and strengths.\n"
                        "\n"
                        "Parameters:\n"
                        "- {Red, Green, Blue} amplitude: amplitude by channel.");

    // add the supported contexts, only filter at the moment
    desc.addSupportedContext(OFX::eContextFilter);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // set a few flags
    desc.setSingleInstance(false);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(true);
    desc.setSupportsTiles(kSupportTiles);
    desc.setTemporalClipAccess(false);
    desc.setRenderTwiceAlways(false);
    desc.setSupportsMultipleClipPARs(false);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void AnisotropicDiffusionPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Source clip only in the filter context
    // create the mandated source clip
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setTemporalClipAccess(false);
    srcClip->setSupportsTiles(kSupportTiles);
    srcClip->setIsMask(false);

    // Create the tensors map input clip
    OFX::ClipDescriptor* tensorsDstClip = desc.defineClip(kClipInputTensors);
    tensorsDstClip->setLabels("Input tensors", "Input tensors", "User tensor image input.");
    tensorsDstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    tensorsDstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    tensorsDstClip->setSupportsTiles(kSupportTiles);
    tensorsDstClip->setOptional(false);
    tensorsDstClip->setIsMask(false);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    // Controls
    // Define PDE Based algorithm controls.
    OFX::GroupParamDescriptor* groupParamsPDE = desc.defineGroupParam(kParamGroupPDEAlgorithm);
    groupParamsPDE->setLabel("PDE Based Algorithm Parameters");

    OFX::BooleanParamDescriptor* fast_approx = desc.defineBooleanParam(kParamFastApproximation);
    fast_approx->setLabel("Fast Approximation");
    fast_approx->setParent(*groupParamsPDE);
    fast_approx->setDefault(true);
    fast_approx->setIsSecret(true);

    OFX::RGBParamDescriptor* amplitude = desc.defineRGBParam(kParamAmplitude);
    amplitude->setLabel("Amplitude");
    amplitude->setParent(*groupParamsPDE);
    amplitude->setDefault(kDefaultAmplitudeValue, kDefaultAmplitudeValue, kDefaultAmplitudeValue);
    //	amplitude->setRange( 0.0, 1000.0 );
    //	amplitude->setDisplayRange( 0.0, 10.0 );
    amplitude->setHint("Amplitude of the anisotropic blur");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* AnisotropicDiffusionPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new AnisotropicDiffusionPlugin(handle);
}
}
}
}
}
