#include "ComponentPluginFactory.hpp"
#include "ComponentPlugin.hpp"
#include "ComponentDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace component
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ComponentPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleComponent", "Component", "Component convertor");
    desc.setPluginGrouping("tuttle/image/process/channel");

    desc.setDescription("Convert channels components.");

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
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ComponentPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    OFX::ChoiceParamDescriptor* outTo = desc.defineChoiceParam(kParamTo);
    outTo->setLabel(kParamToLabel);
    outTo->appendOption(kConvertToGray);
    outTo->appendOption(kConvertToRGB);
    outTo->appendOption(kConvertToRGBA);
    outTo->setDefault(eConvertToRGBA);

    OFX::ChoiceParamDescriptor* outGray = desc.defineChoiceParam(kParamToGray);
    outGray->setLabel(kParamToGrayLabel);
    outGray->appendOption(kConvertToGrayMean);
    outGray->appendOption(kConvertToGrayRec601);
    outGray->appendOption(kConvertToGrayRec709);
    outGray->appendOption(kConvertToGraySelectRed);
    outGray->appendOption(kConvertToGraySelectGreen);
    outGray->appendOption(kConvertToGraySelectBlue);
    outGray->appendOption(kConvertToGraySelectAlpha);
    outGray->setDefault(3); // terry::color::components::eConvertToGrayRec709

    OFX::BooleanParamDescriptor* outPremult = desc.defineBooleanParam(kParamPremutliplied);
    outPremult->setLabel(kParamPremutlipliedLabel);
    outPremult->setDefault(false);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ComponentPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ComponentPlugin(handle);
}
}
}
}
