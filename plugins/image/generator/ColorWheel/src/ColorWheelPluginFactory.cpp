#include "ColorWheelPluginFactory.hpp"
#include "ColorWheelPlugin.hpp"
#include "ColorWheelDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorWheel
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorWheelPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleColorWheel", "ColorWheel", "ColorWheel");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Color Wheel generator.");

    // add the supported contexts, only filter at the moment
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
void ColorWheelPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* mode = desc.defineChoiceParam(kColorWheelMode);
    mode->setLabel("Type");
    mode->setHint("Select mode for the color wheel.");
    mode->appendOption(kColorWheelModeWhite);
    mode->appendOption(kColorWheelModeBlack);
    mode->appendOption(kColorWheelModeRainbow);
    mode->setDefault(0);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorWheelPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ColorWheelPlugin(handle);
}
}
}
}
