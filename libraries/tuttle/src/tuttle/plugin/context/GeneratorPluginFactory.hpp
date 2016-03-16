#ifndef _TUTTLE_PLUGIN_GENERATORPLUGINFACTORY_HPP_
#define _TUTTLE_PLUGIN_GENERATORPLUGINFACTORY_HPP_

#include "GeneratorDefinition.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>
#include <iostream>

namespace tuttle
{
namespace plugin
{

static const bool kSupportTiles = true;

void describeGeneratorParamsInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    // Create the mandated optional input clip
    OFX::ClipDescriptor* srcClip = desc.defineClip(kOfxImageEffectSimpleSourceClipName);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    srcClip->addSupportedComponent(OFX::ePixelComponentRGB);
    srcClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    srcClip->setSupportsTiles(kSupportTiles);
    srcClip->setOptional(true);

    // Create the mandated output clip
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    OFX::ChoiceParamDescriptor* explicitConversion = desc.defineChoiceParam(kParamGeneratorExplicitConversion);
    explicitConversion->setLabel("Explicit conversion");
    explicitConversion->appendOption(kTuttlePluginBitDepthAuto);
    explicitConversion->appendOption(kTuttlePluginBitDepth8);
    explicitConversion->appendOption(kTuttlePluginBitDepth16);
    explicitConversion->appendOption(kTuttlePluginBitDepth32f);
    explicitConversion->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    explicitConversion->setAnimates(false);
    desc.addClipPreferencesSlaveParam(*explicitConversion);

    if(OFX::getImageEffectHostDescription()->supportsMultipleClipDepths)
    {
        explicitConversion->setDefault(0);
    }
    else
    {
        explicitConversion->setIsSecret(true);
        explicitConversion->setDefault(static_cast<int>(OFX::getImageEffectHostDescription()->getDefaultPixelDepth()));
    }

    OFX::ChoiceParamDescriptor* components = desc.defineChoiceParam(kTuttlePluginChannel);
    components->setLabel(kTuttlePluginChannelLabel);
    components->appendOption(kTuttlePluginChannelGray);
    components->appendOption(kTuttlePluginChannelRGB);
    components->appendOption(kTuttlePluginChannelRGBA);
    components->setDefault(2); // rgba

    OFX::ChoiceParamDescriptor* method = desc.defineChoiceParam(kParamMode);
    method->setLabel("Mode");
    method->appendOption(kParamModeFormat);
    method->appendOption(kParamModeSize);
    method->setDefault(eParamModeFormat);

    OFX::ChoiceParamDescriptor* format = desc.defineChoiceParam(kParamFormat);
    format->setLabel("Format");
    format->appendOption(kParamFormatPCVideo);
    format->appendOption(kParamFormatNTSC);
    format->appendOption(kParamFormatPAL);
    format->appendOption(kParamFormatHD);
    format->appendOption(kParamFormatNTSC169);
    format->appendOption(kParamFormatPAL169);
    format->appendOption(kParamFormat1kSuper35);
    format->appendOption(kParamFormat1kCinemascope);
    format->appendOption(kParamFormat2kSuper35);
    format->appendOption(kParamFormat2kCinemascope);
    format->appendOption(kParamFormat4kSuper35);
    format->appendOption(kParamFormat4kCinemascope);
    format->appendOption(kParamFormatSquare256);
    format->appendOption(kParamFormatSquare512);
    format->appendOption(kParamFormatSquare1k);
    format->appendOption(kParamFormatSquare2k);
    format->setDefault(eParamFormat2kCinemascope);

    OFX::BooleanParamDescriptor* specificRatio = desc.defineBooleanParam(kParamSizeSpecificRatio);
    specificRatio->setLabel("Specific ratio");
    specificRatio->setDefault(false);
    specificRatio->setHint("Specific input image ratio.");

    OFX::Int2DParamDescriptor* size = desc.defineInt2DParam(kParamSize);
    size->setLabel("Size");
    size->setDefault(200, 200);
    size->setRange(1, 1, std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    size->setHint("Set the output size (width, height).");

    OFX::ChoiceParamDescriptor* direction = desc.defineChoiceParam(kParamSizeOrientation);
    direction->setLabel("Orientation");
    direction->appendOption(kParamSizeOrientationX);
    direction->appendOption(kParamSizeOrientationY);
    direction->setDefault(eParamSizeOrientationX);

    OFX::DoubleParamDescriptor* ratioValue = desc.defineDoubleParam(kParamSizeRatioValue);
    ratioValue->setLabel("Ratio Value");
    ratioValue->setDefault(1.0);
    ratioValue->setRange(1, std::numeric_limits<int>::max());
    ratioValue->setDisplayRange(0, 50);
    ratioValue->setHint("Set the ratio.");

    OFX::IntParamDescriptor* width = desc.defineIntParam(kParamSizeWidth);
    width->setLabel("Width");
    width->setDefault(200);
    width->setRange(1, std::numeric_limits<int>::max());
    width->setDisplayRange(0, 3000);
    width->setHint("Set the width in pixels and specify the ratio.");

    OFX::IntParamDescriptor* height = desc.defineIntParam(kParamSizeHeight);
    height->setLabel("Height");
    height->setDefault(200);
    height->setRange(1, std::numeric_limits<int>::max());
    height->setDisplayRange(0, 3000);
    height->setHint("Set the height in pixels and specify the ratio.");
}
}
}

#endif
