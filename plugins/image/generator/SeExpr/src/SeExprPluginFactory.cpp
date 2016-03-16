#include "SeExprPluginFactory.hpp"
#include "SeExprPlugin.hpp"
#include "SeExprDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <tuttle/plugin/context/GeneratorPluginFactory.hpp>

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace seExpr
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void SeExprPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleSeExpr", "SeExpr", "SeExpr");
    desc.setPluginGrouping("tuttle/image/generator");

    desc.setDescription("SeExpr is a simple expression language that we use to provide"
                        "artistic control and customization to our core software.\n"
                        "We use it for procedural geometry synthesis, image synthesis,"
                        "simulation control, and much more.\n\n"
                        "language documentation: http://wdas.github.io/SeExpr/doxygen/userdoc.html"
                        "\n\n"
                        "projet website: http://www.disneyanimation.com/technology/seexpr.html");

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
    desc.setSupportsMultiResolution(true);
    desc.setSupportsMultipleClipDepths(true);
    desc.setSupportsTiles(kSupportTiles);
}

/**
 * @brief Function called to describe the plugin controls and features.
 * @param[in, out]   desc       Effect descriptor
 * @param[in]        context    Application context
 */
void SeExprPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    describeGeneratorParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* chooseInput = desc.defineChoiceParam(kParamChooseInput);
    chooseInput->appendOption(kParamChooseInputCode);
    chooseInput->appendOption(kParamChooseInputFile);
    chooseInput->setDefault(eParamChooseInputCode);

    OFX::StringParamDescriptor* code = desc.defineStringParam(kParamSeExprCode);
    code->setLabel("SeExpr code");
    code->setHint("Write your SeExpr code.");
    code->setStringType(OFX::eStringTypeMultiLine);
    code->setDefault("cfbm([10*u,10*v,.5])");

    OFX::StringParamDescriptor* file = desc.defineStringParam(kTuttlePluginFilename);
    file->setLabel(kTuttlePluginFilenameLabel);
    file->setHint("SeExpr source code file.");
    file->setStringType(OFX::eStringTypeFilePath);

    OFX::Double2DParamDescriptor* textureOffset = desc.defineDouble2DParam(kParamTextureOffset);
    textureOffset->setLabel("Texture Offset (u, v)");
    textureOffset->setHint("Set the u,v offset texture parameters.");
    textureOffset->setDisplayRange(-100, -100, 100, 100);
    textureOffset->setDefault(0.0, 0.0);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* SeExprPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new SeExprPlugin(handle);
}
}
}
}
