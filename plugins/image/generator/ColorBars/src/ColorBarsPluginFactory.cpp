#include "ColorBarsPluginFactory.hpp"
#include "ColorBarsPlugin.hpp"
#include "ColorBarsDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorBars
{

// static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorBarsPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleColorBars", "ColorBars", "ColorBars");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Color Bars generator.");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextGenerator);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // plugin flags
    desc.setRenderThreadSafety(OFX::eRenderFullySafe);
    desc.setHostFrameThreading(false);
    desc.setSupportsMultiResolution(false);
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void ColorBarsPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* levels = desc.defineChoiceParam(kColorBarsLevels);
    levels->setLabel("Levels");
    levels->appendOption(kColorBarsLevels100);
    levels->appendOption(kColorBarsLevels75);
    levels->setDefault(eColorBarsLevels75);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorBarsPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ColorBarsPlugin(handle);
}
}
}
}
