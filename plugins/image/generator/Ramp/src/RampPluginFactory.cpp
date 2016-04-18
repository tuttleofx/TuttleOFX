#include "RampPluginFactory.hpp"
#include "RampPlugin.hpp"
#include "RampDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>

namespace tuttle
{
namespace plugin
{
namespace ramp
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void RampPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleRamp", "Ramp", "Ramp");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Ramp generator.");

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
void RampPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* direction = desc.defineChoiceParam(kRampDirection);
    direction->appendOption("horizontal", "Horizontal");
    direction->appendOption("vertical", "Vertical");
    direction->setLabel("Ramp Direction");
    direction->setHint("Select the ramp direction.");

    OFX::BooleanParamDescriptor* color = desc.defineBooleanParam(kRampColor);
    color->setDefault(false);
    color->setLabel("Color Ramp");
    color->setHint("Enable the R/G/B/Gray ramp.");

    OFX::RGBAParamDescriptor* colorStart = desc.defineRGBAParam(kRampColorStart);
    colorStart->setDefault(0, 0, 0, 1);
    colorStart->setLabel("Start color");

    OFX::RGBAParamDescriptor* colorEnd = desc.defineRGBAParam(kRampColorEnd);
    colorEnd->setDefault(1, 1, 1, 1);
    colorEnd->setLabel("End Color");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* RampPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new RampPlugin(handle);
}
}
}
}
