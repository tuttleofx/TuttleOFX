#include "ConstantPluginFactory.hpp"
#include "ConstantPlugin.hpp"
#include "ConstantDefinitions.hpp"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle
{
namespace plugin
{
namespace constant
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void ConstantPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleConstant", "Constant", "Constant");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("Constant"
                        "\n"
                        "is a simple generator of a color.");

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
void ConstantPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::RGBAParamDescriptor* color1 = desc.defineRGBAParam(kConstantColor);
    color1->setDefault(0, 0, 0, 1);
    color1->setLabel("Color");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* ConstantPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new ConstantPlugin(handle);
}
}
}
}
