#include "MathOperatorPluginFactory.hpp"
#include "MathOperatorPlugin.hpp"
#include "MathOperatorDefinitions.hpp"
#include "ofxsImageEffect.h"

#include <limits>

namespace tuttle
{
namespace plugin
{
namespace mathOperator
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out] desc Effect descriptor
 */
void MathOperatorPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleMathOperator", "MathOperator", "MathOperator");
    desc.setPluginGrouping("tuttle/image/process/math");

    desc.setDescription("Plugin under early development.");

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
void MathOperatorPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    OFX::ChoiceParamDescriptor* mathOperatorOp = desc.defineChoiceParam(kMathOperatorOperator);
    mathOperatorOp->setLabel("Operation");
    mathOperatorOp->setHint("Operator selection mode.");
    mathOperatorOp->appendOption(kMathOperatorPlus);
    mathOperatorOp->appendOption(kMathOperatorMultiply);
    mathOperatorOp->appendOption(kMathOperatorPow);
    mathOperatorOp->appendOption(kMathOperatorSqrt);
    mathOperatorOp->appendOption(kMathOperatorLog);
    mathOperatorOp->appendOption(kMathOperatorLn);

    OFX::ChoiceParamDescriptor* mathOperatorType = desc.defineChoiceParam(kMathOperatorType);
    mathOperatorType->setLabel("Type");
    mathOperatorType->setHint("Operator selection mode.");
    mathOperatorType->appendOption(kMathOperatorRgba);
    mathOperatorType->appendOption(kMathOperatorRgb);
    mathOperatorType->appendOption(kMathOperatorChannels);

    OFX::DoubleParamDescriptor* masterValue = desc.defineDoubleParam(kMasterValue);
    masterValue->setLabel("Master");
    masterValue->setDisplayRange(-1.0, 1.0);
    masterValue->setDoubleType(OFX::eDoubleTypePlain);
    masterValue->setDefault(1.0);

    OFX::DoubleParamDescriptor* redValue = desc.defineDoubleParam(kRedValue);
    redValue->setLabel("Red");
    redValue->setDisplayRange(-1.0, 1.0);
    redValue->setDoubleType(OFX::eDoubleTypePlain);
    redValue->setDefault(1.0);

    OFX::DoubleParamDescriptor* greenValue = desc.defineDoubleParam(kGreenValue);
    greenValue->setLabel("Green");
    greenValue->setDisplayRange(-1.0, 1.0);
    greenValue->setDoubleType(OFX::eDoubleTypePlain);
    greenValue->setDefault(1.0);

    OFX::DoubleParamDescriptor* blueValue = desc.defineDoubleParam(kBlueValue);
    blueValue->setLabel("Blue");
    blueValue->setDisplayRange(-1.0, 1.0);
    blueValue->setDoubleType(OFX::eDoubleTypePlain);
    blueValue->setDefault(1.0);

    OFX::DoubleParamDescriptor* alphaValue = desc.defineDoubleParam(kAlphaValue);
    alphaValue->setLabel("Alpha");
    alphaValue->setDisplayRange(-1.0, 1.0);
    alphaValue->setDoubleType(OFX::eDoubleTypePlain);
    alphaValue->setDefault(1.0);

    OFX::BooleanParamDescriptor* redChannel = desc.defineBooleanParam(kRedChannel);
    redChannel->setLabel("Red");
    redChannel->setDefault(true);

    OFX::BooleanParamDescriptor* greenChannel = desc.defineBooleanParam(kGreenChannel);
    greenChannel->setLabel("Green");
    greenChannel->setDefault(true);

    OFX::BooleanParamDescriptor* blueChannel = desc.defineBooleanParam(kBlueChannel);
    blueChannel->setLabel("Blue");
    blueChannel->setDefault(true);

    OFX::BooleanParamDescriptor* alphaChannel = desc.defineBooleanParam(kAlphaChannel);
    alphaChannel->setLabel("Alpha");
    alphaChannel->setDefault(true);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  Effect handle
 * @param[in] context Application context
 * @return  plugin instance
 */
OFX::ImageEffect* MathOperatorPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new MathOperatorPlugin(handle);
}
}
}
}
