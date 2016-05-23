#include "EXRReaderDefinitions.hpp"
#include "EXRReaderPluginFactory.hpp"
#include "EXRReaderPlugin.hpp"

#include <tuttle/ioplugin/context/ReaderPluginFactory.hpp>

namespace tuttle
{
namespace plugin
{
namespace exr
{
namespace reader
{

static const bool kSupportTiles = false;

/**
 * @brief Function called to describe the plugin main features.
 * @param[in, out]   desc     Effect descriptor
 */
void EXRReaderPluginFactory::describe(OFX::ImageEffectDescriptor& desc)
{
    desc.setLabels("TuttleExrReader", "ExrReader", "Exr file reader");
    desc.setPluginGrouping("tuttle/image/io");

    desc.setDescription("EXR File reader\n"
                        "Plugin is used to read exr files.\n");

    // add the supported contexts
    desc.addSupportedContext(OFX::eContextReader);
    desc.addSupportedContext(OFX::eContextGeneral);

    // add supported pixel depths
    desc.addSupportedBitDepth(OFX::eBitDepthUByte);
    desc.addSupportedBitDepth(OFX::eBitDepthUShort);
    desc.addSupportedBitDepth(OFX::eBitDepthFloat);

    // add supported extensions
    desc.addSupportedExtension("exr");
    desc.setPluginEvaluation(90);

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
void EXRReaderPluginFactory::describeInContext(OFX::ImageEffectDescriptor& desc, OFX::EContext context)
{
    OFX::ClipDescriptor* dstClip = desc.defineClip(kOfxImageEffectOutputClipName);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGBA);
    dstClip->addSupportedComponent(OFX::ePixelComponentRGB);
    dstClip->addSupportedComponent(OFX::ePixelComponentAlpha);
    dstClip->setSupportsTiles(kSupportTiles);

    describeReaderParamsInContext(desc, context);

    OFX::ChoiceParamDescriptor* outRedIs = desc.defineChoiceParam(kParamOutputRedIs);
    outRedIs->appendOption("0");
    outRedIs->appendOption("1");
    outRedIs->appendOption("2");
    outRedIs->appendOption("3");
    outRedIs->setLabel("Red is");
    outRedIs->setDefault(0);
    outRedIs->setHint("Read-only information to get index of red component in the file.");

    OFX::ChoiceParamDescriptor* outGreenIs = desc.defineChoiceParam(kParamOutputGreenIs);
    outGreenIs->appendOption("0");
    outGreenIs->appendOption("1");
    outGreenIs->appendOption("2");
    outGreenIs->appendOption("3");
    outGreenIs->setLabel("Green is");
    outGreenIs->setDefault(0);
    outGreenIs->setHint("Read-only information to get index of green component in the file.");

    OFX::ChoiceParamDescriptor* outBlueIs = desc.defineChoiceParam(kParamOutputBlueIs);
    outBlueIs->appendOption("0");
    outBlueIs->appendOption("1");
    outBlueIs->appendOption("2");
    outBlueIs->appendOption("3");
    outBlueIs->setLabel("Blue is");
    outBlueIs->setDefault(0);
    outBlueIs->setHint("Read-only information to get index of blue component in the file.");

    OFX::ChoiceParamDescriptor* outAlphaIs = desc.defineChoiceParam(kParamOutputAlphaIs);
    outAlphaIs->appendOption("0");
    outAlphaIs->appendOption("1");
    outAlphaIs->appendOption("2");
    outAlphaIs->appendOption("3");
    outAlphaIs->setLabel("Alpha is");
    outAlphaIs->setDefault(0);
    outAlphaIs->setHint("Read-only information to get index of alpha component in the file.");

    OFX::ChoiceParamDescriptor* outputData = desc.defineChoiceParam(kParamOutputData);
    outputData->appendOption("display");
    outputData->appendOption("data");
    outputData->setLabel("Output Data");
    outputData->setDefault(0);

    OFX::ChoiceParamDescriptor* fileBitDepth = desc.defineChoiceParam(kParamFileBitDepth);
    fileBitDepth->setLabel("File Bit Depth");
    fileBitDepth->appendOption(kTuttlePluginBitDepth16f);
    fileBitDepth->appendOption(kTuttlePluginBitDepth32);
    fileBitDepth->appendOption(kTuttlePluginBitDepth32f);
    fileBitDepth->appendOption(kTuttlePluginBitDepthNone);
    fileBitDepth->setDefault(eTuttlePluginFileBitDepthNone);
    fileBitDepth->setHint("Read-only information about the image bit depth stored in the file.\n"
                          "Data type is per channel in EXR. So we read the type of the first channel.");

    OFX::ChoiceParamDescriptor* compression = desc.defineChoiceParam(kParamCompression);
    compression->setLabel("Compression");
    compression->appendOption(kParamCompressionNone, kParamCompressionHintNone);
    compression->appendOption(kParamCompressionRLE, kParamCompressionHintRLE);
    compression->appendOption(kParamCompressionZIPS, kParamCompressionHintZIPS);
    compression->appendOption(kParamCompressionZIP, kParamCompressionHintZIP);
    compression->appendOption(kParamCompressionPIZ, kParamCompressionHintPIZ);
    compression->appendOption(kParamCompressionPXR24, kParamCompressionHintPXR24);
    compression->appendOption(kParamCompressionB44, kParamCompressionHintB44);
    compression->appendOption(kParamCompressionB44A, kParamCompressionHintB44A);
    compression->setDefault(eParamCompression_None);
    compression->setHint("Read-only information about the image compression stored in the file.");
}

/**
 * @brief Function called to create a plugin effect instance
 * @param[in] handle  effect handle
 * @param[in] context    Application context
 * @return  plugin instance
 */
OFX::ImageEffect* EXRReaderPluginFactory::createInstance(OfxImageEffectHandle handle, OFX::EContext context)
{
    return new EXRReaderPlugin(handle);
}
}
}
}
}
