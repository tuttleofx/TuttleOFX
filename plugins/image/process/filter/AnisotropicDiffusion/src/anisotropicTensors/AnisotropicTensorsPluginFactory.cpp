#include "AnisotropicTensorsDefinition.hpp"
#include "AnisotropicTensorsMargin.hpp"
#include "AnisotropicTensorsPlugin.hpp"
#include "AnisotropicTensorsProcess.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void AnisotropicTensorsPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    // basic labels
    desc.setLabels("TuttleAnisotropicTensors", "AnisotropicTensors", "Generates structure tensors image");
    desc.setPluginGrouping("tuttle/image/process/filter");

    desc.setDescription(
        "Generates structure tensors image, to use as input of the AnisotropicDiffusion.\n"
        "\n"
        "\n"
        "Tensors are used to get geographic information (edges and directions). "
        "In this map, you will see horizontal edges shown with red color, vertical edges shown in blue and finally, when "
        "the blur must be isotropic, "
        "the color is pink (blur along X and Y directions). The main goal is to get pink on uniform noisy area:"
        "Structure tensors is just used to see how edges are detected, "
        "diffuse tensors map is the final tensor map used by the AnisotropicDiffusion.\n"
        "\n"
        "Inputs:\n"
        "- Source: Source image.\n"
        "\n"
        "Parameters:\n"
        "- Display effect margin: Margin used by the algorithm to avoid border effect.\n"
        "- Alpha: pre-blurring of the edge detection. Can be interpreted as the noise scale. "
        "If too high, it detects noise as edges, if too low, it removes tiny edges. "
        "Play this parameter with geometry factor and thresholding quantization.\n"
        "- Sigma: post-blurring of the edge detection. Interpreted as regularity of the tensor valued geometry.\n"
        "- Geometry factor: tends to transform noise to pink when falling near 0.\n"
        "- Thresholding quantization: Quantization of the structure tensor, to eliminate noise effect.\n"
        "- Sharpness: set how much you want to preserve edges.\n"
        "- Anisotropy: anisotropy factor. Increase the value to get more fluid blurring.\n");

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

    desc.setOverlayInteractDescriptor(new OFX::DefaultEffectOverlayWrap<TensorsMarginOverlay>());
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void AnisotropicTensorsPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Source clip only in the filter context
    // create the mandated source clip
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setTemporalClipAccess(false);
    srcClip->setSupportsTiles(kSupportTiles);
    srcClip->setIsMask(false);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    // Controls
    OFX::BooleanParamDescriptor* algo = desc.defineBooleanParam(kParamDisplayStructureTensors);
    algo->setLabel("Display Structure Tensors");
    algo->setDefault(false);

    OFX::BooleanParamDescriptor* margin = desc.defineBooleanParam(kParamDisplayEffectMargin);
    margin->setLabel("Display Effect Margin");
    margin->setDefault(false);
    margin->setEvaluateOnChange(false);

    OFX::GroupParamDescriptor* groupParamsStrTensors = desc.defineGroupParam(kParamGroupStructureTensors);
    groupParamsStrTensors->setLabel("Structure tensors parameters");

    OFX::DoubleParamDescriptor* alpha = desc.defineDoubleParam(kParamAlpha);
    alpha->setLabel("Alpha");
    alpha->setParent(*groupParamsStrTensors);
    alpha->setDefault(kDefaultAlphaValue);
    alpha->setHint("Image pre-blurring (noise scale)");
    alpha->setRange(0.1, 100.0);
    alpha->setDisplayRange(0.1, 10.0);

    OFX::DoubleParamDescriptor* sigma = desc.defineDoubleParam(kParamSigma);
    sigma->setLabel("Sigma");
    sigma->setParent(*groupParamsStrTensors);
    sigma->setDefault(kDefaultSigmaValue);
    sigma->setRange(0.1, 100.0);
    sigma->setDisplayRange(0.1, 10.0);
    sigma->setHint("Regularity of the tensor-valued geometry");

    OFX::DoubleParamDescriptor* geom_fact = desc.defineDoubleParam(kParamGeometryFactor);
    geom_fact->setLabel("Geometry Factor");
    geom_fact->setParent(*groupParamsStrTensors);
    geom_fact->setDefault(kDefaultGeomFactValue);
    geom_fact->setRange(-1000.0, 1000.0);
    geom_fact->setDisplayRange(-10.0, 10.0);

    OFX::DoubleParamDescriptor* threshold = desc.defineDoubleParam(kParamThresholdingQuantization);
    threshold->setLabel("Thresholding Quantization");
    threshold->setParent(*groupParamsStrTensors);
    threshold->setDefault(kDefaultGeomFactValue);
    threshold->setDisplayRange(0.0, 10.0);
    threshold->setHint("Tresholding quantization to remove noise on uniform areas");

    OFX::GroupParamDescriptor* groupParamsDifTensors = desc.defineGroupParam(kParamGroupDiffuseTensors);
    groupParamsDifTensors->setLabel("Diffuse tensors parameters");

    OFX::ChoiceParamDescriptor* stAlgo = desc.defineChoiceParam(kParamEdgeDetectAlgo);
    stAlgo->setLabel("Edge Detection Algorithm");
    stAlgo->setParent(*groupParamsDifTensors);
    stAlgo->appendOption("Precise forward/backward finite differences");
    stAlgo->appendOption("Harris edge detector");
    stAlgo->appendOption("Canny-deriche filter (derivative order 1)");
    stAlgo->setDefault(kDefaultTensorsAlgo);
    stAlgo->setIsSecret(false);

    OFX::DoubleParamDescriptor* sharpness = desc.defineDoubleParam(kParamSharpness);
    sharpness->setLabel("Sharpness");
    sharpness->setParent(*groupParamsDifTensors);
    sharpness->setDefault(kDefaultSharpnessValue);
    sharpness->setRange(0.0, 10000.0);
    sharpness->setDisplayRange(0.0, 10.0);
    sharpness->setHint("Contour preservation");

    OFX::DoubleParamDescriptor* anisotropy = desc.defineDoubleParam(kParamAnisotropy);
    anisotropy->setLabel("Anisotropy");
    anisotropy->setParent(*groupParamsDifTensors);
    anisotropy->setDefault(kDefaultAnisotropyValue);
    anisotropy->setRange(0.0, 1.0);
    anisotropy->setDisplayRange(0.0, 1.0);
    //    anisotropy->setIsSecret(true);
    anisotropy->setHint("Smoothing anisotropy");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in]   handle  effect handle
 * @param[in]   context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* AnisotropicTensorsPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new TensorsPlugin(handle);
}
}
}
}
}
