#include "CheckerboardPluginFactory.hpp"
#include "CheckerboardPlugin.hpp"
#include "CheckerboardDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle
{
namespace plugin
{
namespace checkerboard
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void CheckerboardPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleCheckerboard", "Checkerboard", "Checkerboard");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Checkerboard / square tiling / square grid / quadrille"
                        "\n"
                        "is a regular tiling of the Euclidean plane."
                        "\n"
                        "It is often used for its ability to be detected by a simple corner detect filter."
                        "\n"
                        "\n"
                        "http://en.wikipedia.org/wiki/Square_tiling");

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
void CheckerboardPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::Int2DParamDescriptor* boxes = desc.defineInt2DParam(kCheckerboardBoxes);
    boxes->setDefault(10, 10);
    boxes->setLabel("boxes number");
    boxes->setHint("Number of boxes of the checkerboard.");

    OFX::RGBAParamDescriptor* color1 = desc.defineRGBAParam(kCheckerboardColor1);
    color1->setDefault(0, 0, 0, 1);
    color1->setLabel("color1");

    OFX::RGBAParamDescriptor* color2 = desc.defineRGBAParam(kCheckerboardColor2);
    color2->setDefault(1, 1, 1, 1);
    color2->setLabel("color2");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* CheckerboardPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new CheckerboardPlugin(handle);
}
}
}
}
