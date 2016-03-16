#include "ColorCubePluginFactory.hpp"
#include "ColorCubePlugin.hpp"
#include "ColorCubeDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>

namespace tuttle
{
namespace plugin
{
namespace colorCube
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void ColorCubePluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleColorCube", "ColorCube", "ColorCube");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Color Cube generator.");

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
void ColorCubePluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* step = desc.defineChoiceParam(kColorCubeStep);
    step->setLabel("Cubes");
    step->setHint("Select cubes on the main Color Cube.");
    step->appendOption("4");
    step->appendOption("8");
    step->appendOption("16");
    step->appendOption("32");
    step->appendOption("64");
    step->appendOption("128");
    step->appendOption("256");
    step->appendOption("512");
    step->appendOption("1024");
    step->setDefault(3);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ColorCubePluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ColorCubePlugin(handle);
}
}
}
}
