#include "InvertPluginFactory.hpp"
#include "InvertPlugin.hpp"
#include <tuttle/plugin/ImageGilProcessor.hpp>
#include <tuttle/plugin/exceptions.hpp>

#include <string>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <boost/gil/gil_all.hpp>
#include <boost/scoped_ptr.hpp>

namespace tuttle
{
namespace plugin
{
namespace invert
{

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void InvertPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleInvert", "Invert", "Image inverter");
    desc.setPluginGrouping("tuttle/image/process/color");

    desc.setDescription("Invert selected channels value."
                        "\n"
                        "\n"
                        "http://en.wikipedia.org/wiki/Negative_image");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextFilter);
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
void InvertPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
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

    OFX::GroupParamDescriptor* processGroup = desc.defineGroupParam(kParamProcessGroup);
    processGroup->setLabel(kParamProcessGroupLabel);

    OFX::BooleanParamDescriptor* processGray = desc.defineBooleanParam(kParamProcessGray);
    processGray->setLabel(kParamProcessGrayLabel);
    processGray->setHint(kParamProcessGrayHint);
    processGray->setDefault(true);
    processGray->setParent(processGroup);

    OFX::BooleanParamDescriptor* processR = desc.defineBooleanParam(kParamProcessR);
    processR->setLabel(kParamProcessRLabel);
    processR->setHint(kParamProcessRHint);
    processR->setDefault(true);
    processR->setParent(processGroup);

    OFX::BooleanParamDescriptor* processG = desc.defineBooleanParam(kParamProcessG);
    processG->setLabel(kParamProcessGLabel);
    processG->setHint(kParamProcessGHint);
    processG->setDefault(true);
    processG->setParent(processGroup);

    OFX::BooleanParamDescriptor* processB = desc.defineBooleanParam(kParamProcessB);
    processB->setLabel(kParamProcessBLabel);
    processB->setHint(kParamProcessBHint);
    processB->setDefault(true);
    processB->setParent(processGroup);

    OFX::BooleanParamDescriptor* processA = desc.defineBooleanParam(kParamProcessA);
    processA->setLabel(kParamProcessALabel);
    processA->setHint(kParamProcessAHint);
    processA->setDefault(false);
    processA->setParent(processGroup);
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* InvertPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new InvertPlugin(handle);
}
}
}
}
