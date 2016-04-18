#include "DiffPluginFactory.hpp"
#include "DiffPlugin.hpp"
#include "DiffDefinitions.hpp"

#include <tuttle/plugin/ImageGilProcessor.hpp>

namespace tuttle
{
namespace plugin
{
namespace quality
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void DiffPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleDiff", "Diff", "Diff");
    desc.setPluginGrouping("tuttle/param/analysis");
    desc.setDescription("Diff\n"
                        "Plugin is used to show the result of a quality mesure function between two given images. \n"
                        "MSE (mean square error) and PSNR (peak signal to noise ratio) are available.");

    // add the supported contexts, only filter at the moment
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
void DiffPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* srcClipA = desc.defineClip(kDiffSourceA);

    assert(srcClipA);
    srcClipA->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClipA->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClipA->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClipA->setSupportsTiles(kSupportTiles);
    srcClipA->setOptional(false);

    OFX::ClipDescriptor* srcClipB = desc.defineClip(kDiffSourceB);
    assert(srcClipB);
    srcClipB->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClipB->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClipB->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClipB->setSupportsTiles(kSupportTiles);
    srcClipB->setOptional(false);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    assert(dstClip);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::ChoiceParamDescriptor* diffFunction = desc.defineChoiceParam(kMeasureFunction);
    assert(diffFunction);
    diffFunction->setLabel(kMeasureFunctionLabel);
    diffFunction->appendOption(kMeasureFunctionMSE);
    diffFunction->appendOption(kMeasureFunctionPSNR);
    //	diffFunction->appendOption( kMeasureFunctionSSIM );
    diffFunction->setDefault(eMeasureFunctionPSNR);

    OFX::RGBAParamDescriptor* outputQualityMesure = desc.defineRGBAParam(kOutputQualityMesure);
    assert(outputQualityMesure);
    outputQualityMesure->setLabel(kOutputQualityMesureLabel);
    outputQualityMesure->setEvaluateOnChange(false);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* DiffPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new DiffPlugin(handle);
}
}
}
}
