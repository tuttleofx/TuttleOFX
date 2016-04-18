#ifndef _TUTTLE_IOPLUGIN_WRITERPLUGINFACTORY_HPP_
#define _TUTTLE_IOPLUGIN_WRITERPLUGINFACTORY_HPP_

#include "WriterDefinition.hpp"

#include <tuttle/plugin/exceptions.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

namespace tuttle
{
namespace plugin
{

void describeWriterParamsInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::StringParamDescriptor* filename = desc.defineStringParam(kTuttlePluginFilename);
    filename->setLabel(kTuttlePluginFilenameLabel);
    filename->setStringType(OFX::eStringTypeFilePath);
    filename->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    // the file doesn't need to exist, the writer will create it!
    filename->setFilePathExists(false);
    desc.addClipPreferencesSlaveParam(*filename);

    OFX::ChoiceParamDescriptor* channel = desc.defineChoiceParam(kTuttlePluginChannel);
    channel->setLabel(kTuttlePluginChannelLabel);
    channel->appendOption(kTuttlePluginChannelAuto);
    channel->appendOption(kTuttlePluginChannelGray);
    channel->appendOption(kTuttlePluginChannelRGB);
    channel->appendOption(kTuttlePluginChannelRGBA);
    channel->setDefault(0);

    OFX::ChoiceParamDescriptor* bitDepth = desc.defineChoiceParam(kTuttlePluginBitDepth);
    bitDepth->setLabel(kTuttlePluginBitDepthLabel);
    bitDepth->appendOption(kTuttlePluginBitDepth8);
    bitDepth->appendOption(kTuttlePluginBitDepth16);
    bitDepth->setDefault(0);

    OFX::BooleanParamDescriptor* premult = desc.defineBooleanParam(kParamPremultiplied);
    premult->setLabel("Premultiplied");
    premult->setDefault(false);

    OFX::ChoiceParamDescriptor* existingFile = desc.defineChoiceParam(kParamWriterExistingFile);
    existingFile->setLabel("Existing File");
    existingFile->appendOption(kParamWriterExistingFile_overwrite);
    existingFile->appendOption(kParamWriterExistingFile_error);
    if(OFX::getImageEffectHostDescription()->hostName == "TuttleOfx")
    {
        // Only Tuttle is able to do that, because we disable the computation
        // using the IsIdentity Action. This is not in the OpenFX standard.
        existingFile->appendOption(kParamWriterExistingFile_skip);
    }
    // existingFile->appendOption( kParamWriterExistingFile_reader ); // TODO: not implemented yet.
    existingFile->setDefault(eParamWriterExistingFile_overwrite);

    OFX::BooleanParamDescriptor* copyToOutput = desc.defineBooleanParam(kParamWriterCopyToOutput);
    copyToOutput->setLabel("Copy buffer to output");
    copyToOutput->setHint("This is only useful if you connect nodes to the output clip of the writer.");
    copyToOutput->setDefault(false);

    OFX::PushButtonParamDescriptor* render = desc.definePushButtonParam(kParamWriterRender);
    render->setLabels("Render", "Render", "Render step");
    render->setHint("Force render (writing)");

    OFX::BooleanParamDescriptor* renderAlways = desc.defineBooleanParam(kParamWriterRenderAlways);
    renderAlways->setLabel("Render always");
    renderAlways->setHint("This is only useful as a workaround for GUI applications.");
    renderAlways->setDefault(true); // because tuttle is not declared as a background renderer

    OFX::IntParamDescriptor* forceNewRender = desc.defineIntParam(kParamWriterForceNewRender);
    forceNewRender->setLabel("Force new render");
    forceNewRender->setHint("This is only useful as a workaround for GUI applications.");
    forceNewRender->setEnabled(false);
    forceNewRender->setIsSecret(true);
    forceNewRender->setIsPersistant(false);
    forceNewRender->setAnimates(false);
    forceNewRender->setCacheInvalidation(OFX::eCacheInvalidateValueAll);
    forceNewRender->setEvaluateOnChange(true);
    forceNewRender->setDefault(0);
}
}
}

#endif
